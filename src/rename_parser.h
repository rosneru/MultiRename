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

/**
 * Initialize the parser. Must be done before every parseActions() call.
 */
void initActionParser(ActionParser* pParser, STRPTR pMask);

/**
 * Parses the given pMask. It creates a number of nodes and adds it to
 * pParser->ActionList. After done with the actions, freeActionNodes()
 * must be called.
 */
BOOL parseActions(ActionParser* pParser);

/**
 * Frees the memory that was allocated for the ACtionNodes in
 * parseActions().
 */
void freeActionNodes(struct List* pActionsList);

#endif
