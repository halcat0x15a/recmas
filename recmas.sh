#!/bin/bash

getopts "ir:w:" OPTS

case "$OPTS" in
    'i' )
	$ANDROID_NDK_HOME/ndk-build -B
	$ANDROID_HOME/platform-tools/adb push libs/armeabi-v7a/recmas /data/local/tmp
	;;
    'r' )
	$ANDROID_HOME/platform-tools/adb shell "/data/local/tmp/recmas -r > /data/local/tmp/$OPTARG"
	;;
    'w' )
	$ANDROID_HOME/platform-tools/adb shell "/data/local/tmp/recmas -w < /data/local/tmp/$OPTARG"
	;;
esac
