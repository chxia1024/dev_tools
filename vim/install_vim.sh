#!/bin/bash
SCRIPT=$(readlink -f "$0")
ROOTPATH=$(dirname $(dirname "$SCRIPT"))

source $ROOTPATH/common.sh

check_env CHXIA
check_env VIM_INSTALL


vim_repos=(
git@github.com:vim/vim.git
)

dir=$(dirname $VIM_INSTALL)
clone_git_repos $dir "${vim_repos[@]}"

cd $dir/vim
./configure --with-features=huge --enable-multibyte --enable-rubyinterp --enable-perlinterp --enable-luainterp --enable-gui=gtk2 --enable-cscope --enable-debug --prefix=$VIM_INSTALL
# TODO python3 install and reconfigure vim with
# --enable-python3interp --with-python3-config-dir=/home/admin/chxia/sw/python/python3_install/lib/python3.5/config-3.5m
check_ret $? "configure vim."
make -j20 && make install
check_ret $? "make vim."
cd -
