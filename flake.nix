{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = nixpkgs.legacyPackages.${system};

        projectName =
          let
            cmakeLists =
              if builtins.pathExists ./CMakeLists.txt then builtins.readFile ./CMakeLists.txt else "";
            matches = builtins.filter builtins.isList (builtins.split ''project\(([A-Za-z0-9_-]+)'' cmakeLists);
            name = if matches == [ ] then null else builtins.elemAt (builtins.head matches) 0;
          in
          if name == null then "app" else name;

        # Kernelspecs visible to Jupyter via JUPYTER_PATH.
        # octave-kernel is a kernel definition set (launcher/definition/sizedLogo),
        # not a Python package - its definition's argv points at a self-contained
        # launcher that brings its own Python env with octave_kernel and Octave on PATH.
        jupyterKernels = pkgs.jupyter-kernel.create {
          definitions = {
            octave = pkgs.octave-kernel.definition;
          };
        };
      in
      {
        packages = rec {
          app = pkgs.stdenv.mkDerivation {
            pname = projectName;
            version = "0.1.0";
            src = ./.;

            nativeBuildInputs = with pkgs; [
              cmake
              ninja
              pkg-config
            ];

            buildInputs = with pkgs; [
              eigen
              stb
            ];
          };
          default = app;
        };

        devShells.default = pkgs.mkShell {
          # bash (not bashInteractive) breaks metakernel-based Jupyter kernels:
          # they spawn bash via pexpect and hang waiting for a readline prompt.
          packages = with pkgs; [
            bashInteractive
            ccache
            clang-tools
            cmake
            gdb
            ninja
            pkg-config

            eigen
            stb

            (python3.withPackages (p: with p; [ jupyterlab ]))
            octave
            gnuplot
            ghostscript
          ];

          env = {
            CMAKE_CXX_COMPILER_LAUNCHER = "ccache";
            CMAKE_GENERATOR = "Ninja";
            JUPYTER_PATH = jupyterKernels;
            FONTCONFIG_FILE = pkgs.makeFontsConf {
              fontDirectories = [ pkgs.dejavu_fonts ];
            };
          };
        };
      }
    );
}
