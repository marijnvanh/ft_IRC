{ pkgs ? import <nixpkgs> { } }:

pkgs.mkShell {
  buildInputs = [

    pkgs.cmake
    pkgs.clang
    pkgs.gtest
    pkgs.python3

  ];
}
