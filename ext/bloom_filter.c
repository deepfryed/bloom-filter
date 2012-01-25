#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "ruby/ruby.h"
#include "bloom-filter.h"
#include "hash-string.h"
#include "version.h"

#define TO_S(v)    rb_funcall(v, rb_intern("to_s"), 0)
#define CSTRING(v) RSTRING_PTR(TO_S(v))

static void bloom_free(BloomFilter *filter) {
    if (filter)
        bloom_filter_free(filter);
}

VALUE bloom_allocate(VALUE klass) {
    BloomFilter *filter = 0;
    return Data_Wrap_Struct(klass, 0, bloom_free, filter);
}

BloomFilter* bloom_handle(VALUE self) {
    BloomFilter *filter = 0;
    Data_Get_Struct(self, BloomFilter, filter);
    if (!filter)
        rb_raise(rb_eArgError, "invalid BloomFilter instance");
    return filter;
}

VALUE bloom_initialize(int argc, VALUE *argv, VALUE self) {
    size_t size;
    VALUE table_size;
    BloomFilter *filter = 0;

    rb_scan_args(argc, argv, "01", &table_size);
    if (NIL_P(table_size))
        size = 1000000;
    else
        size = atol(CSTRING(table_size));

    filter = bloom_filter_new(size, string_nocase_hash, 4);

    if (!filter)
        rb_raise(rb_eNoMemError, "unable to allocate memory for BloomFilter");

    DATA_PTR(self) = filter;
}


VALUE bloom_insert(VALUE klass, VALUE string) {
    BloomFilter *filter = bloom_handle(klass);
    bloom_filter_insert(filter, (BloomFilterValue)CSTRING(string));
    return Qtrue;
}

VALUE bloom_exists(VALUE klass, VALUE string) {
    BloomFilter *filter = bloom_handle(klass);
    return bloom_filter_query(filter, (BloomFilterValue)CSTRING(string)) ? Qtrue : Qfalse;
}

VALUE bloom_dump(VALUE klass, VALUE file) {
    int fd;
    uint64_t table_size;
    BloomFilter *filter = bloom_handle(klass);

    size_t size  = (filter->table_size + 7) / 8;
    void *buffer = malloc(size);

    if (!buffer)
        rb_raise(rb_eNoMemError, "out of memory dumping BloomFilter");

    bloom_filter_read(filter, buffer);

    fd = open(CSTRING(file), O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
    if (fd == -1) {
        free(buffer);
        rb_raise(rb_eIOError, "unable to open file. %s", strerror(errno));
    }

    table_size = filter->table_size;
    if (write(fd, &table_size, sizeof(table_size)) == -1) {
        free(buffer);
        close(fd);
        rb_raise(rb_eIOError, "error dumping BloomFilter: %s\n", strerror(errno));
    }

    if (write(fd, buffer, size) != -1) {
        free(buffer);
        close(fd);
        return Qtrue;
    }
    else {
        free(buffer);
        close(fd);
        rb_raise(rb_eIOError, "error dumping BloomFilter: %s\n", strerror(errno));
    }

    return Qfalse; // not reachable
}

VALUE bloom_load(VALUE klass, VALUE file) {
    int fd;
    void *buffer;
    size_t size, bytes;
    uint64_t table_size;
    BloomFilter *filter;
    VALUE ts, instance;

    fd = open(CSTRING(file), O_RDONLY);
    if (fd == -1)
        rb_raise(rb_eIOError, "unable to open file: %s", strerror(errno));

    read(fd, &table_size, sizeof(table_size));

    size   = (table_size + 7) / 8;
    buffer = malloc(size);
    if (!buffer) {
        close(fd);
        rb_raise(rb_eNoMemError, "out of memory dumping BloomFilter");
    }

    bytes = read(fd, buffer, size);
    if (bytes != size) {
        free(buffer);
        close(fd);
        rb_raise(rb_eStandardError, "unable to load BloomFilter, expected %ld but got %ld bytes", size, bytes);
    }

    ts       = ULONG2NUM(table_size);
    instance = bloom_allocate(klass);
    bloom_initialize(1, &ts, instance);
    bloom_filter_load(bloom_handle(instance), buffer);

    free(buffer);
    close(fd);
    return instance;
}

Init_bloom_filter() {
    VALUE cBloom = rb_define_class("BloomFilter", rb_cObject);

    rb_define_method(cBloom, "initialize", RUBY_METHOD_FUNC(bloom_initialize),  -1);
    rb_define_method(cBloom, "dump",       RUBY_METHOD_FUNC(bloom_dump),         1);
    rb_define_method(cBloom, "insert",     RUBY_METHOD_FUNC(bloom_insert),       1);
    rb_define_method(cBloom, "exists?",    RUBY_METHOD_FUNC(bloom_exists),       1);

    rb_define_alloc_func(cBloom, bloom_allocate);
    rb_define_singleton_method(cBloom, "load", RUBY_METHOD_FUNC(bloom_load), 1);
    rb_define_const(cBloom, "VERSION", rb_str_new2(RUBY_BLOOM_FILTER_VERSION));
}
