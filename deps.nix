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
  librg = pkgs.fetchFromGitHub {
    owner = "zpl-c";
    repo = "librg";
    rev = "b0b3d502d1271c36edb76e2b55342af52381d597";
    sha256 = "sha256-gM32FpckR1jRaC498QQ0UrircR1FrBbVVr+e7yEmsC8=";
  };
  enet = pkgs.fetchFromGitHub {
    owner = "lsalzman";
    repo = "enet";
    rev = "8be2368a8001f28db44e81d5939de5e613025023";
    sha256 = "sha256-YaXdgZIIk1kfDTriKmnEBxRp8yxub3OYfvF0WJl52Ws=";
  };
in
{
  setup_script = ''
    mkdir -p deps
    cp -r ${flecs} deps/flecs
    cp -r ${raylib} deps/raylib
    mkdir -p deps/clay
    cp -r ${clay} deps/clay/clay
    cp -r ${librg} deps/librg
    cp -r ${enet} deps/enet
  '';
}
