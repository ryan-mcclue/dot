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

let tlist_objc_settings = 'ObjectiveC;P:protocols;i:interfaces;types(...)'

filetype plugin indent on
set autoindent

set autoread
au CursorHold,CursorHoldI * :checktime

set mouse=a

set diffopt+=vertical

nnoremap <silent> <expr> Q &diff ? ":cquit!\<CR>" : Q
nnoremap <silent> <expr> q &diff ? ":qall!\<CR>" : q
nnoremap <silent> <expr> t &diff ? ":windo diffoff \<bar> wincmd h\<CR>" : t
nnoremap <silent> <expr> e &diff ? "\<C-W>\<C-O>" : e
nnoremap <silent> <expr> <C-C> &diff ? /^\(<<<<<<<\\|=======\\|>>>>>>>\)<CR> : <C-N>

" NOTE(Ryan): :norm! @<reg> to replay macro over selection
nnoremap m q

" NOTE(Ryan): Unicode --> multiply middle dot 
inoremap <Right>m <C-V>u00b7
" NOTE(Ryan): Unicode --> theta
inoremap <Right>t <C-V>u03b8
" NOTE(Ryan): Unicode --> degree
inoremap <Right>d <C-V>u00b0
" NOTE(Ryan): Unicode --> ohm
inoremap <Right>o <C-V>u2126
" NOTE(Ryan): Unicode --> bullet point
inoremap <Right>b <C-V>u00b7
" NOTE(Ryan): Unicode --> arrow
inoremap <Right>a <C-V>u279e
" NOTE(Ryan): Unicode --> left-right (linking) arrow
inoremap <Right>l <C-V>u27f7
" NOTE(Ryan): Unicode --> micro S.I
inoremap <Right>u <C-V>u00b5
" NOTE(Ryan): Unicode --> approximate
inoremap <Right>= <C-V>u2248
" NOTE(Ryan): Unicode --> therefore
inoremap <Right>f <C-V>u2234
" NOTE(Ryan): Unicode --> equivalent
inoremap <Right>e <C-V>u2261
" NOTE(Ryan): Unicode --> infinity
inoremap <Right>i <C-V>u221e
" NOTE(Ryan): Unicode --> pi
inoremap <Right>p <C-V>u03c0

" NOTE(Ryan): Character subscripts, ctrl-k + <sequence>
execute "digraphs as " . 0x2090
execute "digraphs es " . 0x2091
execute "digraphs hs " . 0x2095
execute "digraphs is " . 0x1D62
execute "digraphs js " . 0x2C7C
execute "digraphs ks " . 0x2096
execute "digraphs ls " . 0x2097
execute "digraphs ms " . 0x2098
execute "digraphs ns " . 0x2099
execute "digraphs os " . 0x2092
execute "digraphs ps " . 0x209A
execute "digraphs rs " . 0x1D63
execute "digraphs ss " . 0x209B
execute "digraphs ts " . 0x209C
execute "digraphs us " . 0x1D64
execute "digraphs vs " . 0x1D65
execute "digraphs xs " . 0x2093
" NOTE(Ryan): Character superscripts
execute "digraphs aS " . 0x1d43
execute "digraphs bS " . 0x1d47
execute "digraphs cS " . 0x1d9c
execute "digraphs dS " . 0x1d48
execute "digraphs eS " . 0x1d49
execute "digraphs fS " . 0x1da0
execute "digraphs gS " . 0x1d4d
execute "digraphs hS " . 0x02b0
execute "digraphs iS " . 0x2071
execute "digraphs jS " . 0x02b2
execute "digraphs kS " . 0x1d4f
execute "digraphs lS " . 0x02e1
execute "digraphs mS " . 0x1d50
execute "digraphs nS " . 0x207f
execute "digraphs oS " . 0x1d52
execute "digraphs pS " . 0x1d56
execute "digraphs rS " . 0x02b3
execute "digraphs sS " . 0x02e2
execute "digraphs tS " . 0x1d57
execute "digraphs uS " . 0x1d58
execute "digraphs vS " . 0x1d5b
execute "digraphs wS " . 0x02b7
execute "digraphs xS " . 0x02e3
execute "digraphs yS " . 0x02b8
execute "digraphs zS " . 0x1dbb

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

function! Make(script, type)
  if &ft !=# "sh" && a:script !=# "misc/lint" && !filereadable(a:script)
    echoerr a:script . " does not exist!"
    return 1
  endif

  if a:script ==# "misc/lint"
    if &ft ==# "asm"
      " NOTE(Ryan): This is for the avra assembler
      let &errorformat = "%f(%l)%m"
    elseif &ft ==# "php"
      " NOTE(Ryan): This is for php -l which only outputs line number 
      let &errorformat = "PHP Parse Error: %m in %f on line %l"
    elseif &ft ==# "sql"
      " NOTE(Ryan): This is for sqlfluff parse 
" L:   8 | P:   1 |  PRS | Line 8, Position 1: Found unparsable section: 'graham\n-- check
"                        | (length(value) >= 3)\n\n--...'
      " TODO
      " let &errorformat = "%f(%l)%m"
      " let &errorformat = "%f: L:%.%#%l | P:%.%#%c | %.%# | Line%.%#: %m"

      " NOTE(Ryan): For psql
      " psql:q1.sql:5: ERROR:  column l.regions does not exist
      " LINE 3: select l.regions
      "                ^
      " HINT:  Perhaps you meant to reference the column "l.region".
      let &errorformat = "psql:%f:%.%# ERROR: %m\nLINE %l: %.%#"
    else
      compiler gcc
    endif
  endif

  if &ft ==# "python"
    compiler gcc
  endif

  if &ft ==# "python"
    compiler gcc
  endif

  if &ft ==# "cs"
    compiler cs
  endif
  
  if &ft ==# "c"
    if a:type ==# "tests"
      " NOTE(Ryan): This pattern is for CMocka. 
      "             %.%# is wildcard for everything
      " let &errorformat = "%.%#---\ %f:%l%.%#"
      let &errorformat = "%.%#---\ %f:%l: error: %m"
    else
      compiler gcc
    endif
  endif

  " IMPORTANT(Ryan): This is a basic first attempt that is not flexible
  if &ft ==# "java"
    if a:type ==# "tests"
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
    let &makeprg="./" . a:script . " " . a:type
  endif

  make! 
  copen
  redraw
endfunction

" IMPORTANT(Ryan): Intended for stdin loop binary named same as cwd
function! RunInTerminal()
  let cwd = getcwd()
  let folder = fnamemodify(cwd, ":p:h:t")
  silent execute "terminal build/" . folder
endfunction
nnoremap <silent> <S-I> :call RunInTerminal()<CR>
tnoremap <silent> <C-C> <C-\><C-N>:q!<CR>

nnoremap <silent> <C-B> :call Make("misc/build", "app")<CR><CR>
nnoremap <silent> <C-T> :call Make("misc/build", "tests")<CR><CR>
nnoremap <silent> <C-S> :call Make("misc/build", expand("%"))<CR><CR>
nnoremap <silent> <C-L> :call Make("misc/lint", expand("%"))<CR><CR>
nnoremap <silent> <S-U> :!misc/flash<CR>
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


function! SearchAcrossAllFiles(search)
  let fts = [expand("%:e")]
  if &ft ==# "c" || &ft ==# "cpp" || &ft ==# "asm" 
    let fts = ["[ch]", "cpp", "m", "mm", "asm", "[sS]"] 
  endif

  let search_fts="% "
  for ft in fts
    let search_fts .= "**/*." . ft . " "
  endfor

  silent! execute "vimgrep /\\<" . a:search . "\\>/gj " . search_fts . "| copen"
endfunction

function! ReplaceAcrossAllFiles(search, replace, bulk)
  let confirm_char=(a:bulk ==# "") ? "c" : ""
  call SearchAcrossAllFiles(a:search)
  silent! execute  "cfdo %s/\\v" . a:search . "/" . a:replace . "/g" . confirm_char
  silent! cfdo update
endfunction

" IMPORTANT(Ryan): C-F in command window to edit (hit enter on current line to execute)
nnoremap <S-F> :call SearchAcrossAllFiles("<C-R>=expand("<cword>")<CR>")<Left><Left>
nnoremap <C-F> :call ReplaceAcrossAllFiles("<C-R>=expand("<cword>")<CR>

" NOTE(Ryan): Command Execution
nnoremap <Leader>i :r!python3 -c 'from math import *; print()'<Left><Left>
nnoremap <Leader>p :echo system("python3 -c 'from math import *; print()'")<Left><Left><Left><Left>
xnoremap <Leader>s yPgv:!python3<CR>

" IMPORTANT(Ryan): Can only use bash variables if substituting due to dollar
" sign
nnoremap <Leader>ii :r!
nnoremap <Leader>pp :echo system("")<Left><Left>
xnoremap <Leader>ss yPgv:!bash<CR>

function! GitBlameDiff()
  let hash=system("git --no-pager blame -w -M3 -L " . line('.') . "," . line('.') . " -- " . expand("%:t") . " | awk '{print $1}'")
  " NOTE(Ryan): Remove newline added by awk
  let hash = substitute(hash, '\n', '', 'g')
  execute "!git difftool " . hash . "^.." . hash . " -- " . expand("%:t")
endfunction
nnoremap <Leader>bd :call GitBlameDiff()<CR>

function! GitBlamePrint()
  let cmd="git --no-pager blame -w -M3 -L" . line('.') . "," . line('.') . " -- " . expand("%:t") . "| awk '{print $2, $3, $4, $5}'"
  echo system(cmd)
endfunction
nnoremap <Leader>bp :call GitBlamePrint()<CR>

" NOTE(Ryan): ctags --list-kinds=c
" find . -type f -iname "*.[chS]" -o -iname "*.cpp" | sudo xargs ctags --c-kinds=+lpx --c++-kinds=+lpx --fields=+iaS -a
" ctags --c++-kinds=+lpx --fields=+iaS --extras=+q -R *
" find . -type f -iname "*.[chS]" | xargs ctags --c-kinds=+lpxL -a
"set tags+=/usr/include/**/tags
"set tags+=~/prog/sources/**/tags
"set tags+=~/prog/hals/**/tags
set path=.

" NOTE(Ryan): This prevents having to use two commands; one for single tag,
"             another for multiple tags.
nnoremap <C-]> :ts <C-R>=expand("<cword>")<CR> <CR>

cs add cscope.out 
" find . -type f -name "*.cpp" -o -name "*.h" > cscope.files && cscope -b
" nnoremap <C-[>v :cs find a <C-R>=expand("<cword>")<CR><CR> <Space> 
" nnoremap <C-[>c :cs find c <C-R>=expand("<cword>")<CR><CR> <Space> 
" nnoremap <C-]> :cs find g <C-R>=expand("<cword>")<CR><CR> <Space> 
nnoremap <C-[> :cs find c <C-R>=expand("<cword>")<CR><CR> <Space> 


augroup IndentSettings
  autocmd!
  
  autocmd FileType c,cs,cpp,asm,cmake,go,php,ld,vhdl,html,javascript,css,vim,java,sh,prolog,python,yaml,markdown,sql setlocal shiftwidth=2 tabstop=2 softtabstop=2
  autocmd FileType qf setlocal wrap
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
