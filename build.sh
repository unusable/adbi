#!/bin/sh

pushd hijack/jni
ndk-build NDK_DEBUG=1
popd

pushd instruments/base/jni
ndk-build
popd

pushd instruments/example/jni
ndk-build
popd


