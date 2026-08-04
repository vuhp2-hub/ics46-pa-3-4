{
  inputs.flake-parts.url = "github:hercules-ci/flake-parts";
  inputs.nixpkgs.url = "https://nixos.org/channels/nixos-unstable/nixexprs.tar.xz";
  outputs =
    inputs:
    inputs.flake-parts.lib.mkFlake { inherit inputs; } {
      systems = [ "x86_64-linux" ];
      perSystem = { pkgs, ... }: {
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "pa3-4";
          version = "0.0.1";
          src = ./.;

          nativeBuildInputs = [ pkgs.libgcc ];

          buildPhase = "g++ -std=c++20 -O2 -Wall -Wextra -g  -fsanitize=address,undefined driver_test.cpp -o pa3-4test";
          installPhase = ''
            mkdir -p $out/bin
            cp pa3-4test $out/bin
          '';
        };
      };
    };
}
