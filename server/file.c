    /*********************************************************************\
    *  Copyright (c) 2003 by Radim Kolar (hsn@cybermail.net)              *
    *  Copyright (c) 1991 by Wen-King Su (wen-king@vlsi.cs.caltech.edu)   *
    *                                                                     *
    *  You may copy or modify this file in any manner you wish, provided  *
    *  that this notice is always included, and that you hold the author  *
    *  harmless for any loss or damage resulting from the installation or *
    *  use of this software.                                              *
    \*********************************************************************/

#include "tweak.h"
#include "server_def.h"
#include "s_extern.h"
#ifdef STDC_HEADERS
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_UTIME_H
#include <utime.h>
#endif
#include "my-string.h"
#include "fifocache.h"

/****************************************************************************
* This is a modified server_file that uses cache for directory listings and
* directory status information.                                             *
*****************************************************************************/

static struct FifoCache *dirlistcache;
static struct FifoCache *dirstatcache;
/* open file handles cache */
static struct FifoCache *fpcache;

#define FOURGIGS 0xffffffffUL
#define TWOGIGS  0x7fffffffUL

static FPCACHE *search_fpcache PROTO3(unsigned long, inet_num,
	                              unsigned short, port_num, 
				      const char *, fname)
{
  unsigned int i;
  FPCACHE *entry;
  const char **key;
  
  for (i=0;i<fp_cache_limit;i++) 
  { 
      /* search for file in cache */
    entry=(FPCACHE *)(fpcache->e_head+i*sizeof(FPCACHE));
    if (port_num==entry->port_num && inet_num==entry->inet_num)
    {
	key=f_cache_get_key(fpcache,entry);
	if(!strcmp(fname,*key))
	{
		/* file found in cache, return cache-pointer */
	        fpcache->hit++;
                return entry;
	}
    }
  }
  fpcache->miss++;
  return((FPCACHE *)0);
}

static void fpcache_free_entry PROTO1(void *,entry)
{
    FPCACHE *f=entry;

    if(f->fp)
	fclose(f->fp);
}

static unsigned int fpcache_entry_profiler PROTO1(void *,entry)
{
    FPCACHE *f=entry;

    if(f->fp)
	return sizeof(FILE);
    else
	return 0;
}


static void dirlistcache_free_entry PROTO1(void *, entry)
{
    DIRLISTING *d=entry;
    
    if(d->listing)
	free(d->listing);
}

static unsigned int dirlistcache_entry_profiler PROTO1(void *,entry)
{
    DIRLISTING *d=entry;

    return d->listing_size;
}

static void dirstatcache_free_entry PROTO1(void *, entry)
{
    DIRINFO *d=entry;
    
    if(d->realname)
	free(d->realname);
    if(d->owner_password)
	free(d->owner_password);
    if(d->public_password)
	free(d->public_password);
    if(d->owner)
	free_ip_table(d->owner);
    if(d->readme)
	free(d->readme);
}

static unsigned int dirstatcache_entry_profiler  PROTO1(void *,entry)
{
   DIRINFO *d=entry;
   unsigned int res=0;
   if(d->realname)
   {
       res=strlen(d->realname);
       res++;
   }
    if(d->owner_password)
    {
	res+=strlen(d->owner_password);
	res++;
    }
    if(d->public_password)
    {
	res+=strlen(d->public_password);
	res++;
    }
    /*
    if(d->owner)
	free_ip_table(d->owner);
	*/
    if(d->readme)
    {
	res+=strlen(d->readme);
	res++;
    }

   return res;
}

static void string_free PROTO1(void *, entry)
{
    char **s=entry;
    if(*s!=NULL)
	free(*s);
}

static unsigned int string_profiler PROTO1(void *,entry)
{
    char **s=entry;
   
    if(*s!=NULL)
	return(strlen(*s));
    else
	return 0;
}

static int string_compare PROTO2(const void *,e1,const void *,e2)
{

    char *const *s1=e1;
    char *const *s2=e2;

    /* strcmp do not likes NULLs */
    if(*s1 && *s2)
    {
       return strcmp(*s1,*s2);
    }else
	return 1;
}

/* should init all types of caches in future */
int init_caches PROTO0((void))
{
   dirlistcache=f_cache_new(dir_cache_limit,sizeof(DIRLISTING),dirlistcache_free_entry,sizeof(char *),string_free,string_compare);
   dirstatcache=f_cache_new(stat_cache_limit,sizeof(DIRINFO),dirstatcache_free_entry,sizeof(char *),string_free,string_compare);
   fpcache=f_cache_new(fp_cache_limit,sizeof(FPCACHE),fpcache_free_entry,sizeof(char *),string_free,string_compare);

   if(dirlistcache && dirstatcache && fpcache)
   {
       f_cache_set_memory_profilers(dirlistcache,string_profiler,dirlistcache_entry_profiler);
       f_cache_set_memory_profilers(dirstatcache,string_profiler,dirstatcache_entry_profiler);
       f_cache_set_memory_profilers(fpcache,string_profiler,fpcache_entry_profiler);
       return 0;
   }
   else
   {
       f_cache_destroy(dirlistcache);
       f_cache_destroy(dirstatcache);
       f_cache_destroy(fpcache);
       return -1;
   }
}

void stat_caches PROTO1(FILE *,fp)
{
          if(fp==NULL) fp=stderr;
 	  fprintf(fp,"DIRLISTCACHE ");
 	  f_cache_stats(dirlistcache,fp);
 	  fprintf(fp,"DIRSTATCACHE ");
 	  f_cache_stats(dirstatcache,fp);
 	  fprintf(fp,"FPCACHE ");
	  f_cache_stats(fpcache,fp);
}

/* should init all types of caches in future */
void shutdown_caches PROTO0((void))
{
    fprintf(stderr,"DIRLISTCACHE ");
    f_cache_stats(dirlistcache,stderr);
    f_cache_clear(dirlistcache);
    f_cache_destroy(dirlistcache);
    fprintf(stderr,"DIRSTATCACHE ");
    f_cache_stats(dirstatcache,stderr);
    f_cache_clear(dirstatcache);
    f_cache_destroy(dirstatcache);
    fprintf(stderr,"FPCACHE ");
    f_cache_stats(fpcache,stderr);
    f_cache_clear(fpcache);
    f_cache_destroy(fpcache);
}

/*****************************************************************************
 * Routine to parse a path string given by the client.
 * Will replace null string by ".".
 * In case of error, returns the error string.
 *
 *  The PPATH structure is filled in by the function check_path when given a
 *  path string.  The elements are filled in as such:
 * 
 *	fullp      pointer to a string containing the full path name
 *	f_ptr      pointer to begining of the last component of the path
 *      f_len      length of the last component of the path
 *	d_ptr      pointer to begining of the directory part of path
 *      d_len      length of the directory part of the path
 *      passwd     ptr to password
 *
 *  fullp is a null-terminated full path string.
 *  f_ptr is always a null-terminated sub-string of fullp.
 *  d_ptr is generally not null-terminated.
 *****************************************************************************/

static const char *parse_path PROTO3(char *, fullp, unsigned int, len, PPATH *, pp)
{
  char *s;
  int state;

  if(len < 1) return("Path must have non-zero length");
  if(fullp[len-1]) return("Path not null terminated");
  
  pp->d_ptr = "."; pp->d_len = 1;	/* initial dir part ---> root */
  pp->passwd = NULL; 			/* default, no password */
  
  if(len == 1 && fullp[0] == 0)	{ /* null path --> root */
    pp->fullp = pp->f_ptr = ".";
    pp->f_len = 1;
    return(NULLP);
  }
  
  for(s = pp->fullp = pp->f_ptr = fullp, state = 0; *s; s++) {
    if(*s == '\n') {
      pp->passwd = s+1; 
      *s = '\0';
      if(dbug) fprintf(stderr,"parse_path: found password field %s\n", s+1);
    }
    else if(*s < ' ' || *s >= '~') return("Path contains illegal chars");
      
    switch(*s) {
      case '\\':
      case '.':
        if(state==0) return("Path can't begin with '.' or '\\'");
	break;
      case '/':
	if(state!=0)
	{
          pp->d_ptr=fullp;
	  pp->d_len=s-fullp;
	  pp->f_ptr=s+1;
	}
      default:
	state = 1;
	break;
    }
  }
  
  pp->f_len = s - pp->f_ptr;
  return(NULLP);
}


/*****************************************************************************
 * Validate path - check that path does not fall outside the bounds of the
 * FSP root directory, weed out references to / and ../.. type links.    
 * Input:     fullp - pointer to full filename
 *         lenfullp - length of full filename (including \0)
 *         *di      - where to return DIRINFO information about directory
 *         want_directory - want to operate on directory, not a file 
 *****************************************************************************/
const char *validate_path PROTO5(char *, fullp, unsigned, lenfullp, PPATH *, pp,DIRINFO **,di, int, want_directory)
{ 
    char work [NBSIZE];
    const char *err;
    char *s;
    struct stat sd;
    DIRINFO newdir;

    if(dbug)
    {
 	  fprintf(stderr,"Validating path = %s len=%u\n",lenfullp>0? fullp:NULL,lenfullp);
    }
    /* split fullpath into parts */
    if( (err=parse_path(fullp,lenfullp,pp)) ) return err;
    if(want_directory)
    {
	/* everything in path is a directory */
	pp->d_ptr=pp->fullp;
	pp->d_len=pp->d_len+pp->f_len+1;
    }
    /* extract dir from path to work */
    memcpy(work,pp->d_ptr,pp->d_len);
    work[pp->d_len]='\0';
    if(dbug)
    {
        fprintf(stderr,"looking for directory %s in statcache.\n",work);
    }
    err=work;
    *di=f_cache_find(dirstatcache,&err);
    if(*di)
    {
	if(dbug) fprintf(stderr,"hit!\n");
	/* need to recheck a cached directory? */
	if((*di)->lastcheck+stat_cache_timeout>cur_time)
	{
	    if(dbug) fprintf(stderr,"...and fresh\n");
	    return NULL; /* no, we have fresh cache entry */
	}
	else
	    if(dbug)
		fprintf(stderr,"...but stale\n");
    }
    /* try to stat directory */
    if(FSP_STAT(work,&sd))
	return("No such directory");
    if(!S_ISDIR(sd.st_mode))
	return("Not a directory");
    /* recheck directory */
    if(*di)
    {
	if(use_directory_mtime && sd.st_mtime==(*di)->mtime)
	{
	    /* rechecked ok */
	    (*di)->lastcheck=cur_time;
	    return NULL;
	} else
	{
	    /* throw out old directory */
	    f_cache_delete_entry(dirstatcache,*di);
	    *di=NULL;
	}
    }

   /* build a new directory info */
  if (chdir(work))
      return("Can't change directory");
  /* save fixed input directory to err */
  s=strdup(work);
  if(!getcwd(work,NBSIZE))
  {
      chdir(home_dir);
      free(s);
      return("Can't get current directory");
  }
  /* check namespace */
  if(homedir_restricted)
      if (strncmp(work,home_dir,strlen(home_dir))) {
	  chdir(home_dir);
	  free(s);
	  return("Not following link out of restricted area");
      }
  /*  init. dir structure */
  newdir.realname=strdup(work);
  if(!newdir.realname)
  {
      free(s);
      chdir(home_dir);
      return("Memory exhausted");
  }
  if(use_directory_mtime)
      newdir.mtime=sd.st_mtime;
  else
      newdir.mtime=cur_time;
  newdir.lastcheck=cur_time;
  /* load access perms */
  load_access_rights(&newdir);
  chdir(home_dir);
  /* put new record into cache */
  if(dbug) fprintf(stderr,"putting into statcache: %s\n",s);
  *di=f_cache_put(dirstatcache,&s,&newdir);
  return NULLP;
}

/* copy file : from -> to */
static const char *copy_file PROTO2(const char *, n1, const char *, n2)
{
    FILE *ft,*fp;
    size_t bytes;
    char buf[1024];

  if(!(ft = fopen(n1,"rb"))) {
    return("Can't open temporary file");
  }
    
  if(!(fp = fopen(n2,"wb"))) {
    fclose(ft);
    return("Can't open file for output");

  }
  /* copy temporary file to actual fput file */
  while( (bytes = fread(buf,1,sizeof(buf),ft)))
      fwrite(buf,1,bytes,fp);
    
  fclose(ft);
  fclose(fp);
  return NULL;
}

/* appends new packet to directory listing, return 0 on success */
static int append_dir_listing PROTO3(DIRLISTING *, dl,const char *, buf,unsigned int, size)
{
      BYTE *newbuf;
      
      /* append this buffer */
      newbuf=realloc(dl->listing,dl->listing_size+size);
      if(newbuf==NULL)
      {
	  if(dl->listing) free(dl->listing);
	  dl->listing=NULL;
	  return -1;
      }
      memcpy(newbuf+dl->listing_size,buf,size);
      dl->listing_size+=size;
      dl->listing=newbuf;
      
      return 0;
}

/* builds directory listing into DIRLISTING structure, in case of any
 * error. nulls dl->listing 
 */
static void build_dir_listing PROTO2(DIRLISTING *, dl,const char *,directory)
{
  int skip;            /* how many bytes skip to next 4byte boundary */
  unsigned int rem;    /* remaining free space in UDP data space */
  DIR *dir_f;          /* opened directory for listing */
  struct dirent *dp;   /* record in directory */
  struct stat    sb;   /* stat data of record */
  register char  *s;   /* pointer to filename */
  size_t nlen;         /* filename length including zero terminator */
  char buffer[UBUF_SPACE]; /* buffer for building packet */
  char name[NBSIZE];   /* buffer for stat name */
  int namelen;         /* directory name length */
  unsigned int bufpos; /* current write pos. in buffer */
  
  /* init pointers */
  dl->listing=NULL;
  dl->listing_size=0;
  bufpos=0;

  if(!(dir_f = opendir(directory)))  {
    fprintf(stderr,"Can't open dir during listing initialization\n");
    return;
  }
  
  memset(buffer,0,packetsize); /* clear memory on the stack */
  strcpy(name,directory); 
  namelen=strlen(directory);
  name[namelen++]='/'; /* add directory separator to name */
  
  for(rem = packetsize; (dp = readdir(dir_f)); ) {
    if (dp->d_ino == 0) continue;
    s = dp->d_name;
	  
    /* hide dot files, but allow . or .. */
    if((s[0]=='.') && ((s[1]!=0) && (s[1] != '.' || s[2] != 0))) continue;

    strcpy(name+namelen,s);
    if(FSP_STAT(name,&sb)) continue;
    if(!S_ISDIR(sb.st_mode) && !S_ISREG(sb.st_mode)) continue;
    if(sb.st_size>FOURGIGS) sb.st_size=FOURGIGS;
	  
    nlen = strlen(s)+1;

    /* do we have space in buffer for entire entry?  */
    if(rem < RDHSIZE + nlen) {
      /* fill rest of buffer with '*' */
      memset(buffer+bufpos,RDTYPE_SKIP,rem);
      /* append this buffer */
      if(append_dir_listing(dl,buffer,packetsize))
      {
	  closedir(dir_f);
	  return;
      }
      rem = packetsize;
      bufpos = 0;
    }
	  
    BB_WRITE4(buffer+bufpos,sb.st_mtime);
    bufpos+=4;
    BB_WRITE4(buffer+bufpos,sb.st_size );
    bufpos+=4;
    buffer[bufpos++]=S_ISDIR(sb.st_mode) ? RDTYPE_DIR : RDTYPE_FILE;
    memcpy(buffer+bufpos,s,nlen);
    bufpos+=nlen;
    rem -= (nlen + RDHSIZE);

    /* pad to 4byte boundary */
    if((skip = (nlen + RDHSIZE) & 0x3)) {
      skip=4-skip;
      bufpos+=skip;
      rem   -=skip;
    }
  }
  closedir(dir_f);
  
  /* do we have space for final END entry? */
  if(rem <RDHSIZE )
  {
      /* no, make a new packet */
      memset(buffer+bufpos,RDTYPE_SKIP,rem);
      /* append this buffer */
      if(append_dir_listing(dl,buffer,packetsize))
	  return;

      bufpos = 0;
  }

  /* add final entry */
  bufpos+=8;
  buffer[bufpos++]=RDTYPE_END;
  /* append last buffer */
  append_dir_listing(dl,buffer,bufpos);
  dl->mtime=cur_time;
}

const char *server_get_dir PROTO2(DIRLISTING **, dl, const DIRINFO *, di)
{
  struct stat sf;
  char   list_p[NBSIZE];
 
  /* get directory from memory cache */
  if(dbug) fprintf(stderr,"finding %s in dirlistcache\n",di->realname);
  *dl=f_cache_find(dirlistcache,&(di->realname));
  if(*dl && (*dl)->mtime < di->mtime)
  {
      /* expired */
      f_cache_delete_entry(dirlistcache,*dl);
      *dl=NULL;
      if(dbug) fprintf(stderr,"... directory was updated, list needs rebuild.\n");
  }
  if(*dl==NULL)
  {
      /* need to build a directory listing */
      DIRLISTING dlnew;
      char *key;
      unsigned int ok;
      
      if(dbug) fprintf(stderr," miss.\n");
      ok=0;
      if(use_prebuild_dirlists)
      {
          sprintf(list_p,"%s/"FSP_DIRLISTING,di->realname);
	  /* we have up-to-date directory listing somewhere? */
          if(!FSP_STAT(list_p,&sf))
	    if(sf.st_mtime>=di->mtime) {
	      /* try to load it */
	      FILE *f;
	      
	      dlnew.listing_size=sf.st_size;
	      dlnew.listing=malloc(dlnew.listing_size);
	      if(dlnew.listing)
	      {
		  if( (f=fopen(list_p,"rb")) ) 
		  {
	              if(dlnew.listing_size==fread(dlnew.listing,1,dlnew.listing_size,f)) 
		         ok=1;
		      fclose(f);
		  }
		  if(!ok)
		  {
		      free(dlnew.listing);
		      dlnew.listing=NULL;
		  }
	      }
	      if(ok)
	      {
		 if(dbug) fprintf(stderr,"cached content sucessfully used.\n");
	         dlnew.mtime=sf.st_mtime;
	      }
	  }
      }
      /* build list if we do not have it */
      if(!ok)
	  build_dir_listing (&dlnew,di->realname);
      if(!dlnew.listing)
	  return("Server can't list directory");
      /* put new list into cache */
      key=strdup(di->realname);
      *dl=f_cache_put(dirlistcache,&key,&dlnew);
      if(use_prebuild_dirlists && !ok)
      {
	  /*  try to write a cache directory content */
	  FILE *f;
	  ok=0;
	  f=fopen(list_p,"wb");
	  if(f)
	  {
	      ok=fwrite(dlnew.listing,1,dlnew.listing_size,f);
	      if(ok==dlnew.listing_size)
		 ok=1;
	      fclose(f);
	  }
	  if(!ok)
	      unlink(list_p);
	  else
	     (*dl)->mtime=cur_time;
      }
  }
  else
  {
    if(dbug) fprintf(stderr," hit!\n");
  }
  return(NULLP);
}

/**********************************************************************/
/* assume path and ACL is validated */
const char *server_del_file PROTO2(PPATH *, pp, DIRINFO *, di)
{
  struct stat sb;
    
  if(FSP_STAT(pp->fullp,&sb)) return("unlink: file not accessible");
  if(!(S_ISREG(sb.st_mode))) return("unlink: not an ordinary file");
    
  if(unlink(pp->fullp) == -1) return("unlink: cannot unlink");
  di->mtime=cur_time;
  di->lastcheck=cur_time;
    
  return(NULLP);
}
  
/**********************************************************************/
  
const char *server_del_dir PROTO2(PPATH *, pp, DIRINFO *,di)
{
  struct stat sb;
  DIRINFO null;

  if(FSP_STAT(pp->fullp,&sb)) return("rmdir: directory not accessible");
  if(!(S_ISDIR(sb.st_mode))) return("rmdir: not an ordinary directory");
    
  memset(&null,0,sizeof(DIRINFO));
  
  chdir(pp->fullp);
  save_access_rights(&null); 
  chdir(home_dir);
  if(rmdir(pp->fullp) != 0) {
    chdir(pp->fullp);
    save_access_rights(di);
    chdir(home_dir);
    return("rmdir: cannot unlink");
  }
  else
  {
      di->lastcheck=0;
  }
    
  return(NULLP);
}
  
/**********************************************************************/
  
const char *server_make_dir PROTO3(PPATH *, pp, unsigned long, inet_num,DIRINFO, **di)
{
  DIRINFO newdir;
  char temp_p[NBSIZE];
  char *name;

  /* make directory and place ownerfile in it */
  if(mkdir(pp->fullp,0777) != 0) return("Can't create directory");
  memset(&newdir,0,sizeof(DIRINFO));
  newdir.protection=(*di)->protection;
  if((*di)->owner_password)
      newdir.owner_password=strdup((*di)->owner_password);
  if((*di)->public_password)
      newdir.public_password=strdup((*di)->public_password);
  newdir.lastcheck=cur_time;
  newdir.mtime=cur_time;
  /* make owner record */
  sprintf(temp_p,"%d.%d.%d.%d O Creator\n",
	      ((unsigned char *)(&inet_num))[0],
	      ((unsigned char *)(&inet_num))[1],
	      ((unsigned char *)(&inet_num))[2],
	      ((unsigned char *)(&inet_num))[3]);
  add_ipline(temp_p,&newdir.owner);
  chdir(pp->fullp);
  save_access_rights(&newdir);
  getcwd(temp_p,NBSIZE);
  chdir(home_dir);
  newdir.realname=strdup(temp_p);
  name=strdup(pp->fullp);
  *di=f_cache_put(dirstatcache,&name,&newdir);
  return(NULLP);
}
  
/**********************************************************************/
  
const char *server_get_file PROTO5(PPATH *, pp, 
	                     FILE **, fp,
			     unsigned long,  inet_num,
			     unsigned short, port_num,
			     DIRINFO *, di
			     )
{
  struct stat sb;
  char   realfn[NBSIZE];
  FPCACHE *cache_f;
  
  sprintf(realfn,"%s/%s",di->realname,pp->f_ptr);
  cache_f=search_fpcache(inet_num,port_num,realfn);
      
  if(!cache_f) {
    FPCACHE newfile;
    char *key;
    /* file not found in cache? */
	
    if (FSP_STAT(realfn,&sb)) return("No such file");
    if(!(S_ISREG(sb.st_mode))) 
    {
	if(S_ISDIR(sb.st_mode))
	    return ("Is a directory");
	else
	    return("Not a regular file");
    }
    /* open new file */
    if(!(*fp = fopen(pp->fullp,"rb"))) return("Can't open file");

    /* add it to the file-cache */
    newfile.inet_num=inet_num;
    newfile.port_num=port_num;
    newfile.fp=*fp;
    key=strdup(realfn);
    f_cache_put(fpcache,&key,&newfile);
  }
  /* get filepoint from cache */
  else *fp = cache_f->fp; 
    
  return(NULLP);
}
  
/**********************************************************************/
/* result and pp->fullp may overlap */
const char *server_get_pro PROTO3(DIRINFO *, di, char *, result, const char *, acc)
{
  result[0]='\0';         /* truncate output buffer */
    
  if(di->readme) strcat(result,di->readme);
  result[strlen(result)+1] = di->protection^DIR_GET;
  if(acc[0]=='O')
            result[strlen(result)+1] |= DIR_OWNER;

  return(NULLP);
}
  
/**********************************************************************/

const char *server_set_pro PROTO2(DIRINFO *,di, const char *, key)
{
  unsigned char act;

  switch(key[1]) {
    case 'c':
      act=DIR_ADD;
      break;
    case 'd':
      act=DIR_DEL;
      break;
    case 'g':
      act=DIR_GET;
      break;
    case 'm':
      act=DIR_MKDIR;
      break;
    case 'l':
      act=DIR_LIST;
    case 'r':
      act=DIR_RENAME;
      break;
    default:
      return("Invalid syntax. <+|-> <c|d|g|m|l|r>");
  }
    
  switch(key[0]) {
    case '+':
      di->protection|=act;
      break;
    case '-':
      di->protection&=~act;
      break;
    default:
      return("Invalid syntax. <+|-> <c|d|g|m|l|r>");
    }
  
  di->mtime=cur_time;
  di->lastcheck=cur_time;

  chdir(di->realname);
  save_access_rights (di);
  chdir(home_dir);
    
  return(NULLP);
}
  
/**********************************************************************
 *  These two are used for file uploading.
 **********************************************************************/
  
const char *server_up_load PROTO5(char *, data, unsigned int, len, unsigned long, pos,
			    unsigned long, inet_num, unsigned short, port_num)
{
  FILE *fp;
  char  tname[NBSIZE];
  FPCACHE *cache_f;
  char *tmp;
  struct stat sf;

  sprintf(tname, "%s/.T%08lX%04X", tmp_dir,inet_num, port_num);
  
  tmp=tname;
  cache_f=f_cache_find(fpcache,&tmp);
  if(! cache_f ) {
    FPCACHE newfile;
    /* file not found in cache? */
    if (pos) {
      fp = fopen(tname, "r+b");
    } else {
      unlink(tname);
      fp = fopen(tname,"wb");
    }
	
    if(!fp) return("Cannot open temporary file");
    
    if(lstat(tname,&sf) || !S_ISREG(sf.st_mode)) 
    {
	fclose(fp);
	unlink(tname);
	return("Temporary file is NOT a regular file");
    }
    /* protect temporary file */
    chmod(tname,S_IRUSR|S_IWUSR);
    /* add it to the file-cache */
    newfile.inet_num=inet_num;
    newfile.port_num=port_num;
    newfile.fp=fp;
    tmp=strdup(tname);
    f_cache_put(fpcache,&tmp,&newfile);
  } else
      fp=cache_f->fp;
	
  if(fseeko(fp, pos, SEEK_SET))
      return("Seeking in file failed");
  if(len!=fwrite(data, 1, len, fp))
      return("Writing to file failed");
  return(NULLP);
}
  
const char *server_install PROTO7(PPATH *, pp, unsigned long, inet_num,
			    unsigned short, port_num, const char *, acc, DIRINFO *,di, unsigned int, l2, const char *,s2)
{
  char tname[NBSIZE];
  const char *tmp;
  FPCACHE *cache_f;
#ifdef HAVE_UTIME_H
  struct utimbuf ut;
#endif
    
  sprintf(tname, "%s/.T%08lX%04X", tmp_dir,inet_num, port_num);
  /* if file still in cache, then close it & remove it from cache */
  tmp=tname;
  cache_f=f_cache_find(fpcache,&tmp);
  f_cache_delete_entry(fpcache,cache_f);
    
  if (dbug)
    fprintf(stderr,"server_install: tname: %s, pp->fullp: %s\n",tname, pp->fullp);
	
  if(fexist(pp->fullp) && 
	( (di->protection & DIR_DEL) || acc[0]=='O' )
    ) 
  {
      unlink(tname);
      if(dbug) 
	  fprintf(stderr,"File %s already exists, but there is no user is not directory owner and public can't delete files.\n",pp->fullp);
      return("no permission for replacing that file. Not an owner.");
  }
  
  di->lastcheck=cur_time;
  di->mtime=cur_time;

  /* delete target file, if any */
  unlink(pp->fullp);

  umask(upload_umask);
  /* so just copy the temp file */
  tmp=copy_file(tname,pp->fullp);
  unlink(tname);
  umask(system_umask);
#ifdef HAVE_UTIME_H
  if(l2>=4) 
  {
      ut.modtime=BB_READ4(s2);
      ut.actime=cur_time;
      utime(pp->fullp,&ut);
  }
#endif

  return(tmp);
}
  
/**********************************************************************/
/* assume path is validated */
/* start GRAB OPERATION! */
const char *server_secure_file PROTO4(PPATH *, pp, unsigned long, inet_num,
				unsigned short, port_num,DIRINFO *,di)
{
  struct stat sb;
  char temp_p[NBSIZE];
  const char *tmp;
    
  if(FSP_STAT(pp->fullp,&sb)) return("grab: file not accessible");
  if(!(S_ISREG(sb.st_mode))) return("grab: not an ordinary file");
    
  sprintf(temp_p,"%s/.G%08lX%04X", tmp_dir, inet_num,port_num);
    
  unlink(temp_p);
  /* link emulated as a filecopy */
  tmp=copy_file(pp->fullp,temp_p);
  if(tmp) return tmp;

  if(unlink(pp->fullp) == -1) {
    unlink(temp_p);
    return("grab: cannot unlink original file");
  }
  
  di->lastcheck=cur_time;
  di->mtime=cur_time;
    
  return(NULLP);
}
  
const char *server_grab_file PROTO3(FILE **, fp,
			      unsigned long, inet_num,
			      unsigned short, port_num)
{
  struct stat sb;
  char temp_p[NBSIZE];
  FPCACHE *cache_f;
  char *key;
  
  sprintf(temp_p,"%s/.G%08lX%04X",tmp_dir,inet_num,port_num);
  key=temp_p;
  cache_f=f_cache_find(fpcache,&key);
  if(!cache_f)
  {
      FPCACHE newfile;

      if(FSP_STAT(temp_p,&sb)) return("grab: can't find file");
      if(!(S_ISREG(sb.st_mode))) return("grab: Not a file");
      if(!(*fp = fopen(temp_p,"rb"))) return("grab: can't open file");
      newfile.inet_num=inet_num;
      newfile.port_num=port_num;
      newfile.fp=*fp;
      key=strdup(temp_p);
      f_cache_put(fpcache,&key,&newfile);
  } else
      *fp=cache_f->fp;

  return(NULLP);
}
  
const char *server_grab_done PROTO2(unsigned long, inet_num,
			      unsigned short, port_num)
{
  struct stat sb;
  char temp_p[NBSIZE];
  FPCACHE *cache_f;
  char *key;
    
  sprintf(temp_p,"%s/.G%08lX%04X",tmp_dir,inet_num,port_num);
  if(FSP_STAT(temp_p,&sb)) return("grabdone: can't find temporary file");
  key=temp_p;
  cache_f=f_cache_find(fpcache,&key);
  f_cache_delete_entry(fpcache,cache_f);
  if(unlink(temp_p) == -1) return("grabdone: can't delete temporary file");
  return(NULLP);
}

/* return STAT info about filesystem object */
const char *server_stat PROTO1(UBUF *, ubuf )
{
  struct stat sb;
  int rc;
  PPATH pp;
  unsigned short len;
  DIRINFO *junk;

  rc=0;
  len=BB_READ2(ubuf->bb_len);
  if(len<2) {
      strcpy(ubuf->buf,"");
      len=1;
  }
  if(parse_path(ubuf->buf,len,&pp))
      rc=1;
  sb.st_mtime=0;
  sb.st_mtime=0;
  if(!rc)
  {
      rc=FSP_STAT(pp.fullp,&sb);
      if(!rc)
      {
	  /* SECURITY CHECK: validate directory */
          if S_ISDIR(sb.st_mode)
	  {
	      if(validate_path(ubuf->buf,len,&pp,&junk,1))
		  rc=1;
	  } else
	      if(S_ISREG(sb.st_mode))
	      {
	        if(validate_path(ubuf->buf,len,&pp,&junk,0))
		    rc=1;
	      }
      }
  }
	  
  BB_WRITE4(ubuf->buf,sb.st_mtime);
  BB_WRITE4(ubuf->buf+4,sb.st_size );
  
  if(rc) 
      rc=0;
  else
      if S_ISDIR(sb.st_mode) rc=RDTYPE_DIR;
      else
	  if S_ISREG(sb.st_mode) rc=RDTYPE_FILE;
          else
	      rc=0; /* not a file or directory */
  
  (ubuf->buf)[8]=rc;
  return(NULLP);
}

/* rename FILE/directory object */
const char *server_rename PROTO3(char *, ub, unsigned int, l1, unsigned int, l2)
{
  return "Not implemented (wait for next beta)";
#if 0
  struct stat sb;
  int srcdir; /* is source object a directory ? */
  PPATH dest;
  const char *pe;
  
  if(FSP_STAT(pp->fullp,&sb)) return("can't find source file or directory");
  if(S_ISDIR(sb.st_mode))
      srcdir=1; 
  else
      if(S_ISREG(sb.st_mode))
	  srcdir=0;
      else
	  return ("Will not rename non file/directory objects");

  pe=parse_path(ub+strlen(fullp), l1, &dest );
  return(NULLP);
#endif
}
/*********************************************************************
 test and resolve home directory
 *********************************************************************/
  
void init_home_dir PROTO0((void))
{
  void *newhd;
  
  /* test and goto home dir */
  if(chdir(home_dir) == -1) {
    perror(home_dir);
    exit(6);
  }

  /* resolve home dir */
  newhd=realloc(home_dir,UBUF_SPACE);
  if(!newhd)
  {
      perror("realloc1 homedir");
      exit(5);
  }

  if(!getcwd(newhd,UBUF_SPACE))
  {
      perror("getcwd for homedir");
      exit(6);
  }

  home_dir=realloc(newhd,strlen(newhd)+1);
  if(!home_dir)
  {
      perror("realloc2 homedir");
      exit(5);
  }

  if(tmp_dir)
  {
      mkdir(tmp_dir,0700);
      chmod(tmp_dir,0700);
      if(chdir(tmp_dir)==-1)
      {
	  free(tmp_dir);
	  tmp_dir=NULL;
	  read_only=1;
      }
      chdir(home_dir);
  }

  if(dbug) {
    fprintf(stderr,"home on %s\n",home_dir);
    if(tmp_dir)
       fprintf(stderr,"tmpdir on %s\n",tmp_dir);
  }
}
