SOURCES=db/builder.cc db/c.cc db/dbformat.cc db/db_impl.cc db/db_iter.cc db/dumpfile.cc db/filename.cc db/log_reader.cc db/log_writer.cc db/memtable.cc db/repair.cc db/table_cache.cc db/version_edit.cc db/version_set.cc db/write_batch.cc table/block_builder.cc table/block.cc table/filter_block.cc table/format.cc table/iterator.cc table/merger.cc table/table_builder.cc table/table.cc table/two_level_iterator.cc util/arena.cc util/bloom.cc util/cache.cc util/coding.cc util/comparator.cc util/crc32c.cc util/env.cc util/env_posix.cc util/env_win.cc util/filter_policy.cc util/hash.cc util/histogram.cc util/logging.cc util/options.cc util/status.cc  port/port_posix.cc
MEMENV_SOURCES=helpers/memenv/memenv.cc
CC=/home/crypto/Projects/Phore/depends/x86_64-pc-linux-gnu/share/../native/bin/ccache gcc -m64
CXX=/home/crypto/Projects/Phore/depends/x86_64-pc-linux-gnu/share/../native/bin/ccache g++ -m64 -std=c++11
PLATFORM=OS_LINUX
PLATFORM_LDFLAGS=-pthread
PLATFORM_LIBS=
PLATFORM_CCFLAGS=  -pthread -DOS_LINUX -DLEVELDB_PLATFORM_POSIX -DLEVELDB_ATOMIC_PRESENT
PLATFORM_CXXFLAGS=-std=c++0x  -pthread -DOS_LINUX -DLEVELDB_PLATFORM_POSIX -DLEVELDB_ATOMIC_PRESENT
PLATFORM_SHARED_CFLAGS=-fPIC
PLATFORM_SHARED_EXT=so
PLATFORM_SHARED_LDFLAGS=-shared -Wl,-soname -Wl,
PLATFORM_SHARED_VERSIONED=true
