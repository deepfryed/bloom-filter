# -*- encoding: utf-8 -*-
# stub: bloom-filter 0.2.2 ruby lib
# stub: ext/extconf.rb

Gem::Specification.new do |s|
  s.name = "bloom-filter"
  s.version = "0.2.2"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.require_paths = ["lib"]
  s.authors = ["Bharanee Rathna"]
  s.date = "2015-11-16"
  s.description = "A fast Bloom Filter library for Ruby for unices."
  s.email = ["deepfryed@gmail.com"]
  s.extensions = ["ext/extconf.rb"]
  s.files = ["CHANGELOG", "LICENSE", "README.md", "ext/bloom-filter.c", "ext/bloom-filter.h", "ext/bloom_filter.c", "ext/extconf.rb", "ext/hash-string.c", "ext/hash-string.h", "ext/version.h", "lib/bloom-filter.rb", "test/helper.rb", "test/test_basic.rb", "test/test_io.rb"]
  s.homepage = "http://github.com/deepfryed/bloom-filter"
  s.licenses = ["MIT"]
  s.rubygems_version = "2.2.2"
  s.summary = "A fast Bloom Filter library for Ruby."

  if s.respond_to? :specification_version then
    s.specification_version = 4

    if Gem::Version.new(Gem::VERSION) >= Gem::Version.new('1.2.0') then
      s.add_development_dependency(%q<rake>, ["~> 0"])
      s.add_development_dependency(%q<rake-compiler>, ["~> 0"])
    else
      s.add_dependency(%q<rake>, ["~> 0"])
      s.add_dependency(%q<rake-compiler>, ["~> 0"])
    end
  else
    s.add_dependency(%q<rake>, ["~> 0"])
    s.add_dependency(%q<rake-compiler>, ["~> 0"])
  end
end
