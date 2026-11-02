#include <stddef.h>
#include <stdint.h>
struct fdb_kvdb { int unused; };
struct fdb_default_kv { void *kvs; unsigned num; };
struct fdb_blob { void *buffer; size_t size; struct { size_t len; } saved; };
#define FDB_NO_ERR 0
int fdb_kvdb_init(struct fdb_kvdb *, const char *, const char *, struct fdb_default_kv *, void *);
int fdb_kvdb_deinit(struct fdb_kvdb *);
struct fdb_blob *fdb_blob_make(struct fdb_blob *, void *, size_t);
size_t fdb_kv_get_blob(struct fdb_kvdb *, const char *, struct fdb_blob *);
char *fdb_kv_get(struct fdb_kvdb *, const char *);
