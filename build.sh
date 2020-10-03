#!/bin/bash

# Exit with an error code if any of the commands fail
set -e
trap 'last_command=$current_command; current_command=$BASH_COMMAND' DEBUG
trap 'echo "\"${last_command}\" command filed with exit code $?."' EXIT

# Compile and deploy custom libcurl if a PKGBUILD url is set
if [[ ! -z $LIBCURL_PKGBUILD_URL ]]; then
    curdir=$(pwd)
    mkdir switch-libcurl
    chown ${LIBCURL_BUILD_USER} switch-libcurl
    cd switch-libcurl
    wget ${LIBCURL_PKGBUILD_URL}
    sudo -u ${LIBCURL_BUILD_USER} dkp-makepkg
    cp -r pkg/switch-curl/opt/devkitpro/portlibs/switch/* /opt/devkitpro/portlibs/switch
    cd ${curdir}
fi

# Run build
make -j NIGHTLY_BUILD=`git rev-parse --short "$GITHUB_SHA"`
