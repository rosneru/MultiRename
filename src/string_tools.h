#ifndef STRING_TOOLS_H
#define STRING_TOOLS_H

#include <exec/types.h>

/**
 * Appends numChars of given string pSrc to destination buffer pDest and
 * takes care of the size of the destination buffer, destSize. So, it
 * doesn't write beyond the destination buffer borders. Instead it
 * truncates if necessary.
 *
 * Set numChars to 0 to (try to) copy all characters of pSrc string.
 *
 * If a non-NULL pNewDestLen is passed, it will be set with the new
 * length of pDest string after the append operation. This may save a
 * later strlen() call.
 *
 * Returns TRUE if truncation was done, otherwise FALSE.
 */
int appendString(STRPTR pDest,
                 ULONG destSize,
                 ULONG* pNewDestLen,
                 STRPTR pSrc,
                 ULONG numChars);

#endif
