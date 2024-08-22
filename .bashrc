#!/bin/bash
# SPDX-License-Identifier: zlib-acknowledgement

# IMPORTANT: kernel graphics issues
# installed-kernels: ls /boot/vmlinuz-*
# current-kernel: uname -r
# list-menu: grep 'menuentry' /boot/grub/grub.cfg 
# /etc/default/grub
# GRUB_CMDLINE_LINUX_DEFAULT="quiet splash nomodeset"
# GRUB_CMDLINE_LINUX_DEFAULT="quiet splash amd_iommu=off"
# GRUB_DEFAULT="1>2"
# update-grub
# sudo apt install linux-image-5.15.0-58-generic linux-headers-5.15.0-58-generic 


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

  local detached_head=$(git symbolic-ref HEAD 2>/dev/null)
  if test -n "$detached_head"; then
    local cur_git_branch=$(git branch --show-current 2>/dev/null)
    local_rev=$(git rev-parse "$cur_git_branch" 2>/dev/null)
    remote_rev=$(git rev-parse "origin/$cur_git_branch" 2>/dev/null)
    base_rev=$(git merge-base "$cur_git_branch" "origin/$cur_git_branch" 2>/dev/null)

    local git_dirty_char;
    if test -n "$(git status --porcelain 2>/dev/null)"; then
      # NOTE(Ryan): Local changes 
      git_dirty_char="*"
    elif test "$local_rev" = "$remote_rev"; then
      # NOTE(Ryan): Up to date. Necessary short-circuit
      git_dirty_char=
    elif test "$local_rev" = "$base_rev"; then
      # NOTE(Ryan): Need to pull
      git_dirty_char="«"
    elif test "$remote_rev" = "$base_rev"; then
      # NOTE(Ryan): Need to push
      git_dirty_char="»"
    else
      # NOTE(Ryan): Have diverged
      git_dirty_char="¿"
    fi

    if test -n "$git_dirty_char"; then
      cur_git_branch="$_pdark_red_fg($cur_git_branch $git_dirty_char)$_pcolour_reset"
    else
      cur_git_branch="$_pdark_yellow_fg($cur_git_branch)$_pcolour_reset"
    fi
  else
    cur_hash="$(git rev-parse --short HEAD 2>/dev/null)"
    cur_tag="$(git describe --tags 2>/dev/null)"
    cur_hash="${cur_hash:+(¦$cur_hash${cur_tag:+ - $cur_tag}¦)}"
    cur_git_branch="${_pdark_yellow_fg}${cur_hash}${_pcolour_reset}"
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

gcd() {
  cd "$@" && git remote update 2>/dev/null
} && export -f

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
export PYTHONSTARTUP="$HOME/.python3-startup.py"
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
export CDPATH=".:$HOME/prog:$HOME/prog/personal:$HOME/prog/apps:$HOME/prog/sources:$HOME/prog/cross/arm"
export HISTTIMEFORMAT="%d/%m/%y %T "


# NOTE(Ryan): Disable control sequences ctrl-s/ctrl-q that buffer/unbuffer terminal.
# They are carry overs from teletypes, which would buffer input due to high latency.
stty -ixon

set -o vi noclobber

# NOTE(Ryan): \command will run unaliased command, e.g. \ls
alias c='clear'
alias ls='ls -a --color=auto'
alias grep='grep --color=auto'
alias lintian='lintian -i --color aUto'
alias tree='tree -a -I .git'
# TODO(Ryan): Safe rm function
alias rm='rm -i'
alias s='cd scratch'
# TODO(Ryan): Add shell completion
alias sizeof='du -sh'
alias py3='python3'

alias git_clear='git rm -r --cached .'
alias gitc='git difftool --cached && git commit'
alias gitcm='git difftool --cached && git commit --no-verify'

alias gcc_defines='gcc -E -dM - </dev/null' 

export IDF_PATH="$HOME/prog/sources/esp/esp-idf"
idf_init() {
  . "$IDF_PATH/export.sh"

  if [[ ! -f "sdkconfig" ]]; then 
    idf.py create-project -p . $(basename "$(pwd)")
    idf.py set-target esp32 
    idf.py menuconfig
    # serial flasher config; set flash size to what is on board; 
    # set clock speed
    # partition table csv
    # idf.py add-dependency mdns  
  fi
} && export -f

# TODO(Ryan): blkid root block; parse from df -h
# word splitting with IFS from apress pro bash programming book

git_compare()
{
  if [ $# -eq 0 ]; then
    git difftool HEAD^ HEAD
  elif [ $# -eq 1 ]; then
    git difftool "$1" HEAD
  elif [ $# -eq 2 ]; then
    git difftool "$1" "$2"
  elif [ $# -eq 3 ]; then
    git difftool "$1" "$2" -- "$3"
  else
    printf "Usage: git_compare [hash1] [hash2] [file] \n" >&2
  fi
} && export -f

git_content()
{
  local content="$1"
  local commit_info=""

  if [ $# -eq 1 ]; then
    commit_info=$(git log -S"$1" | tr '~' ' ')
  elif [ $# -eq 2 ]; then
    commit_info=$(git log -S"$1" -- "$2" | tr '~' ' ')
  else
    printf "Usage: git_content <content> [file_name]\n" >&2
    return
  fi

  set -- $commit_info
  local hash="$2"
  local files_present_info=$(git grep "$content" "$hash")

  printf "%s\n\n%s\n" "$commit_info" "$files_present_info"

} && export -f

git_file()
{
  if [ $# -eq 2 ]; then
    git log --diff-filter="${1^^}" -- "$2" | tr '~' ' '
  else
    printf "Usage: git_file <a|d|m> <file_name>\n" >&2
  fi
} && export -f

git_msg()
{
  if [ $# -eq 1 ]; then
    git show --color -s --format='%C(yellow)(%as)%C(reset) %s' -"$1" | tr '~' ' '
  else
    printf "Usage: git_msg <commit_count>\n" >&2
  fi
} && export -f


git_squash()
{
  # ^ - parent (so rebase -i <hash>^)
  # ~ - ancestor

  # copy and paste 'pick' to reorder
  # change to 'reword' to alter commit message
  # change to 'squash' to combine (git push --force)
  if [ $# -eq 1 ]; then
    git rebase -i HEAD~"$1"
  else
    printf "Usage: git_squash <commit_count>\n" >&2
  fi
} && export -f

apt_files()
{
  # TODO: indicate if not installed and show
  # verify package exists, then go out with apt-file (take long time)
  if [ $# -eq 1 ]; then
    dpkg -L "$1"
  else
    printf "Usage: apt_files <package>\n" >&2
  fi
} && export -f

apt_search()
{
  if [ $# -eq 1 ]; then
    apt-cache search "$1"
  else
    printf "Usage: apt_search <package>\n" >&2
  fi
} && export -f

apt_from()
{
  if [ $# -eq 1 ]; then
    dpkg -S "$1"
  else
    printf "Usage: apt_from <package>\n" >&2
  fi
} && export -f

apt_info()
{
  if [ $# -eq 1 ]; then
    apt-cache show "$1"
  else
    printf "Usage: apt_info <package>\n" >&2
  fi
} && export -f

c_init()
{
  if [[ $# -ne 2 ]]; then
    printf "Usage: c_init <name> {desktop|embedded}\n" >&2 
    return
  fi

  local name="$1"
  local type="$2"

  case "$type" in
    "embedded")
      local files=("embedded.c" "system.c" "startup.S" "linker.ld" "meta.cpp")
      ;;
    "desktop")
      local files=("desktop.cpp" "desktop.h" "desktop-assets.h" "desktop-assets.cpp" "desktop-reload.cpp" "desktop-tests.cpp" "desktop-tests-assembly.asm" "linker.ld" "meta.cpp")
      ;;
    *)
      printf "Usage: c_init <name> {desktop|embedded}\n" >&2 
      return
      ;;
  esac

  mkdir -p "$name/code/base"

  local path="$HOME/prog/personal/dot/c-init"
  for f in "${files[@]}"; do
    cp "$path/$f" "$name"/code
  done

  sudo mount --bind "$path/code/base" "$name/code/base"

  cp -r "$path/code/external" "$name/code"
  if [[ "$type" == "desktop" ]]; then
    printf "Downloading raylib\n"
    local raylib_tar="raylib-5.0.tar.gz"
    curl -L -o "$raylib_tar" "https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz"
    tar -xf "$raylib_tar" -C "$name/code/external" 
    rm -f "$raylib_tar"

    printf "Building raylib-debug\n"
    mkdir -p "$name/build"
    cmake -DCMAKE_BUILD_TYPE=RelWithDebugInfo -DBUILD_SHARED_LIBS=ON -B "$name/build" -S "$name/code/external/raylib-5.0"
    cmake --build "$name/build"
    sudo cmake --install "$name/build" 
  fi

  cp -r "$path/assets" "$name"
  cp -r "$path/misc" "$name"
  cp -r "$path/private" "$name"
  cp -r "$path/.vscode" "$name"
  cp -r "$path/.github" "$name"

  cp "$path/.gitignore.copy" "$name"/.gitignore
  cp "$path/.gitattributes.copy" "$name"/.gitattributes
  cp "$path/LICENSE" "$name"
  cp "$path/README.md" "$name"
} && export -f

mb()
{
  sudo mount --bind "$HOME/prog/personal/dot/c-init/code/base" "code/base"
} && export -f

run_ctags()
{
  if [ $# -ge 1 ]; then
    if [ -f tags ]; then
      read -sn1 -p "Are you sure want to overwrite existing tags file? (y/n): " confirm
      echo
      if [ ! $confirm = "y" ]; then
        printf "Exited without overwriting existing tags file.\n"
        exit 0
      fi
    fi
    rm -f tags
    find $* -type f -iname "*.[chS]" -o -iname "*.cpp" -o -iname "*.mm" \
      | sudo xargs ctags --c-kinds=+lpx --c++-kinds=+lpx --fields=+iaS -a
    printf "Created tags file.\n"
  else
    printf "Usage: run_ctags <folder1> [folder2] ...\n" >&2
  fi
} && export -f

run_cscope()
{
  if [ $# -ge 1 ]; then
    if [ -f cscope.out ]; then
      read -sn1 -p "Are you sure want to overwrite existing database file? (y/n): " confirm
      echo
      if [ ! $confirm = "y" ]; then
        printf "Exited without overwriting existing database file.\n"
        exit 0
      fi
    fi
    rm -f cscope.out
    find $* -type f -iname "*.[chS]" -o -iname "*.cpp" > cscope.files && cscope -bq
    printf "Created database file.\n"
  else
    printf "Usage: run_cscope <folder1> [folder2] ...\n" >&2
  fi
} && export -f

alias c_analyse="run_ctags . && run_cscope"

make_link() {
  if [ $# -eq 2 ]; then
    ln -vfns "$2" "$1"
  else
    printf "Usage: make_link <src> <dest>\n" >&2
  fi
} && export -f

find_func() {
  if [ $# -eq 2 ]; then
    rg --no-line-number --only-matching "$2[\x00-\x7F]+\(" "$1" | sort | uniq
  else
    printf "Usage: find_func <dir> <func-name>\n" >&2
  fi
} && export -f


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

path() {
  local prev_ifs="$IFS"
  IFS=:
  set -- $PATH 
  printf "%s\n" "$@"
  IFS="$prev_ifs"
} && export -f

where() {
  if [ $# -eq 1 ]; then
    realpath $(which $1)
  else
    printf "Usage: where <binary>\n" >&2
  fi
} && export -f

text_rand() {
  if [ $# -eq 1 ]; then
    local num_lines="$1" 
    # IMPORTANT(Ryan): After 76 bytes, base64 requires newline
    local num_bytes=$(( num_lines * 76 ))
    base64 /dev/urandom | head -c $((num_bytes)) > text-rand.txt
  else
    printf "Usage: text_rand <num-bytes>\n" >&2
  fi
} && export -f

ssh_tunnel() {
  if [ $# -eq 4 ]; then
    ssh -NL "$3":"$1":"$2" "$4"@"$1"
  else
    printf "Usage: ssh_tunnel <server-ip> <server-port> <local-port> <user>\n" >&2
  fi
} && export -f

libc_version() {
  [[ ! $# -eq 1 ]] && printf "Usage: libc_version <binary>\n" >&2 && return
  local binary_name="$1"

  local glibc_version=$(ldd --version | grep -Po 'GLIBC \d{1,2}[.]\d{1,2}')
  printf "System libc is: %s\n" "$glibc_version"

  local max_binary_glibc=$(objdump -T "$binary_name" | grep -Eo 'GLIBC_\S+' | sort -uVr | head -n 1)
  printf "%s max. libc is: %s\n" "$binary_name" "$max_binary_glibc"

  read -sn1 -p "Print related symbols? (y/n): " confirm
  echo
  if [[ "$confirm" == "y" ]]; then
    objdump -T "$binary_name" | grep "$max_binary_glibc"
    printf "Earlier symbols: objdump -T /lib/x86_64-linux-gnu/libc.so.6 | grep <symbol>\n"
    printf "Change: __asm__(\".symver <symbol>,<symbol>@GLIBC_2.2.5\");\n"
    printf "Running in container might be necessary\n"
  fi
} && export -f

# imagemagick
# convert -background None img.svg img.png

bat() {
  local rows="(state)|(percentage)|(capacity)|(time to empty)"
  upower -i /org/freedesktop/UPower/devices/battery_BAT0 | grep --color=never -E "$rows"
} && export -f

export SSH_UNSW="z5346008@login.cse.unsw.edu.au"
export SSH_UNSW_DB="z5346008@d2.cse.unsw.edu.au"

copyto_unsw() {
  if [ $# -eq 2 ]; then
    scp -r "$1" "$SSH_UNSW":"$2"
  else
    printf "Usage: copyto_unsw <local_file> <destination>\n" >&2
  fi
} && export -f
copyto_unsw_db() {
  if [ $# -eq 2 ]; then
    scp -r "$1" "$SSH_UNSW_DB":"$2"
  else
    printf "Usage: copyto_unsw_db <local_file> <destination>\n" >&2
  fi
} && export -f

copyfrom_unsw() {
  if [ $# -eq 2 ]; then
    scp -r "$SSH_UNSW":"$1" "$2"
  else
    printf "Usage: copyfrom_unsw <remote_file> <destination>\n" >&2
  fi
} && export -f
copyfrom_unsw_db() {
  if [ $# -eq 2 ]; then
    scp -r "$SSH_UNSW_DB":"$1" "$2"
  else
    printf "Usage: copyfrom_unsw_db <remote_file> <destination>\n" >&2
  fi
} && export -f

# IMPORTANT: c++ function must have c linkage
diss() {
  [[ ! $# -eq 2 ]] && printf "Usage: ${FUNCNAME[0]} <elf> <function>\n" >&2 && return
  local elf="$1"
  local function="$2"

  objdump -M intel --insn-width=10 --disassemble="$function" "$elf"
} && export -f

download_playlist() {
  #(requires ffmpeg)
  #--recode-video "mp4"
  [[ ! $# -eq 2 ]] && printf "Usage: ${FUNCNAME[0]} <music|video> <url>\n" >&2 && return
  local type="$1"
  local url="$2"
  if [[ "$type" == "music" ]]; then
    "$HOME"/Downloads/yt-dlp --ignore-errors --restrict-filenames --extract-audio --audio-format "mp3" \
      --output '%(title)s.%(ext)s' "$url"
  else
    printf "Type must be music currently\n" >&2 && return
  fi
}

# IMPORTANT(Ryan): This path will likely change with postgresql ubuntu repository updates
PGBIN=/usr/lib/postgresql/12/bin
PGDATA="/home/ryan/pgsql-data"
PGHOST="$PGDATA"
export PGDATA PGHOST
alias p0="pg_ctl stop"
alias p1="pg_ctl -l $PGDATA/log start"

# TODO: Remove after comp2511
export DUNGEONS="$HOME/Documents/java-projects/assignment-ii/app/src/test/resources/dungeons"
export CONFIGS="$HOME/Documents/java-projects/assignment-ii/app/src/test/resources/configs"
export ASRC="$HOME/Documents/java-projects/ass2/app/src/main/java/dungeonmania"
export ADST="$HOME/Documents/java-projects/assignment-ii/app/src/main/java/dungeonmania"

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
  ~/prog/cross/arm/*/bin \
  ~/prog/personal/scripts \
  "$PGBIN" \
  "/opt/gradle/gradle-8.5/bin" \

__prefix='arm-none-eabi-'
alias GCC="${__prefix}gcc"
alias G++="${__prefix}g++"
alias OBJDUMP="${__prefix}objdump"
alias NM="${__prefix}nm"
alias READELF="${__prefix}readelf"
alias GDB="${__prefix}gdb"

eval $(keychain --agents ssh --eval id_rsa -q)
