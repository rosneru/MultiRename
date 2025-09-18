#ifndef REQUESTER_H
#define REQUESTER_H

#include <intuition/intuition.h>

/**
 * Displays a modal requester ("Message box") with buttons from
 * `pButtonTexts` (separated by '|') and a text  `pMessage` in window.
 * The parent window is blocked and a wait mouse curser is set to it
 * until the user presses a button of the requester.
 *
 * \return the number of the pressed button in range 0..n-1 or -1 on
 * error
 *
 * NOTE: This requester handles `WMHI_NEWSIZE` messages of the parent
 * window object. When such a message arrives it triggers a redraw of
 * the parent window.
 **/
long showEasyRequest(Object *pWinObject,
  struct Window *pWindow,
  char *pTitle,
  char *pButtonTexts,
  char *pMessage);

/**
 * Allows the user to select one or more files to open and returns a
 * struct to access the selected files.
 *
 * \return NULL on error. On success it returns a pointer to a `struct
 * FileRequester` for access of the selected files. When finished this
 * must be freed with `freeMultiFileSelector(..)`
 *
 * NOTE: This requester handles `IDCMP_NEWSIZE` messages of the parent
 * Intuition window. When such a message arrives it triggers a redraw of
 * the parent window.
 */
struct FileRequester *showMultiFileSelector(
  Object *pWinObject, struct Window *pParentWindow, STRPTR pTitle);

void freeMultiFileSelector(struct FileRequester *pFileRequester);

#endif
