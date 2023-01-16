#!/bin/bash
SCRIPT=$(realpath "$0")
ROOTPATH=$(dirname $(dirname "$SCRIPT"))

source $ROOTPATH/common.sh

check_env CHXIA
check_env VIM_INSTALL
check_env PYTHON_INSTALL

vim_repos=(
git@github.com:vim/vim.git
)

dir=$(dirname $VIM_INSTALL)
clone_git_repos $dir "${vim_repos[@]}"

cd $dir/vim
# TODO other python3 version support
PATH=$PYTHON_INSTALL/bin:$PATH ./configure --with-features=huge --enable-multibyte --enable-rubyinterp --enable-perlinterp --enable-luainterp --enable-gui=gtk2 --enable-cscope --enable-python3interp --with-python3-config-dir=$PYTHON_INSTALL/lib/python3.5/config-3.5m --prefix=$VIM_INSTALL LDFLAGS="-Wl,-rpath,$PYTHON_INSTALL/lib"
check_ret $? "configure vim."
make -j20 && make install
check_ret $? "make vim."
cd -
