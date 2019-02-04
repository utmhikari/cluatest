#ifndef CHAPTER_29_H
#define CHAPTER_29_H

#include "base.h"

static int l_dir(lua_State *L);
static int finishpcall(lua_State *L, int status, intptr_t ctx);
static int luaB_pcall(lua_State *L);

#endif