# Credits
Taken from installation instructions for Dr. Michael J. Reale's CS450 and CS538 courses at SUNY Polytechnic Institute (Spring 2024)


# C++ Compilers
Download [Build Tools for Visual Studio 2022](https://aka.ms/vs/17/release/vs_BuildTools.exe)
* Under workloads, select "Desktop development with C++"


# CMake
Download [CMake](https://cmake.org/download/)
* During installation, select "Add CMake to the system PATH for all users"


# GLFW
Download [GLFW Source Package](https://www.glfw.org/download.html)
* Unzip
* Close all instances of VSCode
* Open VSCode as administrator
* Open unzipped folder
* Open command palette -> "CMake: Select a Kit" -> "Visual Studio Build Tools 202 Release - amd64"
* Configure project (cmd palette -> "CMake: Comfigure")
* Go to "build/CMakeCache.txt"
  * Add "CMAKE_DEBUG_POSTFIX:STRING=_d"
  * Find "CMAKE_INSTALL_PREFIX" and change "GLFW" to "glfw3"
  * Save and reconfigure
* CMake: Set Build Target -> install
* CMake: Select Varient -> Debug
* CMake: Build
* CMake: Select Varient -> Release
* CMake: Build


# GLEW
Download [glew-2.2.0.zip](https://github.com/nigels-com/glew/releases)
* Unzip
* Close all instances of VSCode
* Open VSCode as administrator
* Open [UnzippedFolder]/build/cmake/
* Open command palette -> "CMake: Select a Kit" -> "Visual Studio Build Tools 202 Release - amd64"
* CMake: Configure
* Add CMAKE_DEBUG_POSTFIX:STRING=_d anywhere in "build/CMakeCache.txt"
  * Save and reconfigure
* CMake: Set Build Target -> install
* CMake: Select Varient -> Debug
* CMake: Build
* CMake: Select Varient -> Release
* CMake: Build


# Assimp
Download [Souce Code](https://github.com/assimp/assimp/releases)
* Unzip
* Close all instances of VSCode
* Open VSCode as administrator
* Open unzipped folder
* Open command palette -> "CMake: Select a Kit" -> "Visual Studio Build Tools 202 Release - amd64"
* CMake: Configure
* In build/CMakeChache.txt
  * Set "ASSIMP_ISNTALL_PDB" to "OFF"
  * Set "BUILD_SHARED_LIBS" to "OFF"
  * Save and reconfigure
* CMake: Set Build Target -> install
* CMake: Select Varient -> Debug
* CMake: Build
* CMake: Select Varient -> Release
* CMake: Build


## GLM
 - Clone the [GitHub](https://github.com/g-truc/glm.git) repo
	 - git clone https://github.com/g-truc/glm.git
 - Open terminal in the directory cloned from github ("cd glm" if cloned from terminal)
	 - md build
	 - cmake -S . -B build -DCMAKE_INSTALL_PREFIX="/desired/install/path/glm"
	 	 - If not already, make sure this location is on the system's path
	 - cmake --build build
	 - cmake --build build --target install
