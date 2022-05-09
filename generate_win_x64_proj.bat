cmake ^
    -S . ^
    -B .build/win.x64 ^
    -A x64 ^
    -DCMAKE_SYSTEM_VERSION=10.0 ^
    -DBUILD_APS_DEMO=TRUE