#! /usr/bin/env bash
# SPDX-License-Identifier: zlib-acknowledgement

red='\033[31m'
green='\033[32m'
yellow='\033[33m'
reset='\033[0m'

if [[ $EUID -ne 0 ]]; then
  printf "${red}MUST BE ROOT!\n${reset}"
  exit 1
fi

if [[ ! -f private-key.asc ]]; then
  printf "${red}Require GPG private-key.asc to be in $(cwd)\n${reset}"
  exit 2
fi

if [[ ! -f .ssh/id_rsa || ! -f .ssh/id_rsa.pub ]]; then
  printf "${red}Require SSH .ssh folder to be populated $(cwd)\n${reset}"
  exit 3
fi

printf "${green}INSTALLING APPLICATIONS\n${reset}"
wget https://dl.google.com/linux/direct/google-chrome-stable_current_amd64.deb
sudo apt install ./google-chrome-stable_current_amd64.deb
rm google-chrome-stable_current_amd64.deb

sudo apt install gvim veracrypt gcc git dconf-cli

exec bash

printf "${green}CONFIGURING GNOME TERMINAL AND BASH\n${reset}"
git clone https://github.com/aruhier/gnome-terminal-colors-solarized.git
cd gnome-terminal-colors-solarized
./install.sh --install-dircolors
./set-light.sh

mkdir ~/.bash
wget https://raw.githubusercontent.com/git/git/master/contrib/completion/git-prompt.sh -O ~/.bash/git-prompt.sh

cp .bashrc ~/.bashrc

exec bash

printf "${green}CONFIGURING GPG AND SSH\n${reset}"
gpg --import private-key.asc
cp -r .ssh ~/.ssh 
ssh-add

exec bash

printf "${green}CONFIGURING VIM\n${reset}"
wget https://raw.githubusercontent.com/ryan-mcclue/cas/main/.vimrc -O ~/.gvimrc
wget https://raw.githubusercontent.com/ryan-mcclue/cas/main/.vimrc -O ~/.vimrc
mkdir -p ~/.vim/colors
wget https://raw.githubusercontent.com/altercation/vim-colors-solarized/master/colors/solarized.vim \
  -O ~/.vim/colors/solarized.vim

printf "${green}CONFIGURING GIT\n${reset}"
git config --global user.name "Ryan McClue"
git config --global user.email "re.mcclue@protonmail.com"

gpg_keyid=$(gpg --list-keys | sed -n 4p)
git config --global user.signingkey ${gpg_keyid}

gpg_loc=$(which gpg)
git config --global gpg.program ${gpg_loc}

git config --global commit.gpgsign true
git config --global diff.tool vimdiff
# NOTE(Ryan): This reorders the default vimdiff windows, i.e. the staged file is on the left. $
git config --global difftool.vimdiff.cmd 'vim -f -d -c "wincmd h" -c '\''cd "$GIT_PREFIX"'\'' "$REMOTE" "$LOCAL"'
# NOTE(Ryan): This allows the use of :cquit to exit the entire diff process. $
git config --global difftool.trustExitCode true
git config --global difftool.prompt false


printf "${yellow}-------------------------------\n${reset}"
printf "${yellow}-- MANUALLY INSTALL CODECLAP --\n${reset}"
printf "${yellow}-------------------------------\n${reset}"
