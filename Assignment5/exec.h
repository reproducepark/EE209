#ifndef EXEC_INCLUDED
#define EXEC_INCLUDED

#include "dynarray.h"
#include "util.h"

void exec(DynArray_T oTokens);
void exec_builtin(DynArray_T oTokens, int num_tokens, enum BuiltinType btype);

#endif