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

        # MPI-enabled LIS with the SA-AMG preconditioner (requires gfortran).
        # Build shared so consumers inherit the Fortran runtime and MPI
        # bindings via DT_NEEDED.
        lisSaamg = pkgs.lis.overrideAttrs (old: {
          # -std=gnu17: lis 2.1.11 calls its Fortran callbacks through a
          # K&R-style `void (*)()` cast, which C23 (gcc >= 15 default) rejects.
          configureFlags = (old.configureFlags or [ ]) ++ [
            "--enable-mpi"
            "--enable-saamg"
            "--enable-shared"
            "CFLAGS=-std=gnu17"
            # SA-AMG uses legacy MPI bindings with mixed argument types.
            "FFLAGS=-fallow-argument-mismatch"
            "FCFLAGS=-fallow-argument-mismatch"
          ];
          nativeBuildInputs = (old.nativeBuildInputs or [ ]) ++ [ pkgs.gfortran ];
          buildInputs = (old.buildInputs or [ ]) ++ [ pkgs.openmpi ];
          # libtool links liblis.so with the C compiler; explicitly link the
          # Fortran runtime and legacy MPI bindings used by SA-AMG.
          env = (old.env or { }) // {
            NIX_LDFLAGS = "-L${pkgs.gfortran.cc.lib}/lib -lgfortran -L${pkgs.openmpi}/lib -lmpi_mpifh";
          };
        });

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
          llvmPackages.openmp
          ninja
          pkg-config

          eigen
          lisSaamg
          openmpi
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
              llvmPackages.openmp
              eigen
              lisSaamg
              openmpi
              stb
            ];
          };
          default = app;
          # The overridden LIS, for inspecting/building it directly.
          inherit lisSaamg;
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
