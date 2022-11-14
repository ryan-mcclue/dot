# SPDX-License-Identifier: zlib-acknowledgement
set disassembly-flavor intel

set pagination off
set history save on
# can use ! to search history
set history expansion on

set print pretty

# python print('Hello from python!')

# Debug child process
# set follow-fork-mode child

# Debug both parent and child processes
# set detach-on-fork off

# supports pipe command: | help break | head -4 

# gdb inferior is the program you are debugging

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
