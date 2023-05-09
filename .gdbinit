# SPDX-License-Identifier: zlib-acknowledgement
# NOTE(Ryan): Allow loading .gdbinit from working directory
set auto-load safe-path /

set disassembly-flavor intel

# NOTE(Ryan): Don't print prompt message and wait for user input for large outputs
set pagination off

# NOTE(Ryan): Use ! to search history
set history save on
set history size 10000
set history filename ~/.gdb_history
set history expansion on

# NOTE(Ryan): Print struct fields on new lines
set print pretty on

# disable xmethod

# NOTE(Ryan): Enable breakpoints in CRT
set backtrace past-main on

# NOTE(Ryan): GDB stdout to file
# set logging on

# NOTE(Ryan): Set GDB inferior as child process
# set follow-fork-mode child
# TODO(Ryan): Multiprocessing
# https://www.youtube.com/watch?v=ZBm4RqNvjEw  

# IMPORTANT(Ryan): QTCreator does not load ~/.gdbinit by default.
# Inside Tools-Options-GDB-AdditionalStartupCommands:
#  source ~/.gdbinit
#  source ~/python_gdb.py
# IMPORTANT(Ryan): `info pretty-printer` shows that custom pretty printers not enabled by default
# So, must tick 'Load system GDB pretty printers'. Otherwise, manually enable:
#  enable pretty-printer global my_pp_func

# IMPORTANT(Ryan): To ensure '\n' are interpreted correctly in QTCreator console
# pipe print state->first_entity | cat

# (gdb) set $pc = <pc from fault handler>
# (gdb) set $lr = <lr from fault handler>
# (gdb) set $sp = <sp from fault handler>
# 
# # Hopefully now a real backtrace!
# (gdb) bt

# TODO(Ryan): Can walk up stack-frames for recursive functions?
# This is time-travel/reversible debugging. Very slow in gdb
# https://developers.redhat.com/articles/2022/06/07/how-debug-stack-frames-and-recursion-gdb#intermediate_frame_manipulation
