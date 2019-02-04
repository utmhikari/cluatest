#include "chapter_28.h"

// color table
struct ColorTable colortable[] = {
    {"WHITE", MAX_COLOR, MAX_COLOR, MAX_COLOR},
    {"RED", MAX_COLOR, 0, 0},
    {"GREEN", 0, MAX_COLOR, 0},
    {"BLUE", 0, 0, MAX_COLOR},
    {NULL, 0, 0, 0}
};

// get info from config file
int getGlobInt(lua_State *L, const char *var) {
    int isnum, result;
    lua_getglobal(L, var);
    result = (int)lua_tointegerx(L, -1, &isnum);
    if (!isnum) {
        luaL_error(L, "'%s' should be a number!\n", var);
    }
    lua_pop(L, 1);
    return result;
}

// get the color (assuming that table is at -1)
int getColorField(lua_State *L, const char *key) {
    int result;
    if (lua_getfield(L, -1, key) != LUA_TNUMBER) {
        luaL_error(L, "invalid component '%s' in color", key);
    }
    result = (int)(lua_tonumber(L, -1) * MAX_COLOR);
    lua_pop(L, 1);
    return result;
}

// set a color field (assuming that table is at -1)
void setColorField(lua_State *L, const char *index, int value) {
    lua_pushnumber(L, (double)value / MAX_COLOR);
    lua_setfield(L, -2, index);
}

// set color to table
void setColor(lua_State *L, struct ColorTable *ct) {
    lua_newtable(L);
    setColorField(L, "red", ct->red);
    setColorField(L, "green", ct->green);
    setColorField(L, "blue", ct->blue);
    lua_setglobal(L, ct->name);
}

// call function f defined in lua
double f(lua_State *L, double x, double y) {
    int isnum;
    double z;

    // push f, x, y
    lua_getglobal(L, "f");
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);

    // call f(x, y)
    if (lua_pcall(L, 2, 1, 0) != LUA_OK) {
        luaL_error(L, "error running function 'f': %s", lua_tostring(L, -1));
    }

    // get value
    z = lua_tonumberx(L, -1, &isnum);
    if (!isnum) {
        luaL_error(L, "function 'f' should return a number!");
    }
    lua_pop(L, 1);
    return z;
}   

// universal function wrapper
void call_va(lua_State *L, const char *func, const char *sig, ...) {
    va_list vl;
    int narg, nres;  // nargs & nreturns

    va_start(vl, sig);  // load vl to signature
    lua_getglobal(L, func);  // push function to stack

    // push all the params
    for (narg = 0; *sig; narg++) {
        luaL_checkstack(L, 1, "too many arguments");
        switch(*sig++) {
            case 'd': {
                lua_pushnumber(L, va_arg(vl, double));
                break;
            }
            case 'i': {
                lua_pushinteger(L, va_arg(vl, int));
                break;
            }
            case 's': {
                lua_pushstring(L, va_arg(vl, char*));
                break;
            }
            case '>': {
                goto endargs;  // end of params
            }
            default: {
                luaL_error(L, "invalid option (%c)", *(sig - 1));
            }
        }
    }
    endargs:

    nres = strlen(sig);
    if (lua_pcall(L, narg, nres, 0) != 0) {
        luaL_error(L, "error calling '%s': '%s", func, lua_tostring(L, -1));
    }

    // retrieve results, now sig should be at the bottom of returns
    nres = -nres;  // switch to bottom
    while (*sig) {
        switch(*sig++) {
            case 'd': {
                int isnum;
                double n = lua_tonumberx(L, nres, &isnum);
                if (!isnum) {
                    luaL_error(L, "wrong result type!");
                }
                *va_arg(vl, double *) = n;
                break;
            }
            case 'i': {
                int isnum;
                int n = lua_tointegerx(L, nres, &isnum);
                if (!isnum) {
                    luaL_error(L, "wrong result type!");
                }
                *va_arg(vl, int *) = n;
                break;
            }
            case 's': {
                const char *s = lua_tostring(L, nres);
                if (s == NULL) {
                    luaL_error(L, "wrong result type");
                }
                *va_arg(vl, const char **) = s;
                break;
            }
            default: {
                luaL_error(L, "invalid option (%c)", *(sig - 1));
            }
        }
        nres++;
    }

    va_end(vl);
}