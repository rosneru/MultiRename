from copy import deepcopy
from enum import Enum
from transitions import Machine, State
from typing import List


class DateTimeParts:
    def __init__(self, file_date: str):
        if len(file_date) != 19:
            raise Exception('File date must be in format'
                            ' \'2024-05-07-17-39-30\' (19 chars long).')
        self.year = file_date[0:4]
        self.month = file_date[5:7]
        self.day = file_date[8:10]
        self.hour = file_date[11:13]
        self.minute = file_date[14:16]
        self.second = file_date[17:19]


class FileNode:
    def __init__(self, file_name: str, file_date: str):
        self.original_name = file_name
        self.original_name_len = len(self.original_name)
        self.new_name = ''
        ext_start = file_name.rfind('.')
        if ext_start > -1:
            ext_start += 1
            self.original_ext_len = len(file_name[ext_start:])
        else:
            self.original_ext_len = 0

        if self.original_ext_len > 0:
            self.original_name_len -= (self.original_ext_len + 1)

        self.date_time_parts = DateTimeParts(file_date)


class Command(Enum):
    NONE = 0
    APPLY = 1,
    NAME = 2,
    EXTENSION = 3,
    COUNTER = 4
    YEAR = 5
    MONTH = 6
    DAY = 7
    HOUR = 8
    MINUTE = 9
    SECOND = 10


class Action:
    def __init__(self, command: Command,
                 start: int = -1,
                 end: int = -1):
        self.command: Command = command
        self.start: int = start
        self.end: int = end


class ActionParser(object):
    class States(Enum):
        APPLY = 0
        PARSE_COMMAND = 1
        DETECT_RANGE = 2
        PARSE_FROM = 3
        PARSE_TO = 4
        FINISHED = 5
        ERROR = 6

    states = [
        State(States.APPLY, on_enter='init_state_apply'),
        State(States.PARSE_COMMAND, on_enter='init_state_parse_command'),
        State(States.DETECT_RANGE, on_enter='init_state_detect_range'),
        State(States.PARSE_FROM, on_enter='init_state_parse_from'),
        State(States.PARSE_TO, on_enter='init_state_parse_to'),
        State(States.FINISHED, on_enter='init_state_finished'),
        State(States.ERROR),
    ]

    transitions = [
        ['enter_state_apply', States.DETECT_RANGE, States.APPLY],
        ['enter_state_apply', States.PARSE_TO, States.APPLY],
        ['enter_state_parse_command', States.APPLY, States.PARSE_COMMAND],
        ['enter_state_parse_command', States.DETECT_RANGE, States.PARSE_COMMAND],
        ['enter_state_detect_range', States.PARSE_COMMAND, States.DETECT_RANGE],
        ['enter_state_parse_from', States.DETECT_RANGE, States.PARSE_FROM],
        ['enter_state_parse_to', States.PARSE_FROM, States.PARSE_TO],
        ['enter_state_finished', States.APPLY, States.FINISHED],
        ['enter_state_error', States.APPLY, States.ERROR],
        ['enter_state_error', States.PARSE_COMMAND, States.ERROR],
        ['enter_state_error', States.DETECT_RANGE, States.ERROR],
        ['enter_state_error', States.PARSE_FROM, States.ERROR],
        ['enter_state_error', States.PARSE_TO, States.ERROR],
    ]

    def __init__(self, mask: str):
        self.action_list: List[Action] = []
        self.command: Command = Command.NONE
        self.command_from = 0
        self.command_to = 0
        self.numeric_from = -1
        self.numeric_to = -1
        self.mask = mask
        self.mask_len = len(mask)
        self.mask_idx = 0
        self.machine = Machine(model=self,
                               states=ActionParser.states,
                               transitions=ActionParser.transitions,
                               initial=ActionParser.States.APPLY)

    def get_parsed_actions(self) -> List[Action]:
        return self.action_list

    def parse(self) -> bool:
        self.mask_idx = 0
        self.action_list: List[Action] = []
        self.init_state_apply()
        while self.state is not ActionParser.States.FINISHED:
            if self.state is ActionParser.States.ERROR:
                break
            self.update()
        return self.state is ActionParser.States.FINISHED

    def update(self):
        match self.state:
            case ActionParser.States.APPLY:
                self.do_state_apply()
            case ActionParser.States.FINISHED:
                self.do_state_finished()
            case ActionParser.States.PARSE_COMMAND:
                self.do_state_parse_command()
            case ActionParser.States.DETECT_RANGE:
                self.do_state_detect_range()
            case ActionParser.States.PARSE_FROM:
                self.do_state_parse_from()
            case ActionParser.States.PARSE_TO:
                self.do_state_parse_to()

    def add_pending_action(self):
        '''Create an Action from current command and add it to the list'''
        if self.command is Command.NONE:
            return
        if self.command_from > -1 and self.command_to < 0:
            return
        action = Action(self.command, self.command_from, self.command_to)
        self.action_list.append(action)

    def init_state_apply(self):
        self.command: Command = Command.APPLY
        self.command_from = self.mask_idx
        self.command_to = -1

    def do_state_apply(self):
        if self.mask_idx == self.mask_len:
            self.enter_state_finished()
            return

        c = self.mask[self.mask_idx]
        if self._is_char_allowed(c):
            self.command_to = self.mask_idx
            self.mask_idx += 1
        elif c == '[':
            self.add_pending_action()
            self.mask_idx += 1
            self.enter_state_parse_command()
        else:
            self.enter_state_error()

    def init_state_finished(self):
        self.add_pending_action()

    def do_state_finished(self):
        pass

    def init_state_parse_command(self):
        pass

    def do_state_parse_command(self):
        if self.mask_idx == self.mask_len:
            self.enter_state_error()
            return

        c = self.mask[self.mask_idx]
        self.mask_idx += 1
        match c:
            case 'N':
                self.command = Command.NAME
                self.enter_state_detect_range()
            case 'E':
                self.command = Command.EXTENSION
                self.enter_state_detect_range()
            case 'C':
                self.command = Command.COUNTER
                self.enter_state_detect_range()
            case 'Y':
                self.command = Command.YEAR
                self.enter_state_detect_range()
            case 'M':
                self.command = Command.MONTH
                self.enter_state_detect_range()
            case 'D':
                self.command = Command.DAY
                self.enter_state_detect_range()
            case 'h':
                self.command = Command.HOUR
                self.enter_state_detect_range()
            case 'm':
                self.command = Command.MINUTE
                self.enter_state_detect_range()
            case 's':
                self.command = Command.SECOND
                self.enter_state_detect_range()
            case _:
                self.enter_state_error()

    def init_state_detect_range(self):
        self.command_from = -1
        self.command_to = -1

    def do_state_detect_range(self):
        if self.mask_idx == self.mask_len:
            self.enter_state_error()
            return

        c = self.mask[self.mask_idx]
        if (c == 'Y' or c == 'M' or c == 'D'
                or c == 'h' or c == 'm' or c == 's'):
            self.add_pending_action()
            self.enter_state_parse_command()
        elif c == ']':
            self.add_pending_action()
            self.mask_idx += 1
            self.enter_state_apply()
        elif self.command is Command.COUNTER:
            # A 'Counter' must be finished with an ']' immediately and this
            # hasn't been done here. So its an error.
            self.enter_state_error()
        elif self._is_char_digit(c):
            self.enter_state_parse_from()
        else:
            self.enter_state_error()

    def init_state_parse_from(self):
        self.numeric_from = self.mask_idx
        self.numeric_to = -1

    def do_state_parse_from(self):
        if self.mask_idx == self.mask_len:
            self.enter_state_error()
            return

        c = self.mask[self.mask_idx]
        if self._is_char_digit(c):
            self.numeric_to = self.mask_idx
            self.mask_idx += 1
        elif c == '-' and self.numeric_to > -1:
            self.mask_idx += 1
            self.command_from = self._my_atoi(
                self.mask, self.numeric_from, self.numeric_to)
            self.enter_state_parse_to()
        else:
            self.enter_state_error()

    def init_state_parse_to(self):
        self.numeric_from = self.mask_idx
        self.numeric_to = -1

    def do_state_parse_to(self):
        if self.mask_idx == self.mask_len:
            self.enter_state_error()
            return

        c = self.mask[self.mask_idx]
        if self._is_char_digit(c):
            self.numeric_to = self.mask_idx
            self.mask_idx += 1
        elif c == ']' and self.numeric_to > -1:
            self.mask_idx += 1
            self.command_from -= 1
            self.command_to = self._my_atoi(
                self.mask, self.numeric_from, self.numeric_to) - 1
            self.add_pending_action()
            self.enter_state_apply()
        else:
            self.enter_state_error()

    def _my_atoi(self, s: str, s_from: int, s_to: int)-> int:
        return int(s[s_from:s_to + 1])

    def _is_char_digit(self, c) -> bool:
        asc = ord(c)
        if asc >= 48 and asc < 58:
            # 0..9
            return True
        return False

    def _is_char_allowed(self, c) -> bool:
        '''I know there are better ways in Python. But this will become a C
        implementation and to test that already here, its implemented like
        that.'''
        #                            Sp  '   @   ,   -   .   _   {    }
        allowed_non_alphanumerics = [32, 39, 40, 44, 45, 46, 95, 123, 125]
        asc = ord(c)
        if self._is_char_digit(c):
            return True
        elif asc > 64 and asc < 91:
            # A..Z
            return True
        elif asc > 96 and asc < 123:
            # a..z
            return True
        else:
            for i in allowed_non_alphanumerics:
                if asc == i:
                    return True
        return False


class Counter:
    def __init__(self, start: int, inc: int, width: int):
        self.value = start
        self.start = start
        self.inc = inc
        self.width = width

    def get_value(self) -> str:
        return str(self.value).zfill(self.width)

    def increment(self):
        self.value += self.inc


def apply_actions(actions: List[Action],
                  name: str,
                  name_len: int,
                  ext_len: int,
                  date_time_parts: DateTimeParts,
                  mask: str,
                  counter: Counter) -> str:
    result = ''
    must_increment_counter = False
    for action in actions:
        match action.command:
            case Command.APPLY:
                result += mask[action.start: action.end + 1]
            case Command.COUNTER:
                result += counter.get_value()
                must_increment_counter = True
            case Command.NAME:
                if action.start > -1 and action.end > -1:
                    end = action.end
                    if end >= name_len:
                        end = name_len - 1
                    result += name[action.start: end + 1]
                else:
                    result += name[0:name_len]
            case Command.EXTENSION:
                if action.start > -1 and action.end > -1:
                    start = name_len + action.start + 1
                    end = name_len + action.end
                    if end >= ext_len:
                        end = ext_len - 1
                    result += name[start: end + 1]
                else:
                    start = name_len + 1
                    result += name[start:]
            case Command.YEAR:
                result += date_time_parts.year
            case Command.MONTH:
                result += date_time_parts.month
            case Command.DAY:
                result += date_time_parts.day
            case Command.HOUR:
                result += date_time_parts.hour
            case Command.MINUTE:
                result += date_time_parts.minute
            case Command.SECOND:
                result += date_time_parts.second
    if must_increment_counter:
        counter.increment()

    while result.endswith('.'):
        result = result[:-1]
    return result


def create_new_names(files: List[FileNode],
                     name_mask: str,
                     ext_mask: str,
                     counter: Counter = Counter(1, 1, 1)) -> bool:
    '''Iterate the given list of FileNodes. For every FileNode the field
    new_name is newly filled according the old_name and given parameters
    name_mask, ext_mask and counter settings.'''

    mask: str = f'{name_mask}.{ext_mask}'
    parser = ActionParser(mask)

    if parser.parse() is False:
        return False

    actions: List[Action] = parser.get_parsed_actions()

    cnt = deepcopy(counter)
    for file in files:
        file.new_name = apply_actions(actions,
                                      file.original_name,
                                      file.original_name_len,
                                      file.original_ext_len,
                                      file.date_time_parts,
                                      mask,
                                      cnt)
    return True
