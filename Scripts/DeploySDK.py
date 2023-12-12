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


def copy_files(file, skip):
    result = True
    for s in skip:
        if s in file[0] or s in file[1]:
            result = False
            return result
    if result:
        destination_path = file[0]
        destination_filename = file[1]
        if destination_path.startswith(".\\"):
            destination_path = destination_path[len(".\\"):]
        destination_dir = os.path.join(staging_dir, destination_path)
        if not os.path.exists(destination_dir):
            if echo:
                print(f"creating directory {destination_dir}")
            os.makedirs(destination_dir)
        if echo:
            print(f"copying {os.path.join(destination_path, destination_filename)} to {os.path.join(destination_dir, destination_filename)}")
        shutil.copy(os.path.join(destination_path, destination_filename), destination_dir)
    return result


staging_dir = os.path.join(".", "SDK")
if os.path.exists(staging_dir):
    shutil.rmtree(staging_dir)

if not os.path.exists(staging_dir):
    os.makedirs(staging_dir)

binaries_dir = os.path.join(".", "Binaries")

# Base releases directory which contains base release required to pack new UGC.
releases_dir = os.path.join(".", "Releases")
target_releases_dir = os.path.join(staging_dir, "Releases")

# Base plugins directory which contains built plugin binaries required to build new UGC.
plugins_dir = os.path.join(".", "Plugins")
target_plugins_dir = os.path.join(staging_dir, "Plugins")

# List of all plugin directories.
plugin_dirs = os.listdir(plugins_dir)

platforms = ["Win64", "Win32", "Mac", "HoloLens", "XboxOne", "PS4", "IOS", "Android", "Linux", "LinuxAArch64", "AllDesktop", "TVOS", "Switch", "Lumin", "HTML5"]

skip_content_dir = ["UserInterfaceSFX", "VeUI\\Content\\UI", "VeUI\\Content\\Music", "VeUI\\Content\\Movies"]

# Releases directory
if echo:
    print(f"copying releases directory {releases_dir} to {target_releases_dir}")
copy_and_overwrite(releases_dir, target_releases_dir)

# Plugin content directory
shutil.copy(os.path.join(".", "Metaverse.uproject"), staging_dir)

# Binaries directory (dlls)
binary_files = list_dir_files(binaries_dir, "*.dll")
for f in binary_files:
    # Do not copy debugging files, only development or shipping
    if "DebugGame" in f[1]:
        continue

    target_dir = os.path.join(staging_dir, f[0])
    if not os.path.exists(target_dir):
        os.makedirs(target_dir)
    shutil.copy(os.path.join(f[0], f[1]), target_dir)

# Binaries directory (modules)
modules_files = list_dir_files(binaries_dir, "*.modules")
for f in modules_files:
    # Do not copy debugging files, only development or shipping
    if "DebugGame" in f[1]:
        continue

    target_dir = os.path.join(staging_dir, f[0])
    if not os.path.exists(target_dir):
        os.makedirs(target_dir)
    shutil.copy(os.path.join(f[0], f[1]), target_dir)

# Binaries directory (target)
modules_files = list_dir_files(binaries_dir, "*.target")
for f in modules_files:
    # Do not copy debugging files, only development or shipping
    if "DebugGame" in f[1]:
        continue

    target_dir = os.path.join(staging_dir, f[0])
    if not os.path.exists(target_dir):
        os.makedirs(target_dir)
    shutil.copy(os.path.join(f[0], f[1]), target_dir)

# List of all plugin directories required to build new UGC.
for plugin_dir in plugin_dirs:
    # Filter out all non-VeVerse plugins.
    if not plugin_dir.startswith("Ve"):
        continue

    # Source plugin directory path
    plugin_path = os.path.join(plugins_dir, plugin_dir)
    # Target plugin directory
    target_plugin_dir = os.path.join(target_plugins_dir, plugin_dir)

    if not os.path.exists(target_plugin_dir):
        if echo:
            print(f"creating directory {target_plugin_dir}")
        os.makedirs(target_plugin_dir)

    # Content path of the current plugin
    plugin_content_path = os.path.join(plugin_path, "Content")
    # Resources path of the current plugin
    plugin_resources_path = os.path.join(plugin_path, "Resources")
    # Binaries directory path of the current plugin
    plugin_binaries_path = os.path.join(plugin_path, "Binaries")

    # Plugin descriptor file list (should contain a single file)
    descriptor_files = list_dir_files(plugin_path, "*.uplugin")
    for f in descriptor_files:
        if echo:
            print(f"copying descriptor file {os.path.join(f[0], f[1])} to {target_plugin_dir}")
        shutil.copy(os.path.join(f[0], f[1]), target_plugin_dir)

    # Plugin resources directory
    copy_and_overwrite(plugin_resources_path, os.path.join(target_plugin_dir, "Resources"))

    # Plugin content directory (maps)
    asset_files = list_dir_files(plugin_content_path, "*.umap")
    for f in asset_files:
        if not copy_files(f, skip_content_dir):
            break

    # Plugin content directory (assets)
    asset_files = list_dir_files(plugin_content_path, "*.uasset")
    for f in asset_files:
        if not copy_files(f, skip_content_dir):
            break

    # Plugin content directory
    asset_files = list_dir_files(plugin_content_path, "*.uexp")
    for f in asset_files:
        if not copy_files(f, skip_content_dir):
            break

    # Plugin binaries directory (dlls)
    binary_files = list_dir_files(plugin_path, "*.dll")
    for f in binary_files:
        # Do not copy debugging files, only development or shipping
        if "DebugGame" in f[1]:
            continue

        target_dir = os.path.join(staging_dir, f[0])
        if not os.path.exists(target_dir):
            if echo:
                print(f"creating directory {target_dir}")
            os.makedirs(target_dir)
        if echo:
            print(f"copying modules file {os.path.join(f[0], f[1])} to {target_dir}")
        shutil.copy(os.path.join(f[0], f[1]), target_dir)

    # Plugin binaries directory (modules)
    modules_files = list_dir_files(plugin_path, "*.modules")
    for f in modules_files:
        # Do not copy debugging files, only development or shipping
        if "DebugGame" in f[1]:
            continue

        target_dir = os.path.join(staging_dir, f[0])
        if not os.path.exists(target_dir):
            os.makedirs(target_dir)
        if echo:
            print(f"copying modules file {os.path.join(f[0], f[1])} to {target_dir}")
        shutil.copy(os.path.join(f[0], f[1]), target_dir)

target_zip_name = "MetaverseSDK"

if echo:
    print(f'creating archive {os.path.join(staging_dir, target_zip_name)}.zip')
shutil.make_archive(target_zip_name, 'zip', staging_dir)
