#ifndef DATE_TOOLS_H
#define DATE_TOOLS_H

#include <dos/dos.h>
#include <exec/types.h>
#include <libraries/locale.h>

// Enough space for something like '2024-10-03-11-56-20' + final \0
#define DATETIMEBUF_SIZE 20

typedef struct DateTimeParts
{
  char dateBuf[DATETIMEBUF_SIZE];
  ULONG currentIdx;
  char* pYear;
  char* pMonth;
  char* pDay;
  char* pHour;
  char* pMinute;
  char* pSecond;
} DateTimeParts;


/**
 * Create string parts (year, month, ..., @see struct above) of given
 * pDateStamp and fill given pDateTimeParts struct with these.
 *
 * An open locale is needed too, but can be opened as default (NULL)
 * for this.
 */
BOOL fillDateTimeParts(struct Locale* pLocale,
                       struct DateStamp* pDateStamp,
                       DateTimeParts* pDateTimeParts);




#endif

