" SPDX-License-Identifier: zlib-acknowledgement

set nocompatible

set noswapfile noundofile nobackup

set spell spelllang=en_au

set encoding=utf-8
set termencoding=utf-8

set expandtab
set shiftround 
set autowrite

" NOTE(Ryan): Allow indentation to be set to 2 spaces instead of the default 4 
let g:python_recommended_style = 0

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

set incsearch hlsearch 

nnoremap <silent> <Esc><Esc> :silent! nohls<CR> 
set foldenable
set foldmethod=manual
set foldlevelstart=0

syntax on
set number

if has("gui_running")
  if has("gui_gtk3")
    set guifont=Inconsolata\ 14
  else
    set guifont=Consolas:h11:cANSI
  endif
endif

set showmatch
set backspace=indent,eol,start
set laststatus=2
set showmode
set scrolloff=5
set noscrollbind
set nocursorbind
set nowrap

set guioptions+=!

set background=light
colorscheme solarized

cnoremap w!! w !sudo tee %

function! Make(script)
  if &ft !=# "sh" && a:script !=# "misc/lint" && !filereadable(a:script)
    echoerr a:script . " does not exist!"
    return 1
  endif

  if &ft ==# "sh" && a:script ==# "misc/lint"
    compiler gcc 
  endif

  if &ft ==# "python"
    compiler gcc
  endif

  if &ft ==# "cpp"
    compiler gcc
  endif
  
  if &ft ==# "c"
    if a:script ==# "misc/run-tests"
      " NOTE(Ryan): This pattern is for CMocka. 
      "             %.%# is wildcard for everything
      let &errorformat = "%.%#---\ %f:%l%.%#"
    else
      compiler gcc
    endif
  endif

  " IMPORTANT(Ryan): This is a basic first attempt that is not flexible
  if &ft ==# "java"
    if a:script ==# "misc/run-tests"
      let &errorformat = 
        \ "%.%#unsw\.test%.%#(%f:%l),"
        \. "%DEntering dir '%f',%XLeaving dir,"
    else
      compiler javac
    endif
  endif

  if &ft ==# "sh"
    " NOTE(Ryan): Exclude checking for variables directly used in printf format string 
    let &makeprg="shellcheck -e SC2059 -f gcc " . expand('%')
  else
    let &makeprg="./" . a:script
  endif

  make! 
  copen
  redraw
endfunction

nnoremap <silent> <C-T> :call Make("misc/run-tests")<CR><CR>
nnoremap <silent> <C-B> :call Make("misc/build")<CR><CR>
nnoremap <silent> <C-L> :call Make("misc/lint")<CR><CR>
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

nnoremap <S-F> :vimgrep //gj **/* <bar> copen<C-B><Right><Right><Right><Right><Right><Right><Right><Right><Right>
nnoremap <C-F> :call ReplaceAcrossAllFiles(
function! ReplaceAcrossAllFiles(search, replace)
  silent! execute "vimgrep /" . a:search . "/gj **/* | copen"
  silent! execute  "cfdo %s/\\v" . a:search . "/" . a:replace . "/gc"
  silent! cfdo update
endfunction

" NOTE(Ryan): ctags --list-kinds=c
" ctags --c++-kinds=+lpx --fields=+iaS --extra=+q -R *
set tags+=/usr/include/**/tags
set tags+=~/prog/sources/**/tags

augroup IndentSettings
  autocmd!
  
  autocmd FileType c,cpp,html,javascript,css,vim,java,sh,python,yaml,markdown setlocal shiftwidth=2 tabstop=2 softtabstop=2
augroup end

function! StartPythonFile()
  normal i#!/usr/bin/python3
  normal o# SPDX-License-Identifier: zlib-acknowledgement
endfunction

augroup CommentRegions
  autocmd!

  autocmd BufNewFile *.c,*.h,*.cpp,*.js normal i// SPDX-License-Identifier: zlib-acknowledgement
  autocmd BufNewFile .gitignore,.gitattributes,*.yml normal i# SPDX-License-Identifier: zlib-acknowledgement
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
