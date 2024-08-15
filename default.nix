{ lib
, gcc-arm-embedded
, gnumake
, cmake
, stdenv
, picotool
, python39
}:

stdenv.mkDerivation {
  pname = "slab-firmware";
  version = "0.0.0";

  #  src = fetchGit {
  #url = "https://github.com/headblockhead/slab-firmware.git";
  #rev = "b9334c65192a429c707232ab27a862fde68ef982";
  #submodules = true;
  #};

  src = ./.;

  nativeBuildInputs = [ cmake gnumake gcc-arm-embedded picotool ];
  buildInputs = [ python39 ];

  cmakeFlags = [
    "-DCMAKE_BUILD_TYPE=Release"
    "-DPICO_PLATFORM=rp2350"
    "-DCMAKE_C_COMPILER=${gcc-arm-embedded}/bin/arm-none-eabi-gcc"
    "-DCMAKE_CXX_COMPILER=${gcc-arm-embedded}/bin/arm-none-eabi-g++"
  ];

  installPhase = ''cp -r . $out'';

  meta = with lib; {
    homepage = "https://github.com/headblockhead/slab-firmware";
    description = "Firmware for the slab keyboard.";
    licencse = licenses.mit;
    platforms = with platforms; linux;
  };
}

