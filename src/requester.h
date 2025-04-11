#ifndef REQUESTER_H
#define REQUESTER_H

#include <intuition/intuition.h>

/**
 * Displays a modal requester ("Message box") with buttons from
 * `pButtonTexts` (separated by '|') and a text  `pMessage` in window.
 * The parent window is blocked and a wait mouse curser is set to it
 * until the user presses a button of the requester.
 **/
long showEasyRequest(Object* pWinObject,
                     struct Window* pWindow,
                     char* pTitle,
                     char* pButtonTexts,
                     char* pMessage);

/**
 * Returns NULL on error. On success it returns a pointer to a 
 * `struct FileRequester` for access of the selected files.
 * NOTE: This must be freed after use by `freeMultiFileSelector(..)`
 */
struct FileRequester* showMultiFileSelector(struct Window* pParentWindow,
                                            STRPTR pHeaderText,
                                            struct Hook* pIntuiMsgHook);

void freeMultiFileSelector(struct FileRequester* pFileRequester);

#endif
