cmake \
    -S . \
    -B build.macOS \
    -G "Xcode" \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.10 \
    -DBUILD_APS_DEMO=TRUE