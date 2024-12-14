# Slab Firmware

[![Nix Flake](https://img.shields.io/badge/NIX%20FLAKE-5277C3.svg?logo=NixOS&logoColor=white)](https://nixos.org) [![xc compatible](https://xcfile.dev/badge.svg)](https://xcfile.dev) [![Hackatime](https://waka.hackclub.com/api/badge/U07BMNNGUN9/interval:any/project:slab-firmware)](https://waka.hackclub.com)

The full firmware and downloads for the [Slab keyboard project](https://github.com/headblockhead/slab).

Downloads are in the releases section.

## Building

This project uses [Nix](https://nixos.org), run `nix develop` for a development environment, and `nix build` to build the firmware.

## Tasks

> [!IMPORTANT]
> You must be in the `nix develop` shell for these tasks to work!

### upload
Directory: ./build
Requires: build

Builds and uploads the firmware to the RP2040.

```bash
export PICO_DIR=`findmnt -S LABEL=RPI-RP2 -o TARGET -fn`
cp ./slab.uf2 $PICO_DIR
```

### upload-dbg
Directory: ./build
Requires: build-dbg

Builds and uploads debug firmware to the RP2040.

```bash
export PICO_DIR=`findmnt -S LABEL=RPI-RP2 -o TARGET -fn`
cp ./slab.uf2 $PICO_DIR
```

### build
Directory: ./build

```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j $(nproc)
```

### build-dbg
Directory: ./build

Builds the keyboard firmware with development outputs.

```bash
cmake -DCMAKE_BUILD_TYPE=Debug .. 
cmake --build . -j $(nproc)
cp compile_commands.json ../ # Copies the autocomplete information for ccls.
```

### clean

Deletes the contents of the build directory.

```bash
rm -rf ./build
mkdir build
```

### fetch-submodules

Fetches submodules if not already present.

```bash
git submodule update --init --recursive
```
