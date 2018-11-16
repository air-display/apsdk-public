@echo off
@echo ++++++++++++++++++++++++++ Building airplay module ++++++++++++++++++++++++++
set performUploadArchives=true 
if "%CI%"=="True" (
    @echo Current build environment is CI system, build and upload the artifacts to maven repository.
    @set repo=http://maven.oa.com/nexus/content/repositories/thirdparty
    @set snapshot_repo=http://maven.oa.com/nexus/content/repositories/thirdparty-snapshots
   ) else (
    @echo Current build environment is not CI system.
    if not exist local-maven-repo (mkdir local-maven-repo)
    @set repo=file:///%~dp0local-maven-repo
    if not exist local-maven-snapshot-repo (mkdir local-maven-snapshot-repo)
    @set snapshot_repo=file:///%~dp0local-maven-snapshot-repo
    )
@gradlew.bat :airplay:build :airplay:uploadArchives