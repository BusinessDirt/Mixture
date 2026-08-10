import shutil
import stat
import tarfile
from pathlib import Path
from pathlib import PureWindowsPath
from typing import List, Union
from zipfile import ZipFile
def unzip_file(filepath: Union[str, Path], delete_zip_file: bool = True) -> None:
    filepath = Path(filepath).resolve()
    location = filepath.parent

    if filepath.suffix == ".zip":
        _unzip_zip(filepath, location)
    elif filepath.name.endswith(".tar.gz") or filepath.suffix == ".gz":
        _unzip_tar_gz(filepath, location)
    else:
        raise ValueError(f"Unsupported file format: {filepath.suffix}")

    if delete_zip_file:
        filepath.unlink()


def _unzip_zip(filepath: Path, location: Path) -> None:
    with ZipFile(filepath, 'r') as zf:
        for member in zf.infolist():
            target_path = _safe_archive_target(location, member.filename)
            mode = member.external_attr >> 16
            file_type = stat.S_IFMT(mode)

            if stat.S_ISLNK(mode):
                raise ValueError(f"Archive contains a symbolic link: {member.filename!r}")
            if file_type and not (stat.S_ISREG(mode) or stat.S_ISDIR(mode)):
                raise ValueError(f"Archive contains a special file: {member.filename!r}")

            if member.is_dir():
                target_path.mkdir(parents=True, exist_ok=True)
                continue

            target_path.parent.mkdir(parents=True, exist_ok=True)
            with zf.open(member, 'r') as source, target_path.open('wb') as destination:
                shutil.copyfileobj(source, destination)

            permissions = mode & 0o777
            if permissions:
                target_path.chmod(permissions)


def _unzip_tar_gz(filepath: Path, location: Path) -> None:
    mode = "r:gz" if filepath.name.endswith("gz") else "r"
    with tarfile.open(filepath, mode) as tar:
        for member in tar.getmembers():
            target_path = _safe_archive_target(location, member.name)

            if member.issym() or member.islnk():
                raise ValueError(f"Archive contains a link: {member.name!r}")
            if member.isdir():
                target_path.mkdir(parents=True, exist_ok=True)
                target_path.chmod(member.mode & 0o777)
                continue
            if not member.isfile():
                raise ValueError(f"Archive contains a special file: {member.name!r}")

            source = tar.extractfile(member)
            if source is None:
                raise ValueError(f"Could not read archive member: {member.name!r}")

            target_path.parent.mkdir(parents=True, exist_ok=True)
            with source, target_path.open('wb') as destination:
                shutil.copyfileobj(source, destination)
            target_path.chmod(member.mode & 0o777)


def _safe_archive_target(location: Path, member_name: str) -> Path:
    """Return an extraction target, rejecting paths outside of location."""
    if not member_name or "\x00" in member_name:
        raise ValueError(f"Invalid archive member path: {member_name!r}")

    # ZIP names use '/', but treating backslashes as separators also keeps this
    # safe when setup runs on Windows.
    normalized_name = member_name.replace('\\', '/')
    windows_path = PureWindowsPath(normalized_name)
    if normalized_name.startswith('/') or windows_path.drive:
        raise ValueError(f"Archive member uses an absolute path: {member_name!r}")

    root = location.resolve()
    target = (root / normalized_name).resolve()
    try:
        target.relative_to(root)
    except ValueError as error:
        raise ValueError(
            f"Archive member escapes extraction directory: {member_name!r}"
        ) from error
    return target
