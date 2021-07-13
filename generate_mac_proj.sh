cmake \
    -S . \
    -B .build.mac \
    -G "Xcode" \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.10 \
    -DBUILD_APS_DEMO=TRUE