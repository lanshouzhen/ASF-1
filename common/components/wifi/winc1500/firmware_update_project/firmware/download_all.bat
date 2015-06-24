@ECHO off
SET PORT_NUM=0		
::0 means try to get available port number and if failed will prompt user to enter it
SET FIRMWARE_2B0_PATH=../../../firmware/m2m_aio_2b0.bin		
SET FIRMWARE_3A0_PATH=../../../firmware/m2m_aio_3a0.bin		

echo Mode %1
if "%1" == "UART"  Goto contine_UART
if "%1" == "I2C"   Goto contine_I2C
if "%1" == "OTA"   Goto contine_OTA
	echo Define the mode (I2C/UART/OTA)
	pause
	exit

:contine_I2C
Set  FMode=debug_i2c
set Mode=I2C
goto START

:contine_OTA
Set  FMode=debug_ota
set Mode=OTA
goto START

:contine_UART
Set  FMode=debug_uart
set Mode=UART
goto START

:START
IF %FMode%==debug_ota (echo Creating Image...
cd Tools\image_builder\debug\
@ECHO off
image_builder -no_wait -ota_img ../../../ota_firmware/m2m_ota_2b0.bin -fw_path ../../../firmware/wifi_v111/ASIC_2B0/wifi_firmware.bin -op_path ../../../firmware/m2m_aio_2b0.bin -ate_img ../../../ate_firmware/m2m_ate.bin 
image_builder -no_wait -ota_img  ../../../ota_firmware/m2m_ota_3a0.bin -fw_path ../../../firmware/wifi_v111/ASIC_3A0/wifi_firmware.bin -op_path ../../../firmware/m2m_aio_3a0.bin -ate_img ../../../ate_firmware/m2m_ate.bin 
echo OK
goto END
)
cd Tools\image_downloader\%FMode%
echo Downloading Image...
image_downloader.exe -no_wait -port %PORT_NUM% -fw2b0_path %FIRMWARE_2B0_PATH% -fw3a0_path %FIRMWARE_3A0_PATH%
IF %ERRORLEVEL% NEQ  0 goto END
cd ..\..\gain_builder\%FMode%
echo Downloading Gain Values...
if "%2" == "SAMW25" ( Set GAIN_FILE=-hp ../gain_sheets/samw25_gain_setting.csv
goto DOWNLOAD_GAIN
)
Set GAIN_FILE=-hp ../gain_sheets/default_gain_setting.csv
:DOWNLOAD_GAIN
gain_builder.exe %GAIN_FILE% -no_wait
IF %ERRORLEVEL% NEQ  0 goto END
cd ..\..\..\Tools\root_certificate_downloader\%FMode%
echo Downloading root certificates...
:DOWNLOAD_ROOT_CERT 
setlocal EnableDelayedExpansion
set /a c=0
set seq=
for %%X in (..\binary\*.cer) do (
	set /a c+=1
	@set seq=!seq! %%X
)
root_certificate_downloader.exe -n %c% %seq% -no_wait -port 0
:END
IF %ERRORLEVEL% NEQ  0 ( echo Fail
echo     #######################################################################
echo     ##                                                                   ##
echo     ##                    ########    ###     ####  ##                   ##
echo     ##                    ##         ## ##     ##   ##                   ##
echo     ##                    ##        ##   ##    ##   ##                   ##
echo     ##                    ######   ##     ##   ##   ##                   ##
echo     ##                    ##       #########   ##   ##                   ##
echo     ##                    ##       ##     ##   ##   ##                   ##
echo     ##                    ##       ##     ##  ####  ########             ##
echo     ##                                                                   ##
echo     #######################################################################
pause
exit
)
)

echo OK
echo     #######################################################################
echo     ##                                                                   ##
echo     ##                 ########     ###     ######   ######              ##
echo     ##                 ##     ##   ## ##   ##    ## ##    ##             ##
echo     ##                 ##     ##  ##   ##  ##       ##                   ##
echo     ##                 ########  ##     ##  ######   ######              ##
echo     ##                 ##        #########       ##       ##             ##
echo     ##                 ##        ##     ## ##    ## ##    ##             ##
echo     ##                 ##        ##     ##  ######   ######              ##
echo     ##                                                                   ##
echo     #######################################################################

echo Programming ends successfully
pause
