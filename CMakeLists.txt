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

ADD_EXECUTABLE(LungChangeDetector LungChangeDetector.cxx RegisterOrganFilter.cxx NonlinearRegisterOrganFilter.cxx SegmentLungVolume.cxx)

TARGET_LINK_LIBRARIES(LungChangeDetector ${ITK_LIBRARIES} ${VTK_LIBRARIES} ${Glue})
