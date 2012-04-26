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
    #include?

```

## Usage

```ruby
  require 'bloom-filter'

  filter = BloomFilter.new

  # auto-calculate optimum bitmap size based on maximum number of items stored and desired max error rate.
  filter = BloomFilter.new size: 100_000, error_rate: 0.01

  # specify bitmap size & number of hash functions explicitly.
  filter = BloomFilter.new bits: 100_000_0, hashes: 4

  filter.insert "foo"
  filter.include? "foo" #=> true
  filter.include? "bar" #=> false

  filter.dump "/tmp/random.bloom"
  filter = BloomFilter.load "/tmp/random.bloom"
```

## See Also

[https://github.com/igrigorik/bloomfilter-rb](https://github.com/igrigorik/bloomfilter-rb)

## License

[Creative Commons Attribution - CC BY](http://creativecommons.org/licenses/by/3.0)

## Home Page

[https://github.com/deepfryed/bloom-filter](https://github.com/deepfryed/bloom-filter)
