
// clang-format off
#ifdef __clang__
  #include <clib/gadtools_protos.h>
  #include <clib/intuition_protos.h>
#else
  #include <proto/gadtools.h>
  #include <proto/intuition.h>
#endif
// clang-format on

#include "menu_tools.h"


struct NewMenu* findNewMenuItem(struct NewMenu* pNewMenuArray, ULONG itemId)
{
  ULONG i = 0;
  
  if(!pNewMenuArray)
  {
    return NULL;
  }

  while(pNewMenuArray[i].nm_Type != NM_END)
  {
    if((ULONG)pNewMenuArray[i].nm_UserData == itemId)
    {
      return &pNewMenuArray[i];
    }

    i++;
  }

  return NULL;
}


struct MenuItem* findMenuItem(
  struct Menu* pMenu,
  APTR pUserDataToFind,
  WORD* pFoundMenuNumber)
{
  if(!pMenu || !pUserDataToFind || !pFoundMenuNumber)
  {
    return NULL;
  }

  *pFoundMenuNumber = 0;

  struct MenuItem* pItem = pMenu->FirstItem;
  if(!pItem)
  {
    return NULL;
  }

  int iMenu = 0;
  int iItem = 0;

  do
  {
    do
    {
      APTR pUserData = GTMENUITEM_USERDATA(pItem);
      if(pUserData == pUserDataToFind)
      {
        *pFoundMenuNumber = FULLMENUNUM(iMenu, iItem, 0);
        return pItem;
      }

      pItem = pItem->NextItem;
      iItem++;
    }
    while(pItem != NULL);

    pMenu = pMenu->NextMenu;
    if(pMenu != NULL)
    {
      pItem = pMenu->FirstItem;
      iItem = 0;
      iMenu++;
    }
  }
  while(pItem != NULL);

  return NULL;
}


void disableMenuItem(struct Window* pWindow, APTR pUserDataMenuItemToDisable)
{
  if(!pWindow || !pWindow->MenuStrip || !pUserDataMenuItemToDisable)
  {
    return;
  }

  WORD menuNumber = 0;
  struct MenuItem* pFoundItem = findMenuItem(
    pWindow->MenuStrip,
    pUserDataMenuItemToDisable,
    &menuNumber
  );

  if(!pFoundItem)
  {
    return;
  }

  OffMenu(pWindow, menuNumber);
}


void enableMenuItem(struct Window* pWindow, APTR pUserDataMenuItemToEnable)
{
  if(!pWindow || !pWindow->MenuStrip || !pUserDataMenuItemToEnable)
  {
    return;
  }

  WORD menuNumber = 0;
  struct MenuItem* pFoundItem = findMenuItem(
    pWindow->MenuStrip,
    pUserDataMenuItemToEnable,
    &menuNumber
  );

  if(!pFoundItem)
  {
    return;
  }

  OnMenu(pWindow, menuNumber);
}
