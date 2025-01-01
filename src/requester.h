#ifndef REQUESTER_H
#define REQUESTER_H

#include <intuition/intuition.h>

/**
 * Displays a modal requester ("Message box") with given args in given
 * button texts and message text in given window. The parent window is
 * blocked and a wait mouse curser is set to it until the user presses
 * a button of the requester
 **/
long showEasyRequest(struct Window* pWindow,
                     char* pButtonTexts,
                     char* pMessage);

#endif
