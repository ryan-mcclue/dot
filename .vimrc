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

command! -nargs=1 -complete=file Diffsplit diffsplit <args> | wincmd L | wincmd h

set incsearch hlsearch 

nnoremap <silent> <Esc><Esc> :silent! nohls<CR> 
set foldenable
set foldmethod=manual
set foldlevelstart=0

syntax on
set number

if has("gui_gtk3")
  set guifont=Inconsolata\ 14
elseif has("gui_macvim")
  set guifont=Menlo\ Regular:h14
else
  set guifont=Consolas:h11:cANSI
endif

set showmatch
set backspace=indent,eol,start
set laststatus=2
set showmode
set scrolloff=5
set nowrap

set guioptions+=!

compiler gcc

set background=light
colorscheme solarized

if !exists('g:os')
  if has('win64') || has('win32')
    let g:os = 'Windows'
    autocmd GUIEnter * simalt ~x
  else
    let g:os = substitute(system('uname'), '\n', '', '')
    if g:os ==# 'Linux'
      autocmd GUIEnter * call system('wmctrl -i -b add,maximized_vert,maximized_horz -r ' . v:windowid)
    endif
  endif
endif

if g:os ==# 'Windows'
  let &makeprg="call windows-build.bat"
elseif g:os ==# 'Linux'
  let &makeprg="bash ubuntu-build.bash"
else
  let &makeprg="bash macos-build.bash"
endif

nnoremap <silent> <C-B> :make! <bar> copen <bar> redraw<CR><CR>
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
