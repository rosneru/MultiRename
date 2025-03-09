#include <dos/dosextens.h>
#include <proto/dos.h>
#include <proto/exec.h>

#include "file_tools.h"


BPTR lockFromLongName(STRPTR pPath)
{
  LONG pos = 0;
  BPTR resultLock = 0, lock = 0;
  BPTR oldLock = -1L;     // Never a valid lock
  char buffer[108 + 32];  // Long enough for a component and a device name

  do
  {
    pos = SplitName(pPath,'/', buffer, pos, sizeof(buffer));
    if (pos < 0)
    {
      // No separator found, call now Lock
      resultLock = Lock(buffer, SHARED_LOCK);
      break;
    }
    else
    {
      // Lock the partial path so far and abort on error If two slashes
      // are next to each other, go to the parent directory.
      if (!(lock = Lock(*buffer ? buffer : "/", SHARED_LOCK)))
      {
        break;
      }

      // Rotate directories
      lock = CurrentDir(lock);

      // Unlock previous directory, keep the old directory
      if (oldLock >= 0)
      {
        UnLock(lock);
      }
      else
      {
        oldLock = lock;
      }
    }
  } while (1);

  // Restore the current directory if any. None of the functions touch
  // IoErr().
  if (oldLock >= 0)
  {
    UnLock(CurrentDir(oldLock));
  }

  return resultLock;
}
 

