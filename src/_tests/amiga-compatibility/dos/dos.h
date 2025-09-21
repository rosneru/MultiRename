#ifndef AMIGA_DOS_DOS_H
#define AMIGA_DOS_DOS_H

#include <exec/types.h>

typedef long BPTR;		    /* Long word pointer */

struct DateStamp {
   LONG	 ds_Days;	      /* Number of days since Jan. 1, 1978 */
   LONG	 ds_Minute;	      /* Number of minutes past midnight */
   LONG	 ds_Tick;	      /* Number of ticks past minute */
}; /* DateStamp */

#endif
