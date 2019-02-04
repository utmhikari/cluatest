#ifndef CHAPTER_33_H
#define CHAPTER_33_H

#include "base.h"

// lproc
#define lproc_name "lproc"
typedef struct Proc {
    lua_State *L;  // lua state
    pthread_t thread;  // c thread
    pthread_cond_t cond;  // cond var, block while waiting
    const char *channel;  // the channel on waiting
    struct Proc *previous, *next;  // circular double-linked list
} Proc;

static Proc *waitsend = NULL;  // send waiting list
static Proc *waitreceive = NULL;  // receive waiting list
static pthread_mutex_t kernel_access = PTHREAD_MUTEX_INITIALIZER;

static Proc *getself(lua_State *L);
static void movevalues(lua_State *send, lua_State *rec);
static Proc *searchmatch(const char *channel, Proc **list);
static void waitonlist(lua_State *L, const char *channel, Proc **list);
static int ll_send(lua_State *L);
static int ll_receive(lua_State *L);

static int ll_start(lua_State *L);
static void *ll_thread(void *arg);
static int ll_exit(lua_State *L);
int luaopen_lproc();

static void registerlib(lua_State *L, const char *name, lua_CFunction f);
static void openlibs(lua_State *L);

#endif