export CHXIA=/home/admin/chxia
# binary and plugins' install path
export VIM_INSTALL=$CHXIA/sw/vim/vim_install
export VIM_PLUGINS=$CHXIA/sw/vim/vim_plugins
export ZSH_INSTALL=$CHXIA/sw/zsh/zsh_install
export OH_MY_ZSH=$CHXIA/sw/zsh/oh-my-zsh
export TMUX_INSTALL=$CHXIA/sw/tmux/tmux_install
export PYTHON_INSTALL=$CHXIA/sw/python/python_install
export LLVM_INSTALL=$CHXIA/sw/llvm/llvm_install

export PATH=$TMUX_INSTALL/bin:$ZSH_INSTALL/bin:$PATH

# configuration files
export TMUXRC=$CHXIA/.tmuxrc
export VIMRC=$CHXIA/.vimrc

# for alias
alias vi="$VIM_INSTALL/bin/vim -u $VIMRC"
alias tmux="$TMUX_INSTALL/bin/tmux -S $CHXIA/.tmux_socket -f $TMUXRC"
