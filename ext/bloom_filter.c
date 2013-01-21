#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "ruby/ruby.h"
#include "bloom-filter.h"
#include "hash-string.h"
#include "version.h"

#define TO_S(v)    rb_funcall(v, rb_intern("to_s"), 0)
#define CSTRING(v) RSTRING_PTR(TO_S(v))

typedef struct FileHeader {
    uint64_t table_size;
    uint64_t num_functions;
} FileHeader;

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
    double error;
    size_t nbits, nhash, nmax;

    VALUE max_size, error_rate, bitmap_size, hash_count, options;
    BloomFilter *filter = 0;

    rb_scan_args(argc, argv, "01", &options);
    if (!NIL_P(options) && TYPE(options) != T_HASH)
        rb_raise(rb_eArgError, "invalid options, expect hash");

    if (NIL_P(options)) {
        nbits = 1000000;
        nhash = 4;
    }
    else {
        max_size    = rb_hash_aref(options, ID2SYM(rb_intern("size")));
        error_rate  = rb_hash_aref(options, ID2SYM(rb_intern("error_rate")));
        bitmap_size = rb_hash_aref(options, ID2SYM(rb_intern("bits")));
        hash_count  = rb_hash_aref(options, ID2SYM(rb_intern("hashes")));

        nhash = NIL_P(hash_count) ? 4 : NUM2ULONG(hash_count);

        if (!NIL_P(bitmap_size))
            nbits = NUM2ULONG(bitmap_size);
        else if (!NIL_P(max_size)) {
            nmax  = NUM2ULONG(max_size);
            error = NIL_P(error_rate) ? 0.01 : NUM2DBL(error_rate);
            nbits = ceil(fabs(log(error) * (double)nmax / pow(log(2), 2)));
            nhash = ceil(0.7 * (double)nbits / (double)nmax);
        }
        else
            rb_raise(rb_eArgError, "requires either size & error_rate or bits & hashes");
    }

    filter = bloom_filter_new(nbits, string_nocase_hash, nhash);

    if (!filter)
        rb_raise(rb_eNoMemError, "unable to allocate memory for BloomFilter");

    DATA_PTR(self) = filter;
    return self;
}


VALUE bloom_insert(VALUE klass, VALUE string) {
    BloomFilter *filter = bloom_handle(klass);
    bloom_filter_insert(filter, (BloomFilterValue)CSTRING(string));
    return Qtrue;
}

VALUE bloom_include(VALUE klass, VALUE string) {
    BloomFilter *filter = bloom_handle(klass);
    return bloom_filter_query(filter, (BloomFilterValue)CSTRING(string)) ? Qtrue : Qfalse;
}

VALUE bloom_dump(VALUE klass, VALUE file) {
    int fd;
    void *buffer;
    uint64_t nbits;
    FileHeader header;
    BloomFilter *filter = bloom_handle(klass);

    nbits  = (filter->table_size + 7) / 8;
    buffer = malloc(nbits);

    if (!buffer)
        rb_raise(rb_eNoMemError, "out of memory dumping BloomFilter");

    bloom_filter_read(filter, buffer);

    fd = open(CSTRING(file), O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
    if (fd == -1) {
        free(buffer);
        rb_raise(rb_eIOError, "unable to open file. %s", strerror(errno));
    }

    header.table_size    = filter->table_size;
    header.num_functions = filter->num_functions;

    if (write(fd, &header, sizeof(header)) == -1) {
        free(buffer);
        close(fd);
        rb_raise(rb_eIOError, "error dumping BloomFilter: %s\n", strerror(errno));
    }

    if (write(fd, buffer, nbits) != -1) {
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

VALUE bloom_bits(VALUE klass) {
    BloomFilter *filter = bloom_handle(klass);

    VALUE bitmap;
    char *buffer;
    unsigned char b;
    int i = 0, bit, nbits = filter->table_size;

    buffer = (char *)malloc(nbits);
    if (!buffer)
        rb_raise(rb_eNoMemError, "out of memory dumping BloomFilter");

    for (i = 0; i < nbits; i++) {
        b = filter->table[i / 8];
        bit = 1 << (i % 8);

        if ((b & bit) == 0)
            buffer[i] = '0';
	    else
	        buffer[i] = '1';
    }

    bitmap = rb_str_new(buffer, nbits);
    free(buffer);
    return bitmap;
}

VALUE bloom_load(VALUE klass, VALUE file) {
    int fd;
    void *buffer;
    size_t nbits, bytes;
    FileHeader header;
    BloomFilter *filter;
    VALUE instance;

    fd = open(CSTRING(file), O_RDONLY);
    if (fd == -1)
        rb_raise(rb_eIOError, "unable to open file: %s", strerror(errno));

    if (read(fd, &header, sizeof(header)) != sizeof(header)) {
        close(fd);
        rb_raise(rb_eIOError, "unable to read file, header corrupted\n");
    }

    nbits  = (header.table_size + 7) / 8;
    buffer = malloc(nbits);
    if (!buffer) {
        close(fd);
        rb_raise(rb_eNoMemError, "out of memory dumping BloomFilter");
    }

    bytes = read(fd, buffer, nbits);
    if (bytes != nbits) {
        free(buffer);
        close(fd);
        rb_raise(rb_eStandardError, "unable to load BloomFilter, expected %ld but got %ld bytes", nbits, bytes);
    }

    filter = bloom_filter_new(header.table_size, string_nocase_hash, header.num_functions);
    bloom_filter_load(filter, buffer);
    instance = Data_Wrap_Struct(klass, 0, bloom_free, filter);

    free(buffer);
    close(fd);
    return instance;
}

void Init_bloom_filter() {
    VALUE cBloom = rb_define_class("BloomFilter", rb_cObject);

    rb_define_method(cBloom, "initialize", RUBY_METHOD_FUNC(bloom_initialize),  -1);
    rb_define_method(cBloom, "dump",       RUBY_METHOD_FUNC(bloom_dump),         1);
    rb_define_method(cBloom, "bits",       RUBY_METHOD_FUNC(bloom_bits),         0);
    rb_define_method(cBloom, "insert",     RUBY_METHOD_FUNC(bloom_insert),       1);
    rb_define_method(cBloom, "include?",   RUBY_METHOD_FUNC(bloom_include),      1);

    rb_define_alloc_func(cBloom, bloom_allocate);
    rb_define_singleton_method(cBloom, "load", RUBY_METHOD_FUNC(bloom_load), 1);
    rb_define_const(cBloom, "VERSION", rb_str_new2(RUBY_BLOOM_FILTER_VERSION));
}
