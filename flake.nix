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

        # Shared across compiler shells; the compiler itself comes from each
        # shell's stdenv wrapper (cc/c++), which is what CMake auto-detects.
        # bash (not bashInteractive) breaks metakernel-based Jupyter kernels:
        # they spawn bash via pexpect and hang waiting for a readline prompt.
        devPackages = with pkgs; [
          bashInteractive
          ccache
          clang-tools
          cmake
          gdb
          llvm # llvm-ar/llvm-ranlib, needed by CMake for clang LTO
          ninja
          pkg-config

          eigen
          lis
          stb

          (python3.withPackages (p: with p; [ jupyterlab ]))
          octave
          gnuplot
          ghostscript
        ];

        shellEnv = {
          CMAKE_CXX_COMPILER_LAUNCHER = "ccache";
          CMAKE_GENERATOR = "Ninja";
          JUPYTER_PATH = jupyterKernels;
          FONTCONFIG_FILE = pkgs.makeFontsConf {
            fontDirectories = [ pkgs.dejavu_fonts ];
          };
        };
      in
      {
        packages = rec {
          app = pkgs.clangStdenv.mkDerivation {
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
              lis
              stb
            ];
          };
          default = app;
        };

        # clangStdenv puts the clang wrapper on PATH (cc/c++), so CMake
        # picks clang as default compiler both here and in the app package.
        devShells.default = pkgs.mkShell.override { stdenv = pkgs.clangStdenv; } {
          packages = devPackages;
          env = shellEnv;
        };

        # Same shell with GCC, for compiler comparisons.
        devShells.gcc = pkgs.mkShell.override { stdenv = pkgs.gccStdenv; } {
          packages = devPackages;
          env = shellEnv;
        };
      }
    );
}
