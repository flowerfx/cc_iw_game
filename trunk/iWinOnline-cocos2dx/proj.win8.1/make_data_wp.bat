echo off

pushd "%cd%\.."

set DATA_RAW=iwin\Resources
set DATA_COPY_TO=proj.win8.1\Resources
set DDS_CONVERTER=tools\DDSConverter1.3\bin

echo ================copy data ==================:

xcopy /s/y %DATA_RAW% 				 %DATA_COPY_TO%

echo ================remove un-needed folder data ==================:

del /s /q %DATA_COPY_TO%\iWin\atlas\240320
rmdir /s /q %DATA_COPY_TO%\iWin\atlas\240320
del /s /q %DATA_COPY_TO%\iWin\atlas\320480
rmdir /s /q %DATA_COPY_TO%\iWin\atlas\320480

del /s /q %DATA_COPY_TO%\iWin\frame-sheet\240320
rmdir /s /q %DATA_COPY_TO%\iWin\frame-sheet\240320
del /s /q %DATA_COPY_TO%\iWin\frame-sheet\320480
rmdir /s /q %DATA_COPY_TO%\iWin\frame-sheet\320480

del /s /q %DATA_COPY_TO%\iWin\fonts\240320
rmdir /s /q %DATA_COPY_TO%\iWin\fonts\240320
del /s /q %DATA_COPY_TO%\iWin\fonts\320480
rmdir /s /q %DATA_COPY_TO%\iWin\fonts\320480

echo ================create cache dds folder ==================:

if not exist %DATA_COPY_TO%\iWin\atlas\480800_dds mkdir %DATA_COPY_TO%\iWin\atlas\480800_dds
if not exist %DATA_COPY_TO%\iWin\atlas\640960_dds mkdir %DATA_COPY_TO%\iWin\atlas\640960_dds

if not exist %DATA_COPY_TO%\iWin\frame-sheet\480800_dds mkdir %DATA_COPY_TO%\iWin\frame-sheet\480800_dds
if not exist %DATA_COPY_TO%\iWin\frame-sheet\640960_dds mkdir %DATA_COPY_TO%\iWin\frame-sheet\640960_dds

echo ================create convert dds and save into folder ==================:

FOR  %%f IN (%DATA_COPY_TO%\iWin\atlas\480800\*.png) DO (
	%DDS_CONVERTER%\nvcompress.exe -bc3 -clamp -nomips -alpha %%f %DATA_COPY_TO%\iWin\atlas\480800_dds\%%~nf.png
)
FOR  %%f IN (%DATA_COPY_TO%\iWin\atlas\640960\*.png) DO (
	%DDS_CONVERTER%\nvcompress.exe -bc3 -clamp -nomips -alpha %%f %DATA_COPY_TO%\iWin\atlas\640960_dds\%%~nf.png
)
FOR  %%f IN (%DATA_COPY_TO%\iWin\frame-sheet\480800\*.png) DO (
	%DDS_CONVERTER%\nvcompress.exe -bc3 -clamp -nomips  %%f %DATA_COPY_TO%\iWin\frame-sheet\480800_dds\%%~nf.png
)
FOR  %%f IN (%DATA_COPY_TO%\iWin\frame-sheet\640960\*.png) DO (
	%DDS_CONVERTER%\nvcompress.exe -bc3 -clamp -nomips  %%f %DATA_COPY_TO%\iWin\frame-sheet\640960_dds\%%~nf.png
)

echo ================copy all to png ==================:

REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\480800_dds\background.png %DATA_COPY_TO%\iWin\atlas\480800
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\480800_dds\background2.png %DATA_COPY_TO%\iWin\atlas\480800
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\480800_dds\background3.png %DATA_COPY_TO%\iWin\atlas\480800
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\480800_dds\background4.png %DATA_COPY_TO%\iWin\atlas\480800
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\480800_dds\gamescreen-baucua-screen.png %DATA_COPY_TO%\iWin\atlas\480800
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\480800_dds\gamescreen-cotuong-screen.png %DATA_COPY_TO%\iWin\atlas\480800
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\480800_dds\gamescreen-covua-screen.png %DATA_COPY_TO%\iWin\atlas\480800
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\480800_dds\gamescreen-domino-screen.png %DATA_COPY_TO%\iWin\atlas\480800
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\480800_dds\gamescreen-tienlen-screen.png %DATA_COPY_TO%\iWin\atlas\480800
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\480800_dds\gamescreen-tienlen-screen2.png %DATA_COPY_TO%\iWin\atlas\480800
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\480800_dds\gamescreen-newgui-game-common5.png %DATA_COPY_TO%\iWin\atlas\480800
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\480800_dds\gamescreen-newgui-game-common.png %DATA_COPY_TO%\iWin\atlas\480800
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\480800_dds\gamescreen-newgui-game-common6.png %DATA_COPY_TO%\iWin\atlas\480800
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\480800_dds\gamescreen-newgui-game-common7.png %DATA_COPY_TO%\iWin\atlas\480800
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\480800_dds\gamescreen-newgui-game-common8.png %DATA_COPY_TO%\iWin\atlas\480800
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\480800_dds\gamescreen-newgui-game-common9.png %DATA_COPY_TO%\iWin\atlas\480800

REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\480800_dds\otherscreen.png %DATA_COPY_TO%\iWin\atlas\480800
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\480800_dds\otherscreen4.png %DATA_COPY_TO%\iWin\atlas\480800
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\480800_dds\otherscreen5.png %DATA_COPY_TO%\iWin\atlas\480800
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\480800_dds\otherscreen6.png %DATA_COPY_TO%\iWin\atlas\480800

REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\480800_dds\mainscreen3.png %DATA_COPY_TO%\iWin\atlas\480800
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\480800_dds\mainscreen6.png %DATA_COPY_TO%\iWin\atlas\480800
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\480800_dds\mainscreen4.png %DATA_COPY_TO%\iWin\atlas\480800
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\480800_dds\gamescreen-newgui-game-common4.png %DATA_COPY_TO%\iWin\atlas\480800
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\480800_dds\gamescreen-newgui-game-common5.png %DATA_COPY_TO%\iWin\atlas\480800
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\480800_dds\gamescreen-newgui-game-common6.png %DATA_COPY_TO%\iWin\atlas\480800
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\480800_dds\gamescreen-newgui-game-common7.png %DATA_COPY_TO%\iWin\atlas\480800
del %DATA_COPY_TO%\iWin\atlas\480800_dds\gamescreen-Cards.png
xcopy /s/y %DATA_COPY_TO%\iWin\atlas\480800_dds\*.png %DATA_COPY_TO%\iWin\atlas\480800



REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\640960_dds\background.png %DATA_COPY_TO%\iWin\atlas\640960
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\640960_dds\background2.png %DATA_COPY_TO%\iWin\atlas\640960
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\640960_dds\gamescreen-baucua-screen.png %DATA_COPY_TO%\iWin\atlas\640960
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\640960_dds\gamescreen-cotuong-screen.png %DATA_COPY_TO%\iWin\atlas\640960
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\640960_dds\gamescreen-covua-screen.png %DATA_COPY_TO%\iWin\atlas\640960
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\640960_dds\gamescreen-domino-screen.png %DATA_COPY_TO%\iWin\atlas\640960
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\640960_dds\gamescreen-tienlen-screen.png %DATA_COPY_TO%\iWin\atlas\640960
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\640960_dds\otherscreen.png %DATA_COPY_TO%\iWin\atlas\640960
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\640960_dds\otherscreen6.png %DATA_COPY_TO%\iWin\atlas\640960
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\640960_dds\otherscreen5.png %DATA_COPY_TO%\iWin\atlas\640960
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\640960_dds\otherscreen4.png %DATA_COPY_TO%\iWin\atlas\640960
REM xcopy /s/y %DATA_COPY_TO%\iWin\atlas\640960_dds\mainscreen5.png %DATA_COPY_TO%\iWin\atlas\640960
del %DATA_COPY_TO%\iWin\atlas\640960_dds\gamescreen-Cards.png
xcopy /s/y %DATA_COPY_TO%\iWin\atlas\640960_dds\*.png %DATA_COPY_TO%\iWin\atlas\640960

xcopy /s/y %DATA_COPY_TO%\iWin\frame-sheet\480800_dds\*.png %DATA_COPY_TO%\iWin\frame-sheet\480800
xcopy /s/y %DATA_COPY_TO%\iWin\frame-sheet\640960_dds\*.png %DATA_COPY_TO%\iWin\frame-sheet\640960

del /s /q %DATA_COPY_TO%\iWin\atlas\640960_dds
rmdir /s /q %DATA_COPY_TO%\iWin\atlas\640960_dds
del /s /q %DATA_COPY_TO%\iWin\atlas\480800_dds
rmdir /s /q %DATA_COPY_TO%\iWin\atlas\480800_dds

del /s /q %DATA_COPY_TO%\iWin\frame-sheet\640960_dds
rmdir /s /q %DATA_COPY_TO%\iWin\frame-sheet\640960_dds
del /s /q %DATA_COPY_TO%\iWin\frame-sheet\480800_dds
rmdir /s /q %DATA_COPY_TO%\iWin\frame-sheet\480800_dds

popd