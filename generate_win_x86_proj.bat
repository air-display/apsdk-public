cmake ^
    -S . ^
    -B .build.win.x86 ^
    -G "Visual Studio 16 2019" ^
    -A Win32 ^
    -DBUILD_APS_DEMO=TRUE