#! /usr/bin/env bash
# SPDX-License-Identifier: zlib-acknowledgement

[[ ! $- ~= "i" ]] && return 0;

source ~/.bash/git-prompt.sh
export GIT_PS1_SHOWDIRTYSTATE=1
export GIT_PS1_SHOWCOLORHINTS=true
export PROMPT_COMMAND='__git_ps1 "\u@\h:\w" "\\\$ "'

cd ~/prog/personal
