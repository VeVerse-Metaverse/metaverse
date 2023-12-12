#// Author: Egor A. Pristavka. 
#// Copyright 2022-2023 VeVerse AS.
#// Copyright (c) 2023 LE7EL AS. All right reserved.

# /bin/python

import shutil
import os
from fnmatch import fnmatch

echo = True


def list_dir_files(root, pattern):
    result = []
    for path, _, files in os.walk(root):
        for name in files:
            if fnmatch(name, pattern):
                result.append((path, name))
    return result


def copy_and_overwrite(from_path, to_path):
    if os.path.exists(to_path):
        shutil.rmtree(to_path)
    if os.path.exists(from_path):
        shutil.copytree(from_path, to_path)


def delete_files(root, pattern):
    print(f"deleting files in dir {root} using pattern {pattern}")
    for path, dirs, files in os.walk(root):
        for dir in dirs:
            for path, dirs, files in os.walk(dir):
                for name in files:
                    if fnmatch(name, pattern):
                        print(f"os remove {name}")
                        # os.remove(name)


if __name__ == '__main__':
    start_dir = os.path.join("..", "Plugins")
    if os.path.exists(start_dir):
        print(f"dir exists {start_dir}")
        delete_files(start_dir, "Intermediate/*")
    else:
        print(f"dir doesnt exist {start_dir}")
