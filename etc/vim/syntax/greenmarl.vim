" Vim syntax file
" Language   : GreenMarl

if version < 600
  syntax clear
elseif exists("b:current_syntax")
  finish
endif

syn case match
syn sync minlines=50

" most Scala keywords
syn keyword GMKeyword Node_Property Edge_Property N_P E_P 
syn keyword GMKeyword For Foreach If Else Proc Procedure Do While Return
syn keyword GMKeyword InNbrs OutNbrs Nbrs Nodes Edges Nbr_Edges
syn keyword GMKeyword Items
syn keyword GMBuiltin NumNbrs NumOutNbrs NumInNbrs NumNodes NumEdges Degree InDegree OutDegree
syn keyword GMKeyword InBFS InRBFS UpNbrs DownNbrs From To InReverse
syn keyword GMKeyword Sum Product Max Min Count ArgMax ArgMin
syn keyword GMType Int Float Double Bool Graph Node Edge  DGraph UGraph
syn keyword GMType Node_Set N_S Node_Order N_O Node_Sequence N_Q
syn keyword GMOP And
syn keyword GMOP Or
syn match GMOP "+="
syn match GMOP "&&"
syn match GMOP "||"
syn match GMOP "min="
syn match GMOP "+="
syn match GMOP "*="
syn match GMOP "max="
syn match GMOP "<="
syn match GMOP "@"
syn match GMOP "|"
syn match GMOP "++"
" boolean literals
syn keyword GMBoolean True False

" definitions
"syn keyword scalaDef def nextgroup=scalaDefName skipwhite
"syn keyword scalaVal val nextgroup=scalaValName skipwhite
"syn keyword scalaVar var nextgroup=scalaVarName skipwhite
"syn keyword scalaClass class nextgroup=scalaClassName skipwhite
"syn keyword scalaObject object nextgroup=scalaClassName skipwhite
"syn keyword scalaTrait trait nextgroup=scalaClassName skipwhite
"syn match scalaDefName "[^ =:;([]\+" contained nextgroup=scalaDefSpecializer skipwhite
"syn match scalaValName "[^ =:;([]\+" contained
"syn match scalaVarName "[^ =:;([]\+" contained 
"syn match scalaClassName "[^ =:;(\[]\+" contained nextgroup=scalaClassSpecializer skipwhite
"syn region scalaDefSpecializer start="\[" end="\]" contained contains=scalaDefSpecializer
"syn region scalaClassSpecializer start="\[" end="\]" contained contains=scalaClassSpecializer

" type constructor (actually anything with an uppercase letter)
"syn match scalaConstructor "\<[A-Z][_$a-zA-Z0-9]*\>" nextgroup=scalaConstructorSpecializer
"syn region scalaConstructorSpecializer start="\[" end="\]" contained contains=scalaConstructorSpecializer

" method call
"syn match scalaRoot "\<[a-zA-Z][_$a-zA-Z0-9]*\."me=e-1
"syn match scalaMethodCall "\.[a-z][_$a-zA-Z0-9]*"ms=s+1

" type declarations in val/var/def
"syn match scalaType ":\s*\(=>\s*\)\?[._$a-zA-Z0-9]\+\(\[[^]]*\]\+\)\?\(\s*\(<:\|>:\|#\|=>\)\s*[._$a-zA-Z0-9]\+\(\[[^]]*\]\+\)*\)*"ms=s+1

" comments
syn match scalaTodo "[tT][oO][dD][oO]" contained
syn match scalaLineComment "//.*" contains=scalaTodo
syn region scalaComment start="/\*" end="\*/" contains=scalaTodo
syn case ignore
syn include @scalaHtml syntax/html.vim
unlet b:current_syntax
syn case match
syn region scalaDocComment start="/\*\*" end="\*/" contains=scalaDocTags,scalaTodo,@scalaHtml keepend
syn region scalaDocTags start="{@\(link\|linkplain\|inherit[Dd]oc\|doc[rR]oot\|value\)" end="}" contained
syn match scalaDocTags "@[a-z]\+" contained

"syn match scalaEmptyString "\"\""

" multi-line string literals
"syn region scalaMultiLineString start="\"\"\"" end="\"\"\"" contains=scalaUnicode
"syn match scalaUnicode "\\u[0-9a-fA-F]\{4}" contained

" string literals with escapes
"syn region scalaString start="\"[^"]" skip="\\\"" end="\"" contains=scalaStringEscape " TODO end \n or not?
"syn match scalaStringEscape "\\u[0-9a-fA-F]\{4}" contained
"syn match scalaStringEscape "\\[nrfvb\\\"]" contained

" symbol and character literals
syn match GMSymbol "'[a-zA-Z0-9][_a-zA-Z0-9]*\>"

" number literals
syn match GMNumber "\(\<\d\+\.\d*\|\.\d\+\)\([eE][-+]\=\d\+\)\=[fFdD]\="
syn match GMNumber "\<[-+]\=\d\+[fFdD]\=\>"

syn sync fromstart

" map Scala groups to standard groups
hi link GMKeyword Keyword
hi link GMBuiltin Function
hi link GMOP Special
hi link GMBoolean Boolean
hi link GMNumber Number
hi link GMType Type
hi link scalaPackage Include
hi link scalaImport Include
hi link scalaOperator Normal
hi link scalaEmptyString String
hi link scalaStringEscape Special
hi link scalaSymbol Special
hi link scalaUnicode Special
hi link scalaComment Comment
hi link scalaLineComment Comment
hi link scalaDocComment Comment
hi link scalaDocTags Special
hi link scalaTodo Todo
hi link scalaType Type
hi link scalaTypeSpecializer scalaType
hi link scalaDef Keyword
hi link scalaVar Keyword
hi link scalaVal Keyword
hi link scalaClass Keyword
hi link scalaObject Keyword
hi link scalaTrait Keyword
hi link scalaDefName Function
hi link scalaDefSpecializer Function
hi link scalaClassName Special
hi link scalaClassSpecializer Special

let b:current_syntax = "scala"

" you might like to put these lines in your .vimrc
"
" customize colors a little bit (should be a different file)
" hi scalaNew gui=underline
" hi scalaMethodCall gui=italic
" hi scalaValName gui=underline
" hi scalaVarName gui=underline
