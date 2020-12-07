{ nixpkgs ? import <nixpkgs> { } }:
with nixpkgs;
stdenv.mkDerivation rec {
  pname = "ft_IRC";
  version = "0.0.0";

  src = ./.;

  buildInputs = [ pkgs.cmake ];

  configurePhase = ''
    cmake .
  '';

  buildPhase = ''
    make
  '';

  installPhase = ''
    mkdir -p $out/bin
    mv bin/ft_IRC $out/bin
  '';
}
