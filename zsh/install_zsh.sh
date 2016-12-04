if [ -z $CHXIA ]; then
   echo "environment CHXIA is not set"
   exit 1
else
   echo "environment CHXIA is set to $CHXIA"
fi

source $CHXIA/dev_tools/common.sh

zsh_git="git@github.com:zsh-users/zsh.git"
dir="$CHXIA/sw/zsh/"
clone_git_repos $dir $zsh_git
cd $dir/zsh
./Util/preconfig
./configure --prefix=$dir/zsh_install
make -j20
make install
cd -
