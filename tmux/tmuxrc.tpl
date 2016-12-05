# Set the prefix to C-a
unbind C-b
set -g prefix C-a
bind a send-prefix
# for zsh
set -g remain-on-exit off
# load my resouce
bind-key R source-file /home/admin/chxia/sw/tmux/tmuxrc.conf
# Use a for renaming the window prompt.
unbind-key ,
unbind-key A
bind-key A command-prompt "rename-window '%%'"
# last window
bind-key C-a last-window
#alterscreen
set -g alternate-screen on
#term
set -g default-terminal "xterm"
unbind [
bind-key Escape copy-mode
unbind p
bind p paste-buffer
bind-key -t vi-copy 'v' begin-selection
bind-key -t vi-copy 'y' copy-selection

# screen ^C c
unbind ^C
bind ^C new-window -c "#{pane_current_path}"
unbind c
bind c new-window -c "#{pane_current_path}"

# quit \
unbind \
bind \ confirm-before "kill-server"

# kill K k
unbind K
bind K confirm-before "kill-window"
unbind k
bind k confirm-before "kill-window"

# select pane
unbind Tab
bind Tab select-pane -t:.+
unbind BTab
bind BTab select-pane -t:.-

# split windows like vim
# vim's definition of a horizontal/vertical split is reversed from tmux's
## Use h and v for splitting.
unbind %
unbind '"'
bind v split-window -h -c "#{pane_current_path}"
bind | split-window -h -c "#{pane_current_path}"
bind - split-window -v -c "#{pane_current_path}"
bind H split-window -v -c "#{pane_current_path}"

# move around panes with hjkl, as one would in vim after pressing ctrl-w
bind h select-pane -L
bind j select-pane -D
bind k select-pane -U
bind l select-pane -R

# resize panes like vim
# feel free to change the "1" to however many lines you want to resize by, only
# one at a time can be slow
bind < resize-pane -L 1
bind > resize-pane -R 1
bind _ resize-pane -D 2
bind + resize-pane -U 1

# for move/swap window
bind-key m command-prompt -p "move window to:"    "move-window -t '%%'"
bind-key . command-prompt -p "swap window with:"  "swap-window -t '%%'"

# bind : to command-prompt like vim
# this is the default in tmux already
bind : command-prompt

# vi-style controls for copy mode
setw -g mode-keys vi

set -g status-justify left
set -g status-interval 15 # 15 sec refresh
set -g display-time 3000
set -g status-bg black
set -g status-fg white
set-window-option -g window-status-current-fg blue
set-window-option -g window-status-current-bg yellow
set-window-option -g window-status-current-attr default # bright
set -g status-left-length 15
set -g status-right-length 55
set -g status-left "#[fg=white,bg=blue] > #S < #[default] |" # session-name
set -g status-right "#[fg=yellow,bright]%Y.%m.%d #[fg=green]%H:%M #[default]"

# for process to resurrect , refer to docs/restoring_programs.md
set -g @resurrect-processes 'ssh psql mysql "git log"'
# default 15min
set -g @continuum-save-interval '30'
set -g default-command "ZDOTDIR=/home/admin/chxia/sw/oh-my-zsh/ /home/admin/chxia/sw/zsh_install/bin/zsh -d"
run-shell /home/admin/chxia/sw/tmux/tmux-continuum/continuum.tmux
run-shell /home/admin/chxia/sw/tmux/tmux-resurrect/resurrect.tmux
