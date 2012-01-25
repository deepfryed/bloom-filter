require 'date'
require 'pathname'
require 'rake'
require 'rake/clean'
require 'rake/testtask'
require 'rake/extensiontask'

$rootdir = Pathname.new(__FILE__).dirname
$gemspec = Gem::Specification.new do |s|
  s.name              = 'bloom-filter'
  s.version           = '0'           # modify ext/version.h
  s.date              = Date.today    
  s.authors           = ['Bharanee Rathna']
  s.email             = ['deepfryed@gmail.com']
  s.summary           = 'A fast Bloom Filter library for Ruby.'
  s.description       = 'A fast Bloom Filter library for Ruby for unices.'
  s.homepage          = 'http://github.com/deepfryed/bloom-filter'
  s.files             = Dir['ext/**/*.{cc,c,h}'] + Dir['{ext,test}/*.rb'] + %w(README.md CHANGELOG) + Dir['lib/*']
  s.extensions        = %w(ext/extconf.rb)
  s.require_paths     = %w(lib)

  s.add_development_dependency('rake')
  s.add_development_dependency('rake-compiler')
end

desc 'Generate bloom-filter gemspec'
task :gemspec do 
  $gemspec.date    = Date.today
  $gemspec.version = File.read($rootdir + 'ext/version.h').scan(/[\d.]+/).first
  File.open('bloom-filter.gemspec', 'w') {|fh| fh.write($gemspec.to_ruby)}
end

Rake::ExtensionTask.new do |ext|
  ext.name    = 'bloom_filter'
  ext.ext_dir = 'ext'
  ext.lib_dir = 'lib'
end

Rake::TestTask.new(:test) do |test|
  test.libs   << 'ext' << 'lib' << 'test'
  test.pattern = 'test/**/test_*.rb'
  test.verbose = true
end

task test:    [:compile]  
task default: :test
