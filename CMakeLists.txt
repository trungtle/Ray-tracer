project(raytracer)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_OSX_SYSROOT /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.sdk)

# Add pbrt parser from https://github.com/ingowald/pbrt-parser/
add_subdirectory(thirdparty/pbrt-parser EXCLUDE_FROM_ALL) 

# set creates a variable
set (NUPARU "nuparu")
set (THIRDPARTY "thirdparty")
# add include and src directories to path
include_directories(
        src/
        ${THIRDPARTY}/include
        ${THIRDPARTY}/pbrt-parser
        ${THIRDPARTY}/pbrt-parser/pbrtParser/include/
        ${NUPARU}/include
        ${NUPARU}/src
)

# Add path for pre-compiled libraries here (we will later link them with our compiled source)
# Add Nuparu library to path for OSX, linux and windows
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${NUPARU}/lib/osx)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${NUPARU}/lib/linux /usr/lib64)
elseif(WIN32)
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${NUPARU}/lib/win)
endif()

#set include variables
set(GLFW_INCLUDE_DIR ${NUPARU}/include )
set(GLEW_INCLUDE_DIR ${NUPARU}/include )
set(GLFW_LIBRARY_DIR ${CMAKE_LIBRARY_PATH})
set(GLEW_LIBRARY_DIR ${CMAKE_LIBRARY_PATH})

# Use find_package & find_library to link with
find_package(OPENGL REQUIRED)
find_package(GLEW)
find_library(JSONCPP "jsoncpp")
find_library(GLFW_LIBRARY "glfw3" HINTS ${GLFW_LIBRARY_DIR})
find_library(TBB "tbb")
find_library(HALF "half")
find_library(PBRT "pbrtParser")

add_definitions(
        -DTW_STATIC
        -DTW_NO_LIB_PRAGMA
        -DTW_NO_DIRECT3D
        -DGLEW_STATIC
        -D_CRT_SECURE_NO_WARNINGS
)

set(CORE_LIBS ${GLFW_LIBRARY} ${GLUT_LIBRARY} ${GLEW_LIBRARY} ${JSONCPP} ${TBB} ${HALF} ${PBRT} m)

# OSX-specific hacks/fixes
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    find_library(COCOA Cocoa)
    find_library(COREVIDEO CoreVideo)
    find_library(IOKIT IOKit)
    find_library(COREFOUNDATION CoreFoundation)
    find_library(CARBON_FRAMEWORK Carbon)

    set(CORE_LIBS ${CORE_LIBS} "-framework Cocoa -framework IOKit -framework CoreFoundation -framework CoreVideo -framework Carbon")

endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

# Linux specific hacks/fixes
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lX11 -lXxf86vm -lXrandr -lpthread -lXi")
endif()

# set compiler flags for c++11
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux" OR ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -m64 -msse2 -w")
elseif(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

if(MSVC)
    set(COMPILER_FLAGS
            CMAKE_CXX_FLAGS
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_C_FLAGS
            CMAKE_C_FLAGS_DEBUG
            CMAKE_C_FLAGS_RELEASE
        )
endif()

# Add source files you want to compile (.cpp)
set(CORE_SRC
        src/main.cpp
        src/core/scene.cpp
        src/integrators/integrator.cpp
        src/samplers/sampler.cpp
        src/shapes/mesh.cpp
)

add_executable(Raytracer ${CORE_SRC})
target_link_libraries(Raytracer ${CORE_LIBS})

# Include zlib for osx
find_package( ZLIB REQUIRED )
if ( ZLIB_FOUND )
    include_directories( ${ZLIB_INCLUDE_DIRS} )
    target_link_libraries( Raytracer ${ZLIB_LIBRARIES} )
endif( ZLIB_FOUND )

