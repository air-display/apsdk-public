#!/usr/bin/env sh

echo ++++++++++++++++++++++++++ Building airplay module ++++++++++++++++++++++++++
if [[ "$CI" = "True" ]]; then
    echo Current build environment is CI system, build and upload the artifacts to maven repository.
    export repo=http://maven.oa.com/nexus/content/repositories/thirdparty
    export snapshot_repo=http://maven.oa.com/nexus/content/repositories/thirdparty-snapshots
    ./gradlew clean :airplay:build :airplay:uploadArchives
else
    echo Current build environment is not CI system.
    ./gradlew clean :airplay:build
fi