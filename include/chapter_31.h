#ifndef CHAPTER_31_H
#define CHAPTER_31_H

#include "base.h"

// bits of unsigned int
#define BITS_PER_WORD (CHAR_BIT * sizeof(unsigned int))

// get the actual word index on memory
#define I_WORD(i) ((unsigned int)(i) / BITS_PER_WORD)

// get the bitmask of a specific word
#define I_BIT(i) (1 << ((unsigned int)(i) / BITS_PER_WORD))

// BitArray
typedef struct BitArray {
    int size;
    unsigned int values[1];
} BitArray;

// array type name
#define arrayname "LuaBook.array"

// check the array type
#define checkarray(L) (BitArray *)luaL_checkudata(L, 1, arrayname)

static int newarray(lua_State *L);
static int setarray(lua_State *L);
static int getarray(lua_State *L);
static int getsize(lua_State *L);
static unsigned int *getparams(lua_State *L, unsigned int *mask);
static int setarray_new(lua_State *L);
int array2string(lua_State *L);
int luaopen_array(lua_State *L);

#endif