echo off
del /a/f/q output_32.pcm
for %%i in (*.PCM) do (
echo %%i
if not exist %%~ni (
REM  mkdir %%~ni
test %%i %%~ni
echo done
)else (
echo exist
)
)

pause
