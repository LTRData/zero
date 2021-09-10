!IF EXIST(Makefile.user)
!INCLUDE Makefile.user
!ENDIF

BUILD_DEFAULT=-cegiw -nmake -i

!IF EXIST($(NTMAKEENV)\makefile.def)
!INCLUDE $(NTMAKEENV)\makefile.def
!ENDIF

zero.7z:                          win2k\i386\zero.sys win2k\zero.inf winnet\i386\zero.sys winnet\ia64\zero.sys winnet\amd64\zero.sys winnet\zero.inf win7\x86\zero.sys win7\ia64\zero.sys win7\x64\zero.sys win7\arm\zero.sys win7\arm64\zero.sys win7\zero.inf install.cmd run64.exe w32verc.exe Makefile
	7z a zero.7z -m0=LZMA:a=2 win2k\i386\zero.sys win2k\zero.inf winnet\i386\zero.sys winnet\ia64\zero.sys winnet\amd64\zero.sys winnet\zero.inf win7\x86\zero.sys win7\ia64\zero.sys win7\x64\zero.sys win7\arm\zero.sys win7\arm64\zero.sys win7\zero.inf install.cmd run64.exe w32verc.exe
