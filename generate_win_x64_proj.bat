cmake ^
    -S . ^
    -B .build.win.x64 ^
    -G "Visual Studio 16 2019" ^
    -A x64 ^
    -DBUILD_APS_DEMO=TRUE