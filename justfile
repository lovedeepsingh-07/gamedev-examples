CMAKE_BUILD_TYPE := "Release"

default:
	just -l

init:
	@nix run .#setup

build example-name:
	@mkdir -p build/windows
	@cmake -S . -B build/windows -DEXAMPLE_NAME={{example-name}} -DCMAKE_TOOLCHAIN_FILE=mingw-w64-toolchain.cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE={{CMAKE_BUILD_TYPE}} -DCMAKE_SYSTEM_NAME=Windows -DEXPECTED_BUILD_TESTS=OFF
	@cmake --build ./build/windows
	@cp build/windows/compile_commands.json build/compile_commands.json

run example-name: (build example-name)
	@build/windows/{{example-name}}.exe

runw example-name: (build example-name)
	@wine build/windows/{{example-name}}.exe
