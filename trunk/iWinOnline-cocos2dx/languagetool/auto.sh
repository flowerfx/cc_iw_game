cd "$(dirname "$0")"
java -jar lang_gen_tool.jar lang.xlsx
mv en ../iwin/Resources/iWin/langs/en
mv vi ../iwin/Resources/iWin/langs/vi