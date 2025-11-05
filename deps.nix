{ pkgs, ... }:
let
  flecs = pkgs.fetchFromGitHub {
    owner = "SanderMertens";
    repo = "flecs";
    rev = "fc3b8999905e05b5ebb6562324c8b198d175804a";
    sha256 = "sha256-YC5SvxNP2gFq/tm9YLmCIdDQ1geA4co/l1tteQGPvhU=";
  };
  raylib = pkgs.fetchFromGitHub {
    owner = "raysan5";
    repo = "raylib";
    rev = "1c3f9fa135cafa494af760c4cd0541c9a23cdf60";
    sha256 = "sha256-bJzuF/5UFIHewQFyCDDPrkh6eyEUv4AYAzmkkV/WjjI=";
  };
  clay = pkgs.fetchFromGitHub {
    owner = "nicbarker";
    repo = "clay";
    rev = "b25a31c1a152915cd7dd6796e6592273e5a10aac";
    sha256 = "sha256-6h1aQXqwzPc4oPuid3RfV7W0WzQFUiddjW7OtkKM0P8=";
  };
in
{
  setup_script = ''
    mkdir -p deps
    cp -r ${flecs} deps/flecs
    cp -r ${raylib} deps/raylib
    mkdir -p deps/clay
    cp -r ${clay} deps/clay/clay
  '';
}
