{
  description = "A very basic flake";

  inputs.nixpkgs.url = github:NixOS/nixpkgs/nixos-23.05;


  outputs = { self, nixpkgs }: {

    defaultPackage.x86_64-linux =
      # Notice the reference to nixpkgs here.
      with import nixpkgs { system = "x86_64-linux"; };
      stdenv.mkDerivation {
        name = "mingl";
        src = self;
        nativeBuildInputs = [
          clang
          cmake
        ];
        buildInputs = [
          libGL
          xorg.libX11
          xorg.libXrandr
          xorg.libXcursor
          xorg.libXinerama
          xorg.libXi
          pkg-config
        ];
        buildPhase = "cmake --build build";
        installPhase = "echo install";
      };

  };
}
