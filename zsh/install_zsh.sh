#!/bin/bash
SCRIPT=$(readlink -f "$0")
ROOTPATH=$(dirname $(dirname "$SCRIPT"))

source $ROOTPATH/common.sh

check_env CHXIA
check_env ZSH_INSTALL

zsh_git="git@github.com:zsh-users/zsh.git"
dir="$CHXIA/sw/zsh/"
clone_git_repos $dir $zsh_git
cd $dir/zsh
# checkout branch 5.3 to fix the bug of command line messed up by PASTE
git checkout zsh-5.3 -b zsh-5.3
./Util/preconfig
check_ret $? "preconfig zsh"
./configure --prefix=$ZSH_INSTALL --enable-zsh-debug
check_ret $? "configure zsh"
make -j20 && make install
check_ret $? "make zsh"
cd -
