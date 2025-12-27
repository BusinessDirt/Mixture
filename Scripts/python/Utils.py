import logging
import sys
import tarfile
from pathlib import Path
from typing import List, Union
from zipfile import ZipFile

import requests

logger = logging.getLogger(__name__)

def download_file(url: Union[str, List[str]], filepath: Union[str, Path]) -> None:
    filepath = Path(filepath).resolve()
    filepath.parent.mkdir(parents=True, exist_ok=True)

    if isinstance(url, list):
        for url_option in url:
            try:
                download_file(url_option, filepath)
                return
            except Exception as e:
                logger.warning(f"Download failed: {e}. Trying next mirror...")
                if filepath.exists():
                    filepath.unlink()
        raise RuntimeError(f"Failed to download {filepath.name} from any source.")

    if not isinstance(url, str):
        raise TypeError("Argument 'url' must be a string or list of strings")

    headers = {
        'User-Agent': "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_4) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/83.0.4103.97 Safari/537.36"
    }

    try:
        with requests.get(url, headers=headers, stream=True) as response:
            response.raise_for_status()
            with open(filepath, 'wb') as f:
                for chunk in response.iter_content(chunk_size=8192):
                    f.write(chunk)
    except Exception as e:
        if filepath.exists():
            filepath.unlink()
        raise e


def unzip_file(filepath: Union[str, Path], delete_zip_file: bool = True) -> None:
    filepath = Path(filepath).resolve()
    location = filepath.parent

    if filepath.suffix == ".zip":
        _unzip_zip(filepath, location)
    elif filepath.name.endswith(".tar.gz") or filepath.suffix == ".gz":
        _unzip_tar_gz(filepath, location)
    else:
        logger.error(f"Unsupported file format: {filepath.suffix}")
        return

    if delete_zip_file:
        filepath.unlink()



def _unzip_zip(filepath: Path, location: Path) -> None:
    with ZipFile(filepath, 'r') as zf:
        file_list = zf.infolist()
        for member in file_list:
            target_path = location / member.filename

            # Check if file already exists with same size
            if not (target_path.exists() and target_path.stat().st_size == member.file_size):
                zf.extract(member, location)


def _unzip_tar_gz(filepath: Path, location: Path) -> None:
    mode = "r:gz" if filepath.name.endswith("gz") else "r"
    with tarfile.open(filepath, mode) as tar:
        members = tar.getmembers()
        for member in members:
            tar.extract(member, path=location)
