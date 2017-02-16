@echo off

set NDK_ROOT=d:\DevTools\android-ndk\
set ANT_ROOT=d:\DevTools\apache-ant\bin\
set ANDROID_SDK_ROOT=d:\DevTools\android-sdk\android-sdk-windows

cls

xcopy .\..\iwin\Classes .\app\jni\Classes\ /Y/S

cocos compile -m release -p android --android-studio --app-abi armeabi-v7a
