Synchronet-Linux C/C++ Source Code Archive (09/18/14)

This archive contains a snap-shot of all the source code and library files
necessary for a successful Linux build of the following Synchronet projects
as of 09/18/14:

Project Directory    Build Command
3rdp                 tar --exclude=*output.txt -czvf ../lib-linux-x64.tgz *
src/sbbs3            gmake RELEASE=1
src/sbbs3/scfg       gmake RELEASE=1
src/sbbs3/install    gmake
src/sbbs3/umonitor   gmake RELEASE=1
src/sbbs3/uedit      gmake RELEASE=1

Builds verified on Thu Sep 18 2014 06:25 am
cvs - Linux 2.6.32-5-amd64 x86_64

For more details, see http://wiki.synchro.net/dev:source
and http://wiki.synchro.net/install:nix
