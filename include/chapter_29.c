#include "chapter_29.h"

// load dir
static int l_dir(lua_State *L) {
    DIR *dir;
    struct dirent *entry;
    int i;
    const char *path = luaL_checkstring(L, -1);

    // open dir
    dir = opendir(path);
    if (dir == NULL) {
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        return 2;  // return 2 results
    }

    // create result table
    lua_newtable(L);
    i = 1;
    while ((entry = readdir(dir)) != NULL) {
        lua_pushinteger(L, i++);
        lua_pushstring(L, entry->d_name);
        lua_settable(L, -3);
    }

    closedir(dir);
    return 1;  // return 1 result
}

// pcall on continuation
static int finishpcall(lua_State *L, int status, intptr_t ctx) {
    (void)ctx;  // unused params
    status = (status != LUA_OK && status != LUA_YIELD);
    lua_pushboolean(L, (status == 0));  // push the status
    lua_insert(L, 1);
    return lua_gettop(L);  // status is the first and rets are nexts
}

// new lua pcall
static int luaB_pcall(lua_State *L) {
    int status;
    luaL_checkany(L, 1);  // at least 1 param
    status = lua_pcallk(L, lua_gettop(L) - 1, LUA_MULTRET, 0, 0, finishpcall);
    return finishpcall(L, status, 0);
}