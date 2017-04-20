cmake_minimum_required(VERSION 2.8)
 
project(SegmentLungs)
 
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
if (ITKVtkGlue_LOADED)
  find_package(VTK REQUIRED)
  include(${VTK_USE_FILE})
endif()
 
add_executable(SegmentLungs MACOSX_BUNDLE SegmentLungs CompositeLung)
 
if( "${ITK_VERSION_MAJOR}" LESS 4 )
  target_link_libraries(SegmentLungs ITKReview ${ITK_LIBRARIES})
else( "${ITK_VERSION_MAJOR}" LESS 4 )
  target_link_libraries(SegmentLungs ${ITK_LIBRARIES})
endif( "${ITK_VERSION_MAJOR}" LESS 4 )