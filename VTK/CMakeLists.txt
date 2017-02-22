cmake_minimum_required(VERSION 2.8)
 
PROJECT(prog)
 
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
 
add_executable(prog MACOSX_BUNDLE prog)
 
if(VTK_LIBRARIES)
  target_link_libraries(prog ${VTK_LIBRARIES})
else()
  target_link_libraries(prog vtkHybrid vtkWidgets)
endif()