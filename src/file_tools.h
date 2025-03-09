#ifndef FILE_TOOLS_H
#define FILE_TOOLS_H

#include <dos/dos.h>


/**
 * Safely acquire a lock (`SHARED_LOCK`) even if the final path length
 * is > 255 chars. If path object can't be locked, it returns ZERO.
 *
 * This method originates from `OpenFromLongName()`, described in
 * "Rom Kernel Reference Manual: DOS" from 2024 by Thomas Richter.
 */
BPTR lockFromLongName(STRPTR pPath);

#endif

