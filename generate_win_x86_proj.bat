cmake ^
    -S . ^
    -B .build/win.x86 ^
    -A Win32 ^
    -DCMAKE_SYSTEM_VERSION=10.0 ^
    -DBUILD_APS_DEMO=TRUE