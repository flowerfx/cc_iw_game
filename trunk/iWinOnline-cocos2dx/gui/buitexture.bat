@echo off

set RUN_DIR=.
set MYAPP_LIBS=%RUN_DIR%
set MYAPP_CLASSPATH=%RUN_DIR%

set MYAPP_CLASSPATH=%MYAPP_CLASSPATH%;%MYAPP_LIBS%\*


start /D %RUN_DIR% /B C:\Java\jre1.8\bin\java -Xms512m -Xmx1024m -cp %MYAPP_CLASSPATH% vn.mecorp.iwin.AssetGenerator "texture_config.json"

goto end

:end
