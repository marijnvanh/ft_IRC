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
          drv = import ./default.nix {
            nixpkgs = import nixpkgs { system = system; };
          };
        in { ft-irc = drv; });

      defaultPackage = forAllSystems (system: self.packages."${system}".ft-irc);

      defaultApp = forAllSystems (system: {
        type = "app";
        program = "${self.defaultPackage."${system}"}/bin/ft_IRC";
      });
    };
}
