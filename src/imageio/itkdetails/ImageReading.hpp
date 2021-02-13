#pragma once

#include "itkdetails/ImageIOInfo.hpp"
#include "itkdetails/ImageTypes.hpp"

#include <itkImageBase.h>
#include <itkImage.h>
#include <itkVectorImage.h>


/// @brief Namespace for all ITK-specific stuff
namespace itkdetails
{

namespace reader
{

namespace details
{

/**
 * @brief Read the image and cast to desired output type
 *
 * @todo How to deal with 1D, 2D dimensions?
 *
 * @tparam PixelIsVector Boolean indicating whether the image pixels are vectors (true) or not (false)
 * @tparam InputPixelType Pixel type of the input image being read
 * @tparam OutputPixelType Pixel type of the output image being returned
 * @tparam InputDim Number of image dimensions as an unsigned int
 *
 * @param[in] imageIO \c itk::ImageIOBase::Pointer for reading the input image
 *
 * @returns 3D image represented as a pointer to its base class
 */
template< typename OutputComponentType,
          typename InputComponentType,
          bool PixelIsVector,
          uint32_t InputDim >
image3d::ImageBaseType::Pointer
doReadImageFile( const itk::ImageIOBase::Pointer imageIO );


template< typename OutputComponentType,
          typename InputComponentType,
          bool PixelIsVector,
          uint32_t InputDim >
image3d::ImageBaseType::Pointer
doReadImageSeries( const itk::ImageIOBase::Pointer imageIO,
                   const std::vector< std::string >& fileNames );

/**
 * @brief Intermediate loading function that dispatches on image dimensionality
 *
 * @tparam OutputPixel Pixel type of the output image being returned
 *
 * @param[in] imageIO \c itk::ImageIOBase::Pointer for reading the input image
 *
 * @returns 3D image represented as a pointer to its base class
 */
template< typename OutputComponentType,
          typename InputComponentType,
          bool PixelIsVector >
image3d::ImageBaseType::Pointer
readDispatchOnNumDimensions(
        const itk::ImageIOBase::Pointer imageIO,
        const std::vector< std::string >& fileNames =
                std::vector< std::string >{} );


/**
 * @brief Intermediate loading function that dispatches on pixel type
 *
 * @tparam InputPixelType Pixel type of the input image being read
 * @tparam OutputPixelType Pixel type of the output image being returned
 * @tparam NDim Number of image dimensions as an unsigned int
 *
 * @param[in] imageIO \c itk::ImageIOBase::Pointer for reading the input image
 *
 * @returns 3D image represented as a pointer to its base class
 */
template< typename OutputComponentType,
          typename InputComponentType >
image3d::ImageBaseType::Pointer
readDispatchOnPixelType(
        const itk::ImageIOBase::Pointer imageIO,
        const std::vector< std::string >& fileNames =
                std::vector< std::string >{} );


/**
 * @brief Intermediate loading function that dispatches on pixel component type
 *
 * @tparam OutputPixelType Pixel type of the output image being returned
 * @tparam NDim Number of image dimensions as an unsigned int
 *
 * @param[in] imageIO \c itk::ImageIOBase::Pointer for reading the input image
 *
 * @returns 3D image represented as a pointer to its base class
 */
template< typename OutputComponentType >
image3d::ImageBaseType::Pointer
readDispatchOnComponentType(
        const itk::ImageIOBase::Pointer imageIO,
        const std::vector< std::string >& fileNames =
                std::vector< std::string >{} );

} // namespace details


/**
 * @brief Load image: forwards to dispatchers
 *
 * @tparam OutputPixel Pixel type of the output image being returned
 *
 * @param[in] imageIO \c itk::ImageIOBase::Pointer for reading the input image
 *
 * @returns 3D image represented as a pointer to its base class
 */
template< typename OutputComponentType >
image3d::ImageBaseType::Pointer
read( const itk::ImageIOBase::Pointer imageIO,
      const std::vector< std::string >& fileNames = std::vector< std::string >{} );


template<>
image3d::ImageBaseType::Pointer
read< void >( const itk::ImageIOBase::Pointer imageIO,
              const std::vector< std::string >& fileNames );

} // namespace reader

} // namespace itkdetails


#include "itkdetails/ImageReading.tpp"
