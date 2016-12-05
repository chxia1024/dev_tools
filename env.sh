export CHXIA=/home/admin/chxia
# zsh install path also used for config tmuxrc
export ZSH_INSTALL=$CHXIA/sw/zsh/zsh_install
export OH_MY_ZSH=$CHXIA/sw/zsh/oh-my-zsh
export TMUX_INSTALL=$CHXIA/sw/tmux/tmux_install

# configuration files
export TMUXRC=$CHXIA/.tmuxrc
export VIMRC=$CHXIA/.vimrc

# for vi
alias vi="vim -u $VIMRC"
alias tmux="$TMUX_INSTALL/bin/tmux -S $CHXIA/.tmux_socket -f $TMUXRC"
