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
find_package(glm REQUIRED) # **If you can't get GLM installed, do "find_package(vulkan REQUIRED)", then remove "glm::glm" from "set(ALL_LIBRARIES..."
find_package(assimp REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(GLEW REQUIRED)

# Build GLEW as a static library
add_definitions(-DGLEW_STATIC)


#########################################
### GET GENERAL SOURCES AND LIBRARIES ###

include_directories("src/inc" ${Vulkan_INCLUDE_DIR} ${ASSIMP_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})

file(GLOB GENERAL_SOURCES
	"src/lib/*.cpp"  
	"src/lib/*.hpp"  
)

################################
### CREATE / ADD EXECUTABLES ###

set(ALL_LIBRARIES ${Vulkan_LIBRARIES} ${ASSIMP_LIBRARIES} ${ASSIMP_ZLIB} glfw GLEW::glew_s glm::glm)

# BasicGOL
add_executable(BasicGOL ${GENERAL_SOURCES} "./src/app/BasicGOL.cpp")
target_link_libraries(BasicGOL ${ALL_LIBRARIES})
install(TARGETS BasicGOL RUNTIME DESTINATION bin/BasicGOL)

# FloatGOL
add_executable(FloatGOL ${GENERAL_SOURCES} "./src/app/FloatGOL.cpp")
target_link_libraries(FloatGOL ${ALL_LIBRARIES})
install(TARGETS FloatGOL RUNTIME DESTINATION bin/FloatGOL)

# ExampleGOL
add_executable(ExampleGOL ${GENERAL_SOURCES} "./src/app/ExampleGOL.cpp")
target_link_libraries(ExampleGOL ${ALL_LIBRARIES})
install(TARGETS ExampleGOL RUNTIME DESTINATION bin/ExampleGOL)

# MulticolorGOL
add_executable(MulticolorGOL ${GENERAL_SOURCES} "./src/app/MulticolorGOL.cpp")
target_link_libraries(MulticolorGOL ${ALL_LIBRARIES})
install(TARGETS MulticolorGOL RUNTIME DESTINATION bin/MulticolorGOL)