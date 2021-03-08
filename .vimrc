" SPDX-License-Identifier: zlib-acknowledgement

set nocompatible

set noswapfile noundofile nobackup

set spell spelllang=en_au

set encoding=utf-8
set termencoding=utf-8

set expandtab
set shiftround 
set autowrite

filetype plugin indent on
set autoindent

set mouse=a

set diffopt+=vertical

nnoremap <silent> <expr> Q &diff ? ":cquit!\<CR>" : Q
nnoremap <silent> <expr> q &diff ? ":qall!\<CR>" : q
nnoremap <silent> <expr> t &diff ? ":windo diffoff \<bar> wincmd h\<CR>" : t
nnoremap <silent> <expr> e &diff ? "\<C-W>\<C-O>" : e
nnoremap <silent> <expr> <C-C> &diff ? /^\(<<<<<<<\\|=======\\|>>>>>>>\)<CR> : <C-N>

command! -nargs=1 -complete=file Diffsplit diffsplit <args> | wincmd L | wincmd h

command! Tabnew tabnew | tcd ~/prog/personal 

set incsearch hlsearch 

nnoremap <silent> <Esc><Esc> :silent! nohls<CR> 
set foldenable
set foldmethod=manual
set foldlevelstart=0

syntax on
set number

if has("gui_running")
  set guifont=Inconsolata\ 14
  " IMPORTANT(Ryan): Always use tcd over cd!
  tcd ~/prog/personal/
endif

set showmatch
set backspace=indent,eol,start
set laststatus=2
set showmode
set scrolloff=5
set nowrap

set guioptions+=!

set background=light
colorscheme solarized

cnoremap w!! w !sudo tee %

function! Make(script)
  if &ft ==# "python"
    if a:script ==# "build-tests.bash"
      " NOTE(Ryan): compiler pyunit if wanting to change from pytest
      compiler gcc
    else
      compiler gcc
    endif
  elseif &ft ==# "c" || &ft ==# "cpp"
    compiler gcc
  else
    return 0
  endif

  let &makeprg="bash " . a:script

  make! 
  copen
  redraw
endfunction

nnoremap <silent> <C-T> :call Make("build-tests.bash")<CR><CR>
nnoremap <silent> <C-B> :call Make("build.bash")<CR><CR>
nnoremap <silent> <C-L> :call Make("misc/lint.bash")<CR><CR>
nnoremap <silent> <C-N> :cnext<CR>
nnoremap <silent> <C-P> :cprev<CR>
nnoremap <silent> <C-C> :cclose<CR>

function! TabSelectOrPopupOrIndent()
  if col('.') == 1 || getline('.')[col('.') - 2] =~? '[ ]'
    return "\<Tab>"
  else
    return "\<C-X>\<C-N>"
  endif
endfunction
inoremap <expr> <Tab> TabSelectOrPopupOrIndent()

inoremap <expr> <CR> BraceIndentOrEnter()
function! BraceIndentOrEnter()
    if pumvisible()
        return "\<C-Y>"
    elseif strcharpart(getline('.'),getpos('.')[2]-1,1) == '}'
        return "\<CR>\<Esc>O"
    elseif strcharpart(getline('.'),getpos('.')[2]-1,2) == '</'
        return "\<CR>\<Esc>O"
    else
        return "\<CR>"
    endif
endfunction
inoremap <expr> } getline('.')[col('.')-2] == "{" ? "}\<C-G>U\<Left>" : "}"

" IMPORTANT(Ryan): Mapping <Esc> causes issues in terminal Vim 
inoremap <expr> q pumvisible() ? "\<C-E>" : 'q'
inoremap <expr> n pumvisible() ? "\<C-N>" : 'n'
inoremap <expr> <S-N> pumvisible() ? "\<C-P>" : "\<S-N>"

nnoremap <S-F> :vimgrep //gj **/*.c **/*.cpp **/*.h <bar> copen<C-Left><C-Left><C-Left><C-Left><C-Left><Right>
" NOTE(Ryan): ctags --list-kinds=c
" ctags --c++-kinds=+lpx --fields=+iaS --extra=+q -R *
set tags+=/usr/include/**/tags

augroup IndentSettings
  autocmd!
  
  autocmd FileType c,cpp,html,javascript,css setlocal shiftwidth=2 tabstop=2 softtabstop=2
augroup end

function! StartBashFile()
  normal i#! /usr/bin/env bash
  normal o# SPDX-License-Identifier: zlib-acknowledgement
endfunction
function! StartPythonFile()
  normal i#! /usr/bin/env python3
  normal o# SPDX-License-Identifier: zlib-acknowledgement
endfunction

" NOTE(Ryan): Saved to &viewdir
augroup AutoSaveFolds
  autocmd!

  autocmd BufWinLeave *.c,*.cpp,*.h,*.py,*.html,*.css,*.js mkview
  autocmd BufWinEnter *.c,*.cpp,*.h,*.py,*.html,*.css,*.js silent loadview | set foldmethod=manual
augroup END

augroup CommentRegions
  autocmd!

  autocmd BufNewFile *.c,*.h,*.cpp,*.js normal i// SPDX-License-Identifier: zlib-acknowledgement
  autocmd BufNewFile .gitignore,.gitattributes,*.yml normal i# SPDX-License-Identifier: zlib-acknowledgement
  autocmd BufNewFile *.bash :call StartBashFile()
  autocmd BufNewFile *.py :call StartPythonFile()
  autocmd BufNewFile *.bat normal i:: SPDX-License-Identifier: zlib-acknowledgement
  autocmd BufNewFile *.md,*.html normal i<!-- SPDX-License-Identifier: zlib-acknowledgement -->

  autocmd Syntax * syntax match License +\("\|//\|\#\|::\|<!--\) SPDX-License-Identifier: zlib-acknowledgement\( -->\)\?+

  autocmd Syntax * syntax keyword TodoMarker TODO containedin=.*Comment,vimCommentTitle,cCommentL,htmlCommentPart
  autocmd Syntax * syntax keyword NoteMarker NOTE containedin=.*Comment,vimCommentTitle,cCommentL,htmlCommentPart
  autocmd Syntax * syntax keyword ImportantMarker IMPORTANT containedin=.*Comment,vimCommentTitle,cCommentL,htmlCommentPart
augroup END

if !exists('g:solarized_gui_base1')
  let g:solarized_gui_base1= '#93a1a1'
endif
if !exists('g:solarized_gui_base2')
  let g:solarized_gui_base2= '#eee8d5'
endif
if !exists('g:solarized_gui_red')
  let g:solarized_gui_red = '#dc322f'
endif
if !exists('g:solarized_gui_green')
  let g:solarized_gui_green = '#859900'
endif
if !exists('g:solarized_gui_yellow')
  let g:solarized_gui_yellow = '#b58900'
endif
execute "highlight License gui=italic guibg=" . g:solarized_gui_base2 . " guifg=" . g:solarized_gui_base1 
execute "highlight TodoMarker gui=italic,underline guifg=" . g:solarized_gui_red 
execute "highlight ImportantMarker gui=italic,underline guifg=" . g:solarized_gui_yellow
execute "highlight NoteMarker gui=italic,underline guifg=" . g:solarized_gui_green

if !exists('g:solarized_term_base1')
  let g:solarized_term_base1= '14'
endif
if !exists('g:solarized_term_base2')
  let g:solarized_term_base2= '7'
endif
if !exists('g:solarized_term_red')
  let g:solarized_term_red = '124'
endif
if !exists('g:solarized_term_green')
  let g:solarized_term_green = '64'
endif
if !exists('g:solarized_term_yellow')
  let g:solarized_term_yellow = '136'
endif
execute "highlight License cterm=italic ctermbg=" . g:solarized_term_base2 . " ctermfg=" . g:solarized_term_base1 
execute "highlight TodoMarker cterm=italic,underline ctermfg=" . g:solarized_term_red 
execute "highlight ImportantMarker cterm=italic,underline ctermfg=" . g:solarized_term_yellow
execute "highlight NoteMarker cterm=italic,underline ctermfg=" . g:solarized_term_green
