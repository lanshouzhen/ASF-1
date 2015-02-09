"; Windows 2000, XP, Vista and 7 (x32 and x64) setup file for Atmel CDC Devices\r\n"
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
"\r\n"
"; Copyright (c) 2000 Microsoft Corporation\r\n"
"; Copyright (C) 2000-2014 ATMEL, Inc.\r\n"
"\r\n"
"[Version] \r\n"
"Signature=\"$Windows NT$\" \r\n"
"Class=Ports \r\n"
"ClassGuid={4D36E978-E325-11CE-BFC1-08002BE10318} \r\n"
"\r\n"
"Provider=\%ATMEL\% \r\n"
"LayoutFile=layout.inf \r\n"
"DriverVer=10\/15\/1999,5.0.2153.1 \r\n"
"\r\n"
";----------------------------------------------------------\r\n"
"; Targets\r\n"
";----------------------------------------------------------\r\n"
"[Manufacturer] \r\n"
"\%ATMEL\%=ATMEL, NTamd64 \r\n"
"\r\n"
"[ATMEL] \r\n"
"\%ATMEL_CDC_XPLAINED\%=Reader, USB\\VID_03EB&PID_2122 \r\n"
"\%ATMEL_CDC_SFW_EXAMPLE\%=Reader, USB\\VID_03EB&PID_2307 \r\n"
"\%ATMEL_CDC_EVK1XXX\%=Reader, USB\\VID_03EB&PID_2310 \r\n"
"\%ATMEL_CDC_ASF_EXAMPLE\%=Reader, USB\\VID_03EB&PID_2404\r\n"
"\%ATMEL_CDC_ASF_COMPOSITE_EXAMPLE2\%=Reader, USB\\VID_03EB&PID_2421&MI_00\r\n"
"\%ATMEL_CDC_ASF_COMPOSITE_EXAMPLE4\%=Reader, USB\\VID_03EB&PID_2424&MI_00\r\n"
"\%ATMEL_CDC_ASF_EXAMPLE2_COM1\%=Reader, USB\\VID_03EB&PID_2425&MI_00\r\n"
"\%ATMEL_CDC_ASF_EXAMPLE2_COM2\%=Reader, USB\\VID_03EB&PID_2425&MI_02\r\n"
"\%ATMEL_CDC_ASF_EXAMPLE3_COM1\%=Reader, USB\\VID_03EB&PID_2426&MI_00\r\n"
"\%ATMEL_CDC_ASF_EXAMPLE3_COM2\%=Reader, USB\\VID_03EB&PID_2426&MI_02\r\n"
"\%ATMEL_CDC_ASF_EXAMPLE3_COM3\%=Reader, USB\\VID_03EB&PID_2426&MI_04\r\n"
"\%ATMEL_CDC_ASF_EXAMPLE3_COM4\%=Reader, USB\\VID_03EB&PID_2426&MI_06\r\n"
"\%ATMEL_CDC_ASF_EXAMPLE3_COM5\%=Reader, USB\\VID_03EB&PID_2426&MI_08\r\n"
"\%ATMEL_CDC_ASF_EXAMPLE3_COM6\%=Reader, USB\\VID_03EB&PID_2426&MI_0A\r\n"
"\%ATMEL_CDC_ASF_EXAMPLE3_COM7\%=Reader, USB\\VID_03EB&PID_2426&MI_0C\r\n"
"\r\n"
"[ATMEL.NTamd64]\r\n"
"\%ATMEL_CDC_XPLAINED\%=DriverInstall, USB\\VID_03EB&PID_2122 \r\n"
"\%ATMEL_CDC_SFW_EXAMPLE\%=DriverInstall, USB\\VID_03EB&PID_2307 \r\n"
"\%ATMEL_CDC_EVK1XXX\%=DriverInstall, USB\\VID_03EB&PID_2310 \r\n"
"\%ATMEL_CDC_ASF_EXAMPLE\%=DriverInstall, USB\\VID_03EB&PID_2404 \r\n"
"\%ATMEL_CDC_ASF_COMPOSITE_EXAMPLE2\%=DriverInstall, USB\\VID_03EB&PID_2421&MI_00\r\n"
"\%ATMEL_CDC_ASF_COMPOSITE_EXAMPLE4\%=DriverInstall, USB\\VID_03EB&PID_2424&MI_00\r\n"
"\%ATMEL_CDC_ASF_EXAMPLE2_COM1\%=DriverInstall, USB\\VID_03EB&PID_2425&MI_00\r\n"
"\%ATMEL_CDC_ASF_EXAMPLE2_COM2\%=DriverInstall, USB\\VID_03EB&PID_2425&MI_02\r\n"
"\%ATMEL_CDC_ASF_EXAMPLE3_COM1\%=DriverInstall, USB\\VID_03EB&PID_2426&MI_00\r\n"
"\%ATMEL_CDC_ASF_EXAMPLE3_COM2\%=DriverInstall, USB\\VID_03EB&PID_2426&MI_02\r\n"
"\%ATMEL_CDC_ASF_EXAMPLE3_COM3\%=DriverInstall, USB\\VID_03EB&PID_2426&MI_04\r\n"
"\%ATMEL_CDC_ASF_EXAMPLE3_COM4\%=DriverInstall, USB\\VID_03EB&PID_2426&MI_06\r\n"
"\%ATMEL_CDC_ASF_EXAMPLE3_COM5\%=DriverInstall, USB\\VID_03EB&PID_2426&MI_08\r\n"
"\%ATMEL_CDC_ASF_EXAMPLE3_COM6\%=DriverInstall, USB\\VID_03EB&PID_2426&MI_0A\r\n"
"\%ATMEL_CDC_ASF_EXAMPLE3_COM7\%=DriverInstall, USB\\VID_03EB&PID_2426&MI_0C\r\n"
"\r\n"
";----------------------------------------------------------\r\n"
"; Windows 2K, XP, and Vista32\r\n"
";----------------------------------------------------------\r\n"
"[Reader_Install.NTx86] \r\n"
"\r\n"
"\r\n"
"[DestinationDirs] \r\n"
"DefaultDestDir=12 \r\n"
"Reader.NT.Copy=12 \r\n"
"\r\n"
"[Reader.NT] \r\n"
"include=mdmcpq.inf\r\n"
"CopyFiles=Reader.NT.Copy \r\n"
"AddReg=Reader.NT.AddReg \r\n"
"\r\n"
"[Reader.NT.Copy] \r\n"
"usbser.sys \r\n"
"\r\n"
"[Reader.NT.AddReg]\r\n"
"HKR,,DevLoader,,*ntkern \r\n"
"HKR,,NTMPDriver,,usbser.sys \r\n"
"HKR,,EnumPropPages32,,\"MsPorts.dll,SerialPortPropPageProvider\" \r\n"
"\r\n"
"[Reader.NT.Services] \r\n"
"AddService = usbser, 0x00000002, Service_Inst \r\n"
"\r\n"
"[Service_Inst] \r\n"
"DisplayName = \%Serial.SvcDesc\% \r\n"
"ServiceType = 1 ; SERVICE_KERNEL_DRIVER \r\n"
"StartType = 3 ; SERVICE_DEMAND_START \r\n"
"ErrorControl = 1 ; SERVICE_ERROR_NORMAL \r\n"
"ServiceBinary = \%12\%\\usbser.sys \r\n"
"LoadOrderGroup = Base \r\n"
"\r\n"
"\r\n"
";----------------------------------------------------------\r\n"
"; Vista64\r\n"
";----------------------------------------------------------\r\n"
"\r\n"
"[DriverInstall.NTamd64]\r\n"
"include=mdmcpq.inf\r\n"
"CopyFiles=DriverCopyFiles.NTamd64\r\n"
"AddReg=DriverInstall.NTamd64.AddReg\r\n"
"\r\n"
"[DriverCopyFiles.NTamd64]\r\n"
"usbser.sys,,,0x20\r\n"
"\r\n"
"[DriverInstall.NTamd64.AddReg]\r\n"
"HKR,,DevLoader,,*ntkern\r\n"
"HKR,,NTMPDriver,,usbser.sys\r\n"
"HKR,,EnumPropPages32,,\"MsPorts.dll,SerialPortPropPageProvider\"\r\n"
"\r\n"
"[DriverInstall.NTamd64.Services]\r\n"
"AddService=usbser, 0x00000002, DriverService.NTamd64\r\n"
"\r\n"
"[DriverService.NTamd64]\r\n"
"DisplayName=\%Serial.SvcDesc\%\r\n"
"ServiceType=1\r\n"
"StartType=3\r\n"
"ErrorControl=1\r\n"
"ServiceBinary=\%12\%\\usbser.sys\r\n"
"\r\n"
";----------------------------------------------------------\r\n"
"; String\r\n"
";----------------------------------------------------------\r\n"
"\r\n"
"[Strings] \r\n"
"ATMEL = \"ATMEL, Inc.\" \r\n"
"ATMEL_CDC_XPLAINED = \"XPLAINED Virtual Com Port\"\r\n"
"ATMEL_CDC_SFW_EXAMPLE = \"Communication Device Class SFW example\"\r\n"
"ATMEL_CDC_EVK1XXX = \"EVK1XXX Virtual Com Port\"\r\n"
"ATMEL_CDC_ASF_EXAMPLE = \"Communication Device Class ASF example\"\r\n"
"ATMEL_CDC_ASF_COMPOSITE_EXAMPLE2 = \"Communication Device Class ASF composite example 2\"\r\n"
"ATMEL_CDC_ASF_COMPOSITE_EXAMPLE4 = \"Communication Device Class ASF composite example 4\"\r\n"
"ATMEL_CDC_ASF_EXAMPLE2_COM1 = \"Communication Device Class ASF example2, COM1\"\r\n"
"ATMEL_CDC_ASF_EXAMPLE2_COM2 = \"Communication Device Class ASF example2, COM2\"\r\n"
"ATMEL_CDC_ASF_EXAMPLE3_COM1 = \"Communication Device Class ASF example3, COM1\"\r\n"
"ATMEL_CDC_ASF_EXAMPLE3_COM2 = \"Communication Device Class ASF example3, COM2\"\r\n"
"ATMEL_CDC_ASF_EXAMPLE3_COM3 = \"Communication Device Class ASF example3, COM3\"\r\n"
"ATMEL_CDC_ASF_EXAMPLE3_COM4 = \"Communication Device Class ASF example3, COM4\"\r\n"
"ATMEL_CDC_ASF_EXAMPLE3_COM5 = \"Communication Device Class ASF example3, COM5\"\r\n"
"ATMEL_CDC_ASF_EXAMPLE3_COM6 = \"Communication Device Class ASF example3, COM6\"\r\n"
"ATMEL_CDC_ASF_EXAMPLE3_COM7 = \"Communication Device Class ASF example3, COM7\"\r\n"
"\r\n"
"Serial.SvcDesc = \"USB Serial emulation driver\" \r\n"
"\r\n"
