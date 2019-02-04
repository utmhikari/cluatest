#ifndef CHAPTER_32_H
#define CHAPTER_32_H

#include "base.h"

#define dir_typename "LuaBook.dir"

// DIR with gc
static int l_dir(lua_State *L);
static int dir_iter(lua_State *L);
static int dir_gc(lua_State *L);
int luaopen_dir(lua_State *L);

// luac xml parser: lxp
#define lxp_typename "Expat"
typedef struct lxp_userdata {
    XML_Parser parser;
    lua_State *L;
} lxp_userdata;

static void f_StartElement(void *ud, const char *name, const char **atts);
static void f_CharData(void *ud, const char *s, int len);
static void f_EndElement(void *ud, const char *name);
static int lxp_make_parser(lua_State *L);
static int lxp_close(lua_State *L);
int luaopen_lxp(lua_State *L);

#endif