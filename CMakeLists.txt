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
find_package(Vulkan REQUIRED) # If you have GLM installed properly: find_package(GLM REQUIRED)
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

set(ALL_LIBRARIES ${Vulkan_LIBRARIES} ${ASSIMP_LIBRARIES} ${ASSIMP_ZLIB} glfw GLEW::glew_s)

# # BasicGOL
# add_executable(BasicGOL ${GENERAL_SOURCES} "./src/app/BasicGOL.cpp")
# target_link_libraries(BasicGOL ${ALL_LIBRARIES})
# install(TARGETS BasicGOL RUNTIME DESTINATION bin/BasicGOL)

# # FloatingGOL
# add_executable(FloatingGOL ${GENERAL_SOURCES} "./src/app/FloatingGOL.cpp")
# target_link_libraries(FloatingGOL ${ALL_LIBRARIES})
# install(TARGETS FloatingGOL RUNTIME DESTINATION bin/FloatingGOL)

# GOL
add_executable(GOL ${GENERAL_SOURCES} "./src/app/GOL.cpp")
target_link_libraries(GOL ${ALL_LIBRARIES})
install(TARGETS GOL RUNTIME DESTINATION bin/GOL)

# ExampleGOL
add_executable(ExampleGOL ${GENERAL_SOURCES} "./src/app/ExampleGOL.cpp")
target_link_libraries(ExampleGOL ${ALL_LIBRARIES})
install(TARGETS ExampleGOL RUNTIME DESTINATION bin/ExampleGOL)