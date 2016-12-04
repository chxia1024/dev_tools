config_git() {
   dir=$1
   mkdir -p $dir
   cd $dir
   git init
   git config --local user.name chxia1024
   git config --local user.email chxia1024@gmail.com
   cd -
}

clone_plugins() {
   dir=$1
   plugins=$2
   cd $dir
   for plugin in ${plugins[@]} 
   do
      git clone $plugin
   done 
   cd -
}

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
   cat vimrc.tpl >> $file
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

export CHXIA=/home/admin/chxia                                                                                                         
plugins_dir="$CHXIA/sw/vim/vim_plugins/"
config_git $plugins_dir
clone_plugins $plugins_dir $plugins
config_vimrc $plugins_dir $plugins $CHXIA/vimrc
