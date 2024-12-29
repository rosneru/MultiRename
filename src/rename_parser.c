#include <exec/lists.h>
#ifdef __clang__
  #include <clib/alib_protos.h>
  #include <clib/exec_protos.h>
#else
  #include <proto/alib.h>
  #include <proto/exec.h>
#endif

#include <stdlib.h>
#include <string.h>

#include "rename_parser.h"


BOOL addPendingAction(ActionParser* pParser);
BOOL isCharAllowed(char c);
BOOL isCharDigit(char c);

/**********************************************************************
 * BEGIN State machine stuff
 **********************************************************************/

/* Define the type state_func_t  */
typedef ParserState state_func_t(ActionParser* pParser);

static ParserState do_state_apply(ActionParser* pParser);
static ParserState do_state_parse_command(ActionParser* pParser);
static ParserState do_state_detect_range(ActionParser* pParser);
static ParserState do_state_parse_from(ActionParser* pParser);
static ParserState do_state_parse_to(ActionParser* pParser);
static ParserState do_state_finished(ActionParser* pParser);
static ParserState do_state_error(ActionParser* pParser);

static void init_state_apply(ActionParser* pParser);
static void init_state_parse_command(ActionParser* pParser);
static void init_state_detect_range(ActionParser* pParser);
static void init_state_parse_from(ActionParser* pParser);
static void init_state_parse_to(ActionParser* pParser);
static void init_state_finished(ActionParser* pParser);
static void init_state_error(ActionParser* pParser);

static state_func_t* const state_table[PS_NUM_STATES] = 
{
  do_state_apply,
  do_state_parse_command,
  do_state_detect_range,
  do_state_parse_from,
  do_state_parse_to,
  do_state_finished,
  do_state_error
};


/*  */
typedef void transition_func_t(ActionParser* pParser);

/* An array of function pointers to each state's init function */
static transition_func_t* const  transition_table[PS_NUM_STATES] =
{
  init_state_apply,
  init_state_parse_command,
  init_state_detect_range,
  init_state_parse_from,
  init_state_parse_to,
  init_state_finished,
  init_state_error
};

static ParserState run_state(ParserState cur_state, ActionParser* pParser) 
{
  transition_func_t *transition;

  /* Call the do_state_*() function of curring state */
  ParserState new_state = state_table[cur_state](pParser);
  if(new_state == cur_state)
  {
    /* No additional work necessary */
    return new_state;
  }

  /* Perform transition if one exists */
  if(NULL != (transition = transition_table[new_state]))
  {
    transition(pParser);
  }

  return new_state;
}

/**********************************************************************
 * END State machine stuff
 **********************************************************************/



void initActionParser(ActionParser* pParser, STRPTR pMask)
{
  if(!pParser || !pMask)
  {
    return;
  }

  NewList(&pParser->ActionList);
  pParser->Command = AC_NONE;
  pParser->CommandFrom = 0;
  pParser->CommandTo = 0;
  pParser->NumericFrom = -1;
  pParser->NumericTo = -1;
  pParser->pMask = pMask;
  pParser->MaskLen = strlen(pMask);
  pParser->MaskIndex = 0;
}

BOOL parseActions(ActionParser* pParser)
{
  if(!pParser)
  {
    return FALSE;
  }

  pParser->MaskIndex = 0;
  NewList(&pParser->ActionList);
  init_state_apply(pParser);
  while(pParser->State != PS_FINISHED)
  {
    if(pParser->State == PS_ERROR)
    {
      break;
    }

    run_state(pParser->State, pParser);
  }

  return (BOOL)(pParser->State == PS_FINISHED);
}


void freeActionNodes(struct List* pActionsList)
{
  struct Node* pWorkNode;
  struct Node* pNextNode;

  if(!pActionsList)
  {
    return;
  }

  pWorkNode = pActionsList->lh_Head;
  while((pNextNode = pWorkNode->ln_Succ))
  {
    free(pWorkNode);
    pWorkNode = pNextNode;
  }
}



BOOL addPendingAction(ActionParser* pParser)
{
  ActionNode* pActionNode;

  if(pParser->Command == AC_NONE)
  {
    return TRUE;
  }

  if((pParser->CommandFrom > -1) && (pParser->CommandTo < 0))
  {
    // This is an incomplete Apply command. In this state machine design
    // this can occur during normal operation and is no error.
    return TRUE;
  }

  if(!(pActionNode = malloc(sizeof(ActionNode))))
  {
    return FALSE;
  }

  pActionNode->Command = pParser->Command;
  pActionNode->Start = pParser->CommandFrom;
  pActionNode->End = pParser->CommandTo;
  AddTail(&pParser->ActionList, (struct Node*) pActionNode);

  return TRUE;
}

BOOL isCharAllowed(char c)
{
  // This is the easy implementation. The strict one as in the Python
  // prototype can be done later if really needed.

  if((c > 31) && (c < 91))
  {
    return TRUE;
  }

  // Skipping 91 '[' (Not allowed because MultiRename control char)

  if(c == 92)
  {
    return TRUE;
  }

  // Skipping 93 ']' (Not allowed because MultiRename control char)
  
  if((c > 93) && (c < 128))
  {
    return TRUE;
  }

  return FALSE;
}

BOOL isCharDigit(char c)
{
  return (BOOL)((c >= 48) && (c < 58));
}

static void init_state_apply(ActionParser* pParser)
{
  pParser->State = PS_APPLY;
  pParser->Command = AC_APPLY;
  pParser->CommandFrom = pParser->MaskIndex;
  pParser->CommandTo = -1;
}

static ParserState do_state_apply(ActionParser* pParser)
{
  char c;
  if(pParser->MaskIndex == pParser->MaskLen)
  {
    return PS_FINISHED;
  }

  c = pParser->pMask[pParser->MaskIndex];
  if(isCharAllowed(c))
  {
    pParser->CommandTo = pParser->MaskIndex;
    pParser->MaskIndex++;
    return PS_APPLY;
  }
  else if(c == '[')
  {
    if(!addPendingAction(pParser))
    {
      return PS_ERROR;
    }

    pParser->MaskIndex++;
    return PS_PARSE_COMMAND;
  }
  else
  {
    return PS_ERROR;
  }
}


static void init_state_parse_command(ActionParser* pParser)
{
  pParser->State = PS_PARSE_COMMAND;
}

static ParserState do_state_parse_command(ActionParser* pParser)
{
  char c;
  if(pParser->MaskIndex == pParser->MaskLen)
  {
    return PS_ERROR;
  }

  c = pParser->pMask[pParser->MaskIndex];
  pParser->MaskIndex++;
  switch(c)
  {
    case 'N':
    {
      pParser->Command = AC_NAME;
      return PS_DETECT_RANGE;
    }
    case 'E':
    {
      pParser->Command = AC_EXTENSION;
      return PS_DETECT_RANGE;
    }
    case 'C':
    {
      pParser->Command = AC_COUNTER;
      return PS_DETECT_RANGE;
    }
    case 'Y':
    {
      pParser->Command = AC_YEAR;
      return PS_DETECT_RANGE;
    }
    case 'M':
    {
      pParser->Command = AC_MONTH;
      return PS_DETECT_RANGE;
    }
    case 'D':
    {
      pParser->Command = AC_DAY;
      return PS_DETECT_RANGE;
    }
    case 'h':
    {
      pParser->Command = AC_HOUR;
      return PS_DETECT_RANGE;
    }
    case 'm':
    {
      pParser->Command = AC_MINUTE;
      return PS_DETECT_RANGE;
    }
    case 's':
    {
      pParser->Command = AC_SECOND;
      return PS_DETECT_RANGE;
    }
    default:
    {
      return PS_ERROR;
    }
  }
}


static void init_state_detect_range(ActionParser* pParser)
{
  pParser->State = PS_DETECT_RANGE;
  pParser->CommandFrom = -1;
  pParser->CommandTo = -1;
}

static ParserState do_state_detect_range(ActionParser* pParser)
{
  char c;
  if(pParser->MaskIndex == pParser->MaskLen)
  {
    return PS_ERROR;
  }

  c = pParser->pMask[pParser->MaskIndex];
  if(c == 'Y' || c == 'M'|| c == 'D' || c == 'h' || c == 'm'|| c == 's')
  {
    if(!addPendingAction(pParser))
    {
      return PS_ERROR;
    }

    return PS_PARSE_COMMAND;
  }
  else if(c == ']')
  {
    if(!addPendingAction(pParser))
    {
      return PS_ERROR;
    }

    pParser->MaskIndex++;
    return PS_APPLY;
  }
  else if(pParser->Command == AC_COUNTER)
  {
    // A 'Counter' must be finished with an ']' immediately and this
    // hasn't been done here. So its an error.
    return PS_ERROR;
  }
  else if(isCharDigit(c))
  {
    return PS_PARSE_FROM;
  }
  else
  {
    return PS_ERROR;
  }

}


static void init_state_parse_from(ActionParser* pParser)
{
  pParser->State = PS_PARSE_FROM;
  pParser->NumericFrom = pParser->MaskIndex;
  pParser->NumericTo = -1;
}

static ParserState do_state_parse_from(ActionParser* pParser)
{
  char c;
  if(pParser->MaskIndex == pParser->MaskLen)
  {
    return PS_ERROR;
  }

  c = pParser->pMask[pParser->MaskIndex];
  if(isCharDigit(c))
  {
    pParser->NumericTo = pParser->MaskIndex;
    pParser->MaskIndex++;
    return PS_PARSE_FROM;
  }
  else if((c == '-') && (pParser->NumericTo > -1))
  {
    pParser->MaskIndex++;
    pParser->CommandFrom = strtol(pParser->pMask + pParser->NumericFrom,
                                  NULL,
                                  10);
    return PS_PARSE_TO;
  }
  else
  {
    return PS_ERROR;
  }
}


static void init_state_parse_to(ActionParser* pParser)
{
  pParser->State = PS_PARSE_TO;
  pParser->NumericFrom = pParser->MaskIndex;
  pParser->NumericTo = -1;
}

static ParserState do_state_parse_to(ActionParser* pParser)
{
  char c;
  if(pParser->MaskIndex == pParser->MaskLen)
  {
    return PS_ERROR;
  }

  c = pParser->pMask[pParser->MaskIndex];
  if(isCharDigit(c))
  {
    pParser->NumericTo = pParser->MaskIndex;
    pParser->MaskIndex++;
    return PS_PARSE_TO;
  }
  else if((c == ']') && (pParser->NumericTo > -1))
  {
    pParser->MaskIndex++;
    pParser->CommandFrom--;
    pParser->CommandTo = strtol(pParser->pMask + pParser->NumericFrom,
                                NULL,
                                10);
    pParser->CommandTo--;
    addPendingAction(pParser);
    return PS_APPLY;
  }
  else
  {
    return PS_ERROR;
  }
}


static void init_state_finished(ActionParser* pParser)
{
  addPendingAction(pParser);
  pParser->State = PS_FINISHED;
}

static ParserState do_state_finished(ActionParser* pParser)
{
  return PS_FINISHED;
}


static void init_state_error(ActionParser* pParser)
{
  pParser->State = PS_ERROR;
}

static ParserState do_state_error(ActionParser* pParser)
{
  return PS_ERROR;
}
