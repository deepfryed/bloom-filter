# -*- encoding: utf-8 -*-

Gem::Specification.new do |s|
  s.name = "bloom-filter"
  s.version = "0.2.0"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.authors = ["Bharanee Rathna"]
  s.date = "2012-04-27"
  s.description = "A fast Bloom Filter library for Ruby for unices."
  s.email = ["deepfryed@gmail.com"]
  s.extensions = ["ext/extconf.rb"]
  s.files = ["ext/bloom-filter.c", "ext/hash-string.c", "ext/bloom_filter.c", "ext/bloom-filter.h", "ext/hash-string.h", "ext/version.h", "ext/extconf.rb", "test/helper.rb", "test/test_io.rb", "test/test_basic.rb", "README.md", "CHANGELOG", "lib/bloom-filter.rb"]
  s.homepage = "http://github.com/deepfryed/bloom-filter"
  s.require_paths = ["lib"]
  s.rubygems_version = "1.8.21"
  s.summary = "A fast Bloom Filter library for Ruby."

  if s.respond_to? :specification_version then
    s.specification_version = 3

    if Gem::Version.new(Gem::VERSION) >= Gem::Version.new('1.2.0') then
      s.add_development_dependency(%q<rake>, [">= 0"])
      s.add_development_dependency(%q<rake-compiler>, [">= 0"])
    else
      s.add_dependency(%q<rake>, [">= 0"])
      s.add_dependency(%q<rake-compiler>, [">= 0"])
    end
  else
    s.add_dependency(%q<rake>, [">= 0"])
    s.add_dependency(%q<rake-compiler>, [">= 0"])
  end
end
