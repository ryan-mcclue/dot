" SPDX-License-Identifier: zlib-acknowledgement

set nocompatible

set noswapfile noundofile nobackup

" NOTE(Ryan): Utilise spellcheck with z= $
set spell spelllang=en_au

set expandtab
set shiftround 
set autowrite

filetype plugin indent on

set mouse=a

set incsearch hlsearch 

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

" NOTE(Ryan): This will work for clang also. $
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
  let &makeprg="call C:" . findfile("windows-build.bat", ".;")
elseif g:os ==# 'Linux'
  let &makeprg="bash " . findfile("ubuntu-build.bash", ".;")
else
  let &makeprg="bash " . findfile("macos-build.bash", ".;")
endif

nnoremap <silent> <C-B> :make! <bar> copen <bar> redraw<CR>
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
inoremap <expr> <CR> pumvisible() ? "\<C-Y>" : "\<CR>"
inoremap <expr> <Esc> pumvisible() ? "\<C-E>" : "\<Esc>"
inoremap <expr> n pumvisible() ? "\<C-N>" : 'n'
inoremap <expr> <S-N> pumvisible() ? "\<C-P>" : "\<S-N>"

nnoremap <S-F> :vimgrep //gj **/*.c **/*.cpp **/*.h <bar> copen<C-Left><C-Left><C-Left><C-Left><C-Left><Right>

augroup CommentRegions
  autocmd!

  autocmd BufNewFile *.c,*.h,*.cpp normal i// SPDX-License-Identifier: zlib-acknowledgement
  autocmd BufNewFile *.bash,.gitignore,.gitattributes,*.yml normal i# SPDX-License-Identifier: zlib-acknowledgement
  autocmd BufNewFile *.bat normal i:: SPDX-License-Identifier: zlib-acknowledgement
  autocmd BufNewFile *.md normal i<!-- SPDX-License-Identifier: zlib-acknowledgement -->

  autocmd Syntax * syntax match License +\("\|//\|\#\|::\|<!--\) SPDX-License-Identifier: zlib-acknowledgement\( -->\)\?+

  autocmd Syntax * syntax region TodoRegion start=+\("\|//\|\#\|::\)\s*TODO+ end=+\$+
  autocmd Syntax * syntax match TodoRegionWord +\("\|//\|\#\|::\)\s*\zsTODO+ containedin=TodoRegion contained 

  autocmd Syntax * syntax region ImportantRegion start=+\("\|//\|\#\|::\)\s*IMPORTANT+ end=+\$+
  autocmd Syntax * syntax match ImportantRegionWord +\("\|//\|\#\|::\)\s*\zsIMPORTANT+ containedin=ImportantRegion contained 

  autocmd Syntax * syntax region NoteRegion start=+\("\|//\|\#\|::\)\s*NOTE+ end=+\$+
  autocmd Syntax * syntax match NoteRegionWord +\("\|//\|\#\|::\)\s*\zsNOTE+ containedin=NoteRegion contained 
augroup END

if !exists('g:solarized_base1')
  let g:solarized_base1= '#93a1a1'
endif
if !exists('g:solarized_base2')
  let g:solarized_base2= '#eee8d5'
endif
if !exists('g:solarized_red')
  let g:solarized_red = '#dc322f'
endif
if !exists('g:solarized_green')
  let g:solarized_green = '#859900'
endif
if !exists('g:solarized_yellow')
  let g:solarized_yellow = '#b58900'
endif

execute "highlight License gui=italic guibg=" . g:solarized_base2 . " guifg=" . g:solarized_base1

execute "highlight TodoRegion gui=italic guifg=" . g:solarized_red 
execute "highlight TodoRegionWord gui=italic,bold,underline guifg=" . g:solarized_red 

execute "highlight ImportantRegion gui=italic guifg=" . g:solarized_yellow 
execute "highlight ImportantRegionWord gui=italic,bold,underline guifg=" . g:solarized_yellow

execute "highlight NoteRegion gui=italic guifg=" . g:solarized_green 
execute "highlight NoteRegionWord gui=italic,bold,underline guifg=" . g:solarized_green
