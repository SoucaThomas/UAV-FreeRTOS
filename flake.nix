{
  description = "UAV-FreeRTOS development environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            # ARM embedded toolchain
            gcc-arm-embedded

            # Build tools
            gnumake
            clang-tools # clang-format

            # Flash & debug
            openocd
            stlink
            dfu-util

            # Ground station
            bun
          ];

          shellHook = ''
            echo "UAV-FreeRTOS dev environment loaded"
            echo "  ARM GCC:   $(arm-none-eabi-gcc --version | head -1)"
            echo "  OpenOCD:   $(openocd --version 2>&1 | head -1)"
            echo "  Make:      $(make --version | head -1)"
          '';
        };
      });
}
