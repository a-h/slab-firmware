# Slab Firmware
The full firmware and downloads for the [Slab keyboard project](https://github.com/headblockhead/slab).

Downloads are in the releases section.

## Building

This project uses [Nix](https://nixos.org), run `nix develop` for a development environmnt, `nix build '.?submodules=1#slab-firmware'` to build the firmware and use `nix flake check` to check tests are passing.

## Tasks

### Upload-Release
Directory: ./build
Requires: build-release

Builds, then copies the firmware with debug optimisations to the RP2040.

```bash
export PICO_DIR=`findmnt -S LABEL=RPI-RP2 -o TARGET -fn`
cp ./slab.uf2 $PICO_DIR
```

### Upload-Debug
Directory: ./build
Requires: build-debug

Builds, then copies the firmware with speed optimisations to the RP2040.

```bash
export PICO_DIR=`findmnt -S LABEL=RPI-RP2 -o TARGET -fn`
cp ./slab.uf2 $PICO_DIR
```

### Build-Release
Directory: ./build

Builds the keyboard firmware optimized for size.

```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4 slab
```

### Build-Debug
Directory: ./build

Builds the keyboard firmware with development outputs.

```bash
cmake -DCMAKE_BUILD_TYPE=Debug .. 
make -j4 slab
cp compile_commands.json ../ # Copies the autocomplete information for ccls.
```

### Clean-Development

Resets development files for a clean build.

```bash
rm -rf ./build
mkdir build
```

### Initialize-Development

Fetches submodules and creates the build folder.

```bash
git submodule update --init --recursive
mkdir -p build
```
