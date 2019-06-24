#!/bin/sh

cd hijack/jni
ndk-build NDK_DEBUG=1
cd ../..

cd instruments
cd base/jni
ndk-build
cd ../..

cd example/jni
ndk-build
cd ../..

cd ..

