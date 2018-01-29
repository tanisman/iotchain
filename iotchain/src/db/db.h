#ifndef IOTCHAIN_DB_H
#define IOTCHAIN_DB_H

#include <rocksdb/db.h>
#include <rocksdb/slice.h>
#include <rocksdb/options.h>

extern rocksdb::DB* g_db;

#endif //IOTCHAIN_DB_H