#ifndef RENAME_PARSER_H
#define RENAME_PARSER_H

#include <exec/lists.h>
#include <exec/types.h>

typedef enum
{
  AC_NONE,
  AC_APPLY,
  AC_NAME,
  AC_EXTENSION,
  AC_COUNTER
} ActionCommand;

typedef struct 
{
  struct Node en_Node;
  ActionCommand Command;
  LONG Start;
  LONG End;
} ActionNode;

typedef enum
{
  PS_APPLY,
  PS_PARSE_COMMAND,
  PS_DETECT_RANGE,
  PS_PARSE_FROM,
  PS_PARSE_TO,
  PS_FINISHED,
  PS_ERROR,
  PS_NUM_STATES,
} ParserState;


typedef struct 
{
  ParserState State;
  struct List ActionList;
  ActionCommand Command;
  LONG CommandFrom;
  LONG CommandTo;
  LONG NumericFrom;
  LONG NumericTo;
  STRPTR pMask;
  ULONG MaskLen;
  ULONG MaskIndex;
} ActionParser;

void initActionParser(ActionParser* pParser, STRPTR pMask);
BOOL parseActions(ActionParser* pParser);

#endif
