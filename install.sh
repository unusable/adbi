#! /bin/bash



pushd instruments/base/jni
ndk-build
popd

pushd instruments/example/jni
ndk-build
adb push ../libs/armeabi-v7a/libexample.so /data/local/tmp/
pop




