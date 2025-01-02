#ifndef REQUESTER_H
#define REQUESTER_H

#include <intuition/intuition.h>

/**
 * Displays a modal requester ("Message box") with buttons from
 * `pButtonTexts` (separated by '|') and a text  `pMessage` in window.
 * The parent window is blocked and a wait mouse curser is set to it
 * until the user presses a button of the requester.
 **/
long showEasyRequest(struct Window* pWindow,
                     char* pButtonTexts,
                     char* pMessage);

#endif
