#ifndef CHAPTER_30_H
#define CHAPTER_30_H

#include "base.h"

int l_Map(lua_State *L);
static int l_split(lua_State *L);
static int str_upper(lua_State *L);
static int tconcat(lua_State *L);
int t_tuple(lua_State *L);
int t_new(lua_State *L);

#endif