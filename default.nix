{ lib
, gcc-arm-embedded
, gnumake
, cmake
, stdenv
, picotool
, python39
}:

stdenv.mkDerivation {
  name = "slab-firmware";
  src = ./.;

  nativeBuildInputs = [ cmake gnumake gcc-arm-embedded picotool ];
  buildInputs = [ python39 ];

  cmakeFlags = [
    "-DCMAKE_BUILD_TYPE=Release"
    "-DPICO_PLATFORM=rp2350"
    "-DCMAKE_C_COMPILER=${gcc-arm-embedded}/bin/arm-none-eabi-gcc"
    "-DCMAKE_CXX_COMPILER=${gcc-arm-embedded}/bin/arm-none-eabi-g++"
  ];

  installPhase = ''
    mkdir -p $out
    cp {*.bin,*.elf,*.uf2,*.elf.map,*.dis} $out
  '';

  meta = with lib; {
    homepage = "https://github.com/headblockhead/slab-firmware";
    description = "Firmware for the slab keyboard.";
    platforms = with platforms; linux;
  };
}

