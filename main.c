#include <stdio.h>
#include "./include/chapter_27.h"
#include "./include/chapter_28.h"
#include "./include/chapter_29.h"
#include "./include/chapter_30.h"

// example for stackDump
void stackDumpExample() {
    lua_State *L = luaL_newstate();
    lua_pushboolean(L, 1);
    lua_pushnumber(L, 10);
    lua_pushnil(L);
    lua_pushstring(L, "hello");

    stackDump(L);
    lua_pushvalue(L, -4); stackDump(L);
    lua_replace(L, 3); stackDump(L);
    lua_settop(L, 6); stackDump(L);
    lua_rotate(L, 3, 1); stackDump(L);
    lua_remove(L, -3); stackDump(L);
    lua_settop(L, -5); stackDump(L);
    lua_close(L);
}

// load config integer
void load(lua_State *L, const char *fname, int *w, int *h) {
    if (luaL_loadfile(L, fname) || lua_pcall(L, 0, 0, 0)) {
        luaL_error(L, "Cannot run config. file: %s", lua_tostring(L, -1));
        *w = getGlobInt(L, "width");
        *h = getGlobInt(L, "height");
    }
}

int main (void) {
    stackDumpExample();
    return 0;
}