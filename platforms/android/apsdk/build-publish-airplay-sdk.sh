#!/usr/bin/env sh

export ANDROID_NDK_HOME=$ANDROIDNDK_LINUX_R16
export JAVA_HOME=$JDK8
export PATH=$JDK8/bin:$PATH
export GRADLE_HOME=/data/rdm/apps/gradle/gradle-4.6
export PATH=$GRADLE_HOME/bin:$PATH

mkdir -p bin
pushd $(pwd)
cd platforms/android/aps-sdk
echo ++++++++++++++++++++++++++ Building airplay module ++++++++++++++++++++++++++
if [[ "$CI" = "True" ]]; then
    echo Current build environment is CI system, build and upload the artifacts to maven repository.
    export repo=http://maven.oa.com/nexus/content/repositories/thirdparty
    export snapshot_repo=http://maven.oa.com/nexus/content/repositories/thirdparty-snapshots
    export performUploadArchives=true
    ./gradlew clean :airplay:build :airplay:uploadArchives
else
    echo Current build environment is not CI system.
    ./gradlew clean :airplay:build
fi

popd
if [ $? = 0 ]; then 
    cp platforms/android/aps-sdk/airplay/build/outputs/aar/* bin
fi

echo Build done with error code $?
exit $?