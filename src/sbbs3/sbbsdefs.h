/* sbbsdefs.h */

/* Synchronet constants, macros, and structure definitions */

/* $Id: sbbsdefs.h,v 1.190 2014/03/13 07:17:32 rswindell Exp $ */

/****************************************************************************
 * @format.tab-size 4		(Plain Text/Source Code File Header)			*
 * @format.use-tabs true	(see http://www.synchro.net/ptsc_hdr.html)		*
 *																			*
 * Copyright 2014 Rob Swindell - http://www.synchro.net/copyright.html		*
 *																			*
 * This program is free software; you can redistribute it and/or			*
 * modify it under the terms of the GNU General Public License				*
 * as published by the Free Software Foundation; either version 2			*
 * of the License, or (at your option) any later version.					*
 * See the GNU General Public License for more details: gpl.txt or			*
 * http://www.fsf.org/copyleft/gpl.html										*
 *																			*
 * Anonymous FTP access to the most recent released source is available at	*
 * ftp://vert.synchro.net, ftp://cvs.synchro.net and ftp://ftp.synchro.net	*
 *																			*
 * Anonymous CVS access to the development source and modification history	*
 * is available at cvs.synchro.net:/cvsroot/sbbs, example:					*
 * cvs -d :pserver:anonymous@cvs.synchro.net:/cvsroot/sbbs login			*
 *     (just hit return, no password is necessary)							*
 * cvs -d :pserver:anonymous@cvs.synchro.net:/cvsroot/sbbs checkout src		*
 *																			*
 * For Synchronet coding style and modification guidelines, see				*
 * http://www.synchro.net/source.html										*
 *																			*
 * You are encouraged to submit any modifications (preferably in Unix diff	*
 * format) via e-mail to mods@synchro.net									*
 *																			*
 * Note: If this box doesn't appear square, then you need to fix your tabs.	*
 ****************************************************************************/

#ifndef _SBBSDEFS_H
#define _SBBSDEFS_H

#include <time.h>

#include "gen_defs.h"	/* uchar, ushort, uint, ulong, etc. */
#include "nodedefs.h"	/* node_t */
#include "fidodefs.h"	/* fmsghdr_t, fpkthdr_t, FIDO_*, etc. */
#include "xpbeep.h"		/* BEEP() */
#include "str_list.h"	/* str_list_t */

/*************/
/* Constants */
/*************/

#define VERSION 	"3.16"  /* Version: Major.minor  */
#define REVISION	'a'     /* Revision: lowercase letter */
#define VERSION_NUM	(31600	 + (tolower(REVISION)-'a'))
#define VERSION_HEX	(0x31600 + (tolower(REVISION)-'a'))

#define VERSION_NOTICE		"Synchronet BBS for "PLATFORM_DESC\
								"  Version " VERSION
#define SYNCHRONET_CRC		0x9BCDD162
#define COPYRIGHT_NOTICE	"Copyright 2014 Rob Swindell"
#define COPYRIGHT_CRC		0xB9FF7384

#define Y2K_2DIGIT_WINDOW	70

#define FNOPEN_BUF_SIZE		(2*1024)

#define ILLEGAL_FILENAME_CHARS	"\\/|<>:\";,%"

#define BIND_FAILURE_HELP	"!Another application or service may be using this port"
#define UNKNOWN_LOAD_ERROR	"Unknown load error - Library mismatch?"

#define	JAVASCRIPT_MAX_BYTES		(8*1024*1024)
#define JAVASCRIPT_CONTEXT_STACK	(16*1024)
#define JAVASCRIPT_TIME_LIMIT		(24*60*600)			/* in 100ms ticks */
#define JAVASCRIPT_YIELD_INTERVAL	10000
#define JAVASCRIPT_GC_INTERVAL		100 
#define JAVASCRIPT_LOAD_PATH		"load"
#define JAVASCRIPT_LOAD_PATH_LIST	"load_path_list"

typedef struct {
	uint32_t		counter;
	uint32_t		limit;
	uint32_t		yield_interval;
	uint32_t		gc_interval;
	uint32_t		gc_attempts;
	BOOL			auto_terminate;
	volatile BOOL*	terminated;
} js_callback_t;

#define JSVAL_NULL_OR_VOID(val)		(JSVAL_IS_NULL(val) || JSVAL_IS_VOID(val))

/************/
/* Maximums */
/************/

#define MAX_NODES		250

#define MAX_FILES	  10000 /* Maximum number of files per dir			*/
#define MAX_USERXFER	500 /* Maximum number of dest. users of usrxfer */

#define MAX_TEXTDAT_ITEM_LEN	2000


#define LEN_DIR		63		/* Maximum length of directory paths		*/
#define LEN_CMD		63		/* Maximum length of command lines			*/

							/* MS-DOS limited values					*/
#define LEN_CODE	8		/* Maximum length of internal codes			*/
#define LEN_QWKID	8		/* Maximum length of QWK-ID					*/
#define LEN_MODNAME 8		/* Maximum length of loadable module name	*/
#define LEN_SIFNAME 8		/* Maximum length of SIF filename			*/
#define LEN_EXTCODE (LEN_CODE*2)	/* Code prefix + suffix */

							/* Lengths of various strings				*/
#define LEN_GSNAME	15		/* Group/Lib short name						*/
#define LEN_GLNAME	40		/* Group/Lib long name						*/
#define LEN_SSNAME	25		/* Sub/Dir short name						*/
#define LEN_SLNAME	40		/* Sub/Dir long name						*/
						
									/* User Questions (e.g. for new users)	*/
#define UQ_ALIASES		(1L<<0) 	/* Ask for alias						*/
#define UQ_LOCATION		(1L<<1) 	/* Ask for location 					*/
#define UQ_ADDRESS		(1L<<2) 	/* Ask for address						*/
#define UQ_PHONE		(1L<<3) 	/* Ask for phone number 				*/
#define UQ_HANDLE		(1L<<4) 	/* Ask for chat handle / HAM callsign	*/
#define UQ_DUPHAND		(1L<<5) 	/* Search for duplicate handles 		*/
#define UQ_SEX			(1L<<6) 	/* Ask for sex :)						*/
#define UQ_BIRTH		(1L<<7) 	/* Ask for birth date					*/
#define UQ_UNUSED1 		(1L<<8) 	/* was UQ_COMP							*/
#define UQ_UNUSED2		(1L<<9) 	/* was UQ_MC_COMP						*/
#define UQ_REALNAME		(1L<<10)	/* Ask for real name					*/
#define UQ_DUPREAL		(1L<<11)	/* Search for duplicate real names		*/
#define UQ_COMPANY		(1L<<12)	/* Ask for company name 				*/
#define UQ_NOEXASC		(1L<<13)	/* Don't allow ex-ASCII in user text    */
#define UQ_CMDSHELL		(1L<<14)	/* Ask for command shell				*/
#define UQ_XEDIT		(1L<<15)	/* Ask for external editor				*/
#define UQ_NODEF		(1L<<16)	/* Don't ask for default settings       */
#define UQ_NOCOMMAS		(1L<<17)	/* Do not require commas in location	*/
#define UQ_NONETMAIL	(1L<<18)	/* Don't ask for e-mail/netmail address	*/
#define UQ_NOUPRLWR		(1L<<19)	/* Don't force upper/lower case strings */
#define UQ_COLORTERM	(1L<<20)	/* Ask if new user has color terminal	*/
						
						
									/* Different bits in sys_misc				*/
#define SM_CLOSED		(1L<<0) 	/* System is clsoed to New Users			*/
#define SM_SYSSTAT		(1L<<1) 	/* Sysops activity included in statistics	*/
#define SM_NOSYSINFO	(1L<<2) 	/* Suppress system info display at logon	*/
#define SM_PWEDIT		(1L<<3) 	/* Allow users to change their passwords	*/
#define SM_RA_EMU		(1L<<4) 	/* Reverse R/A commands at msg read prompt	*/
#define SM_ANON_EM		(1L<<5) 	/* Allow anonymous e-mail					*/
#define SM_LISTLOC		(1L<<6) 	/* Use location of caller in user lists 	*/
#define SM_WILDCAT		(1L<<7) 	/* Expand Wildcat color codes in messages	*/
#define SM_PCBOARD		(1L<<8) 	/* Expand PCBoard color codes in messages	*/
#define SM_WWIV 		(1L<<9) 	/* Expand WWIV color codes in messages		*/
#define SM_CELERITY		(1L<<10)	/* Expand Celerity color codes in messages	*/
#define SM_RENEGADE		(1L<<11)	/* Expand Renegade color codes in messages	*/
#define SM_ECHO_PW		(1L<<12)	/* Echo passwords locally					*/
#define SM_UNUSED		(1L<<13)	/* Defaults to *on*	(used to SM_LOCAL_TZ)	*/
#define SM_AUTO_DST		(1L<<14)	/* Automatic Daylight Savings Toggle (US)   */
#define SM_R_SYSOP		(1L<<15)	/* Allow remote sysop logon/commands		*/
#define SM_QUOTE_EM		(1L<<16)	/* Allow quoting of e-mail					*/
#define SM_EURODATE		(1L<<17)	/* Europian date format (DD/MM/YY)			*/
#define SM_MILITARY		(1L<<18)	/* Military time format 					*/
#define SM_TIMEBANK		(1L<<19)	/* Allow time bank functions				*/
#define SM_FILE_EM		(1L<<20)	/* Allow file attachments in E-mail 		*/
#define SM_SHRTPAGE		(1L<<21)	/* Short sysop page 						*/
#define SM_TIME_EXP		(1L<<22)	/* Set to expired values if out-of-time 	*/
#define SM_FASTMAIL		(1L<<23)	/* Fast e-mail storage mode 				*/
#define SM_NONODELIST	(1L<<24)	/* Suppress active node list during logon	*/
#define SM_ERRALARM		(1L<<25)	/* Error beeps on							*/
#define SM_FWDTONET		(1L<<26)	/* Allow forwarding of e-mail to netmail	*/
#define SM_DELREADM		(1L<<27)	/* Delete read mail automatically			*/
#define SM_NOCDTCVT		(1L<<28)	/* No credit to minute conversions allowed	*/
#define SM_DELEMAIL		(1L<<29)	/* Physically remove deleted e-mail immed.	*/
#define SM_USRVDELM		(1L<<30)	/* Users can see deleted msgs				*/
#define SM_SYSVDELM		(1L<<31)	/* Sysops can see deleted msgs				*/
						
									/* Different bits in node_misc				*/
#define NM_NO_NUM		(1<<8)		/* Don't allow logons by user number        */
#define NM_LOGON_R		(1<<9)		/* Allow logons by user real name			*/
#define NM_LOGON_P		(1<<10)		/* Secure logons (always ask for password)	*/
#define NM_LOWPRIO		(1<<15)		/* Always use low priority input			*/
#define NM_7BITONLY		(1L<<16)	/* Except 7-bit input only (E71 terminals)	*/
#define NM_NOPAUSESPIN	(1L<<18)	/* No spinning cursor at pause prompt		*/
#define NM_CLOSENODEDAB	(1L<<19)	/* Keep node.dab file closed (for Samba)	*/

									/* Miscellaneous Modem Settings (mdm_misc)  */
#define MDM_CTS 		(1<<0)		/* Use hardware send flow control			*/
#define MDM_RTS 		(1<<1)		/* Use hardware recv flow control			*/
#define MDM_STAYHIGH	(1<<2)		/* Stay at highest DTE rate 				*/
#define MDM_CALLERID	(1<<3)		/* Supports Caller ID						*/
#define MDM_DUMB		(1<<4)		/* Just watch DCD for answer - dumb modem	*/
#define MDM_NODTR		(1<<5)		/* Don't drop DTR for hang-up               */
#define MDM_KNOWNRES	(1<<6)		/* Allow known result codes only			*/
#define MDM_VERBAL		(1<<7)		/* Use verbal result codes					*/

						
									/* Bit values for level_misc[x] 	*/
#define LEVEL_EXPTOLVL	(1<<0)		/* Expire to level_expireto[x]		*/
#define LEVEL_EXPTOVAL	(1<<1)		/* Expire to val[level_expireto[x]] */

									/* Bit values for prot[x].misc */
#define PROT_DSZLOG		(1<<0)      /* Supports DSZ Log */
#define PROT_NATIVE		(1<<1)		/* Native (32-bit) executable */
#define PROT_SOCKET		(1<<2)		/* Use socket I/O, not stdio on *nix */

									/* Bit values in netmail_misc */

#define NMAIL_ALLOW 	(1<<0)		/* Allow NetMail */
#define NMAIL_CRASH 	(1<<1)		/* Default netmail to crash */
#define NMAIL_HOLD		(1<<2)		/* Default netmail to hold */
#define NMAIL_KILL		(1<<3)		/* Default netmail to kill after sent */
#define NMAIL_ALIAS 	(1<<4)		/* Use Aliases in NetMail */
#define NMAIL_FILE		(1<<5)		/* Allow file attachments */
#define NMAIL_DIRECT	(1<<6)		/* Default netmail to direct */

									/* Bit values for subscan_t.cfg and sav_cfg	*/
#define SUB_CFG_NSCAN	0x0005		/* Auto-scan for new messages				*/
#define SUB_CFG_SSCAN	0x0002		/* Auto-scan for unread messages to you		*/
#define SUB_CFG_YSCAN	0x0100		/* Auto-scan for new messages to you only	*/
									/* (bits 8-15 default to off)				*/

									/* Bit values for sub[x].misc */
#define SUB_QNET		(1L<<3) 	/* Sub-board is netted via QWK network */
#define SUB_PNET		(1L<<4) 	/* Sub-board is netted via PostLink */
#define SUB_FIDO		(1L<<5) 	/* Sub-board is netted via FidoNet */
#define SUB_PRIV		(1L<<6) 	/* Allow private posts on sub */
#define SUB_PONLY		(1L<<7) 	/* Private posts only */
#define SUB_ANON		(1L<<8) 	/* Allow anonymous posts on sub */
#define SUB_AONLY		(1L<<9) 	/* Anonymous only */
#define SUB_NAME		(1L<<10)	/* Must use real names */
#define SUB_DEL 		(1L<<11)	/* Allow users to delete messages */
#define SUB_DELLAST		(1L<<12)	/* Allow users to delete last msg only */
#define SUB_FORCED		(1L<<13)	/* Sub-board is forced scanning */
#define SUB_NOTAG		(1L<<14)	/* Don't add tag or origin lines */
#define SUB_TOUSER		(1L<<15)	/* Prompt for to user on posts */
#define SUB_ASCII		(1L<<16)	/* ASCII characters only */
#define SUB_QUOTE		(1L<<17)	/* Allow online quoting */
#define SUB_NSDEF		(1L<<18)	/* New-Scan on by default */
#define SUB_INET		(1L<<19)	/* Sub-board is netted via Internet */
#define SUB_FAST		(1L<<20)	/* Fast storage mode */
#define SUB_KILL		(1L<<21)	/* Kill read messages automatically */
#define SUB_KILLP		(1L<<22)	/* Kill read pvt messages automatically */
#define SUB_SYSPERM		(1L<<23)	/* Sysop messages are permament */
#define SUB_GATE		(1L<<24)	/* Gateway between Network types */
#define SUB_LZH 		(1L<<25)	/* Use LZH compression for msgs */
#define SUB_SSDEF		(1L<<26)	/* Default ON for Scan for Your msgs */
#define SUB_HYPER		(1L<<27)	/* Hyper allocation */
#define SUB_EDIT		(1L<<28)	/* Users can edit message text after posting */
#define SUB_EDITLAST	(1L<<29)	/* Users can edit last message only */
#define SUB_NOUSERSIG	(1L<<30)	/* Suppress user signatures */
#define SUB_HDRMOD		(1L<<31)	/* Modified sub-board header info (SCFG) */

                                    /* Bit values for dir[x].misc */
#define DIR_FCHK	(1<<0) 			/* Check for file existance */
#define DIR_RATE	(1<<1) 			/* Force uploads to be rated G,R, or X */
#define DIR_MULT	(1<<2) 			/* Ask for multi-disk numbering */
#define DIR_DUPES	(1<<3) 			/* Search this dir for upload dupes */
#define DIR_FREE	(1<<4) 			/* Free downloads */
#define DIR_TFREE	(1<<5) 			/* Time to download is free */
#define DIR_CDTUL	(1<<6) 			/* Credit Uploads */
#define DIR_CDTDL	(1<<7) 			/* Credit Downloads */
#define DIR_ANON	(1<<8) 			/* Anonymous uploads */
#define DIR_AONLY	(1<<9) 			/* Anonymous only */
#define DIR_ULDATE	(1<<10)			/* Include upload date in listing */
#define DIR_DIZ 	(1<<11)			/* FILE_ID.DIZ and DESC.SDI support */
#define DIR_NOSCAN	(1<<12)			/* Don't new-scan this directory */
#define DIR_NOAUTO	(1<<13)			/* Don't auto-add this directory */
#define DIR_ULTIME	(1<<14)			/* Deduct time during uploads */
#define DIR_CDTMIN	(1<<15)			/* Give uploader minutes instead of cdt */
#define DIR_SINCEDL (1<<16)			/* Purge based on days since last dl */
#define DIR_MOVENEW (1<<17)			/* Files marked as new when moved */
#define DIR_QUIET	(1<<18)			/* Do not notify uploader of downloads */
#define DIR_NOSTAT	(1<<19)			/* Do not include transfers in system stats */

                                    /* Bit values for file_t.misc */
#define FM_EXTDESC  (1<<0)          /* Extended description exists */
#define FM_ANON 	(1<<1)			/* Anonymous upload */

									/* Bit values for cfg.file_misc				*/
#define FM_NO_LFN	(1<<0)			/* No long filenames in listings			*/

									/* Bit values for cfg.msg_misc (upper 16-bits default to on) */
#define MM_REALNAME	(1<<16)			/* Allow receipt of e-mail using real names	*/
#define MM_EMAILSIG	(1<<17)			/* Include user signatures in e-mail msgs */

									/* errormsg() codes */
#define ERR_OPEN	"opening"		/* opening a file */
#define ERR_CLOSE	"closing"		/* close a file */
#define ERR_FDOPEN	"fdopen"		/* associating a stream with fd */
#define ERR_READ	"reading"		/* reading from file */
#define ERR_WRITE	"writing"		/* writing to file */
#define ERR_REMOVE 	"removing"		/* removing a file */
#define ERR_ALLOC	"allocating"	/* allocating memory */
#define ERR_CHK		"checking"		/* checking */
#define ERR_LEN		"checking length"
#define ERR_EXEC	"executing"		/* executing */
#define ERR_CHDIR	"changing directory"
#define ERR_CREATE	"creating" 		/* creating */
#define ERR_LOCK	"locking"		/* locking */
#define ERR_UNLOCK 	"unlocking"		/* unlocking */
#define ERR_TIMEOUT	"timeout"		/* timeout waiting for resource */	
#define ERR_IOCTL	"sending IOCTL"	/* IOCTL error */
#define ERR_SEEK	"seeking"		/* SEEKing error */

enum {                              /* Values for dir[x].sort */
     SORT_NAME_A                    /* Sort by filename, ascending */
    ,SORT_NAME_D                    /* Sort by filename, descending */
    ,SORT_DATE_A                    /* Sort by upload date, ascending */
    ,SORT_DATE_D                    /* Sort by upload date, descending */
    };

enum {
	 clr_mnehigh
	,clr_mnelow
	,clr_mnecmd
	,clr_inputline
	,clr_err
	,clr_nodenum
	,clr_nodeuser
	,clr_nodestatus
	,clr_filename
	,clr_filecdt
	,clr_filedesc
	,clr_filelsthdrbox
	,clr_filelstline
	,clr_chatlocal
	,clr_chatremote
	,clr_multichat
	,clr_external
	,MIN_COLORS 
};

enum {								/* Values for xtrn_t.type				*/
	 XTRN_NONE						/* No data file needed					*/
	,XTRN_SBBS						/* Synchronet external					*/
	,XTRN_WWIV						/* WWIV external						*/
	,XTRN_GAP						/* Gap door 							*/
	,XTRN_RBBS						/* RBBS, QBBS, or Remote Access 		*/
	,XTRN_WILDCAT					/* Wildcat								*/
	,XTRN_PCBOARD					/* PCBoard								*/
	,XTRN_SPITFIRE					/* SpitFire 							*/
	,XTRN_UTI						/* UTI Doors - MegaMail 				*/
	,XTRN_SR						/* Solar Realms 						*/
	,XTRN_RBBS1 					/* DORINFO1.DEF always					*/
	,XTRN_TRIBBS					/* TRIBBS.SYS							*/
	,XTRN_DOOR32					/* DOOR32.SYS							*/
	};																		
																			
typedef enum {						/* Values for xtrn_t.event				*/
	 EVENT_NONE 					/* Only accessible by menu				*/
	,EVENT_LOGON					/* Execute during logon sequence		*/
	,EVENT_LOGOFF					/* Execute during logoff sequence		*/
	,EVENT_NEWUSER					/* Execute during newuser app.			*/
	,EVENT_BIRTHDAY 				/* Execute on birthday					*/
	,EVENT_POST						/* Execute after posting a message		*/
	,EVENT_UPLOAD					/* Execute after uploading a file		*/
	,EVENT_DOWNLOAD					/* Execute after downloading a file		*/
	,EVENT_LOCAL_CHAT				/* Execute upon local/sysop chat		*/
} user_event_t;																
																			
									/* Misc bits for event_t.misc			*/
#define EVENT_EXCL		(1<<0) 		/* Exclusive							*/
#define EVENT_FORCE		(1<<1) 		/* Force users off-line for event		*/
#define EVENT_INIT		(1<<2)		/* Always run event after init			*/
#define EVENT_DISABLED	(1<<3)		/* Disabled								*/
																			
									/* Mode bits for QWK stuff */			
#define A_EXPAND		(1<<0)		/* Expand to ANSI sequences */			
#define A_LEAVE 		(1<<1)		/* Leave in */							
#define A_STRIP 		(1<<2)		/* Strip out */							
																			
									/* Bits in xtrn_t.misc					*/
#define MULTIUSER		(1<<0) 		/* allow multi simultaneous users		*/
#define XTRN_ANSI		(1<<1)		/* LEGACY (not used)                    */
#define XTRN_STDIO 		(1<<2) 		/* Intercept Standard I/O (aka IO_INTS)	*/
#define MODUSERDAT		(1<<3) 		/* Program can modify user data 		*/
#define WWIVCOLOR		(1<<4) 		/* Program uses WWIV color codes		*/
#define EVENTONLY		(1<<5) 		/* Program executes as event only		*/
#define STARTUPDIR		(1<<6) 		/* Create drop file in start-up dir		*/
#define REALNAME		(1<<7) 		/* Use real name in drop file			*/
#define SWAP			(1<<8) 		/* Swap for this door					*/
#define FREETIME		(1<<9) 		/* Free time while in this door 		*/
#define QUICKBBS		(1<<10)		/* QuickBBS style editor				*/
#define EXPANDLF		(1<<11)		/* Expand LF to CRLF editor 			*/
#define QUOTEALL		(1<<12)		/* Automatically quote all of msg		*/
#define QUOTENONE		(1<<13)		/* Automatically quote none of msg		*/
#define XTRN_NATIVE		(1<<14)		/* Native application (EX_NATIVE)		*/
#define STRIPKLUDGE		(1<<15)		/* Strip FTN Kludge lines from msg		*/
#define XTRN_CHKTIME	(1<<16)		/* Check time online (EX_CHKTIME)		*/
#define XTRN_LWRCASE	(1<<17)		/* Use lowercase drop-file names		*/
#define XTRN_SH			(1<<18)		/* Use command shell to execute			*/
#define XTRN_PAUSE		(1<<19)		/* Force a screen pause on exit			*/
#define XTRN_NOECHO		(1<<20)		/* Don't echo stdin to stdout			*/
#define WORDWRAP80		(1<<21)		/* Word-wrap editor to 80 columns		*/
#define WORDWRAPTERM	(1<<22)		/* Word-wrap editor to terminal width	*/
#define WORDWRAPLONG	(WORDWRAP80|WORDWRAPTERM)	/* word-wrap to maxlen	*/
#define WORDWRAPNONE	0			/* No word-wrapping on editor in/ouput	*/
#define WORDWRAPMASK	WORDWRAPLONG
#define XTRN_CONIO		(1<<31)		/* Intercept Windows Console I/O (Drwy)	*/
#define QUOTEWRAP		WORDWRAP80	/* for temporary backwards compat.		*/


									/* Bits in cfg.xtrn_misc				*/
#define XTRN_NO_MUTEX	(1<<0)		/* Do not use exec_mutex for FOSSIL VXD	*/

									/* Bits in user.qwk 					*/
#define QWK_FILES	(1L<<0) 		/* Include new files list				*/
#define QWK_EMAIL	(1L<<1) 		/* Include unread e-mail				*/
#define QWK_ALLMAIL (1L<<2) 		/* Include ALL e-mail					*/
#define QWK_DELMAIL (1L<<3) 		/* Delete e-mail after download 		*/
#define QWK_BYSELF	(1L<<4) 		/* Include messages from self			*/
#define QWK_UNUSED	(1L<<5) 		/* Currently unused 					*/
#define QWK_EXPCTLA (1L<<6) 		/* Expand ctrl-a codes to ascii 		*/
#define QWK_RETCTLA (1L<<7) 		/* Retain ctrl-a codes					*/
#define QWK_ATTACH	(1L<<8) 		/* Include file attachments 			*/
#define QWK_NOINDEX (1L<<9) 		/* Do not create index files in QWK		*/
#define QWK_TZ		(1L<<10)		/* Include "@TZ" time zone in msgs		*/
#define QWK_VIA 	(1L<<11)		/* Include "@VIA" seen-bys in msgs		*/
#define QWK_NOCTRL	(1L<<12)		/* No extraneous control files			*/
#define QWK_EXT		(1L<<13)		/* QWK Extended (QWKE) format			*/
#define QWK_MSGID	(1L<<14)		/* Include "@MSGID" in msgs				*/
#define QWK_HEADERS	(1L<<16)		/* Include HEADERS.DAT file				*/

#define QWK_DEFAULT	(QWK_FILES|QWK_ATTACH|QWK_EMAIL|QWK_DELMAIL)
																			
							/* Bits in user.chat							*/
#define CHAT_ECHO	(1<<0)	/* Multinode chat echo							*/
#define CHAT_ACTION (1<<1)	/* Chat actions 								*/
#define CHAT_NOPAGE (1<<2)	/* Can't be paged								*/
#define CHAT_NOACT	(1<<3)	/* No activity alerts							*/
#define CHAT_SPLITP (1<<4)	/* Split screen private chat					*/
																			
																			
#define INVALID_DIR ((uint)-1)	/* Invalid directory value					*/
#define INVALID_SUB ((uint)-1)	/* Invalid sub-board value					*/
																			
#define KEY_BUFSIZE 1024	/* Size of keyboard input buffer				*/
#define SAVE_LINES	 4		/* Maximum number of lines to save				*/
#define LINE_BUFSIZE 512	/* Size of line output buffer					*/
																			
																			
#define EDIT_TABSIZE 4		/* Tab size for internal message/line editor	*/
																			
#define SWAP_NONE	0x80	/* Allow no swapping for executables			*/
																			
#define DSTSDABLEN	50		/* Length of dsts.dab file						*/
																			
								/* Console I/O Bits	(console)				*/
#define CON_R_ECHO		(1<<0)	/* Echo remotely							*/
#define CON_R_ECHOX		(1<<1)	/* Echo X's to remote user					*/
#define CON_R_INPUT		(1<<2)	/* Accept input remotely					*/
#define CON_L_ECHO		(1<<3)	/* Echo locally              				*/
#define CON_L_ECHOX		(1<<4)	/* Echo X's locally							*/
#define CON_L_INPUT		(1<<5)	/* Accept input locally						*/
#define CON_RAW_IN		(1<<8)	/* Raw input mode - no editing capabilities	*/
#define CON_ECHO_OFF	(1<<10)	/* Remote & Local echo disabled for ML/MF	*/
#define CON_UPARROW		(1<<11)	/* Up arrow hit - move up one line			*/
#define CON_DOWNARROW	(1<<12)	/* Down arrow hit, exiting from getstr()	*/
#define CON_NO_INACT	(1<<13)	/* Console inactivity detection disabled	*/
#define CON_BACKSPACE	(1<<14)	/* Backspace hit, exiting from getstr()		*/
#define CON_LEFTARROW	(1<<15)	/* Left arrow hit, exiting from getstr()	*/
#define CON_INSERT		(1<<16)	/* Insert mode, for getstr()				*/
#define CON_DELETELINE	(1<<17)	/* Deleted line, exiting from getstr()		*/
																			
							/* Number of milliseconds						*/
#define DELAY_AUTOHG 1500	/* Delay for auto-hangup (xfer) 				*/
																			
#define SEC_LOGON	1800	/* 30 minutes allowed to logon					*/
#define SEC_BILLING   90	/* under 2 minutes per billing call 			*/
#define SEC_OK		   5	/* Attempt to get an OK response from modem		*/
#define SEC_ANSI	   5	/* Attempt to get a valid ANSI response 		*/
#define SEC_ANSWER	  30	/* Retries to get an answer code from modem		*/
#define SEC_CID 	  10	/* Ten second pause for caller ID				*/
#define SEC_RING	   6	/* Maximum seconds between rings				*/
																			
#define LOOP_NOPEN	  50	/* Retries before file access denied			*/
#define LOOP_NODEDAB  50	/* Retries on node.dab locking/unlocking		*/
																			
							/* String lengths								*/
#define LEN_ALIAS		25	/* User alias									*/
#define LEN_NAME		25	/* User name									*/
#define LEN_HANDLE		8	/* User chat handle 							*/
#define LEN_NOTE		30	/* User note									*/
#define LEN_COMP		30	/* User computer description					*/
#define LEN_COMMENT 	60	/* User comment 								*/
#define LEN_NETMAIL 	60	/* NetMail forwarding address					*/
#define LEN_PASS		 8	/* User password								*/
#define LEN_PHONE		12	/* User phone number							*/
#define LEN_BIRTH		 8	/* Birthday in MM/DD/YY format					*/
#define LEN_ADDRESS 	30	/* User address 								*/
#define LEN_LOCATION	30	/* Location (City, State)						*/
#define LEN_ZIPCODE 	10	/* Zip/Postal code								*/
#define LEN_MODEM		 8	/* User modem type description					*/
#define LEN_FDESC		58	/* File description 							*/
#define LEN_FCDT		 9	/* 9 digits for file credit values				*/
#define LEN_TITLE		70	/* Message title								*/
#define LEN_MAIN_CMD	34	/* Storage in user.dat for custom commands		*/
#define LEN_XFER_CMD	40													
#define LEN_SCAN_CMD	40													
#define LEN_MAIL_CMD	40													
#define LEN_CID 		25	/* Caller ID (phone number) 					*/
#define LEN_ARSTR		40	/* Max length of Access Requirement string		*/
#define LEN_CHATACTCMD	 9	/* Chat action command							*/
#define LEN_CHATACTOUT	65	/* Chat action output string					*/

/****************************************************************************/
/* This is a list of offsets into the USER.DAT file for different variables */
/* that are stored (for each user)											*/
/****************************************************************************/
#define U_ALIAS 	0					/* Offset to alias */
#define U_NAME		U_ALIAS+LEN_ALIAS  /* Offset to name */
#define U_HANDLE	U_NAME+LEN_NAME 
#define U_NOTE		U_HANDLE+LEN_HANDLE+2 
#define U_COMP		U_NOTE+LEN_NOTE 
#define U_COMMENT	U_COMP+LEN_COMP+2 

#define U_NETMAIL	U_COMMENT+LEN_COMMENT+2 

#define U_ADDRESS	U_NETMAIL+LEN_NETMAIL+2 
#define U_LOCATION	U_ADDRESS+LEN_ADDRESS 
#define U_ZIPCODE	U_LOCATION+LEN_LOCATION 

#define U_PASS		U_ZIPCODE+LEN_ZIPCODE+2 
#define U_PHONE  	U_PASS+8  			/* Offset to phone-number */
#define U_BIRTH  	U_PHONE+12 		/* Offset to users birthday	*/
#define U_MODEM     U_BIRTH+8 
#define U_LASTON	U_MODEM+8 
#define U_FIRSTON	U_LASTON+8 
#define U_EXPIRE    U_FIRSTON+8 
#define U_PWMOD     U_EXPIRE+8 

#define U_LOGONS    U_PWMOD+8+2 
#define U_LTODAY    U_LOGONS+5 
#define U_TIMEON    U_LTODAY+5 
#define U_TEXTRA  	U_TIMEON+5 
#define U_TTODAY    U_TEXTRA+5 
#define U_TLAST     U_TTODAY+5 
#define U_POSTS     U_TLAST+5 
#define U_EMAILS    U_POSTS+5 
#define U_FBACKS    U_EMAILS+5 
#define U_ETODAY	U_FBACKS+5 
#define U_PTODAY	U_ETODAY+5 

#define U_ULB       U_PTODAY+5+2 
#define U_ULS       U_ULB+10 
#define U_DLB       U_ULS+5 
#define U_DLS       U_DLB+10 
#define U_CDT		U_DLS+5 
#define U_MIN		U_CDT+10 

#define U_LEVEL 	U_MIN+10+2 	/* Offset to Security Level    */
#define U_FLAGS1	U_LEVEL+2  	/* Offset to Flags */
#define U_TL		U_FLAGS1+8 	/* Offset to unused field */
#define U_FLAGS2	U_TL+2 
#define U_EXEMPT	U_FLAGS2+8 
#define U_REST		U_EXEMPT+8 
#define U_ROWS		U_REST+8+2 	/* Number of Rows on user's monitor */
#define U_SEX		U_ROWS+2 		/* Sex, Del, ANSI, color etc.		*/
#define U_MISC		U_SEX+1 		/* Miscellaneous flags in 8byte hex */
#define U_OLDXEDIT	U_MISC+8 		/* External editor (Version 1 method  */
#define U_LEECH 	U_OLDXEDIT+2 	/* two hex digits - leech attempt count */
#define U_CURSUB	U_LEECH+2  	/* Current sub (internal code)  */
#define U_CURXTRN	U_CURSUB+16 /* Current xtrn (internal code) */
#define U_MAIN_CMD	U_CURXTRN+8+2 	/* unused */
#define U_XFER_CMD	U_MAIN_CMD+LEN_MAIN_CMD 		/* unused */
#define U_SCAN_CMD	U_XFER_CMD+LEN_XFER_CMD+2  	/* unused */
#define U_MAIL_CMD	U_SCAN_CMD+LEN_SCAN_CMD 		/* unused */
#define U_FREECDT	U_MAIL_CMD+LEN_MAIL_CMD+2 
#define U_FLAGS3	U_FREECDT+10 	/* Flag set #3 */
#define U_FLAGS4	U_FLAGS3+8 	/* Flag set #4 */
#define U_XEDIT 	U_FLAGS4+8 	/* External editor (code  */
#define U_SHELL 	U_XEDIT+8  	/* Command shell (code  */
#define U_QWK		U_SHELL+8  	/* QWK settings */
#define U_TMPEXT	U_QWK+8 		/* QWK extension */
#define U_CHAT		U_TMPEXT+3 	/* Chat settings */
#define U_NS_TIME	U_CHAT+8 		/* New-file scan date/time */
#define U_PROT		U_NS_TIME+8 	/* Default transfer protocol */
#define U_LOGONTIME	U_PROT+1
#define U_CURDIR	U_LOGONTIME+8	/* Current dir (internal code  */
#define U_UNUSED	U_CURDIR+16
#define U_LEN		(U_UNUSED+4+2)

/****************************************************************************/
/* Offsets into DIR .DAT file for different fields for each file 			*/
/****************************************************************************/
#define F_CDT		0				/* Offset in DIR#.DAT file for cdts		*/
#define F_DESC		(F_CDT+LEN_FCDT)/* Description							*/
#define F_ULER		(F_DESC+LEN_FDESC+2)   /* Uploader						*/
#define F_TIMESDLED (F_ULER+30+2) 	/* Number of times downloaded 			*/
#define F_OPENCOUNT	(F_TIMESDLED+5+2)										
#define F_MISC		(F_OPENCOUNT+3+2)										
#define F_ALTPATH	(F_MISC+1)		/* Two hex digit alternate path */		
#define F_LEN		(F_ALTPATH+2+2) /* Total length of all fdat in file		*/
																			
#define F_IXBSIZE	22				/* Length of each index entry			*/
																			
#define F_EXBSIZE	512				/* Length of each ext-desc entry		*/
																			
																			
#define SIF_MAXBUF  0x7000			/* Maximum buffer size of SIF data		*/
																			
/* NOTE: Do not change the values of the following block of defines!		*/
																			
#define DELETED 	(1L<<0) 		/* Bit values for user.misc 			*/
#define ANSI		(1L<<1) 		/* Supports ANSI terminal emulation		*/
#define COLOR		(1L<<2) 		/* Send color codes 					*/
#define RIP 		(1L<<3) 		/* Supports RIP terminal emulation		*/
#define UPAUSE		(1L<<4) 		/* Pause on every screen full			*/
#define SPIN		(1L<<5) 		/* Spinning cursor - Same as K_SPIN		*/
#define INACTIVE	(1L<<6) 		/* Inactive user slot					*/
#define EXPERT		(1L<<7) 		/* Expert menu mode 					*/
#define ANFSCAN 	(1L<<8) 		/* Auto New file scan					*/
#define CLRSCRN 	(1L<<9) 		/* Clear screen before each message		*/
#define QUIET		(1L<<10)		/* Quiet mode upon logon				*/
#define BATCHFLAG	(1L<<11)		/* File list allow batch dl flags		*/
#define NETMAIL 	(1L<<12)		/* Forward e-mail to fidonet addr		*/
#define CURSUB		(1L<<13)		/* Remember current sub-board/dir		*/
#define ASK_NSCAN	(1L<<14)		/* Ask for newscanning upon logon		*/
#define NO_EXASCII	(1L<<15)		/* Don't send extended ASCII			*/
#define ASK_SSCAN	(1L<<16)		/* Ask for messages to you at logon		*/
#define AUTOTERM	(1L<<17)		/* Autodetect terminal type 			*/
#define COLDKEYS	(1L<<18)		/* No hot-keys							*/
#define EXTDESC 	(1L<<19)		/* Extended file descriptions			*/
#define AUTOHANG	(1L<<20)		/* Auto-hang-up after transfer			*/
#define WIP 		(1L<<21)		/* Supports WIP terminal emulation		*/
#define AUTOLOGON	(1L<<22)		/* AutoLogon via IP						*/
#define HTML		(1L<<23)		/* Using Deuce's HTML terminal			*/
#define NOPAUSESPIN	(1L<<24)		/* No spinning cursor at pause prompt	*/

#define TERM_FLAGS	(ANSI|COLOR|NO_EXASCII|RIP|WIP|HTML)
																			
#define CLREOL      256     /* Character to erase to end of line 			*/
																			
							/* Online status (online)						*/
#define ON_LOCAL	1	 	/* Online locally								*/
#define ON_REMOTE   2  		/* Online remotely								*/
																			
							/* Various SYSTEM parameters for sys_status		*/
#define SS_UNUSED	(1L<<0)	/* Unused          								*/
#define SS_INITIAL  (1L<<1)	/* The bbs data has been initialized.			*/
#define SS_TMPSYSOP (1L<<2)	/* Temporary Sysop Status						*/
#define SS_USERON   (1L<<3)	/* A User is logged on to the BBS				*/
#define SS_LCHAT    (1L<<4) /* Local chat in progress						*/
#define SS_CAP		(1L<<5)	/* Capture is on								*/
#define SS_ANSCAP	(1L<<6) /* Capture ANSI codes too						*/
#define SS_FINPUT	(1L<<7) /* Using file for input 						*/
#define SS_COMISR	(1L<<8) /* Com port ISR is installed					*/
#define SS_DAILY	(1L<<9) /* Execute System Daily Event on logoff 		*/
#define SS_INUEDIT	(1L<<10) /* Inside Alt-Useredit section 				*/
#define SS_ABORT	(1L<<11) /* Global abort input or output flag			*/
#define SS_SYSPAGE	(1L<<12) /* Paging sysop								*/
#define SS_SYSALERT (1L<<13) /* Notify sysop when users hangs up			*/
#define SS_GURUCHAT (1L<<14) /* Guru chat in progress						*/
#define SS_UNUSED2	(1L<<15) /* not used in v3 (used to be SS_NODEDAB)		*/
#define SS_EVENT	(1L<<16) /* Time shortened due to upcoming event		*/
#define SS_PAUSEON	(1L<<17) /* Pause on, overriding user default			*/
#define SS_PAUSEOFF (1L<<18) /* Pause off, overriding user default			*/
#define SS_IN_CTRLP (1L<<19) /* Inside ctrl-p send node message func		*/
#define SS_NEWUSER	(1L<<20) /* New User online 							*/
#define SS_MDMDEBUG (1L<<21) /* Modem debug output							*/
#define SS_NEST_PF	(1L<<22) /* Nested in printfile function				*/
#define SS_DCDHIGH	(1L<<23) /* Assume DCD is high always					*/
#define SS_SPLITP	(1L<<24) /* Split-screen private chat					*/
#define SS_NEWDAY	(1L<<25) /* Date changed while online					*/
#define SS_RLOGIN	(1L<<26) /* Current login via BSD RLogin				*/
#define SS_FILEXFER	(1L<<27) /* File transfer in progress, halt spy			*/
#define SS_SSH		(1L<<28) /* Current login via SSH						*/
#define SS_MOFF		(1L<<29) /* Disable automatic messages					*/

								/* Bits in 'mode' for getkey and getstr     */
#define K_NONE		0			/* Use as a place holder for no mode flags	*/
#define K_UPPER 	(1L<<0) 	/* Converts all letters to upper case		*/
#define K_UPRLWR	(1L<<1) 	/* Upper/Lower case automatically			*/
#define K_NUMBER	(1L<<2) 	/* Allow numbers only						*/
#define K_WRAP		(1L<<3) 	/* Allows word wrap 						*/
#define K_MSG		(1L<<4) 	/* Allows ANSI, ^N ^A ^G					*/
#define K_SPIN		(1L<<5) 	/* Spinning cursor (same as SPIN)			*/
#define K_LINE		(1L<<6) 	/* Input line (inverse color)				*/
#define K_EDIT		(1L<<7) 	/* Edit string passed						*/
#define K_CHAT		(1L<<8) 	/* In chat multi-chat						*/
#define K_NOCRLF	(1L<<9) 	/* Don't print CRLF after string input      */
#define K_ALPHA 	(1L<<10)	/* Only allow alphabetic characters 		*/
#define K_GETSTR	(1L<<11)	/* getkey called from getstr()				*/
#define K_LOWPRIO	(1L<<12)	/* low priority input						*/
#define K_NOEXASC	(1L<<13)	/* No extended ASCII allowed				*/
#define K_E71DETECT (1L<<14)	/* Detect E-7-1 terminal type				*/
#define K_AUTODEL	(1L<<15)	/* Auto-delete text (used with K_EDIT)		*/
#define K_COLD		(1L<<16)	/* Possible cold key mode					*/
#define K_NOECHO	(1L<<17)	/* Don't echo input                         */
#define K_TAB		(1L<<18)	/* Treat TAB key as CR						*/
#define K_LEFTEXIT	(1L<<19)	/* Allow exit by backspacing/arrowing left	*/
#define K_USEOFFSET	(1L<<20)	/* Use getstr_offset for cursor	position	*/
#define K_NOSPIN	(1L<<21)	/* Do not honor the user's spinning cursor	*/
#define K_ANSI_CPR	(1L<<22)	/* Expect ANSI Cursor Position Report		*/

								/* Bits in 'mode' for putmsg and printfile  */
#define P_NOABORT  	(1<<0)		/* Disallows abortion of a message          */
#define P_SAVEATR   (1<<1)		/* Save the new current attributres after	*/
								/* msg has printed. */
#define P_NOATCODES (1<<2)		/* Don't allow @ codes                      */
#define P_OPENCLOSE (1<<3)		/* Open and close the file					*/
#define P_NOPAUSE	(1<<4)		/* Disable screen pause						*/
#define P_HTML		(1<<5)		/* Message is HTML							*/
#define P_NOCRLF	(1<<6)		/* Don't prepend a CRLF	in printfile()		*/
#define P_WORDWRAP	(1<<7)		/* Word-wrap long lines for user's terminal	*/
								
								/* Bits in 'mode' for listfiles             */
#define FL_ULTIME   (1<<0)		/* List files by upload time                */
#define FL_DLTIME   (1<<1)		/* List files by download time              */
#define FL_NO_HDR   (1<<2)		/* Don't list directory header              */
#define FL_FINDDESC (1<<3)		/* Find text in description                 */
#define FL_EXFIND   (1<<4)		/* Find text in description - extended info */
#define FL_VIEW     (1<<5)		/* View ZIP/ARC/GIF etc. info               */
								
								/* Bits in the mode of writemsg and email() */
#define WM_EXTDESC	(1<<0)		/* Writing extended file description		*/
#define WM_EMAIL	(1<<1)		/* Writing e-mail							*/
#define WM_NETMAIL	(1<<2)		/* Writing NetMail							*/
#define WM_ANON 	(1<<3)		/* Writing anonymous message				*/
#define WM_FILE 	(1<<4)		/* Attaching a file to the message			*/
#define WM_NOTOP	(1<<5)		/* Don't add top because we need top line   */
#define WM_QUOTE	(1<<6)		/* Quote file available 					*/
#define WM_QWKNET	(1<<7)		/* Writing QWK NetMail (25 char title)		*/
#define WM_PRIVATE	(1<<8)		/* Private (for creating MSGINF file)		*/
#define WM_SUBJ_RO	(1<<9)		/* Subject/title is read-only				*/
#define WM_EDIT		(1<<10)		/* Editing existing message					*/
#define WM_FORCEFWD	(1<<11)		/* Force "yes" to ForwardMailQ for email	*/
								
								/* Bits in the mode of loadposts()			*/
#define LP_BYSELF	(1<<0)		/* Include messages sent by self			*/
#define LP_OTHERS	(1<<1)		/* Include messages sent to others			*/
#define LP_UNREAD	(1<<2)		/* Un-read messages only					*/
#define LP_PRIVATE	(1<<3)		/* Include all private messages 			*/
#define LP_REP		(1<<4)		/* Packing REP packet						*/
								
								/* Bits in the mode of loadmail()			*/
#define LM_UNREAD	(1<<0)		/* Include un-read mail only				*/
#define LM_INCDEL	(1<<1)		/* Include deleted mail		 				*/
								
enum {							/* readmail and delmailidx which types		*/
	 MAIL_YOUR					/* mail sent to you							*/
	,MAIL_SENT					/* mail you have sent						*/
	,MAIL_ANY					/* mail sent to or from you					*/
	,MAIL_ALL					/* all mail (ignores usernumber arg)		*/
	};							
								
								/* Bits in the mode of external()           */
#define EX_SH       (1<<0)		/* Use command shell to load other process  */
#define EX_STDOUT   (1<<1)		/* Copy DOS output to remote                */
#define EX_OUTL 	(1<<2)		/* Use _lputc() for local output (*legacy*)	*/
#define EX_STDIN	(1<<3)		/* Trap int 16h keyboard input requests     */
#define EX_WWIV 	WWIVCOLOR	/* Expand WWIV color codes to ANSI sequence */
#define EX_SWAP 	(1<<5)		/* Swap out for this external (*legacy*)	*/
#define EX_POPEN	(1<<7)		/* Leave COM port open	(*legacy*)			*/
#define EX_OFFLINE	(1<<8)		/* Run this program offline					*/
#define EX_BG		(1<<10)		/* Back-ground/detached process				*/
#define EX_BIN		(1<<11)		/* Binary mode (no Unix LF to CRLF)			*/
#define EX_NATIVE	XTRN_NATIVE		/* Native 32-bit application 			*/
#define EX_CHKTIME	XTRN_CHKTIME	/* Check time left						*/
#define EX_NOECHO	XTRN_NOECHO		/* Don't echo stdin to stdout 			*/
#define EX_STDIO	(EX_STDIN|EX_STDOUT)
#define EX_CONIO	(1<<31)		/* Intercept Windows console I/O (doorway)	*/

#if defined(__unix__)
#define EX_WILDCARD	EX_SH		/* Expand wildcards using 'sh' on Unix		*/
#else
#define EX_WILDCARD	0
#endif
								
								/* telnet_gate() mode bits					*/							
#define TG_ECHO			(1<<0)	/* Turn on telnet echo						*/
#define TG_CRLF			(1<<1)	/* Expand sole CR to CRLF					*/
#define TG_LINEMODE		(1<<2)	/* Send entire lines only					*/
#define TG_NODESYNC		(1<<3)	/* Call Nodesync, get msgs, etc.			*/
#define TG_CTRLKEYS		(1<<4)	/* Interpret ^P ^U ^T, etc locally			*/
#define TG_PASSTHRU		(1<<5)	/* Pass-through telnet commands/responses	*/
#define TG_RLOGIN		(1<<6)	/* Use BSD RLogin protocol					*/
#define TG_NOCHKTIME	(1<<7)	/* Don't check time left while gated		*/
#define TG_NOTERMTYPE	(1<<8)	/* Request client "DONT TERM_TYPE"			*/
#define TG_SENDPASS		(1<<9)	/* Send password instead of real name (RLogin) - DEPRECATED	(it sent the password as the server user name) */
#define TG_NOLF			(1<<10)	/* Do not send line-feeds (opposite of TG_CRLF) */
								
enum {							/* Values for 'mode' in listfileinfo        */
	 FI_INFO            		/* Just list file information               */
	,FI_REMOVE           		/* Remove/Move/Edit file information        */
	,FI_DOWNLOAD         		/* Download files                           */
	,FI_OLD              		/* Search/Remove files not downloaded since */
	,FI_OLDUL	 				/* Search/Remove files uploaded before      */
	,FI_OFFLINE   				/* Search/Remove files not online			*/
	,FI_USERXFER  				/* User Xfer Download                       */
	,FI_CLOSE 	  				/* Close any open records					*/
	};							

enum XFER_TYPE {				/* Values for type in xfer_prot_select()	*/
	 XFER_UPLOAD
	,XFER_DOWNLOAD
	,XFER_BATCH_UPLOAD
	,XFER_BATCH_DOWNLOAD
	,XFER_BIDIR
};
	
#define L_LOGON     1			/* Logon List maintenance                   */
#define LOL_SIZE    81			/* Length of each logon list entry          */
								
								/* Bits in mode of scanposts() function 	*/
#define SCAN_CONST	(1<<0)		/* Continuous message scanning				*/
#define SCAN_NEW	(1<<1)		/* New scanning								*/
#define SCAN_BACK	(1<<2)		/* Scan the last message if no new			*/
#define SCAN_TOYOU	(1<<3)		/* Scan for messages to you 				*/
#define SCAN_FIND	(1<<4)		/* Scan for text in messages				*/
#define SCAN_UNREAD (1<<5)		/* Find un-read messages to you 			*/
								
								/* Bits in misc of chan_t					*/
#define CHAN_PW 	(1<<0)		/* Can be password protected				*/
#define CHAN_GURU	(1<<1)		/* Guru joins empty channel 				*/
								
enum {							/* Values of mode for userlist function     */
	 UL_ALL						/* List all users in userlist				*/
	,UL_SUB      				/* List all users with access to cursub     */
	,UL_DIR						/* List all users with access to curdir 	*/
	};							
								
								
#define BO_LEN		16			/* backout.dab record length				*/
								
#define BO_OPENFILE 0			/* Backout types */

/**********/
/* Macros */
/**********/

#define CRLF			{ outchar(CR); outchar(LF); }
#define SYSOP_LEVEL		90
#define SYSOP			(useron.level>=SYSOP_LEVEL || sys_status & SS_TMPSYSOP)
#define REALSYSOP		(useron.level>=SYSOP_LEVEL)
#define FLAG(x) 		(ulong)(1UL<<(x-'A'))
#define CLS         	outchar(FF)
#define WHERE       	__LINE__,getfname(__FILE__)
#define SAVELINE		{ if(slcnt<SAVE_LINES) { \
							slatr[slcnt]=latr; \
							slcuratr[slcnt]=curatr; \
							sprintf(slbuf[slcnt],"%.*s",lbuflen,lbuf); \
							slcnt++; \
							lbuflen=0; } }
#define RESTORELINE 	{ lbuflen=0; if(slcnt) --slcnt; \
							attr(slatr[slcnt]); \
							rputs(slbuf[slcnt]); \
							curatr=slcuratr[slcnt]; }
#define SYNC			{ getnodedat(cfg.node_num,&thisnode,0); \
						  nodesync(); }
#define ASYNC			{ getnodedat(cfg.node_num,&thisnode,0); \
						  nodesync(); }
#define TM_YEAR(yy)		((yy)%100)
#define sbbs_beep(f,d)	BEEP(f,d)
#define mswait(x)		SLEEP(x)

/**************************************/
/* Text Attribute (color) Definitions */
/**************************************/
#define HIGH  0x08      /* High intensity foreground text */
#ifndef BLINK
#define BLINK 0x80		/* Blinking text */
#endif

#ifndef __COLORS
#define __COLORS

enum COLORS {
	BLACK,			/* dark colors (HIGH bit unset) */
	BLUE,
	GREEN,
	CYAN,
	RED,
	MAGENTA,
	BROWN,
	LIGHTGRAY,
	DARKGRAY,		/* light colors (HIGH bit set) */
	LIGHTBLUE,
	LIGHTGREEN,
	LIGHTCYAN,
	LIGHTRED,
	LIGHTMAGENTA,
	YELLOW,
	WHITE
};

#endif	/* __COLORS */

#define ANSI_NORMAL		0x100
#define BG_BLACK		0x200
#define BG_BLUE			(BLUE<<4)
#define BG_GREEN		(GREEN<<4)
#define BG_CYAN			(CYAN<<4)
#define BG_RED			(RED<<4)
#define BG_MAGENTA		(MAGENTA<<4)
#define BG_BROWN		(BROWN<<4)
#define BG_LIGHTGRAY	(LIGHTGRAY<<4)

/********************/
/* Type Definitions */
/********************/

typedef struct {						/* Users information */
	ushort	number, 					/* Number */
			uls,						/* Number of uploads */
			dls,						/* Number of downloads */
			posts,						/* Number of posts */
			emails,						/* Number of emails */
			fbacks,						/* Number of emails sent to sysop */
			etoday,						/* Emails today */
			ptoday,						/* Posts today */
			timeon,						/* Total time on */
			textra,						/* Extra time for today */
			logons,						/* Total logons */
			ttoday,						/* Time on today */
			tlast,						/* Time on last call */
			ltoday, 					/* Logons today */
			xedit,						/* External editor (1 based) */
			shell;						/* Command shell */

	char	alias[LEN_ALIAS+1], 		/* Alias */
			name[LEN_NAME+1],			/* Name - Real */
			handle[LEN_HANDLE+1],		/* Chat handle */
			comp[LEN_COMP+1],			/* Computer type */
			note[LEN_NOTE+1],			/* Public notice about this user */
			address[LEN_ADDRESS+1], 	/* Street Address */
			location[LEN_LOCATION+1],	/* Location of user */
			zipcode[LEN_ZIPCODE+1], 	/* Zip/Postal code */
			pass[LEN_PASS+1],			/* Password - not case sensitive */
			birth[LEN_BIRTH+1], 		/* Birthday in MM/DD/YY format */
			phone[LEN_PHONE+1],			/* Phone number xxx-xxx-xxxx format */
			modem[LEN_MODEM+1],			/* Modem type - 8 chars max */
			netmail[LEN_NETMAIL+1], 	/* NetMail forwarding address */
			tmpext[4],					/* QWK Packet extension */
			comment[LEN_COMMENT+1], 	/* Private comment about user */
			cursub[LEN_EXTCODE+1],		/* Current sub-board internal code */
			curdir[LEN_EXTCODE+1],		/* Current directory internal code */
			curxtrn[9];					/* Current external program internal code */

	uchar	level,						/* Security level */
			sex,						/* Sex - M or F */
			rows,               		/* Rows of text */
			prot,						/* Default transfer protocol */
			leech;						/* Leech attempt counter */

	ulong	misc,						/* Misc. bits - ANSI, Deleted etc. */
			qwk,						/* QWK settings */
			chat,						/* Chat defaults */
			flags1, 					/* Flag set #1 */
			flags2, 					/* Flag set #2 */
			flags3, 					/* Flag set #3 */
			flags4, 					/* Flag set #4 */
			exempt,						/* Exemption Flags */
			rest,						/* Restriction Flags */
			ulb,						/* Total bytes uploaded */
			dlb,						/* Total bytes downloaded */
			cdt,						/* Credits */
			min,						/* Minutes */
			freecdt;					/* Free credits (renewed daily) */
	time32_t firston,					/* Date/Time first called */
			laston, 					/* Last logoff date/time */
			expire, 					/* Expiration date */
			pwmod,						/* Password last modified */
			ns_time,					/* Date/Time of last new file scan */
			logontime;					/* Time of current logon */

} user_t;

typedef struct {						/* File (transfers) Data */
	char    name[13],					/* Name of file FILENAME.EXT */
			desc[LEN_FDESC+1],			/* Uploader's Description */
			uler[LEN_ALIAS+1];			/* User who uploaded */
	uchar	opencount;					/* Times record is currently open */
	time32_t  date,						/* File date/time */
			dateuled,					/* Date/Time (Unix) Uploaded */
			datedled;					/* Date/Time (Unix) Last downloaded */
	uint16_t	dir,						/* Directory file is in */
			altpath,
			timesdled,					/* Total times downloaded */
			timetodl;					/* How long transfer time */
	int32_t	datoffset,					/* Offset into .DAT file */
			size,						/* Size of file */
			misc;						/* Miscellaneous bits */
	uint32_t	cdt;						/* Credit value for this file */

} file_t;

typedef idxrec_t post_t;				/* defined in smbdefs.h */
typedef idxrec_t mail_t;				/* defined in smbdefs.h */
typedef fidoaddr_t faddr_t;				/* defined in smbdefs.h */

typedef struct {						/* System/Node Statistics */
	uint32_t	logons,						/* Total Logons on System */
				ltoday,						/* Total Logons Today */
				timeon,						/* Total Time on System */
				ttoday,						/* Total Time Today */
				uls,						/* Total Uploads Today */
				ulb,						/* Total Upload Bytes Today */
				dls,						/* Total Downloads Today */
				dlb,						/* Total Download Bytes Today */
				ptoday,						/* Total Posts Today */
				etoday,						/* Total Emails Today */
				ftoday; 					/* Total Feedbacks Today */
	uint16_t	nusers; 					/* Total New Users Today */

} stats_t;

typedef struct {						/* Sub-board scan information */
	uint16_t	cfg;						/* User's configuration */
	uint32_t	ptr;						/* New-message pointer */
	uint32_t	last;						/* Last read message number */
	uint16_t	sav_cfg;					/* Saved configuration */
	uint32_t	sav_ptr;					/* Saved New-message pointer */
	uint32_t	sav_last;					/* Saved Last read message number */
} subscan_t;

#endif /* Don't add anything after this #endif statement */
