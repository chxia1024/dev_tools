#!/bin/bash
SCRIPT=$(readlink -f "$0")
ROOTPATH=$(dirname $(dirname "$SCRIPT"))

source $ROOTPATH/common.sh

check_env CHXIA
check_env PYTHON_INSTALL


python_repos=(
git@github.com:python-git/python.git
)

dir=$(dirname $PYTHON_INSTALL)
clone_git_repos $dir "${python_repos[@]}"

cd $dir/python
./configure --prefix=$PYTHON_INSTALL --enable-shared --enable-debug LDFLAGS="-Wl,-rpath,$PYTHON_INSTALL/lib"
check_ret $? "configure python."
make -j20 && make install
check_ret $? "make python."
cd -
