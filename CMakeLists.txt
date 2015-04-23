cmake_minimum_required (VERSION 2.6)
project (bwProject)
file (GLOB CURRENT_SOURCE_FILE source/*.cxx)

set ( SOURCE_FILE ${CURRENT_SOURCE_FILE} CACHE INTERNAL "list for source files" )

find_package(MPI REQUIRED) #make it REQUIRED, if you want
include_directories(SYSTEM ${MPI_INCLUDE_PATH})
set(LINK_LIBRARIES "${MPI_C_LIBRARIES}")

set(USE_EasyBMP 1)
# Add EasyBMP for image output
add_subdirectory( Modules )

#FIND_PACKAGE( OpenMP REQUIRED)
#if(OPENMP_FOUND)
#	message("OPENMP FOUND")
#	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
#endif()

#find_package( VTK COMPONENTS
#	vtkFiltersSources
#	vtkInteractionStyle
#	vtkRenderingOpenGL
#)
#include (${VTK_USE_FILE})
#set(VTK_DIR "/home/jie/study/cs595/VTK/VTK-build")

#message( ${SOURCE_FILE})

if (USE_EasyBMP)
	include_directories(${EasyBMP_INCLUDE})
endif (USE_EasyBMP)

add_executable(demo ${SOURCE_FILE})
#target_link_libraries(demo ${MPI_C_LIBRARIES})
target_link_libraries(demo ${LINK_LIBRARIES})
