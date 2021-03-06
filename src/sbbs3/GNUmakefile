# GNUmakefile

#########################################################################
# Makefile for Synchronet BBS for Unix									#
# For use with GNU make and GNU C Compiler or Borland Kylix C++			#
# @format.tab-size 4, @format.use-tabs true								#
#																		#
# gcc: gmake															#
# Borland (still in testing/debuging stage): gmake bcc=1				#
#																		#
# Optional build targets: dlls, utils, mono, all (default)				#
#########################################################################

# $Id: GNUmakefile,v 1.224 2014/06/23 11:51:42 deuce Exp $

PWD	:=	$(shell pwd)
SRC_ROOT	?=	${PWD}/..
include $(SRC_ROOT)/build/Common.gmake

UTIL_LDFLAGS	:=	$(LDFLAGS)
UTIL_LDFLAGS	+=	$(SMBLIB_LDFLAGS) $(UIFC-MT_LDFLAGS) $(CIOLIB-MT_LDFLAGS) $(XPDEV_LDFLAGS)
CONSOLE_LDFLAGS	+=	$(LDFLAGS) $(SMBLIB_LDFLAGS) $(XPDEV_LDFLAGS)
UTIL_LIBS	+=	$(SMBLIB_LIBS)

ifndef bcc
 ifneq ($(os),sunos)
  LDFLAGS	+=	-lutil
 endif
endif

ifeq ($(os),sunos)    # Solaris
 LDFLAGS += -lnsl -lrt
 SBBS_LIBS	+=	-lkstat
endif

# So far, only QNX has sem_timedwait()
ifeq ($(os),qnx)
 LDFLAGS += -lsocket
endif

ifdef PREFIX
 CFLAGS += -DPREFIX=$(PREFIX)
endif

ifdef USE_DOSEMU
 CFLAGS += -DUSE_DOSEMU
endif

ifdef DONT_BLAME_SYNCHRONET
 CFLAGS += -DDONT_BLAME_SYNCHRONET
endif

ifdef BUILD_JSDOCS
 CFLAGS += -DBUILD_JSDOCS
 SBBS_LIBS += ${UIFC-MT_LDFLAGS} ${UIFC-MT_LIBS} $(CIOLIB-MT_LDFLAGS) ${CIOLIB-MT_LIBS}
 OBJS += $(MTOBJODIR)$(DIRSEP)js_uifc$(OFILE)
 EXTRA_SBBS_DEPENDS += $(CIOLIB-MT) $(UIFCLIB-MT)
endif

ifeq ($(os),linux)
 SBBS_LIBS += -lutil
 ifeq ($(shell test -f /usr/include/sys/capability.h && echo "yes"),yes)
  CFLAGS += -DUSE_LINUX_CAPS
  CON_LIBS += -lcap
 endif
endif

include sbbsdefs.mk
MT_CFLAGS	+=	$(SBBSDEFS)

# Set up LD_RUN_PATH for run-time locating of the .so files
ifdef SBBSDIR
 ifeq ($(os),sunos)
  LD_RUN_PATH	:=	$(SBBSDIR)/exec:$(LIBODIR):$(JSLIBDIR):$(NSPRDIR):$(CRYPTLIBDIR):/opt/sfw/gcc-3/lib
 else
  LD_RUN_PATH	:=	$(SBBSDIR)/exec:$(LIBODIR):$(JSLIBDIR):$(NSPRDIR):$(CRYPTLIBDIR)
 endif
else
 ifeq ($(os),sunos)
  LD_RUN_PATH	:=	$(PWD)/$(LIBODIR):$(JSLIBDIR):$(NSPRDIR):$(CRYPTLIBDIR):/opt/sfw/gcc-3/lib
 else
  LD_RUN_PATH	:=	$(PWD)/$(LIBODIR):$(JSLIBDIR):$(NSPRDIR):$(CRYPTLIBDIR)
 endif
endif
export LD_RUN_PATH

CON_LIBS	+= -lsbbs -lftpsrvr -lwebsrvr -lmailsrvr -lservices
SHLIBOPTS	:=	-shared
ifeq ($(os),darwin)
 MKSHLIB		:=	libtool -dynamic -framework System -lcc_dynamic
 MKSHPPLIB		:=	libtool -dynamic -framework System -lcc_dynamic -lstdc++
 SHLIBOPTS	:=	
else
 ifeq ($(os),sunos)
  MKSHLIB		:=	/usr/ccs/bin/ld -G
  MKSHPPLIB		:=	/usr/ccs/bin/ld -G
  SHLIBOPTS	:=	
 else
  MKSHLIB		:=	$(CC)
  MKSHPPLIB		:=	$(CXX)
 endif
endif

CFLAGS	+=	$(UIFC-MT_CFLAGS) $(XPDEV-MT_CFLAGS) $(SMBLIB_CFLAGS) $(CIOLIB-MT_CFLAGS) $(JS_CFLAGS) $(CRYPT_CFLAGS)
CFLAGS	+=	-I../comio
vpath %.c ../comio
OBJS	+= $(MTOBJODIR)$(DIRSEP)comio_nix$(OFILE)
LDFLAGS +=	$(UIFC-MT_LDFLAGS) $(XPDEV-MT_LDFLAGS) $(SMBLIB_LDFLAGS) $(CIOLIB-MT_LDFLAGS) $(JS_LDFLAGS) $(CRYPT_LDFLAGS)

# Monolithic Synchronet executable Build Rule
$(SBBSMONO): $(MONO_OBJS) $(OBJS)
	@echo Linking $@
	$(QUIET)$(CXX) -o $@ $(LDFLAGS) $(MT_LDFLAGS) $(MONO_OBJS) $(OBJS) $(SBBS_LIBS) $(SMBLIB_LIBS) $(XPDEV-MT_LIBS) $(JS_LIBS) $(CRYPT_LIBS)

# Synchronet BBS library Link Rule
$(SBBS): $(JS_DEPS) $(CRYPT_DEPS) $(OBJS) $(LIBS) $(EXTRA_SBBS_DEPENDS) | $(LIBODIR)
	@echo Linking $@
	$(QUIET)$(MKSHPPLIB) $(LDFLAGS) -o $@ $(OBJS) $(SBBS_LIBS) $(LIBS) $(SHLIBOPTS) $(JS_LIBS) $(CRYPT_LIBS) $(XPDEV-MT_LIBS)

# FTP Server Link Rule
$(FTPSRVR): $(MTOBJODIR)/ftpsrvr.o
	@echo Linking $@
	$(QUIET)$(MKSHLIB) $(LDFLAGS) $(MTOBJODIR)/ftpsrvr.o $(SHLIBOPTS) -o $@

# Mail Server Link Rule
$(MAILSRVR): $(MAIL_OBJS)
	@echo Linking $@
	$(QUIET)$(MKSHLIB) $(LDFLAGS) $(MAIL_OBJS) $(SHLIBOPTS) -o $@

# Web Server Link Rule
$(WEBSRVR): $(WEB_OBJS)
	@echo Linking $@
	$(QUIET)$(MKSHLIB) $(LDFLAGS) $(WEB_OBJS) $(SHLIBOPTS) -o $@

# Services Link Rule
$(SERVICES): $(SERVICE_OBJS)
	@echo Linking $@
	$(QUIET)$(MKSHLIB) $(LDFLAGS) $(SERVICE_OBJS) $(SHLIBOPTS) -o $@

# Synchronet Console Build Rule
$(SBBSCON): $(CON_OBJS) $(SBBS) $(FTPSRVR) $(WEBSRVR) $(MAILSRVR) $(SERVICES)
	@echo Linking $@
	$(QUIET)$(CXX) $(LDFLAGS) $(MT_LDFLAGS) -o $@ $(CON_OBJS) $(CON_LIBS) $(SMBLIB_LIBS) $(XPDEV-MT_LIBS)

# Baja Utility
$(BAJA): $(BAJA_OBJS)
	@echo Linking $@
	$(QUIET)$(CC) $(CONSOLE_LDFLAGS) -o $@ $(BAJA_OBJS) $(SMBLIB_LIBS) $(XPDEV_LIBS)

# UnBaja Utility
$(UNBAJA): $(UNBAJA_OBJS) | $(OBJODIR) $(EXEODIR)
	@echo Linking $@
	$(QUIET)$(CC) $(CONSOLE_LDFLAGS) -o $@ $(UNBAJA_OBJS) $(XPDEV_LIBS) $(UTIL_LIBS)

# Node Utility
$(NODE): $(NODE_OBJS)
	@echo Linking $@
	$(QUIET)$(CC) $(CONSOLE_LDFLAGS) -o $@ $(NODE_OBJS) $(XPDEV_LIBS)

# FIXSMB Utility
$(FIXSMB): $(FIXSMB_OBJS)
	@echo Linking $@
	$(QUIET)$(CC) $(CONSOLE_LDFLAGS) -o $@ $(FIXSMB_OBJS) $(SMBLIB_LIBS) $(XPDEV_LIBS)

# CHKSMB Utility
$(CHKSMB): $(CHKSMB_OBJS)
	@echo Linking $@
	$(QUIET)$(CC) $(CONSOLE_LDFLAGS) -o $@ $(CHKSMB_OBJS) $(SMBLIB_LIBS) $(XPDEV_LIBS)

# SMB Utility
$(SMBUTIL): $(SMBUTIL_OBJS)
	@echo Linking $@
	$(QUIET)$(CC) $(CONSOLE_LDFLAGS) -o $@ $(SMBUTIL_OBJS) $(SMBLIB_LIBS) $(XPDEV_LIBS)

# SBBSecho (FidoNet Packet Tosser)
$(SBBSECHO): $(SBBSECHO_OBJS)
	@echo Linking $@
	$(QUIET)$(CC) $(CONSOLE_LDFLAGS) -o $@ $(SBBSECHO_OBJS) $(SMBLIB_LIBS) $(XPDEV_LIBS)

# SBBSecho Configuration Program
$(ECHOCFG): $(ECHOCFG_OBJS)
	@echo Linking $@
	$(QUIET)$(CC) $(UTIL_LDFLAGS) $(MT_LDFLAGS) -o $@ $(ECHOCFG_OBJS) $(UIFC-MT_LDFLAGS) $(SMBLIB_LIBS) $(UIFC-MT_LIBS) $(CIOLIB-MT_LIBS) $(XPDEV-MT_LIBS)

# ADDFILES
$(ADDFILES): $(ADDFILES_OBJS)
	@echo Linking $@
	$(QUIET)$(CC) $(CONSOLE_LDFLAGS) -o $@ $(ADDFILES_OBJS) $(XPDEV_LIBS)

# FILELIST
$(FILELIST): $(FILELIST_OBJS)
	@echo Linking $@
	$(QUIET)$(CC) $(CONSOLE_LDFLAGS) -o $@ $(FILELIST_OBJS) $(XPDEV_LIBS)

# MAKEUSER
$(MAKEUSER): $(MAKEUSER_OBJS)
	@echo Linking $@
	$(QUIET)$(CC) $(CONSOLE_LDFLAGS) -o $@ $(MAKEUSER_OBJS) $(XPDEV_LIBS)

# JSEXEC
$(JSEXEC): $(JSEXEC_OBJS) $(SBBS)
	@echo Linking $@
	$(QUIET)$(CXX) $(LDFLAGS) $(MT_LDFLAGS) -o $@ $(JSEXEC_OBJS) -lsbbs $(SMBLIB_LIBS) $(UIFC-MT_LIBS) $(CIOLIB-MT_LIBS) $(XPDEV-MT_LIBS)

# ANS2ASC
$(ANS2ASC): $(OBJODIR)/ans2asc.o
	@echo Linking $@
	$(QUIET)$(CC) $(CONSOLE_LDFLAGS) -o $@ $(OBJODIR)/ans2asc.o

# ASC2ANS
$(ASC2ANS): $(OBJODIR)/asc2ans.o
	@echo Linking $@
	$(QUIET)$(CC) $(CONSOLE_LDFLAGS) -o $@ $(OBJODIR)/asc2ans.o

# SEXYZ
$(SEXYZ): $(SEXYZ_OBJS)
	@echo Linking $@
	$(QUIET)$(CC) $(CONSOLE_LDFLAGS) $(MT_LDFLAGS) -o $@ $(SEXYZ_OBJS) $(SMBLIB_LIBS) $(XPDEV-MT_LIBS)

# QWKNODES
$(QWKNODES): $(QWKNODES_OBJS)
	@echo Linking $@
	$(QUIET)$(CC) $(CONSOLE_LDFLAGS) -o $@ $(QWKNODES_OBJS) $(SMBLIB_LIBS) $(XPDEV_LIBS)

# SLOG
$(SLOG): $(SLOG_OBJS)
	@echo Linking $@
	$(QUIET)$(CC) $(CONSOLE_LDFLAGS) -o $@ $(SLOG_OBJS) $(XPDEV_LIBS)

# ALLUSERS
$(ALLUSERS): $(ALLUSERS_OBJS)
	@echo Linking $@
	$(QUIET)$(CC) $(CONSOLE_LDFLAGS) -o $@ $(ALLUSERS_OBJS) $(XPDEV_LIBS)

# DELFILES
$(DELFILES): $(DELFILES_OBJS)
	@echo Linking $@
	$(QUIET)$(CC) $(CONSOLE_LDFLAGS) -o $@ $(DELFILES_OBJS) $(XPDEV_LIBS)

# DUPEFIND
$(DUPEFIND): $(DUPEFIND_OBJS)
	@echo Linking $@
	$(QUIET)$(CC) $(CONSOLE_LDFLAGS) -o $@ $(DUPEFIND_OBJS) $(SMBLIB_LIBS) $(XPDEV_LIBS)

# SMBACTIV
$(SMBACTIV): $(SMBACTIV_OBJS)
	@echo Linking $@
	$(QUIET)$(CC) $(CONSOLE_LDFLAGS) -o $@ $(SMBACTIV_OBJS) $(SMBLIB_LIBS) $(XPDEV_LIBS)

# DSTSEDIT
$(DSTSEDIT): $(DSTSEDIT_OBJS)
	@echo Linking $@
	$(QUIET)$(CC) $(CONSOLE_LDFLAGS) -o $@ $(DSTSEDIT_OBJS) $(XPDEV_LIBS)
