#!/bin/bash
SCRIPT=$(readlink -f "$0")
ROOTPATH=$(dirname $(dirname "$SCRIPT"))

source $ROOTPATH/common.sh

check_env CHXIA
check_env OH_MY_ZSH

config_zshrc() {
   zshrc=$OH_MY_ZSH/.zshrc
   log_info "Configure zshrc to $zshrc"
   echo "# Path to your oh-my-zsh installation.
CHXIA=$CHXIA

export ZSH=$OH_MY_ZSH
" >  $zshrc
   cat $ROOTPATH/zsh/zshrc.tpl >> $zshrc
}

oh_my_zsh_git="git@github.com:chxia1024/oh-my-zsh.git"
dir=$(dirname $OH_MY_ZSH)
path=$(basename $OH_MY_ZSH)

git_repos=("$oh_my_zsh_git $path")
clone_git_repos "$dir" "${git_repos[@]}"
config_zshrc
