/* table_header.h */

#ifndef TABLE_HEADER_H
#define TABLE_HEADER_H

typedef struct  table_head_st {
   char name[20];	/* table name */
   char type[20];	/* table type */
   int  maxlen;		/* # rows allocated */
   int  nok;		/* # rows filled */
   int  rbytes;		/* number of bytes per row */
   int  dsl_pointer;	/* swizzled (DS_DATASET_T*) */
   int  data_pointer;	/* swizzled (char*) */
} TABLE_HEAD_ST;

#endif /*TABLE_HEADER_H*/

