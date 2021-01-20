{
  description = "ft_IRC";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-20.03";

  outputs = { self, nixpkgs }:
    let
      supportedSystems =
        [ "x86_64-linux" "i686-linux" "aarch64-linux" "x86_64-darwin" ];
      forAllSystems = f:
        nixpkgs.lib.genAttrs supportedSystems (system: f system);
    in {

      packages = forAllSystems (system:
        let
          pkgs = import nixpkgs { system = system; };
          drv = import ./default.nix {
            nixpkgs = pkgs;
          };
        in rec { 
          ft-irc = drv; 
          tests = pkgs.writeScriptBin "tests" ''
            find ${self.defaultPackage.${system}}/bin/tests -type f -exec {} \;
          '';
        });

      defaultPackage = forAllSystems (system: self.packages."${system}".ft-irc);

      defaultApp = forAllSystems (system: {
        type = "app";
        program = "${self.defaultPackage."${system}"}/bin/irc_server";
      });

      apps = forAllSystems (system: {
        tests = {
          type = "app";
          program = "${self.packages.${system}.tests}/bin/tests";
        };
      });
    };
}
