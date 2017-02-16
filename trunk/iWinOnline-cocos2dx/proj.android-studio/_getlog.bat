@echo off

set PROJECT_PATH=%CD%/app
set NDK_ROOT=d:\DevTools\android-ndk\


rem adb logcat | %NDK_ROOT%\ndk-stack -sym %PROJECT_PATH%/obj/local/x86
adb logcat -c
cls
adb logcat
