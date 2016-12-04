config_vimrc() {
   plugins_dir=$1
   plugins=$2
   file=$3
   # plugins special configs
   echo "set nocompatible
filetype off 
set runtimepath+=$plugins_dir/Vundle.vim
\" plugins
call vundle#begin('$plugins_dir')
\" let Vundle manage Vundle, required
   " > $file
   for plugin in ${plugins[@]} 
   do
      echo "Plugin '$plugin'" >> $file
   done
   echo "
call vundle#end()   \" required
filetype plugin indent on    \" required
   " >> $file

   # default config in vimrc.tpl
   cat $CHXIA/dev_tools/vim/vimrc.tpl >> $file
}

plugins=(
git@github.com:VundleVim/Vundle.vim.git
git@github.com:scrooloose/nerdtree.git
git@github.com:Valloric/YouCompleteMe.git
git@github.com:tpope/vim-fugitive.git
git@github.com:tpope/vim-sleuth.git
git@github.com:tpope/vim-surround.git
git@github.com:brookhong/cscope.vim.git
git@github.com:ctrlpvim/ctrlp.vim.git
git@github.com:rdnetto/YCM-Generator.git
)

if [ -z $CHXIA ]; then
   echo "environment CHXIA is not set"
   exit 1
else
   echo "environment CHXIA is set to $CHXIA"
fi

source $CHXIA/dev_tools/common.sh

plugins_dir="$CHXIA/sw/vim/vim_plugins/"
clone_git_repos $plugins_dir $plugins
config_vimrc $plugins_dir $plugins $CHXIA/vimrc
