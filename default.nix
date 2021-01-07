{ nixpkgs ? import <nixpkgs> { } }:
with nixpkgs;
stdenv.mkDerivation rec {
  pname = "ft_IRC";
  version = "0.0.0";

  src = ./.;

  buildInputs = [ pkgs.cmake pkgs.gtest ];

  configurePhase = ''
    cmake . -DPACKAGE_TESTS=ON -DNIX=ON
  '';

  buildPhase = ''
    make
  '';

  installPhase = ''
    mkdir -p $out/bin
    mv bin/* $out/bin
  '';
}
