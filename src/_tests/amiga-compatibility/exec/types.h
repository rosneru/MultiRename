#ifndef AMIGA_EXEC_TYPES_H
#define AMIGA_EXEC_TYPES_H

#define GLOBAL   extern		/* the declaratory use of an external */
#define IMPORT   extern		/* reference to an external */
#define STATIC   static		/* a local static variable */
#define REGISTER register	/* a (hopefully) register variable */


typedef long		LONG;		/* signed 32-bit quantity */
typedef unsigned long	ULONG;		/* unsigned 32-bit quantity */
typedef unsigned long	LONGBITS;	/* 32 bits manipulated individually */
typedef short		WORD;		/* signed 16-bit quantity */
typedef unsigned short	UWORD;		/* unsigned 16-bit quantity */
typedef unsigned short	WORDBITS;	/* 16 bits manipulated individually */
typedef signed char	BYTE;		/* signed 8-bit quantity */
typedef unsigned char	UBYTE;		/* unsigned 8-bit quantity */
typedef unsigned char	BYTEBITS;	/* 8 bits manipulated individually */
typedef unsigned short	RPTR;		/* unsigned relative pointer */

typedef unsigned char*	STRPTR;

/* For compatibility with Kickstart/Workbench 1.x (1985) only!
 * Do not use these in newer code!
 */
typedef short		SHORT;		/* signed 16-bit quantity (use WORD) */
typedef unsigned short	USHORT;		/* unsigned 16-bit quantity (use UWORD) */
typedef short		COUNT;
typedef unsigned short	UCOUNT;
typedef ULONG		CPTR;

#ifndef BOOL
typedef short	BOOL;
#endif

#ifndef TRUE
#define TRUE		1
#endif

#ifndef FALSE
#define FALSE		0
#endif

#ifndef NULL
#define NULL		0L
#endif

#define BYTEMASK	0xFF


#endif
