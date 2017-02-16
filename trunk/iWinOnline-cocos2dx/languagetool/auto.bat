@echo off

java -jar lang_gen_tool.jar lang.xlsx

move ./en ../iwin/Resources/iWin/langs/en
move ./vi ../iwin/Resources/iWin/langs/vi