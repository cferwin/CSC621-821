cmake_minimum_required(VERSION 2.8)
 
project(SegmentLungs)
 
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

IF(ITKVtkGlue_LOADED)
    FIND_PACKAGE(VTK REQUIRED)
    INCLUDE(${VTK_USE_FILE})
ELSE()
    FIND_PACKAGE(ItkVtkGlue REQUIRED)
    INCLUDE(${ItkVtkGlue_USE_FILE})
    SET(Glue ItkVtkGlue)
ENDIF()
 
add_executable(SegmentLungs MACOSX_BUNDLE SegmentLungs CompositeLung)
 
if( "${ITK_VERSION_MAJOR}" LESS 4 )
    target_link_libraries(SegmentLungs ITKReview ${ITK_LIBRARIES} ${VTK_LIBRARIES} ${Glue})
else( "${ITK_VERSION_MAJOR}" LESS 4 )
    target_link_libraries(SegmentLungs ${ITK_LIBRARIES} ${VTK_LIBRARIES} ${Glue})
endif( "${ITK_VERSION_MAJOR}" LESS 4 )
