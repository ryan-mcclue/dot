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

# break sort_entities_by_z_index

# IMPORTANT(Ryan): QTCreator does not load ~/.gdbinit by default.
# Inside Tools-Options-GDB-AdditionalStartupCommands:
#  source ~/.gdbinit
#  python exec(open("~/python_gdb.py").read())

# (gdb) set $pc = <pc from fault handler>
# (gdb) set $lr = <lr from fault handler>
# (gdb) set $sp = <sp from fault handler>
# 
# # Hopefully now a real backtrace!
# (gdb) bt


# supports pipe command: | help break | head -4 

# break *main (address, i.e. function prologue)
# break *0x1234 + 16

# LINESPEC
# break main
# break do_work thread 1
# break overloaded(int)
# break print_name if strcmp(name, "ryan") == 0 && age == 26

# EXPLICIT (to save lookups)
# break -func main

# these populate convenience variable $bpnum
# to execute a series of commands after breakpoint has been hit
# commands $bpnum
# silent
# enable 1 (to allow a breakpoint to only be run after another function was called)
# continue

# NOW WHEN run IS EXECUTED, THIS HOOK WILL BE RUN PRIOR
# define hook-run
# disable 1

# print *struct_addr; will print out structure and its variable values
# TODO(Ryan): python pretty print wrapper to perform ascii visualisations say of data structures, heap, etc.
# https://github.com/chrisc11/debug-tips/blob/master/gdb/python-linked-list.md
# https://interrupt.memfault.com/blog/automate-debugging-with-gdb-python-api 

# save breakpoints <file>; source <file>

# ignore $bpnum 1000

# TODO(Ryan): Can walk up stack-frames for recursive functions?
# https://developers.redhat.com/articles/2022/06/07/how-debug-stack-frames-and-recursion-gdb#intermediate_frame_manipulation
