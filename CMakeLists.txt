######################################
### VERSION / LIBRARY REQUIREMENTS ###

cmake_minimum_required(VERSION 3.5)
project( BasicOpenGL )

set(CMAKE_INSTALL_PREFIX .)

# set(MASTER_DEPEND "E:/Path/To/Dependencies/" CACHE PATH "Variable description")

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Libraries
find_package(Vulkan REQUIRED)
find_package(assimp REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)

# Build GLEW as a static library
add_definitions(-DGLEW_STATIC)


#########################################
### GET GENERAL SOURCES AND LIBRARIES ###

include_directories("src/inc" ${Vulkan_INCLUDE_DIR} ${ASSIMP_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})

file(GLOB GENERAL_SOURCES
	"src/lib/*.cpp"  
	"src/lib/*.hpp"  
)

# Adapted by Dr. Michael J. Reale from: https://gist.github.com/evilactually/a0d191701cb48f157b05be7f74d79396
macro(COMPILE_VULKAN_SHADERS target)
	file(GLOB SHADER_SOURCES	
		"vulkanshaders/${target}/*.vert"
		"vulkanshaders/${target}/*.frag"
	)

	foreach(GLSL ${SHADER_SOURCES})
		message("Shader Path: ${GLSL}")
		cmake_path(GET GLSL FILENAME filename)
		message("Shader filename: ${filename}")
		set(SPRIV "${PROJECT_BINARY_DIR}/compiledshaders/${target}/${filename}.spv")
		message("SPRIV path is: ${SPRIV}")

		add_custom_command(
			OUTPUT {$SPRIV}
			COMMAND cd
			# ** A bit unsure about how this line works: 
			COMMAND "${CMAKE_COMMAND}" -E make_directory "${PROJECT_BINARY_DIR}/compiledshaders/${target}/"
			COMMAND Vulkan::glslc ${GLSL} -o {$SPRIV}
			DEPENDS ${GLSL} # Rexecute if shader file changes
		)

		list(APPEND SPRIV_BINARY_FILES ${SPRIV})

	endforeach(GLSL)

	add_custom_target(
		# Creates a target with the name "Shaders_" + ${target}
		Shaders_${target}
		DEPENDS ${SPRIV_BINARY_FILES}
	)

	add_dependencies(${target} Shaders_${target})

endmacro()

################################
### CREATE / ADD EXECUTABLES ###

set(ALL_LIBRARIES ${Vulkan_LIBRARIES} ${ASSIMP_LIBRARIES} ${ASSIMP_ZLIB} glfw GLEW::glew_s)

# DeferredRender
add_executable(DeferredRender ${GENERAL_SOURCES} "./src/app/DeferredRender.cpp")
target_link_libraries(DeferredRender ${ALL_LIBRARIES})
install(TARGETS DeferredRender RUNTIME DESTINATION bin/DeferredRender)