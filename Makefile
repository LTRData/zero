#
# DO NOT EDIT THIS FILE!!!  Edit .\sources. if you want to add a new source
# file to this component.  This file merely indirects to the real make file
# that is shared by all the driver components of the Windows NT DDK
#

BUILD_DEFAULT=-cegiw -nmake -i

SIGNTOOL="C:\Program Files (x86)\Windows Kits\10\bin\x64\signtool.exe"

!INCLUDE $(NTMAKEENV)\makefile.def

publish: p:\utils\zerodrv.exe p:\utils\zerodrv_source.7z

p:\utils\zerodrv.exe: 7zS2.sfx zero.7zsfxcfg zero.7z
	copy /b 7zS2.sfx + zero.7zsfxcfg + zero.7z p:\utils\zerodrv.exe
	$(SIGNTOOL) sign /a /v /n "Lagerkvist Teknisk R�dgivning i Bor�s HB" /du "http://www.ltr-data.se" /ac "z:\kod\cert\GlobalSign Root CA.crt" /tr "http://sha256timestamp.ws.symantec.com/sha256/timestamp" p:\utils\zerodrv.exe
	xcopy p:\utils\zerodrv.exe z:\ltr-website\ltr-data.se\files\ /d/y

zero.7z:                      win2k\i386\zero.sys win2k\zero.inf winnet\i386\zero.sys winnet\ia64\zero.sys winnet\amd64\zero.sys winnet\zero.inf win7\x86\zero.sys win7\ia64\zero.sys win7\x64\zero.sys win7\arm\zero.sys win7\arm64\zero.sys win7\zero.inf install.cmd run64.exe w32verc.exe Makefile
	7z a zero.7z -m0=LZMA:a=2 win2k\i386\zero.sys win2k\zero.inf winnet\i386\zero.sys winnet\ia64\zero.sys winnet\amd64\zero.sys winnet\zero.inf win7\x86\zero.sys win7\ia64\zero.sys win7\x64\zero.sys win7\arm\zero.sys win7\arm64\zero.sys win7\zero.inf install.cmd run64.exe w32verc.exe

p:\utils\zerodrv_source.7z: zero.7z
	del p:\utils\zerodrv_source.7z
	7z a -r p:\utils\zerodrv_source.7z -m0=PPMd install.cmd Makefile Sources zero.7zsfxcfg zero.c zero.inf
	xcopy p:\utils\zerodrv_source.7z z:\ltr-website\ltr-data.se\files\ /d/y
