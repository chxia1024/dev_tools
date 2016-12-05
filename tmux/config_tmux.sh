#!/bin/bash
SCRIPT=$(readlink -f "$0")
ROOTPATH=$(dirname $(dirname "$SCRIPT"))

source $ROOTPATH/common.sh

check_env CHXIA
check_env OH_MY_ZSH
check_env ZSH_INSTALL

config_tmuxrc() {
   plugins_dir=$1
   tmuxrc=$CHXIA/tmuxrc
   log_info "Configure tmuxrc to $tmuxrc"
   cat $ROOTPATH/tmux/tmuxrc.tpl > $tmuxrc
   echo "
set -g default-command \"ZDOTDIR=$OH_MY_ZSH $ZSH_INSTALL/bin/zsh -d\"
run-shell $plugins_dir/tmux-continuum/continuum.tmux
run-shell $plugins_dir/tmux-resurrect/resurrect.tmux
" >>  $tmuxrc
}

tmux_plugins=(
git@github.com:chxia1024/tmux-resurrect.git
git@github.com:chxia1024/tmux-continuum.git
)

plugins_dir=$CHXIA/sw/tmux
clone_git_repos "$plugins_dir" "${tmux_plugins[@]}"
config_tmuxrc $plugins_dir
