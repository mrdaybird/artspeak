cmake_minimum_required(VERSION 3.10)
project(artspeak
		VERSION 0.0.1
		DESCRIPTION "language for art"
		LANGUAGES CXX)

link_libraries(fmt)
add_executable(artspeak 
		core/src/artspeak.cpp
		core/src/Runner.cpp
		core/src/Token.cpp
		core/src/Scanner.cpp)

target_include_directories(artspeak PUBLIC
						core/include)

