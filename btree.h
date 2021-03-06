#ifndef _BTREE_H
#define _BTREE_H

#ifdef __CHECKER__
#define FORCE           __attribute__((force))
#else
#define FORCE
#endif

#ifdef __CHECKER__
#define BITWISE         __attribute__((bitwise))
#else
#define BITWISE
#endif

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>

typedef uint16_t BITWISE __be16; /* big endian, 16 bits */
typedef uint32_t BITWISE __be32; /* big endian, 32 bits */
typedef uint64_t BITWISE __be64; /* big endian, 64 bits */

#define SHA1_LENGTH	20

#define CACHE_SLOTS	23 /* prime */
// btree的一个项
// 首先是20bytes的sha1
// 接着是8bytes的偏移
// 再接着是8bytes的子节点
struct btree_item {
	uint8_t sha1[SHA1_LENGTH];
	__be64 offset;
	__be64 child;
} __attribute__((packed));

#define TABLE_SIZE	((4096 - 1) / sizeof(struct btree_item))
// btree一个表的大小
struct btree_table {
	struct btree_item items[TABLE_SIZE];
	uint8_t size;
} __attribute__((packed));
// btree的缓存大小
struct btree_cache {
	off_t offset;
	struct btree_table *table;
};
// 内容信息
struct blob_info {
	__be32 len;
};
// 超级节点定义
struct btree_super {
	__be64 top;
	__be64 free_top;
	__be64 alloc;
} __attribute__((packed));

struct btree {
	// 记录
	off_t top;
	off_t free_top;
	off_t alloc;
	int fd;
	// 23个缓存位置
	struct btree_cache cache[CACHE_SLOTS];
};

/*
 * Open an existing database file.
 */
int btree_open(struct btree *btree, const char *file);

/*
 * Create and initialize a new database file.
 */
int btree_creat(struct btree *btree, const char *file);

/*
 * Close a database file opened with btree_creat() or btree_open().
 */
void btree_close(struct btree *btree);

/*
 * Insert a new item with key 'sha1' with the contents in 'data' to the
 * database file.
 */
void btree_insert(struct btree *btree, const uint8_t *sha1, const void *data,
		  size_t len);

/*
 * Look up item with the given key 'sha1' in the database file. Length of the
 * item is stored in 'len'. Returns a pointer to the contents of the item.
 * The returned pointer should be released with free() after use.
 */
void *btree_get(struct btree *btree, const uint8_t *sha1, size_t *len);

/*
 * Remove item with the given key 'sha1' from the database file.
 */
int btree_delete(struct btree *btree, const uint8_t *sha1);

#endif
