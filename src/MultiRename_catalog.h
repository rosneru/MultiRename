#ifndef MULTIRENAME_CATALOG_H
#define MULTIRENAME_CATALOG_H


/****************************************************************************/


/* This file was created automatically by CatComp.
 * Do NOT edit by hand!
 */


#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifdef CATCOMP_ARRAY
#undef CATCOMP_NUMBERS
#undef CATCOMP_STRINGS
#define CATCOMP_NUMBERS
#define CATCOMP_STRINGS
#endif

#ifdef CATCOMP_BLOCK
#undef CATCOMP_STRINGS
#define CATCOMP_STRINGS
#endif


/****************************************************************************/


#ifdef CATCOMP_NUMBERS

#define MSG_TYPE_IS_DIRECTORY 0
#define MSG_LOG_SELECTED_PATH 1
#define MSG_LOG_SKIPPED_WRONG_PATHS 2
#define MSG_LOG_SKIPPED_OVERLONG_PATHS 3
#define MSG_LOG_SKIPPED_DATESTAMP 4
#define MSG_LOG_SKIPPED_EXAMINE 5
#define MSG_LOG_SKIPPED_LOCK 6
#define MSG_LOG_SKIPPED_LINKS 7
#define MSG_LOG_SKIPPED_ALREADY_IN_LIST 8
#define MSG_LOG_RENAME_FAILED 9
#define MSG_LONGEST_ITEM 10
#define MSG_FROM 11
#define MSG_TO 12
#define MSG_SELECTION_RESULT 13
#define MSG_RESULT_MASK 14
#define MSG_OK_GAD 15
#define MSG_CANCEL_GAD 16
#define MSG_ABOUT_1 17
#define MSG_SETTINGS_ALLOW_LONG_NAMES 18
#define MSG_SETTINGS_SKIP_ICONS 19
#define MSG_PROJECT_MENU 20
#define MSG_PROJECT_NEW 21
#define MSG_PROJECT_ADD_FILES 22
#define MSG_PROJECT_ABOUT 23
#define MSG_PROJECT_ 24
#define MSG_SETTINGS_MENU 25
#define MSG_FAILED_LOCK_PUBSCREEN 26
#define MSG_FAILED_LOCK_DEFAULT_PUBSCREEN 27
#define MSG_FAILED_CREATE_ABOUT_MSG 28
#define MSG_FAILED_CREATE_RANGESEL_WINDOW 29
#define MSG_FAILED_CREATE_MAIN_WINDOW 30
#define MSG_FAILED_CREATE_LAYOUT 31
#define MSG_FAILED_PARSE_ARGS 32
#define MSG_FAILED_CREATE_FILES_LIST 33
#define MSG_FAILED_CREATE_LOG_OBJECT 34
#define MSG_FAILED_MSG_PORT 35
#define MSG_FAILED_OPEN_LOCALE 36
#define MSG_FAILED_ALLOC_APP_MEMORY 37
#define MSG_FAILED_OPEN_WINDOW 38
#define MSG_MAIN_WINDOW 39
#define MSG_OK_SHOW_GAD 40
#define MSG_LOGREQ_FAILED_TO_ADD 41
#define MSG_NO_FILES_TO_RENAME 42
#define MSG_FAILED_CREATE_TOKENS 43
#define MSG_WARN_DUPLICATES_1 44
#define MSG_WARN_DUPLICATES_2 45
#define MSG_WARN_DUPLICATES_3 46
#define MSG_WARN_DUPLICATES_4 47
#define MSG_WARN_DUPLICATES_5 48
#define MSG_PROCEED_CANCEL_GAD 49
#define MSG_ERROR_AUTOCREATE_NAMES 50
#define MSG_ERROR_AUTOCREATED_NAMES_TOO_LONG 51
#define MSG_STATE_OK 52
#define MSG_STATE_COMMAND_ERROR 53
#define MSG_NAME_ERROR 54
#define MSG_NAME_TOOL_NEEDS_ITEMS_TITLE 55
#define MSG_NAME_TOOL_NEEDS_ITEMS 56
#define MSG_EXT_TOOL_NEEDS_ITEMS_TITLE 57
#define MSG_EXT_TOOL_NEEDS_ITEMS_AND_EXT 58
#define MSG_OK_SHOW_ERRORS_GAD 59
#define MSG_RENAME_FAILED_FOR_SOME 60
#define MSG_CONTINUE_CANCEL_GAD 61
#define MSG_CONTINUE_WILL_SCRATCH_1 62
#define MSG_CONTINUE_WILL_SCRATCH_2 63
#define MSG_CONTINUE_WILL_SCRATCH_3 64
#define MSG_FAILED_RECREATE_FILES_LIST 65
#define MSG_FILESELECTOR_TITLE 66
#define MSG_FAILED_APPLY_RANGE 67
#define MSG_COLUMN_TITLE_STATE 68
#define MSG_COLUMN_TITLE_TYPE 69
#define MSG_COLUMN_TITLE_OLD_NAME 70
#define MSG_COLUMN_TITLE_NEW_NAME 71
#define MSG_NAME_GROUP 72
#define MSG_NAME_NAME_GAD 73
#define MSG_NAME_DATE_GAD 74
#define MSG_NAME_PART_GAD 75
#define MSG_NAME_TIME_GAD 76
#define MSG_NAME_COUNTER_GAD 77
#define MSG_EXTENSION_GROUP 78
#define MSG_EXT_EXT_GAD 79
#define MSG_EXT_PART_GAD 80
#define MSG_EXT_COUNTER_GAD 81
#define MSG_COUNTER_GROUP 82
#define MSG_COUNTER_START_GAD 83
#define MSG_COUNTER_STEP_GAD 84
#define MSG_COUNTER_PLACES_GAD 85
#define MSG_PROCESSING_LIST_GROUP 86
#define MSG_START_RENAME_GAD 87
#define MSG_FAILED_TO_OPEN_INTUITION 88
#define MSG_FAILED_TO_OPEN_WINCLASS 89
#define MSG_FAILED_TO_OPEN_LAYOUTGAD 90
#define MSG_FAILED_TO_OPEN_BEVELIMG 91
#define MSG_FAILED_TO_OPEN_BUTTONGAD 92
#define MSG_FAILED_TO_OPEN_CHOOSERGAD 93
#define MSG_FAILED_TO_OPEN_INTEGERGAD 94
#define MSG_FAILED_TO_OPEN_LISTBROWSERGAD 95
#define MSG_FAILED_TO_OPEN_LABELIMG 96
#define MSG_FAILED_TO_OPEN_SLIDERGAD 97
#define MSG_FAILED_TO_OPEN_STRINGGAD 98
#define MSG_SEARCH_REPLACE_GROUP 99
#define MSG_SEARCH_FOR_GAD 100
#define MSG_REPLACE_WITH_GAD 101
#define MSG_PROJECT_LOAD_NAMES_FROM_FILE 102
#define MSG_WRONG_NUMBER_OF_LINES 103

#endif /* CATCOMP_NUMBERS */


/****************************************************************************/


#ifdef CATCOMP_STRINGS

#define MSG_TYPE_IS_DIRECTORY_STR "DIR"
#define MSG_LOG_SELECTED_PATH_STR "Selected path is '%s'\n\n"
#define MSG_LOG_SKIPPED_WRONG_PATHS_STR "Skipped %lu input file(s) because of wrong paths:\n"
#define MSG_LOG_SKIPPED_OVERLONG_PATHS_STR "Skipped %lu input file(s) because of over long / truncated paths:\n"
#define MSG_LOG_SKIPPED_DATESTAMP_STR "Skipped %lu input file(s) because their date stamp couldn't be split into parts:\n"
#define MSG_LOG_SKIPPED_EXAMINE_STR "Skipped %lu input file(s) because they couldn't be examined:\n"
#define MSG_LOG_SKIPPED_LOCK_STR "Failed to add (lock) %lu input file(s) / dir(s):\n"
#define MSG_LOG_SKIPPED_LINKS_STR "Skipped %lu input file(s) because links are not supported:\n"
#define MSG_LOG_SKIPPED_ALREADY_IN_LIST_STR "Skipped %lu input file(s) because they are already in processing list:\n"
#define MSG_LOG_RENAME_FAILED_STR "Rename failed for %lu new file names:\n"
#define MSG_LONGEST_ITEM_STR "Longest item:"
#define MSG_FROM_STR "From:"
#define MSG_TO_STR "To:"
#define MSG_SELECTION_RESULT_STR "Selection result:"
#define MSG_RESULT_MASK_STR "Result mask:"
#define MSG_OK_GAD_STR "Ok"
#define MSG_CANCEL_GAD_STR "Cancel"
#define MSG_ABOUT_1_STR "\n\nCopyright(c) 2025 Uwe Rosner (u.rosner@ymail.com)\n\n"
#define MSG_SETTINGS_ALLOW_LONG_NAMES_STR "Allow long filenames"
#define MSG_SETTINGS_SKIP_ICONS_STR "Skip icons"
#define MSG_PROJECT_MENU_STR "Project"
#define MSG_PROJECT_NEW_STR "New"
#define MSG_PROJECT_ADD_FILES_STR "Add files..."
#define MSG_PROJECT_ABOUT_STR "About"
#define MSG_PROJECT__STR "Quit"
#define MSG_SETTINGS_MENU_STR "Settings"
#define MSG_FAILED_LOCK_PUBSCREEN_STR "Failed to lock public screen '%s'\n"
#define MSG_FAILED_LOCK_DEFAULT_PUBSCREEN_STR "Failed to lock default public screen.\n"
#define MSG_FAILED_CREATE_ABOUT_MSG_STR "Failed to create the about message.\n"
#define MSG_FAILED_CREATE_RANGESEL_WINDOW_STR "Failed to create the range select window.\n"
#define MSG_FAILED_CREATE_MAIN_WINDOW_STR "Failed to create the application main window.\n"
#define MSG_FAILED_CREATE_LAYOUT_STR "Failed to create layout.\n"
#define MSG_FAILED_PARSE_ARGS_STR "Failed to parse the arguments.\n"
#define MSG_FAILED_CREATE_FILES_LIST_STR "Failed to create the files list.\n"
#define MSG_FAILED_CREATE_LOG_OBJECT_STR "Failed to create the notifications object.\n"
#define MSG_FAILED_MSG_PORT_STR "Failed to create the message port for window drag'n drop.\n"
#define MSG_FAILED_OPEN_LOCALE_STR "Failed to open the default Locale.\n"
#define MSG_FAILED_ALLOC_APP_MEMORY_STR "Failed to allocate memory for application instance data.\n"
#define MSG_FAILED_OPEN_WINDOW_STR "Failed to open window.\n"
#define MSG_MAIN_WINDOW_STR "MultiRename in ["
#define MSG_OK_SHOW_GAD_STR "Ok"
#define MSG_LOGREQ_FAILED_TO_ADD_STR "Failed to add some of the input files"
#define MSG_NO_FILES_TO_RENAME_STR "No files to rename."
#define MSG_FAILED_CREATE_TOKENS_STR "Error, failed to create file name tokens!"
#define MSG_WARN_DUPLICATES_1_STR "Warning, duplicate names! Proceed anyway?\n"
#define MSG_WARN_DUPLICATES_2_STR "NOTE: Proceed will auto rename duplicate files to \n"
#define MSG_WARN_DUPLICATES_3_STR "  name (2).ext\n"
#define MSG_WARN_DUPLICATES_4_STR "  name (3).ext\n"
#define MSG_WARN_DUPLICATES_5_STR "and so on."
#define MSG_PROCEED_CANCEL_GAD_STR "Proceed"
#define MSG_ERROR_AUTOCREATE_NAMES_STR "Error, failed to automatically create name for duplicate file!"
#define MSG_ERROR_AUTOCREATED_NAMES_TOO_LONG_STR "Error, auto-renamed file name would be too long for file system!"
#define MSG_STATE_OK_STR "Ok"
#define MSG_STATE_COMMAND_ERROR_STR "Cmd"
#define MSG_NAME_ERROR_STR "<Error!>"
#define MSG_NAME_TOOL_NEEDS_ITEMS_TITLE_STR "MultiRename: Select name part"
#define MSG_NAME_TOOL_NEEDS_ITEMS_STR "This tool is only available if you have files in the processing list."
#define MSG_EXT_TOOL_NEEDS_ITEMS_TITLE_STR "MultiRename: Select extension part"
#define MSG_EXT_TOOL_NEEDS_ITEMS_AND_EXT_STR "This tool is only available if you have files in the processing list and if at least one of them has an extension like '.iff'."
#define MSG_OK_SHOW_ERRORS_GAD_STR "Ok"
#define MSG_RENAME_FAILED_FOR_SOME_STR "Failed to rename some of the input files"
#define MSG_CONTINUE_CANCEL_GAD_STR "Continue"
#define MSG_CONTINUE_WILL_SCRATCH_1_STR "Continue to create a new project will clear the processing list\n"
#define MSG_CONTINUE_WILL_SCRATCH_2_STR "and set the masks to a default value.\n\n"
#define MSG_CONTINUE_WILL_SCRATCH_3_STR "Continue anyway?"
#define MSG_FAILED_RECREATE_FILES_LIST_STR "Failed to re-create the files list.\n"
#define MSG_FILESELECTOR_TITLE_STR "Select files to rename"
#define MSG_FAILED_APPLY_RANGE_STR "Failed to apply selected range\n"
#define MSG_COLUMN_TITLE_STATE_STR "State"
#define MSG_COLUMN_TITLE_TYPE_STR "Type"
#define MSG_COLUMN_TITLE_OLD_NAME_STR "Old name"
#define MSG_COLUMN_TITLE_NEW_NAME_STR "New name"
#define MSG_NAME_GROUP_STR "Name"
#define MSG_NAME_NAME_GAD_STR "[N] Name"
#define MSG_NAME_DATE_GAD_STR "[YMD] Date"
#define MSG_NAME_PART_GAD_STR "[N#-#] Part..."
#define MSG_NAME_TIME_GAD_STR "[hms] Time"
#define MSG_NAME_COUNTER_GAD_STR "[C] Counter"
#define MSG_EXTENSION_GROUP_STR "Extension"
#define MSG_EXT_EXT_GAD_STR "[E] Ext."
#define MSG_EXT_PART_GAD_STR "[E#-#] Part..."
#define MSG_EXT_COUNTER_GAD_STR "[C] Counter"
#define MSG_COUNTER_GROUP_STR "Define counter"
#define MSG_COUNTER_START_GAD_STR "Start:"
#define MSG_COUNTER_STEP_GAD_STR "Step:"
#define MSG_COUNTER_PLACES_GAD_STR "Places:"
#define MSG_PROCESSING_LIST_GROUP_STR "Processing list"
#define MSG_START_RENAME_GAD_STR "Start rename"
#define MSG_FAILED_TO_OPEN_INTUITION_STR "Failed to open intuition.library v47.\n"
#define MSG_FAILED_TO_OPEN_WINCLASS_STR "Failed to open window.class v47.\n"
#define MSG_FAILED_TO_OPEN_LAYOUTGAD_STR "Failed to open layout.gadget v47.\n"
#define MSG_FAILED_TO_OPEN_BEVELIMG_STR "Failed to open bevel.image v47.\n"
#define MSG_FAILED_TO_OPEN_BUTTONGAD_STR "Failed to open button.gadget v47.\n"
#define MSG_FAILED_TO_OPEN_CHOOSERGAD_STR "Failed to open chooser.gadget v47.\n"
#define MSG_FAILED_TO_OPEN_INTEGERGAD_STR "Failed to open integer.gadget v47.\n"
#define MSG_FAILED_TO_OPEN_LISTBROWSERGAD_STR "Failed to open listbrowser.gadget v47.\n"
#define MSG_FAILED_TO_OPEN_LABELIMG_STR "Failed to open label.image v47.\n"
#define MSG_FAILED_TO_OPEN_SLIDERGAD_STR "Failed to open slider.gadget v47.\n"
#define MSG_FAILED_TO_OPEN_STRINGGAD_STR "Failed to open string.gadget v47.\n"
#define MSG_SEARCH_REPLACE_GROUP_STR "Search & Replace"
#define MSG_SEARCH_FOR_GAD_STR "Search For"
#define MSG_REPLACE_WITH_GAD_STR "Replace With"
#define MSG_PROJECT_LOAD_NAMES_FROM_FILE_STR "Load names from file..."
#define MSG_WRONG_NUMBER_OF_LINES_STR "File contains wrong number of lines: %lu, should be %lu!"

#endif /* CATCOMP_STRINGS */


/****************************************************************************/


#ifdef CATCOMP_BLOCK

static const char CatCompBlock[] =
{
    "\x00\x00\x00\x00\x00\x04"
    MSG_TYPE_IS_DIRECTORY_STR "\x00"
    "\x00\x00\x00\x01\x00\x18"
    MSG_LOG_SELECTED_PATH_STR "\x00"
    "\x00\x00\x00\x02\x00\x34"
    MSG_LOG_SKIPPED_WRONG_PATHS_STR "\x00\x00"
    "\x00\x00\x00\x03\x00\x44"
    MSG_LOG_SKIPPED_OVERLONG_PATHS_STR "\x00\x00"
    "\x00\x00\x00\x04\x00\x52"
    MSG_LOG_SKIPPED_DATESTAMP_STR "\x00"
    "\x00\x00\x00\x05\x00\x3E"
    MSG_LOG_SKIPPED_EXAMINE_STR "\x00"
    "\x00\x00\x00\x06\x00\x32"
    MSG_LOG_SKIPPED_LOCK_STR "\x00"
    "\x00\x00\x00\x07\x00\x3C"
    MSG_LOG_SKIPPED_LINKS_STR "\x00"
    "\x00\x00\x00\x08\x00\x48"
    MSG_LOG_SKIPPED_ALREADY_IN_LIST_STR "\x00"
    "\x00\x00\x00\x09\x00\x28"
    MSG_LOG_RENAME_FAILED_STR "\x00\x00"
    "\x00\x00\x00\x0A\x00\x0E"
    MSG_LONGEST_ITEM_STR "\x00"
    "\x00\x00\x00\x0B\x00\x06"
    MSG_FROM_STR "\x00"
    "\x00\x00\x00\x0C\x00\x04"
    MSG_TO_STR "\x00"
    "\x00\x00\x00\x0D\x00\x12"
    MSG_SELECTION_RESULT_STR "\x00"
    "\x00\x00\x00\x0E\x00\x0E"
    MSG_RESULT_MASK_STR "\x00\x00"
    "\x00\x00\x00\x0F\x00\x04"
    MSG_OK_GAD_STR "\x00\x00"
    "\x00\x00\x00\x10\x00\x08"
    MSG_CANCEL_GAD_STR "\x00\x00"
    "\x00\x00\x00\x11\x00\x36"
    MSG_ABOUT_1_STR "\x00"
    "\x00\x00\x00\x12\x00\x16"
    MSG_SETTINGS_ALLOW_LONG_NAMES_STR "\x00\x00"
    "\x00\x00\x00\x13\x00\x0C"
    MSG_SETTINGS_SKIP_ICONS_STR "\x00\x00"
    "\x00\x00\x00\x14\x00\x08"
    MSG_PROJECT_MENU_STR "\x00"
    "\x00\x00\x00\x15\x00\x04"
    MSG_PROJECT_NEW_STR "\x00"
    "\x00\x00\x00\x16\x00\x0E"
    MSG_PROJECT_ADD_FILES_STR "\x00\x00"
    "\x00\x00\x00\x17\x00\x06"
    MSG_PROJECT_ABOUT_STR "\x00"
    "\x00\x00\x00\x18\x00\x06"
    MSG_PROJECT__STR "\x00\x00"
    "\x00\x00\x00\x19\x00\x0A"
    MSG_SETTINGS_MENU_STR "\x00\x00"
    "\x00\x00\x00\x1A\x00\x24"
    MSG_FAILED_LOCK_PUBSCREEN_STR "\x00\x00"
    "\x00\x00\x00\x1B\x00\x28"
    MSG_FAILED_LOCK_DEFAULT_PUBSCREEN_STR "\x00\x00"
    "\x00\x00\x00\x1C\x00\x26"
    MSG_FAILED_CREATE_ABOUT_MSG_STR "\x00\x00"
    "\x00\x00\x00\x1D\x00\x2C"
    MSG_FAILED_CREATE_RANGESEL_WINDOW_STR "\x00\x00"
    "\x00\x00\x00\x1E\x00\x30"
    MSG_FAILED_CREATE_MAIN_WINDOW_STR "\x00\x00"
    "\x00\x00\x00\x1F\x00\x1A"
    MSG_FAILED_CREATE_LAYOUT_STR "\x00"
    "\x00\x00\x00\x20\x00\x20"
    MSG_FAILED_PARSE_ARGS_STR "\x00"
    "\x00\x00\x00\x21\x00\x22"
    MSG_FAILED_CREATE_FILES_LIST_STR "\x00"
    "\x00\x00\x00\x22\x00\x2C"
    MSG_FAILED_CREATE_LOG_OBJECT_STR "\x00"
    "\x00\x00\x00\x23\x00\x3C"
    MSG_FAILED_MSG_PORT_STR "\x00\x00"
    "\x00\x00\x00\x24\x00\x24"
    MSG_FAILED_OPEN_LOCALE_STR "\x00"
    "\x00\x00\x00\x25\x00\x3A"
    MSG_FAILED_ALLOC_APP_MEMORY_STR "\x00"
    "\x00\x00\x00\x26\x00\x18"
    MSG_FAILED_OPEN_WINDOW_STR "\x00"
    "\x00\x00\x00\x27\x00\x12"
    MSG_MAIN_WINDOW_STR "\x00\x00"
    "\x00\x00\x00\x28\x00\x04"
    MSG_OK_SHOW_GAD_STR "\x00\x00"
    "\x00\x00\x00\x29\x00\x26"
    MSG_LOGREQ_FAILED_TO_ADD_STR "\x00"
    "\x00\x00\x00\x2A\x00\x14"
    MSG_NO_FILES_TO_RENAME_STR "\x00"
    "\x00\x00\x00\x2B\x00\x2A"
    MSG_FAILED_CREATE_TOKENS_STR "\x00"
    "\x00\x00\x00\x2C\x00\x2C"
    MSG_WARN_DUPLICATES_1_STR "\x00\x00"
    "\x00\x00\x00\x2D\x00\x34"
    MSG_WARN_DUPLICATES_2_STR "\x00"
    "\x00\x00\x00\x2E\x00\x10"
    MSG_WARN_DUPLICATES_3_STR "\x00"
    "\x00\x00\x00\x2F\x00\x10"
    MSG_WARN_DUPLICATES_4_STR "\x00"
    "\x00\x00\x00\x30\x00\x0C"
    MSG_WARN_DUPLICATES_5_STR "\x00\x00"
    "\x00\x00\x00\x31\x00\x08"
    MSG_PROCEED_CANCEL_GAD_STR "\x00"
    "\x00\x00\x00\x32\x00\x40"
    MSG_ERROR_AUTOCREATE_NAMES_STR "\x00\x00"
    "\x00\x00\x00\x33\x00\x42"
    MSG_ERROR_AUTOCREATED_NAMES_TOO_LONG_STR "\x00\x00"
    "\x00\x00\x00\x34\x00\x04"
    MSG_STATE_OK_STR "\x00\x00"
    "\x00\x00\x00\x35\x00\x04"
    MSG_STATE_COMMAND_ERROR_STR "\x00"
    "\x00\x00\x00\x36\x00\x0A"
    MSG_NAME_ERROR_STR "\x00\x00"
    "\x00\x00\x00\x37\x00\x1E"
    MSG_NAME_TOOL_NEEDS_ITEMS_TITLE_STR "\x00"
    "\x00\x00\x00\x38\x00\x46"
    MSG_NAME_TOOL_NEEDS_ITEMS_STR "\x00"
    "\x00\x00\x00\x39\x00\x24"
    MSG_EXT_TOOL_NEEDS_ITEMS_TITLE_STR "\x00\x00"
    "\x00\x00\x00\x3A\x00\x80"
    MSG_EXT_TOOL_NEEDS_ITEMS_AND_EXT_STR "\x00\x00"
    "\x00\x00\x00\x3B\x00\x04"
    MSG_OK_SHOW_ERRORS_GAD_STR "\x00\x00"
    "\x00\x00\x00\x3C\x00\x2A"
    MSG_RENAME_FAILED_FOR_SOME_STR "\x00\x00"
    "\x00\x00\x00\x3D\x00\x0A"
    MSG_CONTINUE_CANCEL_GAD_STR "\x00\x00"
    "\x00\x00\x00\x3E\x00\x42"
    MSG_CONTINUE_WILL_SCRATCH_1_STR "\x00\x00"
    "\x00\x00\x00\x3F\x00\x28"
    MSG_CONTINUE_WILL_SCRATCH_2_STR "\x00"
    "\x00\x00\x00\x40\x00\x12"
    MSG_CONTINUE_WILL_SCRATCH_3_STR "\x00\x00"
    "\x00\x00\x00\x41\x00\x26"
    MSG_FAILED_RECREATE_FILES_LIST_STR "\x00\x00"
    "\x00\x00\x00\x42\x00\x18"
    MSG_FILESELECTOR_TITLE_STR "\x00\x00"
    "\x00\x00\x00\x43\x00\x20"
    MSG_FAILED_APPLY_RANGE_STR "\x00"
    "\x00\x00\x00\x44\x00\x06"
    MSG_COLUMN_TITLE_STATE_STR "\x00"
    "\x00\x00\x00\x45\x00\x06"
    MSG_COLUMN_TITLE_TYPE_STR "\x00\x00"
    "\x00\x00\x00\x46\x00\x0A"
    MSG_COLUMN_TITLE_OLD_NAME_STR "\x00\x00"
    "\x00\x00\x00\x47\x00\x0A"
    MSG_COLUMN_TITLE_NEW_NAME_STR "\x00\x00"
    "\x00\x00\x00\x48\x00\x06"
    MSG_NAME_GROUP_STR "\x00\x00"
    "\x00\x00\x00\x49\x00\x0A"
    MSG_NAME_NAME_GAD_STR "\x00\x00"
    "\x00\x00\x00\x4A\x00\x0C"
    MSG_NAME_DATE_GAD_STR "\x00\x00"
    "\x00\x00\x00\x4B\x00\x10"
    MSG_NAME_PART_GAD_STR "\x00\x00"
    "\x00\x00\x00\x4C\x00\x0C"
    MSG_NAME_TIME_GAD_STR "\x00\x00"
    "\x00\x00\x00\x4D\x00\x0C"
    MSG_NAME_COUNTER_GAD_STR "\x00"
    "\x00\x00\x00\x4E\x00\x0A"
    MSG_EXTENSION_GROUP_STR "\x00"
    "\x00\x00\x00\x4F\x00\x0A"
    MSG_EXT_EXT_GAD_STR "\x00\x00"
    "\x00\x00\x00\x50\x00\x10"
    MSG_EXT_PART_GAD_STR "\x00\x00"
    "\x00\x00\x00\x51\x00\x0C"
    MSG_EXT_COUNTER_GAD_STR "\x00"
    "\x00\x00\x00\x52\x00\x10"
    MSG_COUNTER_GROUP_STR "\x00\x00"
    "\x00\x00\x00\x53\x00\x08"
    MSG_COUNTER_START_GAD_STR "\x00\x00"
    "\x00\x00\x00\x54\x00\x06"
    MSG_COUNTER_STEP_GAD_STR "\x00"
    "\x00\x00\x00\x55\x00\x08"
    MSG_COUNTER_PLACES_GAD_STR "\x00"
    "\x00\x00\x00\x56\x00\x10"
    MSG_PROCESSING_LIST_GROUP_STR "\x00"
    "\x00\x00\x00\x57\x00\x0E"
    MSG_START_RENAME_GAD_STR "\x00\x00"
    "\x00\x00\x00\x58\x00\x28"
    MSG_FAILED_TO_OPEN_INTUITION_STR "\x00\x00"
    "\x00\x00\x00\x59\x00\x22"
    MSG_FAILED_TO_OPEN_WINCLASS_STR "\x00"
    "\x00\x00\x00\x5A\x00\x24"
    MSG_FAILED_TO_OPEN_LAYOUTGAD_STR "\x00\x00"
    "\x00\x00\x00\x5B\x00\x22"
    MSG_FAILED_TO_OPEN_BEVELIMG_STR "\x00\x00"
    "\x00\x00\x00\x5C\x00\x24"
    MSG_FAILED_TO_OPEN_BUTTONGAD_STR "\x00\x00"
    "\x00\x00\x00\x5D\x00\x24"
    MSG_FAILED_TO_OPEN_CHOOSERGAD_STR "\x00"
    "\x00\x00\x00\x5E\x00\x24"
    MSG_FAILED_TO_OPEN_INTEGERGAD_STR "\x00"
    "\x00\x00\x00\x5F\x00\x28"
    MSG_FAILED_TO_OPEN_LISTBROWSERGAD_STR "\x00"
    "\x00\x00\x00\x60\x00\x22"
    MSG_FAILED_TO_OPEN_LABELIMG_STR "\x00\x00"
    "\x00\x00\x00\x61\x00\x24"
    MSG_FAILED_TO_OPEN_SLIDERGAD_STR "\x00\x00"
    "\x00\x00\x00\x62\x00\x24"
    MSG_FAILED_TO_OPEN_STRINGGAD_STR "\x00\x00"
    "\x00\x00\x00\x63\x00\x12"
    MSG_SEARCH_REPLACE_GROUP_STR "\x00\x00"
    "\x00\x00\x00\x64\x00\x0C"
    MSG_SEARCH_FOR_GAD_STR "\x00\x00"
    "\x00\x00\x00\x65\x00\x0E"
    MSG_REPLACE_WITH_GAD_STR "\x00\x00"
    "\x00\x00\x00\x66\x00\x18"
    MSG_PROJECT_LOAD_NAMES_FROM_FILE_STR "\x00"
    "\x00\x00\x00\x67\x00\x3A"
    MSG_WRONG_NUMBER_OF_LINES_STR "\x00\x00"
};

#endif /* CATCOMP_BLOCK */


/****************************************************************************/


struct LocaleInfo
{
    APTR li_LocaleBase;
    APTR li_Catalog;
};



#endif /* MULTIRENAME_CATALOG_H */
