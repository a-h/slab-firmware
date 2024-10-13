{
  description = "Tools for developing and building slab-firmware";
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.05";
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = { nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem
      (system:
        let
          pkgs = import nixpkgs {
            inherit system;
            overlays = [
              (final: prev: {
                picotool = pkgs.callPackage ./picotool.nix {
                  pico-sdk = pkgs.callPackage ./pico-sdk.nix { };
                };
                pico-sdk = pkgs.callPackage ./pico-sdk.nix { };
              })
            ];
          };
        in
        {
          # Development shell (nix develop)
          devShells.default = pkgs.mkShell {
            buildInputs = with pkgs; [
              xc
              cmake
              gcc
              ccls
              gcc-arm-embedded
              openocd
              picotool
              python39
              minicom
              gnumake
              gdb
              git
              cacert
            ];
          };
          # The firmware (nix build)
          packages.slab-firmware = pkgs.callPackage ./default.nix { };
        }
      );
}
