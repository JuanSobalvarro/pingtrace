
@_default:
    just --list

[group('build')]
@build_dev:
    cmake -S . -B build
    cmake --build build --config Debug

[group('build')]
@build_release:
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build --config Release

[group('run')]
@run:
    echo "Running pingtrace..."
    ./build/Debug/pingtrace

[group('clean')]
@clean:
    rm -rf build

@build_run:
    just build_dev
    just run