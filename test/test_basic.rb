require 'helper'

describe 'BloomFilter primitives' do
  it 'should create one with default size' do
    assert BloomFilter.new
  end

  it 'should create one with given size' do
    assert BloomFilter.new(100)
  end

  it 'should insert' do
    assert filter = BloomFilter.new(100)
    assert filter.insert("foo")
  end

  it 'should allow membership checks' do
    assert filter = BloomFilter.new(100)
    assert filter.insert("foo")
    assert filter.exists?("foo")
    assert !filter.exists?("bar")
  end
end
