#include "chapter_33.h"

// each process owns a Proc struct which saved as userdata at the top of stack
static Proc *getself(lua_State *L) {
    Proc *p;
    lua_getfield(L, LUA_REGISTRYINDEX, "_SELF");
    p = (Proc *)lua_touserdata(L, -1);
    lua_pop(L, 1);
    return p;
}

// move values between lua states
static void movevalues(lua_State *send, lua_State *rec) {
    int n = lua_gettop(send), i;
    luaL_checkstack(rec, n, "too many results");
    // stack[1] is the channel
    for (int i = 2; i <= n; i++) {
        lua_pushstring(rec, lua_tostring(send, i));
    }
}

// search the corresponding proc
static Proc *searchmatch(const char *channel, Proc **list) {
    Proc *node;
    for (node = *list; node != NULL; node = node->next) {
        if (strcmp(channel, node->channel) == 0) {
            // remove the node
            if (*list == node) {
                // check if only 1 node (circular double-linked!)
                *list = (node->next == node) ? NULL : node->next;
            }
            node->previous->next = node->next;
            node->next->previous = node->previous;
            return node;
        }
    }
    return NULL;
}

// add a proc 
static void waitonlist(lua_State *L, const char *channel, Proc **list) {
    Proc *p = getself(L);
    // add at the end of waiting list
    if (*list == NULL) {
        *list = p;
        p->previous = p->next = p;
    }
    else {
        p->previous = (*list)->previous;
        p->next = *list;
        p->previous->next = p->next->previous = p;
    }
    // wait for the cond var
    p->channel = channel;
    do {
        pthread_cond_wait(&p->cond, &kernel_access);
    } while (p->channel);
}

// lproc.send(channel, val1, val2, ...)
static int ll_send(lua_State *L) {
    Proc *p;
    const char *channel = luaL_checkstring(L, 1);
    pthread_mutex_lock(&kernel_access);
    p = searchmatch(channel, &waitreceive);
    if (p) {
        movevalues(L, p->L);  // move vals to p->L
        p->channel = NULL;  // set null for ignoring other matches
        pthread_cond_signal(&p->cond);  // notify
    } else {
        waitonlist(L, channel, &waitsend);
    }
    pthread_mutex_unlock(&kernel_access);
    return 0;
}

// lproc.receive(channel)
static int ll_receive(lua_State *L) {
    Proc *p;
    const char *channel = luaL_checkstring(L, 1);
    lua_settop(L, 1);
    pthread_mutex_lock(&kernel_access);
    p = searchmatch(channel, &waitsend);
    if (p) {
        movevalues(p->L, L);  // move vals from p->L
        p->channel = NULL;
        pthread_cond_signal(&p->cond);
    } else {
        waitonlist(L, channel, &waitreceive);
    }
    pthread_mutex_unlock(&kernel_access);
    return lua_gettop(L) - 1;
}

// create thread
static int ll_start(lua_State *L) {
    pthread_t thread;
    const char *chunk = luaL_checkstring(L, 1);  // lua code chunk
    lua_State *L1 = luaL_newstate();
    if (L1 == NULL) {
        luaL_error(L, "unable to create new state");
    }
    if (luaL_loadstring(L1, chunk) != 0) {
        luaL_error(L, "error in thread body: %s", lua_tostring(L1, -1));
    }
    if (pthread_create(&thread, NULL, ll_thread, L1) != 0) {
        luaL_error(L, "unable to create new thread");
    }
    pthread_detach(thread);  // ignore any result from the thread
    return 0;
}

// thread prototype
static void *ll_thread(void *arg) {
    lua_State *L = (lua_State *)arg;
    Proc *self;
    // require libs
    openlibs(L);
    luaL_requiref(L, lproc_name, luaopen_lproc, 1);  // use luaopen_lproc to require
    lua_pop(L, 1);
    // create and init the control block (struct with thread)
    self = (Proc *)lua_newuserdata(L, sizeof(Proc));
    lua_setfield(L, LUA_REGISTRYINDEX, "_SELF");
    self->L = L;
    self->thread = pthread_self();
    self->channel = NULL;
    pthread_cond_init(&self->cond, NULL);
    // call code chunk
    if (lua_pcall(L, 0, 0, 0) != 0) {
        fprintf(stderr, "thread error: %s", lua_tostring(L, -1));
    }
    pthread_cond_destroy(&getself(L)->cond);
    lua_close(L);
    return NULL;
}

static int ll_exit(lua_State *L) {
    pthread_exit(NULL);
    return 0;
}

static const struct luaL_Reg ll_funcs[] = {
    {"start", ll_start},
    {"send", ll_send},
    {"receive", ll_receive},
    {"exit", ll_exit},
    {NULL, NULL}
};

int luaopen_lproc(lua_State *L) {
    luaL_newlib(L, ll_funcs);
    return 1;
}

// package.preload[name] = f
static void registerlib(lua_State *L, const char *name, lua_CFunction f) {
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "preload");
    lua_pushcfunction(L, f);
    lua_setfield(L, -2, name);
    lua_pop(L, 2);
}

// open basic libs
static void openlibs(lua_State *L) {
    luaL_requiref(L, "_G", luaopen_base, 1);
    luaL_requiref(L, "package", luaopen_package, 1);
    lua_pop(L, 2);  // remove the results of luaL_requiref
    registerlib(L, "coroutine", luaopen_coroutine);
    registerlib(L, "table", luaopen_table);
    registerlib(L, "io", luaopen_io);
    registerlib(L, "os", luaopen_os);
    registerlib(L, "string", luaopen_string);
    registerlib(L, "math", luaopen_math);
    registerlib(L, "utf8", luaopen_utf8);
    registerlib(L, "debug", luaopen_debug);
}

