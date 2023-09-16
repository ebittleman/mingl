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
          pkg-config
        ];
        buildInputs = [
          libGL
          xorg.libX11
          xorg.libXrandr
          xorg.libXcursor
          xorg.libXinerama
          xorg.libXi
        ];
        buildPhase = "make -j4";
        installPhase = "mkdir -p $out/bin; install -t $out/bin src/mingl";
      };

  };
}
