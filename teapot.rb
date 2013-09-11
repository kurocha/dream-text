
#
#  This file is part of the "Teapot" project, and is released under the MIT license.
#

teapot_version "0.8.0"

define_project "Dream Text" do |project|
	project.add_author "Samuel Williams"
	project.license = "MIT License"
	
	project.version = "0.1.0"
end

define_target "dream-text" do |target|
	target.build do |environment|
		build_directory(package.path, 'source', environment)
	end
	
	target.depends "Library/Dream"
	target.depends "Library/Dream/Imaging"
	target.depends "Language/C++11"
	
	target.depends "Library/freetype"
	
	target.provides "Library/Dream/Text" do
		append linkflags ["-lDreamText"]
	end
end

define_target "dream-text-tests" do |target|
	target.build do |environment|
		build_directory(package.path, 'test', environment)
	end
	
	target.run do |environment|
		run_executable("bin/dream-text-test-runner", environment)
	end
	
	target.depends "Library/Dream/Text"
	target.depends "Library/UnitTest"
	
	target.provides "Test/Dream/Text"
end

define_configuration "dream-text" do |configuration|
	configuration.public!
	
	configuration.require "dream"
	configuration.require "dream-imaging"
	configuration.require "freetype"
end

define_configuration "travis" do |configuration|
	configuration[:source] = "https://github.com/dream-framework"

	configuration.require "platforms"
	
	configuration.import "dream-text"
	
	configuration[:run] = ["Test/Dream/Text"]
end
