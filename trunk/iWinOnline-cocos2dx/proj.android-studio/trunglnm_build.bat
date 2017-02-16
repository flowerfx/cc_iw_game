@echo trunglnm release build

cls
cocos compile -m debug -p android --android-studio --app-abi armeabi-v7a:x86
