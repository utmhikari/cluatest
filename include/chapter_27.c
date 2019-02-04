#include "chapter_27.h"

// helloworld
void helloWorld() {
    char buff[256];
    int error;
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    while(fgets(buff, sizeof(buff), stdin) != NULL) {
        error = luaL_loadstring(L, buff) || lua_pcall(L, 0, 0, 0);
        if (error) {
            fprintf(stderr, "%s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    }

    lua_close(L);
}

// stackdump
void stackDump(lua_State *L) {
    int top = lua_gettop(L);
    for (int i = 1; i <= top; i++) {
        int t = lua_type(L, i);
        printf("%d: ", i);
        switch(t) {
            case LUA_TSTRING: {
                printf("'%s'", lua_tostring(L, i));
                break;
            }
            case LUA_TBOOLEAN: {
                printf(lua_toboolean(L, i) ? "true" : "false");
                break;
            }
            case LUA_TNUMBER: {
                printf("'%g'", lua_tonumber(L, i));
                break;
            }
            default: {
                printf("'%s'", lua_typename(L, t));
                break;
            }
        }
        printf("    ");
    }
    printf("\n");
}


