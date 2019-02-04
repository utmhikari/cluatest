#include "chapter_30.h"

// map() => {}
int l_Map(lua_State *L) {
    int i, n;

    // par1: table, par2: func
    luaL_checktype(L, 1, LUA_TTABLE);
    luaL_checktype(L, 2, LUA_TFUNCTION);

    n = luaL_len(L, 1);  // get size of table

    for (int i = 1; i <= n; i++) {
        lua_pushvalue(L, 2);  // push func
        lua_geti(L, 1, i);  // push t[i]
        lua_call(L, 1, 1);  // call f(t[i])
        lua_seti(L, 1, i);  // t[i] = result
    }

    return 0;
}

// string.split(str sep)
static int l_split(lua_State *L) {
    const char *s = luaL_checkstring(L, 1);
    const char *sep = luaL_checkstring(L, 2);
    const char *e;
    int i = 1;

    lua_newtable(L);  // create result table

    while((e = strchr(s, *sep)) != NULL) {
        lua_pushlstring(L, s, e - s);  // push substr
        lua_rawseti(L, -2, i++);  // set table at -2
        s = e + 1;  // next idx = e + 1
    }

    // push the last substr
    lua_pushstring(L, s);
    lua_rawseti(L, -2, i);
    return 1;
}

// string.upper
static int str_upper(lua_State *L) {
    size_t l, i;
    luaL_Buffer b;
    const char *s = luaL_checklstring(L, 1, &l);
    char *p = luaL_buffinitsize(L, &b, l);
    for (i = 0; i < l; i++) {
        p[i] = toupper(s[i]);  // should be unsigned char
    }
    luaL_pushresultsize(&b, l);
    return 1;
}

// table concat
static int tconcat(lua_State *L) {
    luaL_Buffer b;
    int i, n;
    luaL_checktype(L, 1, LUA_TTABLE);
    n = luaL_len(L, 1);
    luaL_buffinit(L, &b);
    for (i = 1; i <= n; i++) {
        lua_geti(L, 1, i);
        luaL_addvalue(&b);  // put value to buffer
    }
    luaL_pushresult(&b);  // push the buffer & flush
    return 1;
}

// tuple
int t_tuple(lua_State *L) {
    lua_Integer op = luaL_optinteger(L, 1, 0);
    if (op == 0) {  // no param, return 0
        int i;
        for (i = 1; !lua_isnone(L, lua_upvalueindex(i)); i++) {
            lua_pushvalue(L, lua_upvalueindex(i));
        }
        return i - 1;  // return number of valid upvalues
    } else {  // get field op (param 1)
        luaL_argcheck(L, 0 < op && op <= 256, 1, "index out of range");
        if (lua_isnone(L, lua_upvalueindex(op))) {
            return 0;
        } else {
            lua_pushvalue(L, lua_upvalueindex(op));
            return 1;
        }
    }
}

int t_new(lua_State *L) {
    int top = lua_gettop(L);
    luaL_argcheck(L, top < 256, top, "too man fields");
    lua_pushcclosure(L, t_tuple, top);
    return 1;
}

static const struct luaL_Reg tuplelib[] = {
    {"new", t_new},
    {NULL, NULL}
};

int luaopen_tuple(lua_State *L) {
    luaL_newlib(L, tuplelib);
    return 1;
}