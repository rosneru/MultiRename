#include <utility/hooks.h>
#include <clib/compiler-specific.h>
#include <libraries/locale.h>

#ifdef __clang__
  #include <clib/locale_protos.h>
#else
  #include <proto/locale.h>
#endif

#include "date_tools.h"

void __ASM__ __SAVE_DS__ FormatDateFunc(__REG__(a0, struct Hook *pHook),
                                        __REG__(a2, struct Locale* pLocale),
                                        __REG__(a1, void* pData))
{
  char charToProcess = pData;
  DateTimeParts* pDateTimeParts = (DateTimeParts*)pHook->h_Data;
  pDateTimeParts->dateBuf[pDateTimeParts->currentIdx++] = charToProcess;
}




BOOL fillDateTimeParts(struct Locale* pLocale,
                       struct DateStamp* pDateStamp,
                       DateTimeParts* pDateTimeParts)
{
  struct Hook hook;
  char* pBuf;
  if(!pLocale || ! pDateStamp || !pDateTimeParts)
  {
    return FALSE;
  }

  pBuf = pDateTimeParts->dateBuf;
  pDateTimeParts->currentIdx = 0;

  hook.h_Entry = (ULONG (* )())FormatDateFunc;
  hook.h_SubEntry = NULL;
  hook.h_Data = pDateTimeParts;

  FormatDate(pLocale, "%Y-%m-%d-%H:%M:%S", pDateStamp, &hook);

  // Manually overwrite the separator ('-' or ':') after each part
  // with \0. So every part has its own string finalizer.
  pBuf[4]  = '\0';
  pBuf[7]  = '\0';
  pBuf[10] = '\0';
  pBuf[13] = '\0';
  pBuf[16] = '\0';

  // Manually adjust the start pointers of the DateTimeParts to its
  // positions in pDateTimeBuf.
  pDateTimeParts->pYear   = pBuf;
  pDateTimeParts->pMonth  = pBuf + 5;
  pDateTimeParts->pDay    = pBuf + 8;
  pDateTimeParts->pHour   = pBuf + 11;
  pDateTimeParts->pMinute = pBuf + 14;
  pDateTimeParts->pSecond = pBuf + 17;

  return TRUE;
}

