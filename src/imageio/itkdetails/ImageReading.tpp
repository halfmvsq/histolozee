#include "itkdetails/ImageTypes.hpp" // for IDE

#include <itkCastImageFilter.h>
#include <itkGDCMImageIO.h>
#include <itkImageFileReader.h>
#include <itkImageSeriesReader.h>

#include <iostream>


namespace
{

bool isDicomIO( const ::itk::ImageIOBase* imageIO )
{
    if ( nullptr == imageIO )
    {
        return false;
    }

    return ( typeid( ::itk::GDCMImageIO ) == typeid( *imageIO ) );
}

} // namespace anonymous


namespace itkdetails
{

namespace reader
{

namespace details
{

template< typename OutputComponentType,
          typename InputComponentType,
          bool PixelIsVector,
          uint32_t InputDim >
image3d::ImageBaseType::Pointer
doReadImageFile( const ::itk::ImageIOBase::Pointer imageIO )
{
    using InputImageType = typename std::conditional< PixelIsVector,
    ::itk::VectorImage< InputComponentType, InputDim >,
    ::itk::Image< InputComponentType, InputDim > >::type;

    using OutputImageType = typename std::conditional< PixelIsVector,
    ::itk::VectorImage< OutputComponentType, image3d::NDIM >,
    ::itk::Image< OutputComponentType, image3d::NDIM > >::type;

    using ImageReaderType = ::itk::ImageFileReader< InputImageType >;
    using CastFilterType = ::itk::CastImageFilter< InputImageType, OutputImageType >;

    typename ImageReaderType::Pointer imageReader = ImageReaderType::New();
    typename CastFilterType::Pointer castFilter = CastFilterType::New();

    imageReader->SetImageIO( imageIO );
    imageReader->SetFileName( imageIO->GetFileName() );

    castFilter->SetInput( imageReader->GetOutput() );

    try
    {
        castFilter->UpdateLargestPossibleRegion();
    }
    catch ( const ::itk::ExceptionObject& e )
    {
        std::cerr << "Reading of image '" << imageIO->GetFileName() << "' failed: "
                  << e.what() << std::endl;
        return ITK_NULLPTR;
    }

    /// @note Cast image to its base pointer type
    return static_cast< image3d::ImageBaseType::Pointer >( castFilter->GetOutput() );
}

template< typename OutputComponentType,
          typename InputComponentType,
          bool PixelIsVector,
          uint32_t InputDim >
image3d::ImageBaseType::Pointer
doReadImageSeries(
        const ::itk::ImageIOBase::Pointer imageIO,
        const std::vector< std::string >& fileNames )
{
    using InputImageType = typename std::conditional< PixelIsVector,
    ::itk::VectorImage< InputComponentType, InputDim >,
    ::itk::Image< InputComponentType, InputDim > >::type;

    using OutputImageType = typename std::conditional< PixelIsVector,
    ::itk::VectorImage< OutputComponentType, image3d::NDIM >,
    ::itk::Image< OutputComponentType, image3d::NDIM > >::type;

    using ImageSeriesReaderType = ::itk::ImageSeriesReader< InputImageType >;
    using CastFilterType = ::itk::CastImageFilter< InputImageType, OutputImageType >;

    //    typename InputImageType::Pointer image = InputImageType::New();
    typename ImageSeriesReaderType::Pointer seriesReader = ImageSeriesReaderType::New();
    typename CastFilterType::Pointer castFilter = CastFilterType::New();

    seriesReader->SetImageIO( imageIO );
    seriesReader->SetFileNames( fileNames );

    castFilter->SetInput( seriesReader->GetOutput() );

    try
    {
        castFilter->UpdateLargestPossibleRegion();
    }
    catch ( const ::itk::ExceptionObject& e )
    {
        std::cerr << "Reading of image '" << imageIO->GetFileName() << "' failed: "
                  << e.what() << std::endl;
        return ITK_NULLPTR;
    }

    /// @note Cast image to its base pointer type
    return static_cast< image3d::ImageBaseType::Pointer >( castFilter->GetOutput() );
}


template< typename OutputComponentType,
          typename InputComponentType,
          bool PixelIsVector >
image3d::ImageBaseType::Pointer
readDispatchOnNumDimensions(
        const ::itk::ImageIOBase::Pointer imageIO,
        const std::vector< std::string >& fileNames )
{
    const uint32_t numDimensions = imageIO->GetNumberOfDimensions();

    if ( isDicomIO( imageIO.GetPointer() ) && ! fileNames.empty() )
    {
        switch ( numDimensions )
        {
        case 1: return doReadImageSeries< OutputComponentType, InputComponentType, PixelIsVector, 1 >( imageIO, fileNames );
        case 2: return doReadImageSeries< OutputComponentType, InputComponentType, PixelIsVector, 2 >( imageIO, fileNames );
        case 3: return doReadImageSeries< OutputComponentType, InputComponentType, PixelIsVector, 3 >( imageIO, fileNames );

        default:
        {
            std::cerr << "There is no support for images of dimension " << numDimensions << std::endl;
            return ITK_NULLPTR;
        }
        }
    }
    else
    {
        switch ( numDimensions )
        {
        case 1: return doReadImageFile< OutputComponentType, InputComponentType, PixelIsVector, 1 >( imageIO );
        case 2: return doReadImageFile< OutputComponentType, InputComponentType, PixelIsVector, 2 >( imageIO );
        case 3: return doReadImageFile< OutputComponentType, InputComponentType, PixelIsVector, 3 >( imageIO );

        default:
        {
            std::cerr << "There is no support for images of dimension " << numDimensions << std::endl;
            return ITK_NULLPTR;
        }
        }
    }

    return ITK_NULLPTR;
}


template< typename OutputComponentType,
          typename InputComponentType >
image3d::ImageBaseType::Pointer
readDispatchOnPixelType(
        const ::itk::ImageIOBase::Pointer imageIO,
        const std::vector< std::string >& fileNames )
{
    using IOB = ::itk::ImageIOBase;

    switch ( const IOB::IOPixelType pixelType = imageIO->GetPixelType() )
    {
    case IOB::SCALAR :
    {
        return readDispatchOnNumDimensions<
                OutputComponentType, InputComponentType, false >( imageIO, fileNames );
    }

    case IOB::RGB :
    case IOB::RGBA :
    case IOB::POINT :
    case IOB::VECTOR :
    case IOB::COVARIANTVECTOR :
    case IOB::SYMMETRICSECONDRANKTENSOR :
    case IOB::DIFFUSIONTENSOR3D :
    case IOB::COMPLEX :
    case IOB::FIXEDARRAY :
    case IOB::MATRIX :
    {
        return readDispatchOnNumDimensions<
                OutputComponentType, InputComponentType, true >( imageIO, fileNames );
    }

    case IOB::OFFSET :
    case IOB::UNKNOWNPIXELTYPE :
    default:
    {
        std::cerr << "There is no support for pixel type "
                  << IOB::GetPixelTypeAsString( pixelType ) << std::endl;
        return ITK_NULLPTR;
    }
    }

    return ITK_NULLPTR;
}


template< typename OutputComponentType >
image3d::ImageBaseType::Pointer
readDispatchOnComponentType(
        const ::itk::ImageIOBase::Pointer imageIO,
        const std::vector< std::string >& fileNames )
{
    using IOB = ::itk::ImageIOBase;

    switch ( const IOB::IOComponentType componentType = imageIO->GetComponentType() )
    {
    case IOB::CHAR:   return readDispatchOnPixelType< OutputComponentType,   int8_t >( imageIO, fileNames );
    case IOB::UCHAR:  return readDispatchOnPixelType< OutputComponentType,  uint8_t >( imageIO, fileNames );
    case IOB::SHORT:  return readDispatchOnPixelType< OutputComponentType,  int16_t >( imageIO, fileNames );
    case IOB::USHORT: return readDispatchOnPixelType< OutputComponentType, uint16_t >( imageIO, fileNames );
    case IOB::INT:    return readDispatchOnPixelType< OutputComponentType,  int32_t >( imageIO, fileNames );
    case IOB::UINT:   return readDispatchOnPixelType< OutputComponentType, uint32_t >( imageIO, fileNames );
    case IOB::LONG:   return readDispatchOnPixelType< OutputComponentType,  int64_t >( imageIO, fileNames );
    case IOB::ULONG:  return readDispatchOnPixelType< OutputComponentType, uint64_t >( imageIO, fileNames );
    case IOB::FLOAT:  return readDispatchOnPixelType< OutputComponentType,    float >( imageIO, fileNames );
    case IOB::DOUBLE: return readDispatchOnPixelType< OutputComponentType,   double >( imageIO, fileNames );

    case IOB::UNKNOWNCOMPONENTTYPE:
    default:
    {
        std::cerr << "Unknown and unsupported component type: "
                  << IOB::GetComponentTypeAsString( componentType ) << std::endl;
        return ITK_NULLPTR;
    }
    }

    return ITK_NULLPTR;
}

} // namespace details


template< typename OutputComponentType >
image3d::ImageBaseType::Pointer
read( const ::itk::ImageIOBase::Pointer imageIO,
      const std::vector< std::string >& fileNames )
{
    try
    {
        return details::readDispatchOnComponentType< OutputComponentType >( imageIO, fileNames );
    }
    catch ( const ::itk::ExceptionObject& e )
    {
        std::cerr << "Exception caught while reading image: " << e.what() << std::endl;
        return ITK_NULLPTR;
    }
    catch ( const std::exception& e )
    {
        std::cerr << "Exception caught while reading image: " << e.what() << std::endl;
        return ITK_NULLPTR;
    }
    catch ( ... )
    {
        std::cerr << "Exception caught while reading image." << std::endl;
        return ITK_NULLPTR;
    }
}

} // namespace reader

} // namespace itkdetails
