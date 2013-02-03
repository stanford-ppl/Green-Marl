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
syn keyword GMKeyword Node_Property Edge_Property N_P E_P  Node_Prop Edge_Prop
syn keyword GMKeyword For Foreach If Else Proc Procedure Do While Return Local
syn keyword GMKeyword InNbrs OutNbrs Nbrs Nodes Edges CommonNbrs Nbr_Edges
syn keyword GMKeyword Items INF +INF -INF
syn keyword GMBuiltin NumNbrs NumOutNbrs NumInNbrs NumNodes NumEdges Degree InDegree OutDegree
syn keyword GMBuiltin Log Rand Pow Exp Uniform
syn keyword GMBuiltin PushBack PopBack PushFront PopFront Has Add Remove Union Intersect Complement IsSubsetOf Clear
syn keyword GMBuiltin FromNode ToNode ToEdge IsNbrFrom
syn keyword GMKeyword InBFS UpNbrs DownNbrs From InDFS InReverse InPost
syn keyword GMKeyword Sum Product Max Min Count All Exist Avg
syn keyword GMType Int Float Double Bool Graph Long Node Edge DGraph UGraph
syn keyword GMType Node_Set N_S Node_Order N_O Node_Sequence Node_Seq N_Q 
syn keyword GMType Map Collection
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
syn match GMTodo "[tT][oO][dD][oO]" contained
syn match GMLineComment "//.*" contains=GMTodo
syn region GMComment start="/\*" end="\*/" contains=GMTodo

syn region GMForeign start="\[" end="\]"

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
hi link GMOP Operator
hi link GMBoolean Boolean
hi link GMNumber Number
hi link GMType Type
hi link GMComment Comment
hi link GMLineComment Comment
hi link GMTodo Todo
hi link GMForeign Macro

let b:current_syntax = "greenmarl"

