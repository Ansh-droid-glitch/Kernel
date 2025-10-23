// fs.h
#ifndef FS_H
#define FS_H


#include <stdint.h>
#include <stddef.h>


typedef enum {
FS_NODE_FILE = 0,
FS_NODE_DIR = 1,
} fs_node_type_t;


void fs_init(void);
void fs_ls(const char *path);
int fs_mkdir(const char *path);
int fs_touch(const char *path);
int fs_write(const char *path, const char *data, size_t len);
int fs_read(const char *path, char *buf, size_t bufsize);


#endif // FS_H