<<<<<<< HEAD
PROJECT(LungChangeDetector)

FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
ENDIF(ITK_FOUND)

IF(ITKVtkGlue_LOADED)
    FIND_PACKAGE(VTK REQUIRED)
    INCLUDE(${VTK_USE_FILE})
ELSE()
    FIND_PACKAGE(ItkVtkGlue REQUIRED)
    INCLUDE(${ItkVtkGlue_USE_FILE})
    SET(Glue ItkVtkGlue)
ENDIF()

ADD_EXECUTABLE(LungChangeDetector LungChangeDetector.cxx RegisterOrganFilter.cxx NonlinearRegisterOrganFilter.cxx)

TARGET_LINK_LIBRARIES(LungChangeDetector ${ITK_LIBRARIES} ${VTK_LIBRARIES} ${Glue})
=======
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
>>>>>>> Updated file name and gitignore
