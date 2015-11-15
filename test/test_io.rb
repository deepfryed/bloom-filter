require 'helper'
require 'tempfile'

describe 'BloomFilter load & dump' do
  it 'should dump and load a filter' do
    file = Tempfile.new("bloom-filter-test")

    assert filter = BloomFilter.new(size: 100)
    assert filter.insert("foo")
    assert filter.dump(file.path), "dump filter"
    assert filter = BloomFilter.load(file.path)
    assert filter.include?("foo")
    assert !filter.include?("bar")
  end

  it 'should accept assigning the bits directly' do
    assert filter = BloomFilter.new(bits: 80)
    assert filter.insert("foo")
    assert filter2 = BloomFilter.new(bits: 80)
    assert filter2.binary = filter.binary
    assert filter2.include?("foo")
    assert !filter2.include?("bar")
  end
end
