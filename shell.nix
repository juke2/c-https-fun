{ pkgs ? import <nixpkgs> {} }:
    
pkgs.mkShell {

  nativeBuildInputs = with pkgs.buildPackages; [ 
    gcc
    ccls
  ];
  __USE_XOPEN2K=true;
}