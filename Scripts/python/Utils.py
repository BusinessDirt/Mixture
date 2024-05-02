import sys
import os
import platform

import requests
import time
import urllib3 as urllib

from zipfile import ZipFile
import gzip
import tarfile

def download_file(url, filepath):
    path = filepath
    filepath = os.path.abspath(filepath)
    os.makedirs(os.path.dirname(filepath), exist_ok=True)

    if (type(url) is list):
        for url_option in url:
            print("Downloading", url_option)
            try:
                download_file(url_option, filepath)
                return
            except urllib.exceptions.LocationValueError as e:
                print(f"Location Value Error encountered: {e.__cause__}. Proceeding with backup...\n\n")
                os.remove(filepath)
                pass
            except urllib.exceptions.HTTPError as e:
                print(f"HTTP Error encountered: {e.__cause__}. Proceeding with backup...\n\n")
                os.remove(filepath)
                pass
            except:
                print("Something went wrong. Proceeding with backup...\n\n")
                os.remove(filepath)
                pass
        raise ValueError(f"Failed to download {filepath}")
    if not(type(url) is str):
        raise TypeError("Argument 'url' must be of type list or string")
    
    with open(filepath, 'wb') as f:
        headers = {'User-Agent': "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_4) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/83.0.4103.97 Safari/537.36"}
        response = requests.get(url, headers=headers, stream=True)
        total = response.headers.get('content-length')

        if total is None:
            f.write(response.content)
        else:
            downloaded: int = 0
            total: int = int(total)
            start_time:float = time.time()
            for data in response.iter_content(chunk_size=max(int(total / 1000), 1024*1024)):
                downloaded += len(data)
                f.write(data)

                done, percentage = __percentage_done_helper__(downloaded, total)
                elapsed_time: float = time.time() - start_time
                avg_speed: str = __speed_helper__(downloaded, elapsed_time)
                __speed_write_helper__(done, percentage, avg_speed)
                
    sys.stdout.write('\n')

def unzip_file(filepath: str, delete_zip_file=True):
    path = os.path.abspath(filepath)
    location = os.path.dirname(path)

    if filepath.endswith(".zip"):
        __unzip_zip__(path, location)
    elif filepath.endswith(".gz"):
        __unzip_gz__(path, location)
    else:
        print("Unsupported file format.")
        return


    if delete_zip_file:
        os.remove(path)

def __unzip_zip__(path, location):
    content = dict()
    size = 0
    with ZipFile(path, 'r') as folder:
        for name in folder.namelist():
            content[name] = folder.getinfo(name).file_size
        size = sum(content.values())
        extracted_size = 0
        start_time = time.time()
        for file_name, file_size in content.items():
            unzipped_file_path = os.path.abspath(f"{location}/{file_name}")
            os.makedirs(os.path.dirname(unzipped_file_path), exist_ok=True)

            if (os.path.isfile(unzipped_file_path)):
                size -= file_size
            else:
                folder.extract(file_name, path=location, pwd=None)
                extracted_size += file_size
            
            done, percentage = __percentage_done_helper__(extracted_size, size)
            elapsed_time: float = time.time() - start_time
            avg_speed: str = __speed_helper__(extracted_size, elapsed_time)
            __speed_write_helper__(done, percentage, avg_speed)
    sys.stdout.write('\n')

def __unzip_gz__(path, location):
    with gzip.open(path, 'rb') as f:
        with tarfile.open(fileobj=f, mode='r') as tar:
            tar.extractall(path=location)


def __percentage_done_helper__(x, total):
    try:
        done = int(50 * x / total)
        percentage = (x / total) * 100
    except ZeroDivisionError:
        done = 50
        percentage = 100

    return done, percentage

def __speed_helper__(finished, elapsed_time):
    try:
        avg_kb_per_second = (finished / 1024) / elapsed_time
    except ZeroDivisionError:
        avg_kb_per_second = 0.0
                
    avg_speed = '{:.2f} KB/s'.format(avg_kb_per_second)
    if avg_kb_per_second > 1024:
        avg_mb_per_second = avg_kb_per_second / 1024
        avg_speed = '{:.2f} MB/s'.format(avg_mb_per_second)

    return avg_speed

def __speed_write_helper__(done, percentage, avg_speed):
    sys.stdout.write('\r[{}{}] {:.2f}% ({})     '.format('█' * done, '.' * (50-done), percentage, avg_speed))
    sys.stdout.flush()