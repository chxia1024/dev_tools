config_zshrc() {
   zsh_dir=$1
   zshrc=$zsh_dir/.zshrc
   echo "# Path to your oh-my-zsh installation.
CHXIA=/home/admin/chxia

export ZSH=$zsh_dir
" >  $zshrc
   cat $CHXIA/dev_tools/zsh/zshrc.tpl >> $zshrc
}

if [ -z $CHXIA ]; then
   echo "environment CHXIA is not set"
   exit 1
else
   echo "environment CHXIA is set to $CHXIA"
fi

source $CHXIA/dev_tools/common.sh

oh_my_zsh_git="git@github.com:chxia1024/oh-my-zsh.git"
dir="$CHXIA/sw/zsh"

clone_git_repos $dir $oh_my_zsh_git
config_zshrc $dir/oh-my-zsh
