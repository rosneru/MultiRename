#include <exec/lists.h>
#ifdef __clang__
  #include <clib/alib_protos.h>
  #include <clib/exec_protos.h>
#else
  #include <proto/alib.h>
  #include <proto/exec.h>
#endif

#include <string.h>

#include "rename_parser.h"


/**********************************************************************
 * BEGIN State machine stuff
 **********************************************************************/

/* Define the type state_func_t  */
typedef ParserState state_func_t(ActionParser* pParser);

static ParserState do_state_apply(ActionParser* pParser);
static ParserState do_state_finished(ActionParser* pParser);
static ParserState do_state_parse_command(ActionParser* pParser);
static ParserState do_state_detect_range(ActionParser* pParser);
static ParserState do_state_parse_from(ActionParser* pParser);
static ParserState do_state_parse_to(ActionParser* pParser);

static void init_state_apply(ActionParser* pParser);
static void init_state_finished(ActionParser* pParser);
static void init_state_parse_command(ActionParser* pParser);
static void init_state_detect_range(ActionParser* pParser);
static void init_state_parse_from(ActionParser* pParser);
static void init_state_parse_to(ActionParser* pParser);


static state_func_t* const state_table[PS_NUM_STATES] = 
{
  do_state_apply, do_state_finished,
  do_state_parse_command, do_state_detect_range,
  do_state_parse_from, do_state_parse_to
};


/*  */
typedef void transition_func_t(ActionParser* pParser);

/* An array of function pointers to each state's init function */
static transition_func_t* const  transition_table[PS_NUM_STATES] =
{
  init_state_apply, init_state_finished,
  init_state_parse_command, init_state_detect_range,
  init_state_parse_from, init_state_parse_to
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
  return TRUE;
}




static ParserState do_state_apply(ActionParser* pParser)
{
  return PS_APPLY;
}

static ParserState do_state_finished(ActionParser* pParser)
{
  return PS_FINISHED;
}

static ParserState do_state_parse_command(ActionParser* pParser)
{
  return PS_PARSE_COMMAND;
}

static ParserState do_state_detect_range(ActionParser* pParser)
{
  return PS_DETECT_RANGE;
}

static ParserState do_state_parse_from(ActionParser* pParser)
{
  return PS_PARSE_FROM;
}

static ParserState do_state_parse_to(ActionParser* pParser)
{
  return PS_PARSE_TO;
}



static void init_state_apply(ActionParser* pParser)
{

}

static void init_state_finished(ActionParser* pParser)
{

}

static void init_state_parse_command(ActionParser* pParser)
{

}

static void init_state_detect_range(ActionParser* pParser)
{

}

static void init_state_parse_from(ActionParser* pParser)
{

}

static void init_state_parse_to(ActionParser* pParser)
{

}
