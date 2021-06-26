
#ifndef _FILEITEM_H_
#define _FILEITEM_H_

/* ========================================================= */

struct fileitem
{
  char *Value;
  struct fileitem *Next;
};

/* --------------------------------------------------------- */

struct fileitem* CAPINT_freeFile( struct fileitem *File );
struct fileitem* CAPINT_createFile( const char *Name );

/* ========================================================= */

#endif

