# SPDX-License-Identifier: zlib-acknowledgement
set disassembly-flavor intel
break __bp_debugger
break __ebp_debugger

# Debug child process
set follow-fork-mode child

# Debug both parent and child processes
# set detach-on-fork off
