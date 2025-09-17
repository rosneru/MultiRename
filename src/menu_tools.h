#ifndef MENU_TOOLS_H
#define MENU_TOOLS_H

/**
 * Returns item with `idemId` in given `struct NewMenu` or `NULL` if no such
 * item is found.
 */
struct NewMenu *findNewMenuItem(struct NewMenu *pNewMenuArray, ULONG itemId);

/**
 * Returns item with requested `pUserDataToFind` in given `struct NewMenu` or
 * `NULL` if no such item is found. On success (item is found) it also sets the
 * *value* of pointer `pFoundMenuNumber` to the menu number of the found item.
 */
struct MenuItem *findMenuItem(
  struct Menu *pMenu, APTR pUserDataToFind, WORD *pFoundMenuNumber);

/**
 * Enable menu item with given user data.
 */
void disableMenuItem(struct Window *pWindow, APTR pUserDataMenuItemToDisable);

/**
 * Disable menu item with given user data.
 */
void enableMenuItem(struct Window *pWindow, APTR pUserDataMenuItemToEnable);

#endif
