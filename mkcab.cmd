set STAMPINF_DATE=*
set STAMPINF_VERSION=*

stampinf -f zero_dummy.inf -a NTx86,NTamd64,NTia64,NTARM || goto :eof

pushd ..

inf2cat /driver:zero /os:XP_X86,2000,XP_X64,Vista_X86,Vista_X64,7_X86,7_X64,Server2003_X86,Server2003_X64,Server2003_IA64,Server2008_X86,Server2008_X64,Server2008_IA64,Server2008R2_X64,Server2008R2_IA64 || goto :eof

if exist zero\cab\zero.cab del zero\cab\zero.cab

cabarc -p -r n zero\cab\zero.cab zero\zero_dummy.inf zero\zero_dummy.cat zero\i386\zero.sys zero\amd64\zero.sys zero\ia64\zero.sys zero\arm\zero.sys || goto :eof

popd

signtool sign /a /v /n "Lagerkvist Teknisk R†dgivning i Bor†s HB" /d "Zero and Random device driver" /du "http://ltr-data.se" /ac "z:\kod\cert\GlobalSign Root CA.crt" /t "http://timestamp.verisign.com/scripts/timestamp.dll" cab\zero.cab || goto :eof
