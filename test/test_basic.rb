require 'helper'

describe 'BloomFilter primitives' do
  it 'should create one with default size' do
    assert BloomFilter.new
  end

  it 'should create one with given size' do
    assert BloomFilter.new(size: 100)
  end

  it 'should insert' do
    assert filter = BloomFilter.new(size: 100)
    assert filter.insert("foo")
  end

  it 'should allow membership checks' do
    assert filter = BloomFilter.new(size: 100)
    assert filter.insert("foo")
    assert filter.include?("foo")
    assert !filter.include?("bar")
  end

  it 'should return bits as a string' do
    assert filter = BloomFilter.new(bits: 5, hashes: 2)
    assert filter.insert("a")
    assert_equal "10010", filter.bits
  end
end
