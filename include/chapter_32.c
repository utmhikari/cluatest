#include "chapter_32.h"

/**
 * DIR with gc
 */
 
// open DIR
static int l_dir(lua_State *L) {
    const char *path = luaL_checkstring(L, 1);
    // create a userdata pointing to DIR struct 
    DIR **d = (DIR **)lua_newuserdata(L, sizeof(DIR *));
    *d = NULL;
    // set metatable
    luaL_getmetatable(L, dir_typename);
    lua_setmetatable(L, -2);
    // try to open path
    *d = opendir(path);
    if (*d == NULL) {
        luaL_error(L, "cannot open %s: %s", path, strerror(errno));
    }
    // create and return the iter closure
    lua_pushcclosure(L, dir_iter, 1);
    return 1;
}

// iterator
static int dir_iter(lua_State *L) {
    DIR *d = *(DIR **)lua_touserdata(L, lua_upvalueindex(1));
    struct dirent *entry = readdir(d);
    if (entry != NULL) {
        // push next() to stack
        lua_pushstring(L, entry->d_name);
        return 1;
    } else { return 0; }
}

// close DIR
static int dir_gc(lua_State *L) {
    DIR *d = *(DIR **)lua_touserdata(L, 1);
    if (d) { closedir(d); }
    return 0;
}

static const struct luaL_Reg dirlib[] = {
    {"open", l_dir},
    {NULL, NULL}
};

int luaopen_dir(lua_State *L) {
    // set metatable with gc
    luaL_newmetatable(L, dir_typename);
    lua_pushcfunction(L, dir_gc);
    lua_setfield(L, -2, "__gc");
    // create lib
    luaL_newlib(L, dirlib);
    return 1;
}


/**
 * lxp
 */

// callback StartElement
static void f_StartElement(void *ud, const char *name, const char **atts) {
    lxp_userdata *xpu = (lxp_userdata *)ud;
    lua_State *L = xpu->L;
    lua_getfield(L, 3, "StartElement");
    if (lua_isnil(L, -1)) { lua_pop(L, 1); return; }
    lua_pushvalue(L, 1);
    lua_pushstring(L, name);
    // fill the attributes
    lua_newtable(L);
    for (; *atts; atts += 2) {
        // table[*atts] = *(atts + 1)
        lua_pushstring(L, *(atts + 1));
        lua_setfield(L, -2, *atts);
    }
    lua_call(L, 3, 0);
}

// callback CharacterData
static void f_CharData(void *ud, const char *s, int len) {
    lxp_userdata *xpu = (lxp_userdata *)ud;
    lua_State *L = xpu->L;
    // get chardata handler function
    lua_getfield(L, 3, "CharacterData");
    if (lua_isnil(L, -1)) { lua_pop(L, 1); return; }
    lua_pushvalue(L, 1);  // push parser
    lua_pushlstring(L, s, len);  // push xml string
    lua_call(L, 2, 0);
}

// callback EndElement
static void f_EndElement(void *ud, const char *name) {
    lxp_userdata *xpu = (lxp_userdata *)ud;
    lua_State *L = xpu->L;
    lua_getfield(L, 3, "EndElement");
    if (lua_isnil(L, -1)) { lua_pop(L, 1); return; }
    lua_pushvalue(L, 1);
    lua_pushstring(L, name);
    lua_call(L, 2, 0);
}

static int lxp_make_parser(lua_State *L) {
    XML_Parser p;
    // create parser object
    lxp_userdata *xpu = (lxp_userdata *)lua_newuserdata(L, sizeof(lxp_userdata));
    xpu->parser = NULL;
    // set metatable
    luaL_getmetatable(L, lxp_typename);
    lua_setmetatable(L, -2);
    // create expat parser
    p = xpu->parser = XML_ParserCreate(NULL);
    if (!p) {
        luaL_error(L, "XML_ParserCreate failed");
    }
    // check and save callbacks
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_pushvalue(L, 1);
    lua_setuservalue(L, -2);
    // set expat parser
    XML_SetUserData(p, xpu);
    XML_SetElementHandler(p, f_StartElement, f_EndElement);
    XML_SetCharacterDataHandler(p, f_CharData);
    return 1;
}

// parse xml
static int lxp_parse(lua_State *L) {
    int status;
    size_t len;
    const char *s;
    lxp_userdata *xpu;
    // fetch and check the first param (parser)
    xpu = (lxp_userdata *)luaL_checkudata(L, 1, lxp_typename);
    // check if the parser is closed
    luaL_argcheck(L, xpu->parser != NULL, 1, "parser is closed");
    // gain the second param (string)
    s = luaL_optlstring(L, 2, NULL, &len);
    // put callback at stack index 3
    lua_settop(L, 2);
    lua_getuservalue(L, 1);
    // set lua state
    xpu->L = L;
    // call expat parser
    status = XML_Parse(xpu->parser, s, (int)len, s == NULL);
    lua_pushboolean(L, status);
    return 1;
}

// close parser
static int lxp_close(lua_State *L) {
    lxp_userdata *xpu = (lxp_userdata *)luaL_checkudata(L, 1, lxp_typename);
    if (xpu->parser) {
        XML_ParserFree(xpu->parser);
    }
    xpu->parser = NULL;
    return 0;
}

static const struct luaL_Reg lxp_meths[] = {
    {"parse", lxp_parse},
    {"close", lxp_close},
    {"__gc", lxp_close},
    {NULL, NULL}
};

static const struct luaL_Reg lxp_funcs[] = {
    {"new", lxp_make_parser},
    {NULL, NULL}
};

int luaopen_lxp(lua_State *L) {
    luaL_setmetatable(L, lxp_typename);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, lxp_meths, 0);
    luaL_newlib(L, lxp_funcs);
    return 1;
}
