cmake_minimum_required( VERSION 3.1 )

set( CMAKE_BUILD_TYPE Release )

# <REPLACE ITK_DIR WITH THE PATH TO ITK 5.1.0+ ON YOUR SYSTEM, IF NECESSARY DUE TO MULTIPLE BUILDS>

set( IMAGEIO_SOURCES
    HZeeTypes.cpp
    ImageCpuRecord.cpp
    ImageHeader.cpp
    ImageLoader.cpp
    ImageSettings.cpp
    ImageTransformations.cpp
    ParcellationCpuRecord.cpp
    itkbridge/ITKBridge.cpp
    itkbridge/ImageDataFactory.cpp
    itkdetails/ImageBaseData.cpp
    itkdetails/ImageIOInfo.cpp
    itkdetails/ImageReading.cpp
    itkdetails/ImageTypes.cpp
    itkdetails/ImageUtility.cpp
    util/MathFuncs.cpp )

set( IMAGEIO_HEADERS
    HZeeTypes.hpp
    ImageCpuRecord.h
    ImageHeader.h
    ImageLoader.h
    ImageSettings.h
    ImageTransformations.hpp
    ParcellationCpuRecord.h
    itkbridge/ITKBridge.hpp
    itkbridge/ImageDataFactory.hpp
    itkdetails/IITKImageIOInfo.hpp
    itkdetails/ImageBaseData.hpp
    itkdetails/ImageData.hpp
    itkdetails/ImageIOInfo.hpp
    itkdetails/ImageReading.hpp
    itkdetails/ImageTypes.hpp
    itkdetails/ImageUtility.hpp
    util/CreateParcellationImage.h
    util/Factory.hpp
    util/HZeeException.hpp
    util/MathFuncs.hpp )

set( IMAGEIO_TEMPLATES
    itkdetails/ImageData.tpp
    itkdetails/ImageReading.tpp )


#--------------------------------------------------------------------------------
# ITK Library
#--------------------------------------------------------------------------------

find_package( ITK 5.1.0 REQUIRED )

if( ITK_FOUND )
    include( ${ITK_USE_FILE} )
else( ITK_FOUND )
    message( FATAL_ERROR "Cannot build without ITK. Please set ITK_DIR." )
endif( ITK_FOUND )


#--------------------------------------------------------------------------------
# Boost Library
#--------------------------------------------------------------------------------

find_package( Boost 1.66.0 COMPONENTS REQUIRED
    system filesystem )

if( NOT Boost_FOUND )
    message( FATAL_ERROR "Boost was not found." )
endif()

set( Boost_USE_MULTITHREAD ON )


#--------------------------------------------------------------------------------
# Define HZeeImageIO library target
#--------------------------------------------------------------------------------

add_library( HZeeImageIO STATIC
    ${IMAGEIO_SOURCES}
    ${IMAGEIO_HEADERS}
    ${IMAGEIO_TEMPLATES} )

target_link_libraries( HZeeImageIO PRIVATE
    ${ITK_LIBRARIES}
    ${Boost_LIBRARIES}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY} )

target_include_directories( HZeeImageIO PRIVATE
    ${CMAKE_SOURCE_DIR}/src/imageio
    ${CMAKE_SOURCE_DIR}/externals/glm
    ${Boost_INCLUDE_DIR} )

target_include_directories( HZeeImageIO PUBLIC SYSTEM
    ${ITK_INCLUDE_DIRS} )

set_target_properties( HZeeImageIO PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON )
