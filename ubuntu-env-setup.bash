#! /usr/bin/env bash
# SPDX-License-Identifier: zlib-acknowledgement

red='\033[31m'
green='\033[32m'
reset='\033[0m'

if [[ $EUID -eq 0 ]]; then
  printf "${red}CANNOT BE ROOT TO IMPORT GPG/SSH KEYS FOR USER!\n${reset}"
  exit 1
fi

if [[ ! -f private-key.asc ]]; then
  printf "${red}REQUIRE GPG PRIVATE-KEY.ASC TO BE IN $(pwd)\n${reset}"
  exit 2
fi

if [[ ! -f .ssh/id_rsa || ! -f .ssh/id_rsa.pub ]]; then
  printf "${red}REQUIRE .SSH FOLDER TO BE POPULATED IN $(pwd)\n${reset}"
  exit 3
fi

printf "${green}CONFIGURING GNOME TERMINAL AND BASH\n${reset}"
git clone https://github.com/aruhier/gnome-terminal-colors-solarized.git
pushd gnome-terminal-colors-solarized
./install.sh --install-dircolors
popd
rm -rf gnome-terminal-colors-solarized

mkdir /root/.dir_colors
cp ~/.dir_colors/dircolors /root/.dir_colors

mkdir ~/.bash /root/.bash
wget https://raw.githubusercontent.com/git/git/master/contrib/completion/git-prompt.sh -O ~/.bash/git-prompt.sh
cp ~/.bash/git-prompt.sh /root/.bash

echo ~ /root | xargs -n 1 cp .bashrc
echo ~ /root | xargs -n 1 cp .inputrc

cp .gdbinit ~/.gdbinit

cp {sourcetrail,ghidra,vtune}.desktop ~/.local/share/applications  

add-apt-repository ppa:ryan-mcclue/ppa-test -y
sudo apt install ubuntu-unity-desktop

mkdir -p ~/prog/{personal,apps,sources}

printf "${green}CONFIGURING GPG AND SSH\n${reset}"
gpg --import private-key.asc
mkdir ~/.ssh
cp -r .ssh/. ~/.ssh 
chmod 600 ~/.ssh/id_rsa 
ssh-add


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
git config --global push.default simple
git config --global pull.rebase true
git config --global alias.adog "log --all --decorate --oneline --graph" 
git config --global merge.tool vimdiff
git config --global merge.conflictstyle diff3
git config --global mergetool.prompt false
