{ pkgs, ... }:
let
  raylib = pkgs.fetchFromGitHub {
    owner = "raysan5";
    repo = "raylib";
    rev = "1c3f9fa135cafa494af760c4cd0541c9a23cdf60";
    sha256 = "sha256-bJzuF/5UFIHewQFyCDDPrkh6eyEUv4AYAzmkkV/WjjI=";
  };
  flecs = pkgs.fetchFromGitHub {
    owner = "SanderMertens";
    repo = "flecs";
    rev = "fc3b8999905e05b5ebb6562324c8b198d175804a";
    sha256 = "sha256-YC5SvxNP2gFq/tm9YLmCIdDQ1geA4co/l1tteQGPvhU=";
  };
in
{
  setup_script = ''
    mkdir -p deps
    cp -r ${raylib} deps/raylib
    cp -r ${flecs} deps/flecs
  '';
}
