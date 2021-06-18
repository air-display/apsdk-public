cmake ^
    -S . ^
    -B .build.win.x64 ^
    -G "Visual Studio 16 2019" ^
    -A x64 ^
    -DCMAKE_SYSTEM_VERSION=10.0 ^
    -DBUILD_APS_DEMO=TRUE