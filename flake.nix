{
  description = "Tools for developing and building slab-firmware";
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";

    squirrel.url = "github:headblockhead/squirrel/dev";
    pico_pca9555.url = "github:headblockhead/pico-pca9555";
    pico-ssd1306.url = "github:headblockhead/pico-ssd1306";
  };
  outputs = { nixpkgs, flake-utils, squirrel, pico_pca9555, pico-ssd1306, ... }:
    flake-utils.lib.eachDefaultSystem
      (system:
        let
          pkgs = import nixpkgs {
            inherit system;
            overlays = [
              (final: prev: {
                pico-sdk = prev.pico-sdk.override {
                  withSubmodules = true;
                };
              })
            ];
          };
        in
        rec {
          devShells.default = pkgs.mkShell {
            buildInputs = with pkgs; [
            ];
          };
          packages.slab-firmware = pkgs.stdenv.mkDerivation {
            name = "slab-firmware";
            src = ./.;

            nativeBuildInputs = with pkgs; [
              cmake
              gcc-arm-embedded
              picotool
              python39

              pkg-config
            ];
            cmakeFlags = [
              "-DCMAKE_C_COMPILER=${pkgs.gcc-arm-embedded}/bin/arm-none-eabi-gcc"
              "-DCMAKE_CXX_COMPILER=${pkgs.gcc-arm-embedded}/bin/arm-none-eabi-g++"
            ];
            env = {
              PICO_SDK_PATH = "${pkgs.pico-sdk}/lib/pico-sdk";
              SQUIRREL_PATH = "${squirrel.outputs.packages.${system}.squirrel.src}";
              PCA9555_PATH = "${pico_pca9555.outputs.packages.${system}.pico_pca9555.src}";
              SSD1306_PATH = "${pico-ssd1306.outputs.packages.${system}.pico-ssd1306.src}";
            };
            installPhase = ''
              runHook preInstall
              mkdir -p $out
              cp right/{*.bin,*.elf,*.uf2,*.elf.map,*.dis} $out
              runHook postInstall
            '';
          };
          packages.default = packages.slab-firmware;
        }
      );
}
