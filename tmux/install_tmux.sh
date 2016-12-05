#!/bin/bash
SCRIPT=$(readlink -f "$0")
ROOTPATH=$(dirname $(dirname "$SCRIPT"))

source $ROOTPATH/common.sh

check_env CHXIA
check_env TMUX_INSTALL

tmux_repos=(
git@github.com:tmux/tmux.git
git@github.com:nmathewson/Libevent.git
)

dir=$(dirname $TMUX_INSTALL)
clone_git_repos $dir "${tmux_repos[@]}"

# install libevent
cd $dir/Libevent
libevent_install="$dir/libevent_install"
sh autogen.sh
check_ret $? "libevent autogen."
./configure --prefix=$libevent_install --enable-debug && make -j20 && make install
check_ret $? "make libevent."
cd -
# install tmux
cd $dir/tmux
sh autogen.sh
check_ret $? "tmux autogen."
./configure CFLAGS="-I$libevent_install/include" LDFLAGS="-L$libevent_install/lib -Wl,-rpath,$libevent_install/lib" --prefix=$TMUX_INSTALL --enable-debug
check_ret $? "configure tmux with libevent"
make -j20 && make install
check_ret $? "make tmux."
cd -
