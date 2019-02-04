#ifndef CHAPTER_28_H
#define CHAPTER_28_H

#include "base.h"

#define MAX_COLOR 255

// color table
struct ColorTable {
    char *name;
    unsigned char red, green, blue;
};

int getGlobInt(lua_State *L, const char* var);
int getColorField(lua_State *L, const char *key);
void setColorField(lua_State *L, const char *index, int value);
void setColor(lua_State *L, struct ColorTable *ct);
double f(lua_State *L, double x, double y);
void call_va(lua_State *L, const char *func, const char *sig, ...);

#endif