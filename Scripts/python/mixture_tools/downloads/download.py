import hashlib
import hmac
import logging
import os
import tempfile
from pathlib import Path
from typing import List, Union

import requests

logger = logging.getLogger(__name__)


def download_file(
    url: Union[str, List[str]],
    filepath: Union[str, Path],
    expected_sha256: str = None,
) -> None:
    filepath = Path(filepath).resolve()
    filepath.parent.mkdir(parents=True, exist_ok=True)

    if isinstance(url, list):
        for url_option in url:
            try:
                download_file(url_option, filepath, expected_sha256)
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

    temp_path = None
    try:
        hasher = hashlib.sha256()
        with tempfile.NamedTemporaryFile(
            dir=filepath.parent,
            prefix=f".{filepath.name}.",
            delete=False,
        ) as temp:
            temp_path = Path(temp.name)
            with requests.get(
                url,
                headers=headers,
                stream=True,
                timeout=(10, 60),
            ) as response:
                response.raise_for_status()
                for chunk in response.iter_content(chunk_size=64 * 1024):
                    if chunk:
                        temp.write(chunk)
                        hasher.update(chunk)

        if expected_sha256 is not None and not hmac.compare_digest(
            hasher.hexdigest(), expected_sha256.lower()
        ):
            raise RuntimeError("Downloaded file failed SHA-256 verification")

        os.replace(temp_path, filepath)
        temp_path = None
    finally:
        if temp_path is not None:
            temp_path.unlink(missing_ok=True)
