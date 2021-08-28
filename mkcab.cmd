stampinf -d * -v * -f win2k\zero_dummy.inf -a NTx86 || goto :eof
stampinf -d * -v * -f winnet\zero_dummy.inf -a NTx86,NTamd64,NTia64 || goto :eof
stampinf -d * -v * -f win7\zero_dummy.inf -a NTx86,NTamd64,NTia64,NTARM,NTARM64 || goto :eof

inf2cat /driver:win2k /os:XP_X86,2000 || goto :eof
inf2cat /driver:winnet /os:XP_X86,2000,XP_X64,Vista_X86,Vista_X64,7_X86,7_X64,Server2003_X86,Server2003_X64,Server2003_IA64,Server2008_X86,Server2008_X64,Server2008_IA64,Server2008R2_X64,Server2008R2_IA64,8_X86,8_X64,8_ARM,10_RS3_X86,10_RS3_X64,10_RS3_ARM64 || goto :eof
inf2cat /driver:win7 /os:XP_X86,2000,XP_X64,Vista_X86,Vista_X64,7_X86,7_X64,Server2003_X86,Server2003_X64,Server2003_IA64,Server2008_X86,Server2008_X64,Server2008_IA64,Server2008R2_X64,Server2008R2_IA64,8_X86,8_X64,8_ARM,10_RS3_X86,10_RS3_X64,10_RS3_ARM64 || goto :eof

if exist cab\zero.cab del cab\zero.cab

cabarc -p -r n cab\zero.cab win2k\zero_dummy.inf win2k\*.sys win2k\*.pdb winnet\zero_dummy.inf winnet\*.sys winnet\*.pdb win7\zero_dummy.inf win7\*.sys win7\*.pdb || goto :eof

signtool sign /a /v /n "Lagerkvist Teknisk R†dgivning i Bor†s HB" /d "Zero and Random device driver" /du "http://ltr-data.se" /ac "z:\kod\cert\GlobalSign Root CA.crt" /tr "http://sha256timestamp.ws.symantec.com/sha256/timestamp" cab\zero.cab || goto :eof
