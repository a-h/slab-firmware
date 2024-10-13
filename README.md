# Slab Firmware
The full firmware and downloads for the [Slab keyboard project](https://github.com/headblockhead/slab).

Downloads are in the releases section.

## Building

This project uses [Nix](https://nixos.org), run `nix develop` for a development environmnt, `nix build '.?submodules=1#slab-firmware'` to build the firmware and use `nix flake check` to check tests are passing.

## Tasks

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
cmake --build . -j 16
```

### build-dbg
Directory: ./build

Builds the keyboard firmware with development outputs.

```bash
cmake -DCMAKE_BUILD_TYPE=Debug .. 
cmake --build . -j 16
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
