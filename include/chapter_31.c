#include "chapter_31.h"

/**
 * operating boolean arrays
 * newarray, setarray, getarray, getsize, ...
 */

// array.new(size)
static int newarray(lua_State *L) {
    int i;
    size_t nbytes;
    struct BitArray *a;

    // num of bits, check n
    int n = (int)luaL_checkinteger(L, 1);  
    luaL_argcheck(L, n >= 1, 1, "invalid size");

    // as 1 unsigned int space is allocated in BitArray, add iword(n - 1)
    nbytes = sizeof(BitArray) + I_WORD(n - 1) * sizeof(unsigned int);
    a = (BitArray *)lua_newuserdata(L, nbytes);
    a->size = n;
    for (i = 0; i <= I_WORD(n - 1); i++) {
        a->values[i] = 0;  // init array
    }
    luaL_getmetatable(L, arrayname);
    lua_setmetatable(L, -2);
    return 1;
}

// array.set(arr, value, condition)
static int setarray(lua_State *L) {
    BitArray *a = checkarray(L);
    int index = (int) luaL_checkinteger(L, 2) - 1;

    // check arr, value, condition
    luaL_argcheck(L, a != NULL, 1, "'array' expected");
    luaL_argcheck(L, 0 <= index && index < a->size, 2, "index out of range");
    luaL_checkany(L, 3);

    // set true or false based on condition
    if (lua_toboolean(L, 3)) {
        a->values[I_WORD(index)] |= I_BIT(index);  // set true
    } else {
        a->values[I_WORD(index)] &= ~I_BIT(index);  // set false
    }
    return 0;
}

// array.get(arr, index)
static int getarray(lua_State *L) {
    BitArray *a = checkarray(L);
    int index = (int)luaL_checkinteger(L, 2);

    // check arr, index
    luaL_argcheck(L, a != NULL, 1, "'array' expected");
    luaL_argcheck(L, 0 <= index && index < a->size, 2, "index out of range");
    
    // get the boolean on specific index
    lua_pushboolean(L, a->values[I_WORD(index)] & I_BIT(index));
    return 1;
}

// array.size(arr)
static int getsize(lua_State *L) {
    BitArray *a = checkarray(L);
    luaL_argcheck(L, a != NULL, 1, "'array' expected");
    lua_pushinteger(L, a->size);
    return 1;
}

static unsigned int *getparams(lua_State *L, unsigned int *mask) {
    BitArray *a = checkarray(L);
    int index = (int)luaL_checkinteger(L, 2) - 1;
    luaL_argcheck(L, 0 <= index && index < a->size, 2, "index out of range");
    *mask = I_BIT(index);  // bitmask on specific index
    return &a->values[I_WORD(index)];  // index of word
}

static int setarray_new(lua_State *L) {
    unsigned int mask;
    unsigned int *entry = getparams(L, &mask);
    luaL_checkany(L, 3);
    if (lua_toboolean(L, 3)) {
        *entry |= mask;
    } else {
        *entry &= mask;
    }
    return 0;
}

static int getarray_new(lua_State *L) {
    unsigned int mask;
    unsigned int *entry = getparams(L, &mask);
    lua_pushboolean(L, *entry & mask);
    return 1;
}

int array2string(lua_State *L) {
    BitArray *a = checkarray(L);
    lua_pushfstring(L, "array(%d)", a->size);
    return 1;
}

static const struct luaL_Reg arraylib_f[] = {
    {"new", newarray},
    {NULL, NULL}
};

static const struct luaL_Reg arraylib_m[] = {
    {"__tostring", array2string},
    {"set", setarray},
    {"get", getarray},
    {"size", getsize},
    {NULL, NULL}
};

int luaopen_array(lua_State *L) {
    // create a new metatable for array class
    luaL_newmetatable(L, arrayname);  // create metatable
    lua_pushvalue(L, -1);  // copy metatable
    lua_setfield(L, -2, "__index");  // mt.__index = mt
    luaL_setfuncs(L, arraylib_m, 0);  // set functions
    luaL_newlib(L, arraylib_f);  // create lib
    return 1;
}

