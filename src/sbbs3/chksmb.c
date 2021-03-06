/* chksmb.c */

/* Synchronet message base (SMB) validity checker */

/* $Id: chksmb.c,v 1.52 2012/10/24 19:03:13 deuce Exp $ */

/****************************************************************************
 * @format.tab-size 4		(Plain Text/Source Code File Header)			*
 * @format.use-tabs true	(see http://www.synchro.net/ptsc_hdr.html)		*
 *																			*
 * Copyright 2012 Rob Swindell - http://www.synchro.net/copyright.html		*
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

/* ANSI */
#include <stdio.h>	
#include <stdlib.h>		/* exit */
#include <string.h>		/* strrchr */
#include <time.h>		/* ctime */
#include <ctype.h>		/* toupper */

/* SMB-specific */
#include "genwrap.h"
#include "conwrap.h"	/* getch */
#include "dirwrap.h"	/* fexist */
#include "filewrap.h"	/* filelength */
#include "smblib.h"

/****************************************************************************/
/* Returns in 'string' a character representation of the number in l with   */
/* commas.																	*/
/****************************************************************************/
char *ultoac(ulong l, char *string)
{
	char str[256];
	signed char i,j,k;

	sprintf(str,"%lu",l);
	i=strlen(str)-1;
	j=i/3+1+i;
	string[j--]=0;
	for(k=1;i>-1;k++) {
		string[j--]=str[i--];
		if(j>0 && !(k%3))
			string[j--]=','; 
	}
	return(string);
}

/****************************************************************************/
/* Returns an ASCII string for FidoNet address 'addr'                       */
/****************************************************************************/
char *faddrtoa(fidoaddr_t addr)
{
	static char str[25];
	char point[25];

	sprintf(str,"%hu:%hu/%hu",addr.zone,addr.net,addr.node);
	if(addr.point) {
		sprintf(point,".%u",addr.point);
		strcat(str,point); 
	}
	return(str);
}

char* DLLCALL strip_ctrl(char *str)
{
	char tmp[1024];
	int i,j;

	for(i=j=0;str[i] && j<sizeof(tmp)-1;i++) {
		if(str[i]==CTRL_A && str[i+1]!=0)
			i++;
		else if((uchar)str[i]>=' ')
			tmp[j++]=str[i];
	}
	if(i!=j) {
		tmp[j]=0;
		strcpy(str,tmp);
	}
	return(str);
}

char *usage="\nusage: chksmb [-opts] <filespec.SHD>\n"
			"\n"
			" opts:\n"
			"       b - beep on error\n"
			"       s - stop after errored message base\n"
			"       p - pause after errored messsage base\n"
			"       h - don't check hash file\n"
			"       a - don't check allocation files\n"
			"       t - don't check translation strings\n"
			"       e - display extended info on corrupted msgs\n";

int main(int argc, char **argv)
{
	char		str[128],*p,*s,*beep="";
	char		from[26];
	char*		body;
	char*		tail;
	int 		h,i,j,x,y,lzh,errors,errlast;
	BOOL		stop_on_error=FALSE,pause_on_error=FALSE,chkxlat=TRUE,chkalloc=TRUE,chkhash=TRUE
				,lzhmsg,extinfo=FALSE,msgerr;
	uint16_t	xlat;
	uint32_t	m;
	ulong		l,n,length,size,total=0,orphan,deleted,headers
				,*offset,*number,xlaterr
				,delidx
				,delhdrblocks,deldatblocks,hdrerr,lockerr,hdrnumerr,hdrlenerr
				,getbodyerr,gettailerr
				,hasherr,badhash
				,acthdrblocks,actdatblocks
				,dfieldlength,dfieldoffset
				,dupenum,dupenumhdr,dupeoff,attr,actalloc
				,datactalloc,misnumbered,timeerr,idxofferr,idxerr
				,subjcrc,fromcrc,tocrc
				,intransit,unvalidated
				,zeronum,idxzeronum,idxnumerr,packable=0L,totallzhsaved=0L
				,totalmsgs=0,totallzhmsgs=0,totaldelmsgs=0,totalmsgbytes=0L
				,lzhblocks,lzhsaved;
	smb_t		smb;
	idxrec_t	idx;
	smbmsg_t	msg;
	hash_t**	hashes;
	char		revision[16];
	time_t		now=time(NULL);

	sscanf("$Revision: 1.52 $", "%*s %s", revision);

	fprintf(stderr,"\nCHKSMB v2.30-%s (rev %s) SMBLIB %s - Check Synchronet Message Base\n"
		,PLATFORM_DESC,revision,smb_lib_ver());

	if(argc<2) {
		printf("%s",usage);
		exit(1); 
	}

	errlast=errors=0;
	for(x=1;x<argc;x++) {
		if(stop_on_error && errors)
			break;
		if(pause_on_error && errlast!=errors) {
			fprintf(stderr,"\7\nHit any key to continue...");
			if(!getch())
				getch();
			printf("\n"); 
		}
		errlast=errors;
		if(argv[x][0]=='-'
#if !defined(__unix__)	/* just for backwards compatibility */
			|| argv[x][0]=='/'
#endif
			) {
			for(y=1;argv[x][y];y++)
				switch(toupper(argv[x][y])) {
					case 'Q':
						beep="";
						break;
					case 'B':
						beep="\a";
						break;
					case 'P':
						pause_on_error=TRUE;
						break;
					case 'S':
						stop_on_error=TRUE;
						break;
					case 'T':
						chkxlat=FALSE;
						break;
					case 'A':
						chkalloc=FALSE;
						break;
					case 'H':
						chkhash=FALSE;
						break;
					case 'E':
						extinfo=TRUE;
						break;
					default:
						printf("%s",usage);
						exit(1); 
			}
			continue; 
		}

	SAFECOPY(smb.file,argv[x]);
	p=strrchr(smb.file,'.');
	s=strrchr(smb.file,'\\');
	if(p>s) *p=0;

	sprintf(str,"%s.shd",smb.file);
	if(!fexist(str)) {
		printf("\n%s doesn't exist.\n",smb.file);
		continue; 
	}

	fprintf(stderr,"\nChecking %s Headers\n\n",smb.file);

	smb.retry_time=30;
	if((i=smb_open(&smb))!=0) {
		printf("smb_open returned %d: %s\n",i,smb.last_error);
		errors++;
		continue; 
	}

	length=filelength(fileno(smb.shd_fp));
	if(length<sizeof(smbhdr_t)) {
		printf("Empty\n");
		smb_close(&smb);
		continue; 
	}

	if((i=smb_locksmbhdr(&smb))!=0) {
		smb_close(&smb);
		printf("smb_locksmbhdr returned %d: %s\n",i,smb.last_error);
		errors++;
		continue; 
	}

	if((length/SHD_BLOCK_LEN)*sizeof(ulong)) {
		if((number=(ulong *)malloc(((length/SHD_BLOCK_LEN)+2)*sizeof(ulong)))
			==NULL) {
			printf("Error allocating %lu bytes of memory\n"
				,(length/SHD_BLOCK_LEN)*sizeof(ulong));
			return(++errors); 
		} 
	}
	else
		number=NULL;

	if(chkalloc && !(smb.status.attr&SMB_HYPERALLOC)) {
		if((i=smb_open_ha(&smb))!=0) {
			printf("smb_open_ha returned %d: %s\n",i,smb.last_error);
			return(++errors); 
		}

		if((i=smb_open_da(&smb))!=0) {
			printf("smb_open_da returned %d: %s\n",i,smb.last_error);
			return(++errors); 
		} 
	}

	headers=deleted=orphan=dupenumhdr=attr=zeronum=timeerr=lockerr=hdrerr=0;
	subjcrc=fromcrc=tocrc=0;
	hdrnumerr=hdrlenerr=0;
	actalloc=datactalloc=deldatblocks=delhdrblocks=xlaterr=0;
	lzhblocks=lzhsaved=acthdrblocks=actdatblocks=0;
	getbodyerr=gettailerr=0;
	hasherr=badhash=0;
	unvalidated=0;
	intransit=0;
	acthdrblocks=actdatblocks=0;
	dfieldlength=dfieldoffset=0;

	for(l=smb.status.header_offset;l<length;l+=size) {
		size=SHD_BLOCK_LEN;
		fprintf(stderr,"\r%2lu%%  ",(long)(100.0/((float)length/l)));
		fflush(stderr);
		memset(&msg,0,sizeof(msg));
		msg.idx.offset=l;
		msgerr=FALSE;
		if((i=smb_lockmsghdr(&smb,&msg))!=0) {
			printf("\n(%06lX) smb_lockmsghdr returned %d: %s\n",l,i,smb.last_error);
			lockerr++;
			headers++;
			continue; 
		}
		if((i=smb_getmsghdr(&smb,&msg))!=0) {
			smb_unlockmsghdr(&smb,&msg);
			if(chkalloc && !(smb.status.attr&SMB_HYPERALLOC)) {
				fseek(smb.sha_fp
					,(l-smb.status.header_offset)/SHD_BLOCK_LEN,SEEK_SET);
				j=fgetc(smb.sha_fp);
				if(j) { 			/* Allocated block or at EOF */
					printf("%s\n(%06lX) smb_getmsghdr returned %d: %s\n",beep,l,i,smb.last_error);
					hdrerr++; 
				}
				else
					delhdrblocks++; 
			}
			else {
				/* printf("%s\n(%06lX) smb_getmsghdr returned %d\n",beep,l,i); */
				delhdrblocks++; 
			}
			continue; 
		}
		smb_unlockmsghdr(&smb,&msg);
		size=smb_hdrblocks(smb_getmsghdrlen(&msg))*SHD_BLOCK_LEN;

		SAFECOPY(from,msg.from);
		truncsp(from);
		strip_ctrl(from);
		fprintf(stderr,"#%-5"PRIu32" (%06lX) %-25.25s ",msg.hdr.number,l,from);

		if(msg.hdr.length!=smb_getmsghdrlen(&msg)) {
			fprintf(stderr,"%sHeader length mismatch\n",beep);
			msgerr=TRUE;
			if(extinfo)
				printf("MSGERR: Header length (%hu) does not match calculcated length (%lu)\n"
					,msg.hdr.length,smb_getmsghdrlen(&msg));
			hdrlenerr++; 
		}

		/* Test reading of the message text (body and tails) */
		if(msg.hdr.attr&MSG_DELETE)
			body=tail=NULL;
		else {
			if((body=smb_getmsgtxt(&smb,&msg,GETMSGTXT_BODY_ONLY))==NULL) {
				fprintf(stderr,"%sGet text body failure\n",beep);
				msgerr=TRUE;
				if(extinfo)
					printf("MSGERR: %s\n", smb.last_error);
				getbodyerr++;
			}
			if((tail=smb_getmsgtxt(&smb,&msg,GETMSGTXT_TAIL_ONLY))==NULL) {
				fprintf(stderr,"%sGet text tail failure\n",beep);
				msgerr=TRUE;
				if(extinfo)
					printf("MSGERR: %s\n", smb.last_error);
				gettailerr++;
			}
		}

		if(!(smb.status.attr&SMB_EMAIL) && chkhash) {
			/* Look-up the message hashes */
			hashes=smb_msghashes(&msg,(uchar*)body,SMB_HASH_SOURCE_DUPE);
			if(hashes!=NULL 
				&& hashes[0]!=NULL 
				&& (i=smb_findhash(&smb,hashes,NULL,SMB_HASH_SOURCE_DUPE,/* mark */TRUE ))
					!=SMB_SUCCESS) {
				for(h=0;hashes[h]!=NULL;h++) {
					if(hashes[h]->flags&SMB_HASH_MARKED)
						continue;
					fprintf(stderr,"%sFailed to find %s hash\n"
						,beep,smb_hashsourcetype(hashes[h]->source));
					msgerr=TRUE;
					if(extinfo) {
						printf("MSGERR: %d searching for %s: %s\n"
							,i
							,smb_hashsourcetype(hashes[h]->source)
							,smb_hashsource(&msg,hashes[h]->source));
#ifdef _DEBUG
						printf("\n");
						printf("%-10s: %s\n",		"Source",	smb_hashsourcetype(hashes[h]->source));
						printf("%-10s: %"PRIu32"\n",		"Length",	hashes[h]->length);
						printf("%-10s: %x\n",		"Flags",	hashes[h]->flags);
						if(hashes[h]->flags&SMB_HASH_CRC16)
							printf("%-10s: %04x\n",	"CRC-16",	hashes[h]->crc16);
						if(hashes[h]->flags&SMB_HASH_CRC32)
							printf("%-10s: %08"PRIx32"\n","CRC-32",	hashes[h]->crc32);
						if(hashes[h]->flags&SMB_HASH_MD5)
							printf("%-10s: %s\n",	"MD5",		MD5_hex((BYTE*)str,hashes[h]->md5));

#endif
					}
					hasherr++;
				}
			}
			
			smb_close_hash(&smb);	/* just incase */

			FREE_LIST(hashes,i);
		}
		FREE_AND_NULL(body);
		FREE_AND_NULL(tail);

		lzhmsg=FALSE;
		if(msg.hdr.attr&MSG_DELETE) {
			deleted++;
			if(number)
				number[headers]=0;
			if(smb.status.attr&SMB_HYPERALLOC)
				deldatblocks+=smb_datblocks(smb_getmsgdatlen(&msg)); 
		}
		else {
			actdatblocks+=smb_datblocks(smb_getmsgdatlen(&msg));
			if(msg.hdr.number>smb.status.last_msg) {
				fprintf(stderr,"%sOut-Of-Range message number\n",beep);
				msgerr=TRUE;
				if(extinfo)
					printf("MSGERR: Header number (%"PRIu32") greater than last (%"PRIu32")\n"
						,msg.hdr.number,smb.status.last_msg);
				hdrnumerr++; 
			}
			if(smb_getmsgidx(&smb,&msg)) {
				fprintf(stderr,"%sNot found in index\n",beep);
				msgerr=TRUE;
				if(extinfo)
					printf("MSGERR: Header number (%"PRIu32") not found in index\n"
						,msg.hdr.number);
				orphan++; 
			}
			else {
				if(msg.hdr.attr!=msg.idx.attr) {
					fprintf(stderr,"%sAttributes mismatch\n",beep);
					msgerr=TRUE;
					if(extinfo)
						printf("MSGERR: Header attributes (%04X) do not match index "
							"attributes (%04X)\n"
							,msg.hdr.attr,msg.idx.attr);
					attr++; 
				}
				if(msg.hdr.when_imported.time!=msg.idx.time) {
					fprintf(stderr,"%sImport date/time mismatch\n",beep);
					msgerr=TRUE;
					if(extinfo)
						printf("MSGERR: Header import date/time does not match "
							"index import date/time\n");
					timeerr++; 
				}
				if(msg.idx.subj!=smb_subject_crc(msg.subj)) {
					fprintf(stderr,"%sSubject CRC mismatch\n",beep);
					msgerr=TRUE;
					if(extinfo)
						printf("MSGERR: Subject (%04X) does not match index "
							"CRC (%04X)\n"
							,smb_subject_crc(msg.subj),msg.idx.subj);
					subjcrc++; 
				}
				if(smb.status.attr&SMB_EMAIL 
					&& (msg.from_ext!=NULL || msg.idx.from) 
					&& (msg.from_ext==NULL || msg.idx.from!=atoi(msg.from_ext))) {
					fprintf(stderr,"%sFrom extension mismatch\n",beep);
					msgerr=TRUE;
					if(extinfo)
						printf("MSGERR: From extension (%s) does not match index "
							"(%u)\n"
							,msg.from_ext,msg.idx.from);
					fromcrc++; 
				}
				if(!(smb.status.attr&SMB_EMAIL) 
					&& msg.idx.from!=smb_name_crc(msg.from)) {
					fprintf(stderr,"%sFrom CRC mismatch\n",beep);
					msgerr=TRUE;
					if(extinfo)
						printf("MSGERR: From (%04X) does not match index "
							"CRC (%04X)\n"
							,smb_name_crc(msg.from),msg.idx.from);
					fromcrc++; 
				}
				if(smb.status.attr&SMB_EMAIL 
					&& (msg.to_ext!=NULL || msg.idx.to) 
					&& (msg.to_ext==NULL || msg.idx.to!=atoi(msg.to_ext))) {
					fprintf(stderr,"%sTo extension mismatch\n",beep);
					msgerr=TRUE;
					if(extinfo)
						printf("MSGERR: To extension (%s) does not match index "
							"(%u)\n"
							,msg.to_ext,msg.idx.to);
					tocrc++; 
				}
				if(!(smb.status.attr&SMB_EMAIL) 
					&& msg.to_ext==NULL && msg.idx.to!=smb_name_crc(msg.to)) {
					fprintf(stderr,"%sTo CRC mismatch\n",beep);
					msgerr=TRUE;
					if(extinfo)
						printf("MSGERR: To (%04X) does not match index "
							"CRC (%04X)\n"
							,smb_name_crc(msg.to),msg.idx.to);
					tocrc++; 
				}
				if(msg.hdr.netattr&MSG_INTRANSIT) {
					fprintf(stderr,"%sIn transit\n",beep);
					msgerr=TRUE;
					if(extinfo)
						printf("MSGERR: Flagged 'in transit'\n");
					intransit++;
				}
				if((msg.hdr.attr&(MSG_MODERATED|MSG_VALIDATED)) == MSG_MODERATED) {
					fprintf(stderr,"%sUnvalidated\n",beep);
					msgerr=TRUE;
					if(extinfo)
						printf("MSGERR: Flagged 'moderated', but not yet 'validated'\n");
					unvalidated++;
				}
			}
			if(msg.hdr.number==0) {
				fprintf(stderr,"%sZero message number\n",beep);
				msgerr=TRUE;
				if(extinfo)
					printf("MSGERR: Header number is zero (invalid)\n");
				zeronum++; 
			}
			if(number) {
				for(m=0;m<headers;m++)
					if(number[m] && msg.hdr.number==number[m]) {
						fprintf(stderr,"%sDuplicate message number\n",beep);
						msgerr=TRUE;
						if(extinfo)
							printf("MSGERR: Header number (%"PRIu32") duplicated\n"
								,msg.hdr.number);
						dupenumhdr++;
						break; 
					}
				number[headers]=msg.hdr.number; 
			}
			if(chkxlat) {		/* Check translation strings */
				for(i=0;i<msg.hdr.total_dfields;i++) {
					fseek(smb.sdt_fp,msg.hdr.offset+msg.dfield[i].offset,SEEK_SET);
					if(!fread(&xlat,2,1,smb.sdt_fp))
						xlat=0xffff;
					lzh=0;
					if(xlat==XLAT_LZH) {
						lzh=1;
						if(!fread(&xlat,2,1,smb.sdt_fp))
							xlat=0xffff; 
					}
					if(xlat!=XLAT_NONE) {
						fprintf(stderr,"%sUnsupported Xlat %04X dfield[%u]\n"
							,beep,xlat,i);
						msgerr=TRUE;
						if(extinfo)
							printf("MSGERR: Unsupported translation type (%04X) "
								"in dfield[%u] (offset %"PRIu32")\n"
								,xlat,i,msg.dfield[i].offset);
						xlaterr++; 
					}
					else {
						if(lzh) {
							lzhmsg=TRUE;
							if(fread(&m,4,1,smb.sdt_fp)) { /* Get uncompressed len */
								lzhsaved+=(smb_datblocks(m+2)
									-smb_datblocks(msg.dfield[i].length))
									*SDT_BLOCK_LEN;
								lzhblocks+=smb_datblocks(msg.dfield[i].length);
							} 
						} 
					} 
				} 
			} 
		}

		if(chkalloc && !(smb.status.attr&SMB_HYPERALLOC)) {
			fseek(smb.sha_fp,(l-smb.status.header_offset)/SHD_BLOCK_LEN,SEEK_SET);
			for(m=0;m<size;m+=SHD_BLOCK_LEN) {
	/***
				if(msg.hdr.attr&MSG_DELETE && (i=fgetc(smb.sha_fp))!=0) {
					fprintf(stderr,"%sDeleted Header Block %lu marked %02X\n"
						,beep,m/SHD_BLOCK_LEN,i);
					msgerr=TRUE;
					delalloc++; 
					}
	***/
				if(!(msg.hdr.attr&MSG_DELETE) && (i=fgetc(smb.sha_fp))!=1) {
					fprintf(stderr,"%sActive Header Block %"PRIu32" marked %02X\n"
						,beep,m/SHD_BLOCK_LEN,i);
					msgerr=TRUE;
					if(extinfo)
						printf("MSGERR: Active header block %"PRIu32" marked %02X "
							"instead of 01\n"
							,m/SHD_BLOCK_LEN,i);
					actalloc++; 
				} 
			}

			if(!(msg.hdr.attr&MSG_DELETE)) {
				acthdrblocks+=(size/SHD_BLOCK_LEN);
				for(n=0;n<msg.hdr.total_dfields;n++) {
					if(msg.dfield[n].offset&0x80000000UL) {
						msgerr=TRUE;
						if(extinfo)
							printf("MSGERR: Invalid Data Field [%lu] Offset: %"PRIu32"\n"
								,n,msg.dfield[n].offset);
						dfieldoffset++; 
					}
					if(msg.dfield[n].length&0x80000000UL) {
						msgerr=TRUE;
						if(extinfo)
							printf("MSGERR: Invalid Data Field [%lu] Length: %"PRIu32"\n"
								,n,msg.dfield[n].length);
						dfieldlength++; 
					}
					fseek(smb.sda_fp
						,((msg.hdr.offset+msg.dfield[n].offset)/SDT_BLOCK_LEN)*2
						,SEEK_SET);
					for(m=0;m<msg.dfield[n].length;m+=SDT_BLOCK_LEN) {
						/* TODO: LE Only */
						i=0;
						if(!fread(&i,2,1,smb.sda_fp) || !i) {
							fprintf(stderr
								,"%sActive Data Block %lu.%"PRIu32" marked free\n"
								,beep,n,m/SHD_BLOCK_LEN);
							msgerr=TRUE;
							if(extinfo)
								printf("MSGERR: Active Data Block %lu.%"PRIu32" "
									"marked free\n"
									,n,m/SHD_BLOCK_LEN);
							datactalloc++; 
						} 
					} 
				} 
			}
			else
				delhdrblocks+=(size/SHD_BLOCK_LEN); 
		}

		else {	 /* Hyper Alloc */
			if(msg.hdr.attr&MSG_DELETE)
				delhdrblocks+=(size/SHD_BLOCK_LEN);
			else
				acthdrblocks+=(size/SHD_BLOCK_LEN); 
		}

		totallzhmsgs+=lzhmsg;
		headers++;
		if(msgerr && extinfo) {
			printf("\n");
			printf("%-20s %s\n","message base",smb.file);
			smb_dump_msghdr(stdout,&msg);
			printf("\n"); 
		}

		smb_freemsgmem(&msg); 
	}

	FREE_AND_NULL(number);

	fprintf(stderr,"\r%79s\r100%%\n","");

	if(chkalloc && !(smb.status.attr&SMB_HYPERALLOC)) {

		fprintf(stderr,"\nChecking %s Data Blocks\n\n",smb.file);

		length=filelength(fileno(smb.sda_fp));

		fseek(smb.sda_fp,0L,SEEK_SET);
		for(l=0;l<length;l+=2) {
			if((l%10)==0)
				fprintf(stderr,"\r%2lu%%  ",l ? (long)(100.0/((float)length/l)) : 0);
			/* TODO: LE Only */
			i=0;
			if(!fread(&i,2,1,smb.sda_fp))
				break;
			if(!i)
				deldatblocks++; 
		}

		smb_close_ha(&smb);
		smb_close_da(&smb);

		fprintf(stderr,"\r%79s\r100%%\n",""); 
	}

	total=filelength(fileno(smb.sid_fp))/sizeof(idxrec_t);

	dupenum=dupeoff=misnumbered=idxzeronum=idxnumerr=idxofferr=idxerr=delidx=0;

	if(total) {

	fprintf(stderr,"\nChecking %s Index\n\n",smb.file);

	if((offset=(ulong *)malloc(total*sizeof(ulong)))==NULL) {
		printf("Error allocating %lu bytes of memory\n",total*sizeof(ulong));
		return(++errors); 
	}
	if((number=(ulong *)malloc(total*sizeof(ulong)))==NULL) {
		printf("Error allocating %lu bytes of memory\n",total*sizeof(ulong));
		return(++errors); 
	}
	fseek(smb.sid_fp,0L,SEEK_SET);

	for(l=0;l<total;l++) {
		fprintf(stderr,"\r%2lu%%  %5lu ",l ? (long)(100.0/((float)total/l)) : 0,l);
		if(!fread(&idx,sizeof(idxrec_t),1,smb.sid_fp))
			break;
		fprintf(stderr,"#%-5"PRIu32" (%06"PRIX32") 1st Pass ",idx.number,idx.offset);
		if(idx.attr&MSG_DELETE) {
			/* Message Disabled... why?  ToDo */
			/* fprintf(stderr,"%sMarked for deletion\n",beep); */
			delidx++; 
		}
		for(m=0;m<l;m++)
			if(number[m]==idx.number) {
				fprintf(stderr,"%sDuplicate message number\n",beep);
				dupenum++;
				break; 
			}
		for(m=0;m<l;m++)
			if(offset[m]==idx.offset) {
				fprintf(stderr,"%sDuplicate offset: %"PRIu32"\n",beep,idx.offset);
				dupeoff++;
				break; 
			}
		if(idx.offset<smb.status.header_offset) {
			fprintf(stderr,"%sInvalid offset\n",beep);
			idxofferr++;
			break; 
		}
		if(idx.number==0) {
			fprintf(stderr,"%sZero message number\n",beep);
			idxzeronum++;
			break; 
		}
		if(idx.number>smb.status.last_msg) {
			fprintf(stderr,"%sOut-Of-Range message number\n",beep);
			idxnumerr++;
			break; 
		}
		number[l]=idx.number;
		offset[l]=idx.offset; 
	}

	if(l<total) {
		fprintf(stderr,"%sError reading index record\n",beep);
		idxerr=1; 
	}
	else {
		fprintf(stderr,"\r%79s\r","");
		for(m=0;m<total;m++) {
			fprintf(stderr,"\r%2lu%%  %5"PRIu32" ",m ? (long)(100.0/((float)total/m)) : 0,m);
			fprintf(stderr,"#%-5lu (%06lX) 2nd Pass ",number[m],offset[m]);
			for(n=0;n<m;n++)
				if(number[m] && number[n] && number[m]<number[n]) {
					fprintf(stderr,"%sMisordered message number\n",beep);
					misnumbered++;
					number[n]=0;
					break; 
				} 
		}
		fprintf(stderr,"\r%79s\r100%%\n",""); 
	}
	FREE_AND_NULL(number);
	FREE_AND_NULL(offset);

	}	/* if(total) */

	if(!(smb.status.attr&SMB_EMAIL) && chkhash && smb_open_hash(&smb)==SMB_SUCCESS) {
		hash_t hash;

		fprintf(stderr,"\nChecking %s Hashes\n\n",smb.file);

		length=filelength(fileno(smb.hash_fp));

		fseek(smb.hash_fp,0L,SEEK_SET);
		for(l=0;l<length;l+=sizeof(hash_t)) {
			if(((l/sizeof(hash_t))%10)==0)
				fprintf(stderr,"\r%2lu%%  ",l ? (long)(100.0/((float)length/l)) : 0);
			if(!fread(&hash,sizeof(hash),1,smb.hash_fp))
				break;
			if(hash.number==0 || hash.number > smb.status.last_msg)
				fprintf(stderr,"\r%sInvalid message number (%u)\n", beep, hash.number), badhash++;
			else if(hash.time < 0x40000000 || hash.time > (ulong)now)
				fprintf(stderr,"\r%sInvalid time (0x%08"PRIX32")\n", beep, hash.time), badhash++;
			else if(hash.length < 1 || hash.length > 1024*1024)
				fprintf(stderr,"\r%sInvalid length (%"PRIu32")\n", beep, hash.length), badhash++;
			else if(hash.source >= SMB_HASH_SOURCE_TYPES)
				fprintf(stderr,"\r%sInvalid source type (%u)\n", beep, hash.source), badhash++;
		}

		smb_close_hash(&smb);

		fprintf(stderr,"\r%79s\r100%%\n",""); 
	}


	totalmsgs+=smb.status.total_msgs;
	totalmsgbytes+=(acthdrblocks*SHD_BLOCK_LEN)+(actdatblocks*SDT_BLOCK_LEN);
	totaldelmsgs+=deleted;
	totallzhsaved+=lzhsaved;
	printf("\n");
	printf("%-35.35s (=): %"PRIu32"\n"
		,"Status Total"
		,smb.status.total_msgs);
	printf("%-35.35s (=): %lu\n"
		,"Total Indexes"
		,total);
	printf("%-35.35s (=): %lu\n"
		,"Active Indexes"
		,total-delidx);
	printf("%-35.35s (=): %lu\n"
		,"Active Headers"
		,headers-deleted);
	printf("%-35.35s ( ): %-8lu %13s bytes used\n"
		,"Active Header Blocks"
		,acthdrblocks,ultoac(acthdrblocks*SHD_BLOCK_LEN,str));
	printf("%-35.35s ( ): %-8lu %13s bytes used\n"
		,"Active Data Blocks"
		,actdatblocks,ultoac(actdatblocks*SDT_BLOCK_LEN,str));
	if(lzhblocks)
		printf("%-35.35s ( ): %-8lu %13s bytes saved\n"
			,"Active LZH Compressed Data Blocks"
			,lzhblocks,ultoac(lzhsaved,str));
	printf("%-35.35s ( ): %lu\n"
		,"Header Records"
		,headers);
	printf("%-35.35s ( ): %lu\n"
		,"Deleted Indexes"
		,delidx);
	printf("%-35.35s ( ): %lu\n"
		,"Deleted Headers"
		,deleted);
	printf("%-35.35s ( ): %-8lu %13s bytes used\n"
		,"Deleted Header Blocks"
		,delhdrblocks,ultoac(delhdrblocks*SHD_BLOCK_LEN,str));
	packable+=(delhdrblocks*SHD_BLOCK_LEN);
	printf("%-35.35s ( ): %-8lu %13s bytes used\n"
		,"Deleted Data Blocks"
		,deldatblocks,ultoac(deldatblocks*SDT_BLOCK_LEN,str));
	packable+=(deldatblocks*SDT_BLOCK_LEN);

	if(orphan)
		printf("%-35.35s (!): %lu\n"
			,"Orphaned Headers"
			,orphan);
	if(idxzeronum)
		printf("%-35.35s (!): %lu\n"
			,"Zeroed Index Numbers"
			,idxzeronum);
	if(zeronum)
		printf("%-35.35s (!): %lu\n"
			,"Zeroed Header Numbers"
			,zeronum);
	if(idxofferr)
		printf("%-35.35s (!): %lu\n"
			,"Invalid Index Offsets"
			,idxofferr);
	if(dupenum)
		printf("%-35.35s (!): %lu\n"
			,"Duplicate Index Numbers"
			,dupenum);
	if(dupeoff)
		printf("%-35.35s (!): %lu\n"
			,"Duplicate Index Offsets"
			,dupeoff);
	if(dupenumhdr)
		printf("%-35.35s (!): %lu\n"
			,"Duplicate Header Numbers"
			,dupenumhdr);
	if(misnumbered)
		printf("%-35.35s (!): %lu\n"
			,"Misordered Index Numbers"
			,misnumbered);
	if(lockerr)
		printf("%-35.35s (!): %lu\n"
			,"Unlockable Header Records"
			,lockerr);
	if(hdrerr)
		printf("%-35.35s (!): %lu\n"
			,"Unreadable Header Records"
			,hdrerr);
	if(idxnumerr)
		printf("%-35.35s (!): %lu\n"
			,"Out-Of-Range Index Numbers"
			,idxnumerr);
	if(hdrnumerr)
		printf("%-35.35s (!): %lu\n"
			,"Out-Of-Range Header Numbers"
			,hdrnumerr);
	if(hdrlenerr)
		printf("%-35.35s (!): %lu\n"
			,"Mismatched Header Lengths"
			,hdrlenerr);
#define INDXERR "Index/Header Mismatch: "
	if(attr)
		printf("%-35.35s (!): %lu\n"
			,INDXERR "Attributes"
			,attr);
	if(timeerr)
		printf("%-35.35s (!): %lu\n"
			,INDXERR "Import Time"
			,timeerr);
	if(subjcrc)
		printf("%-35.35s (!): %lu\n"
			,INDXERR "Subject CRCs"
			,subjcrc);
	if(fromcrc)
		printf("%-35.35s (!): %lu\n"
			,smb.status.attr&SMB_EMAIL ? INDXERR "From Ext" : INDXERR "From CRCs"
			,fromcrc);
	if(tocrc)
		printf("%-35.35s (!): %lu\n"
			,smb.status.attr&SMB_EMAIL ? INDXERR "To Ext" : INDXERR "To CRCs"
			,tocrc);
	if(intransit)
		printf("%-35.35s (?): %lu\n"
			,"Message flagged as 'In Transit'"
			,intransit);
	if(unvalidated)
		printf("%-35.35s (?): %lu\n"
			,"Moderated message not yet validated"
			,unvalidated);
	if(getbodyerr)
		printf("%-35.35s (!): %lu\n"
			,"Message Body Text Read Failures"
			,getbodyerr);
	if(gettailerr)
		printf("%-35.35s (!): %lu\n"
			,"Message Tail Text Read Failures"
			,gettailerr);
	if(xlaterr)
		printf("%-35.35s (!): %lu\n"
			,"Unsupported Translation Types"
			,xlaterr);
	if(hasherr)
		printf("%-35.35s (!): %lu\n"
			,"Missing Hash Records"
			,hasherr);
	if(datactalloc)
		printf("%-35.35s (!): %lu\n"
			,"Misallocated Active Data Blocks"
			,datactalloc);
	if(actalloc)
		printf("%-35.35s (!): %lu\n"
			,"Misallocated Active Header Blocks"
			,actalloc);
	/***
	if(delalloc)
		printf("%-35.35s (!): %lu\n"
			,"Misallocated Deleted Header Blocks"
			,delalloc);
	***/

	if(dfieldoffset)
		printf("%-35.35s (!): %lu\n"
			,"Invalid Data Field Offsets"
			,dfieldoffset);

	if(dfieldlength)
		printf("%-35.35s (!): %lu\n"
			,"Invalid Data Field Lengths"
			,dfieldlength);

	if(badhash)
		printf("%-35.35s (!): %lu\n"
			,"Invalid Hash Entries"
			,badhash);

	printf("\n%s Message Base ",smb.file);
	if(/* (headers-deleted)!=smb.status.total_msgs || */
		total!=smb.status.total_msgs
		|| (headers-deleted)!=total-delidx
		|| idxzeronum || zeronum
		|| hdrlenerr || hasherr || badhash
		|| getbodyerr || gettailerr
		|| orphan || dupenumhdr || dupenum || dupeoff || attr
		|| lockerr || hdrerr || hdrnumerr || idxnumerr || idxofferr
		|| actalloc || datactalloc || misnumbered || timeerr 
		|| intransit || unvalidated
		|| subjcrc || fromcrc || tocrc
		|| dfieldoffset || dfieldlength || xlaterr || idxerr) {
		printf("%shas Errors!\n",beep);
		errors++; 
	}
	else
		printf("is OK\n");

	smb_unlocksmbhdr(&smb);
	smb_close(&smb);
	}

	if((totalmsgs && (totalmsgs!=smb.status.total_msgs || totallzhmsgs))
		|| packable)
		printf("\n");
	if(totalmsgs && totalmsgs!=smb.status.total_msgs)
		printf("%-39.39s: %-8lu %13s bytes used\n"
			,"Total Active Messages"
			,totalmsgs,ultoac(totalmsgbytes,str));
	if(totallzhmsgs && totalmsgs!=smb.status.total_msgs)
		printf("%-39.39s: %-8lu %13s bytes saved\n"
			,"Total LZH Compressed Messages"
			,totallzhmsgs,ultoac(totallzhsaved,str));
	if(packable)
		printf("%-39.39s: %-8lu %13s bytes used\n"
			,"Total Deleted Messages"
			,totaldelmsgs,ultoac(packable,str));

	if(pause_on_error && errlast!=errors) {
		fprintf(stderr,"\7\nHit any key to continue...");
		if(!getch())
			getch();
		fprintf(stderr,"\n"); 
	}

	if(errors)
		printf("\n'fixsmb' can be used to repair most message base problems.\n");

	return(errors);
}
