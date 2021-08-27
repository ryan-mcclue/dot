#!/bin/bash
# SPDX-License-Identifier: zlib-acknowledgement

case $- in
  *i*) ;;
  *) return ;;
esac

# IMPORTANT(Ryan): For PS1, non-printable characters must be enclosed with \[...\].
# Without this, line-wrapping will be off.
_pdark_yellow_fg=$'\[\e[01;33m\]'
_pdark_red_fg=$'\[\e[01;31m\]'
_pdark_green_fg=$'\[\e[01;32m\]'
_pdark_blue_fg=$'\[\e[01;34m\]'
_pdark_cyan_fg=$'\[\e[01;36m\]'
_pcolour_reset=$'\[\e[0m\]'

__ps1() {
  local privelege_char;
  if test $EUID -eq 0; then
    privelege_char='#' 
  else
    privelege_char='$'
  fi

  local cur_dir;
  case "$PWD" in
    "$HOME"*) cur_dir="~${PWD#"$HOME"}" ;;
    *) cur_dir="$PWD"
  esac

  local git_dirty_char;
  test -n "$(git status --porcelain 2>/dev/null)" && git_dirty_char="*"
  local cur_git_branch=$(git branch --show-current 2>/dev/null)
  if test -n "$cur_git_branch"; then
    if test -n "$git_dirty_char"; then
      cur_git_branch="$_pdark_red_fg($cur_git_branch $git_dirty_char)$_pcolour_reset"
    else
      cur_git_branch="$_pdark_yellow_fg($cur_git_branch)$_pcolour_reset"
    fi
  fi

  # NOTE(Ryan): Chroot takes precedence. 
  # Situation where both are active is discounted based on unlikelihood of occuring.
  local venv_name="${VIRTUAL_ENV##*/}"
  test -r '/etc/debian_chroot' && local chroot_name=$(cat /etc/debian_chroot)
  test -n "$venv_name" && local env="$venv_name"
  test -n "$chroot_name" && local env="$chroot_name"
  local env_prompt="${env:+$_pdark_cyan_fg($env)$_pcolour_reset}"

  local prompt_length_calc="$env_prompt$USER@$(hostname):$cur_dir$cur_git_branch\$ "
  local single_line_fmt="\
$env_prompt\
$_pdark_green_fg\u@\h:$_pcolour_reset\
$_pdark_blue_fg$cur_dir$_pcolour_reset\
$cur_git_branch\
$_pdark_green_fg$privelege_char$_pcolour_reset "
  local double_line_fmt="\
$_pdark_green_fg╔ $_pcolour_reset\
$env_prompt\
$_pdark_green_fg\u@\h:$_pcolour_reset\
$_pdark_blue_fg$cur_dir$_pcolour_reset\
$cur_git_branch\n\
$_pdark_green_fg╚$_pcolour_reset \
$_pdark_green_fg$privelege_char$_pcolour_reset "
  local triple_line_fmt="\
$_pdark_green_fg╔ $_pcolour_reset\
$env_prompt\
$_pdark_green_fg\u@\h:$_pcolour_reset\
$_pdark_blue_fg$cur_dir\n$_pcolour_reset\
$_pdark_green_fg║ $_pcolour_reset\
$cur_git_branch\n\
$_pdark_green_fg╚ $privelege_char$_pcolour_reset "

  local triple_line_threshold=95
  local double_line_threshold=20
  if test ${#prompt_length_calc} -gt "${triple_line_threshold}"; then
    PS1="$triple_line_fmt"
  elif test ${#prompt_length_calc} -gt "${double_line_threshold}"; then
    PS1="$double_line_fmt"
  else
    PS1="$single_line_fmt"
  fi
}
PROMPT_COMMAND="__ps1"

_dark_blue_fg=$'\e[01;34m'
_dark_green_fg=$'\e[01;32m'
_dark_yellow_bg_white_fg=$'\e[01;43;37m'
_colour_reset=$'\e[0m'

# NOTE(Ryan): These are base colours reflecting Vim solarized light.
# Different man pages don't seem to have the same formatting.
# This makes creating a consistent layout tedious. 
# Therefore, have left in this 'good enough' state.
export LESS_TERMCAP_us=${_dark_blue_fg}
export LESS_TERMCAP_md=${_dark_green_fg}
export LESS_TERMCAP_so=${_dark_yellow_bg_white_fg}
export LESS_TERMCAP_me=${_colour_reset}
export LESS_TERMCAP_se=${_colour_reset}
export LESS_TERMCAP_ue=${_colour_reset}

if test -x '/usr/bin/lesspipe'; then
  export LESSOPEN="| /usr/bin/lesspipe %s";
  export LESSCLOSE="/usr/bin/lesspipe %s %s";
fi

export EDITOR=vim
export VISUAL=vim
export PYTHONDONTWRITEBYTECODE=1
export DEBEMAIL='re.mcclue@protonmail.com'
export DEBFULLNAME='Ryan McClue'

# NOTE(Ryan): Almost all terminals are xterm, so no real need to detect type.
export TERM=xterm-256color

# NOTE(Ryan): Starting a command with 2 spaces will result in it not being added to history
shopt -s histappend
export HISTSIZE=5000
export HISTFILESIZE=10000

export PAGER='less -s -M +Gg'
export MANPAGER="$PAGER"
export CDPATH=".:$HOME/prog:$HOME/prog/personal:$HOME/prog/apps:$HOME/prog/sources:$HOME/prog/toolchains"
export HISTTIMEFORMAT="%d/%m/%y %T "

# NOTE(Ryan): Disable control sequences ctrl-s/ctrl-q that buffer/unbuffer terminal.
# They are carry overs from teletypes, which would buffer input due to high latency.
stty -ixon

set -o vi noclobber

# NOTE(Ryan): \command will run unaliased command, e.g. \ls
alias c='clear'
alias ls='ls -a --color=auto'
alias grep='grep --color=auto'
alias lintian='lintian -iIEcv --pedantic --color auto'
alias tree='tree -a -I .git'
alias gitc='git difftool --cached && git commit'
alias gitcm='git difftool --cached && git commit --no-verify'

if command -v dircolors >/dev/null 2>&1; then 
  if test -r "$HOME/.dir_colors/dircolors"; then
    eval "$(dircolors ~/.dir_colors/dircolors)"
  else
    eval "$(dircolors)"
  fi
fi 

# TODO(Ryan): Current dircolors file displays CDPATH autocompletion with MISSING/ORPHAN style.
# This style is quite obstrusive as it blinks. 
# Investigate why this is to hopefully avoid having to manually edit the file.
test -f /usr/share/bash-completion/bash_completion && \
  source /usr/share/bash-completion/bash_completion

__path_append() {
  for d in "$@"; do
    test -d "${d}" || continue
    # IMPORTANT(Ryan): Although bash provides convenient pattern substitution it is not POSIX.
    # Whilst unimportant for .bashrc, I want to use POSIX if it's not tedious to do so.
    # Bash is bloated and there are places where bash is not available, e.g. containers, Dockerfiles, etc.
    case "$PATH" in
      *":$d"* | *":$d:"* | *"$d:"*) ;;
      *) export PATH="${PATH:+"${PATH}:"}${d}" ;;
    esac
  done
}

__path_append \
  /bin \
  /sbin \
  /usr/bin \
  /usr/sbin \
  /snap/bin \
  "$HOME/.local/bin" \
  /usr/local/bin \
  /usr/local/sbin \
  ~/prog/apps/*/bin \
  ~/prog/toolchains/*/bin \
  ~/prog/personal/scripts
