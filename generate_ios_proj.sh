cmake \
    -S . \
    -B .build.ios \
    -G "Xcode" \
    -DBUILD_APS_DEMO=FALSE \
    -DPLATFORM=OS \
    -DCMAKE_TOOLCHAIN_FILE=platforms/ios/ios.toolchain.cmake