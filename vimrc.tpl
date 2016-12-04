let g:ycm_confirm_extra_conf=0

syn on
set guifont=Luxi\ Mono\ 9
set tabstop=4
set expandtab
set shiftwidth=4
filetype plugin indent on
set backspace=2
set t_kb=
color default
set incsearch
set hlsearch
set nonu
set showmatch
set cursorline
set ruler
cmap w!! %!sudo tee > /dev/null %
cs add $PWD/cscope.out $PWD/
