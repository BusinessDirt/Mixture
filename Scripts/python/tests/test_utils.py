import io
import hashlib
import ssl
import stat
import sys
import tarfile
import tempfile
import unittest
import zipfile
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from mixture_tools.downloads import archive as archive_utils
from mixture_tools.downloads import download


class FakeResponse:
    def __init__(self, chunks):
        self.contents = io.BytesIO(b"".join(chunks))

    def __enter__(self):
        return self

    def __exit__(self, *_args):
        return False

    def read(self, chunk_size):
        return self.contents.read(chunk_size)


class VerifiedDownloadTests(unittest.TestCase):
    def setUp(self):
        self.temporary_directory = tempfile.TemporaryDirectory()
        self.root = Path(self.temporary_directory.name)

    def tearDown(self):
        self.temporary_directory.cleanup()

    @mock.patch("mixture_tools.downloads.download.urllib.request.urlopen")
    def test_verified_download_replaces_destination(self, urlopen):
        contents = b"verified archive"
        urlopen.return_value = FakeResponse([contents[:8], contents[8:]])
        destination = self.root / "archive.zip"
        destination.write_bytes(b"old contents")

        download.download_file(
            "https://example.test/archive.zip",
            destination,
            hashlib.sha256(contents).hexdigest(),
        )

        self.assertEqual(destination.read_bytes(), contents)
        self.assertEqual(urlopen.call_count, 1)
        self.assertEqual(urlopen.call_args.kwargs["timeout"], 60)
        self.assertIsInstance(urlopen.call_args.kwargs["context"], ssl.SSLContext)
        self.assertEqual(
            urlopen.call_args.args[0].full_url,
            "https://example.test/archive.zip",
        )
        self.assertEqual(list(self.root.glob(".archive.zip.*")), [])

    @mock.patch("mixture_tools.downloads.download.urllib.request.urlopen")
    def test_checksum_mismatch_preserves_destination(self, urlopen):
        urlopen.return_value = FakeResponse([b"untrusted archive"])
        destination = self.root / "archive.zip"
        destination.write_bytes(b"known good archive")

        with self.assertRaisesRegex(RuntimeError, "SHA-256 verification"):
            download.download_file(
                "https://example.test/archive.zip",
                destination,
                "0" * 64,
            )

        self.assertEqual(destination.read_bytes(), b"known good archive")
        self.assertEqual(list(self.root.glob(".archive.zip.*")), [])


class SafeArchiveExtractionTests(unittest.TestCase):
    def setUp(self):
        self.temporary_directory = tempfile.TemporaryDirectory()
        self.root = Path(self.temporary_directory.name)

    def tearDown(self):
        self.temporary_directory.cleanup()

    def test_extracts_valid_zip(self):
        archive = self.root / "valid.zip"
        with zipfile.ZipFile(archive, "w") as output:
            output.writestr("premake/bin/premake5", b"binary")

        archive_utils.unzip_file(archive, delete_zip_file=False)

        self.assertEqual((self.root / "premake/bin/premake5").read_bytes(), b"binary")

    def test_rejects_zip_parent_traversal(self):
        archive = self.root / "traversal.zip"
        with zipfile.ZipFile(archive, "w") as output:
            output.writestr("../outside.txt", b"malicious")

        with self.assertRaisesRegex(ValueError, "escapes extraction directory"):
            archive_utils.unzip_file(archive)

        self.assertFalse((self.root.parent / "outside.txt").exists())
        self.assertTrue(archive.exists())

    def test_rejects_zip_absolute_path(self):
        archive = self.root / "absolute.zip"
        with zipfile.ZipFile(archive, "w") as output:
            output.writestr("/outside.txt", b"malicious")

        with self.assertRaisesRegex(ValueError, "absolute path"):
            archive_utils.unzip_file(archive)

    def test_rejects_zip_symlink(self):
        archive = self.root / "symlink.zip"
        link = zipfile.ZipInfo("link")
        link.create_system = 3
        link.external_attr = (stat.S_IFLNK | 0o777) << 16
        with zipfile.ZipFile(archive, "w") as output:
            output.writestr(link, "../outside.txt")

        with self.assertRaisesRegex(ValueError, "symbolic link"):
            archive_utils.unzip_file(archive)

    def test_extracts_valid_tar(self):
        archive = self.root / "valid.tar.gz"
        contents = b"binary"
        with tarfile.open(archive, "w:gz") as output:
            member = tarfile.TarInfo("premake/bin/premake5")
            member.size = len(contents)
            member.mode = 0o755
            output.addfile(member, io.BytesIO(contents))

        archive_utils.unzip_file(archive, delete_zip_file=False)

        target = self.root / "premake/bin/premake5"
        self.assertEqual(target.read_bytes(), contents)
        if sys.platform != "win32":
            self.assertEqual(stat.S_IMODE(target.stat().st_mode), 0o755)

    def test_rejects_tar_parent_traversal(self):
        archive = self.root / "traversal.tar.gz"
        with tarfile.open(archive, "w:gz") as output:
            member = tarfile.TarInfo("../outside.txt")
            member.size = 1
            output.addfile(member, io.BytesIO(b"x"))

        with self.assertRaisesRegex(ValueError, "escapes extraction directory"):
            archive_utils.unzip_file(archive)

        self.assertFalse((self.root.parent / "outside.txt").exists())
        self.assertTrue(archive.exists())

    def test_rejects_tar_absolute_path(self):
        archive = self.root / "absolute.tar.gz"
        with tarfile.open(archive, "w:gz") as output:
            member = tarfile.TarInfo("/outside.txt")
            member.size = 1
            output.addfile(member, io.BytesIO(b"x"))

        with self.assertRaisesRegex(ValueError, "absolute path"):
            archive_utils.unzip_file(archive)

    def test_rejects_tar_links(self):
        for link_type in (tarfile.SYMTYPE, tarfile.LNKTYPE):
            with self.subTest(link_type=link_type):
                archive = self.root / f"link-{link_type!r}.tar.gz"
                with tarfile.open(archive, "w:gz") as output:
                    member = tarfile.TarInfo("link")
                    member.type = link_type
                    member.linkname = "../outside.txt"
                    output.addfile(member)

                with self.assertRaisesRegex(ValueError, "contains a link"):
                    archive_utils.unzip_file(archive)

    def test_rejects_tar_special_file(self):
        archive = self.root / "fifo.tar.gz"
        with tarfile.open(archive, "w:gz") as output:
            member = tarfile.TarInfo("fifo")
            member.type = tarfile.FIFOTYPE
            output.addfile(member)

        with self.assertRaisesRegex(ValueError, "special file"):
            archive_utils.unzip_file(archive)


if __name__ == "__main__":
    unittest.main()
