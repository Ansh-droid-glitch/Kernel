#include "fs.h"
#include <stdint.h>
#include <stddef.h>

#define FS_MAX_NODES 128
#define FS_MAX_NAME   64
#define FS_MAX_DATA   1024

struct fs_node {
    char name[FS_MAX_NAME];
    fs_node_type_t type;
    struct fs_node *child;
    struct fs_node *next;
    struct fs_node *parent;
    char data[FS_MAX_DATA];
    size_t size;
};

static struct fs_node fs_nodes[FS_MAX_NODES];
static int fs_node_used[FS_MAX_NODES];
static struct fs_node *fs_root = NULL;

// --- minimal string functions ---
static void str_copy(char *dst, const char *src, size_t n) {
    for (size_t i = 0; i < n && src[i]; i++) dst[i] = src[i];
    if (n > 0) dst[n - 1] = 0;
}

static int str_compare(const char *a, const char *b, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (a[i] != b[i] || a[i] == 0 || b[i] == 0) return (unsigned char)a[i] - (unsigned char)b[i];
    }
    return 0;
}

static void mem_set(void *ptr, uint8_t val, size_t n) {
    uint8_t *p = ptr;
    for (size_t i = 0; i < n; i++) p[i] = val;
}

static void mem_copy(void *dst, const void *src, size_t n) {
    uint8_t *d = dst;
    const uint8_t *s = src;
    for (size_t i = 0; i < n; i++) d[i] = s[i];
}

// --- fs node management ---
static struct fs_node *alloc_node(void) {
    for (int i = 0; i < FS_MAX_NODES; ++i) {
        if (!fs_node_used[i]) {
            fs_node_used[i] = 1;
            mem_set(&fs_nodes[i], 0, sizeof(struct fs_node));
            return &fs_nodes[i];
        }
    }
    return NULL;
}

static struct fs_node *find_child(struct fs_node *dir, const char *name) {
    struct fs_node *c = dir->child;
    while (c) {
        if (str_compare(c->name, name, FS_MAX_NAME) == 0) return c;
        c = c->next;
    }
    return NULL;
}

// path parsing
static char *next_segment(const char **p) {
    static char seg[FS_MAX_NAME];
    const char *s = *p;
    while (*s == '/') s++;
    size_t i = 0;
    while (*s && *s != '/' && i + 1 < FS_MAX_NAME) seg[i++] = *s++;
    seg[i] = 0;
    *p = s;
    return seg;
}

static struct fs_node *walk_path(const char *path, int create_final, fs_node_type_t final_type) {
    if (!fs_root || !path) return NULL;
    if (path[0] == '/' && path[1] == 0) return fs_root;
    const char *p = path;
    struct fs_node *cur = fs_root;
    while (1) {
        char *seg = next_segment(&p);
        if (seg[0] == 0) break;
        struct fs_node *child = find_child(cur, seg);
        const char *peek = p; while (*peek == '/') peek++; int more = (*peek != 0);
        if (!child) {
            if (create_final) {
                struct fs_node *n = alloc_node();
                if (!n) return NULL;
                str_copy(n->name, seg, FS_MAX_NAME);
                n->type = (more ? FS_NODE_DIR : final_type);
                n->parent = cur;
                n->next = cur->child;
                cur->child = n;
                child = n;
            } else return NULL;
        }
        cur = child;
        if (!more) break;
    }
    return cur;
}

void fs_init(void) {
    mem_set(fs_node_used, 0, sizeof(fs_node_used));
    fs_root = alloc_node();
    if (!fs_root) return;
    str_copy(fs_root->name, "/", FS_MAX_NAME);
    fs_root->type = FS_NODE_DIR;
}

int fs_ls(const char *path, void (*cb)(const char*, fs_node_type_t, void*), void *user) {
    struct fs_node *n = walk_path(path ? path : "/", 0, FS_NODE_DIR);
    if (!n || n->type != FS_NODE_DIR) return -1;
    struct fs_node *c = n->child;
    while (c) { cb(c->name, c->type, user); c = c->next; }
    return 0;
}

int fs_mkdir(const char *path) {
    struct fs_node *n = walk_path(path, 1, FS_NODE_DIR);
    return (n && n->type == FS_NODE_DIR) ? 0 : -1;
}

int fs_touch(const char *path) {
    struct fs_node *n = walk_path(path, 1, FS_NODE_FILE);
    return (n && n->type == FS_NODE_FILE) ? 0 : -1;
}

int fs_write(const char *path, const char *data, size_t len) {
    struct fs_node *n = walk_path(path, 1, FS_NODE_FILE);
    if (!n || n->type != FS_NODE_FILE) return -1;
    size_t to_copy = (len < FS_MAX_DATA) ? len : FS_MAX_DATA;
    mem_copy(n->data, data, to_copy);
    n->size = to_copy;
    return (int)to_copy;
}

int fs_read(const char *path, char *buf, size_t bufsize) {
    struct fs_node *n = walk_path(path, 0, FS_NODE_FILE);
    if (!n || n->type != FS_NODE_FILE || !buf || bufsize == 0) return -1;
    size_t to_copy = (n->size < bufsize - 1) ? n->size : (bufsize - 1);
    mem_copy(buf, n->data, to_copy);
    buf[to_copy] = 0;
    return (int)to_copy;
}
