# BloomFilter

BloomFilter is a ruby library that implements an in-memory [Bloom Filter](http://en.wikipedia.org/wiki/Bloom_filter)

## Dependencies

  * ruby 1.9.1 or later

## Installation

```
  gem install bloom-filter
```

## API

```
  BloomFilter
    .new
    .load
    #dump
    #insert
    #exists?

```

## Usage

```ruby
  require 'bloom-filter'

  filter = BloomFilter.new
  filter = BloomFilter.new 100_000

  filter.insert "foo"
  filter.exists? "foo" #=> true
  filter.exists? "bar" #=> false

  filter.dump "/tmp/random.bloom"
  filter = BloomFilter.load "/tmp/random.bloom", 100_000
```

## See Also

[https://github.com/igrigorik/bloomfilter-rb](https://github.com/igrigorik/bloomfilter-rb)

## License

[Creative Commons Attribution - CC BY](http://creativecommons.org/licenses/by/3.0)
