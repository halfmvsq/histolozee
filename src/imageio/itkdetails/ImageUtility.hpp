#pragma once

#include "itkdetails/ImageTypes.hpp"

#include <itkGDCMSeriesFileNames.h>
#include <itkImage.h>
#include <itkImageIOFactory.h>
#include <itkImageToHistogramFilter.h>
#include <itkImageToVTKImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkStatisticsImageFilter.h>
#include <itkVectorImage.h>

#include <vtkSmartPointer.h>

#include <array>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>


template< class T, uint32_t NumRows, uint32_t NumCols >
class vnl_matrix_fixed;


namespace itkdetails
{

namespace utility
{

template<typename T, typename U>
struct is_same : std::false_type { };

template<typename T>
struct is_same<T, T> : std::true_type { };

template<typename T, typename U>
constexpr bool equalTypes()
{
    return is_same<T, U>::value;
}


enum class ImageFileType
{
    SingleImage, //!< Any image type supported by itk::ImageIOBase
    DICOMSeries, //!< DICOM image series supported by GDCM
    Undefined
};


template< class PixelType >
struct PixelStatistics
{
    PixelType m_minimum;
    PixelType m_maximum;

    double m_mean;
    double m_stdDeviation;
    double m_variance;
    double m_sum;

    std::vector< double > m_histogram;
    std::array< double, 101 > m_quantiles;
};


/**
 * @brief Get the closest canonical anatomical "SPIRAL" orientation code for a 3x3 direction cosine matrix,
 * in which the world coordinate space is assumed to follow the LPS orientation convention.
 *
 * @param matrix 3x3 direction cosine matrix that transforms from voxel to world space coordinates,
 * where the world space axes are assumed to be oriented along with respect to the anatomical
 * directions as follows:
 * -ve X to +ve X :    right (R) to      left (L)
 * -ve Y to +ve Y : anterior (A) to posterior (P)
 * -ve Z to +ve Z : inferior (I) to  superior (S)
 *
 * @return Pair consisting of the closest 3-character "SPIRAL" orientation code (first element)
 * and a flag indicating whether the orientation is oblique (second element)
 */
std::pair< std::string, bool >
getSpiralCodeFromDirectionMatrix( const vnl_matrix_fixed< double, 3, 3 >& matrix );


template< class ImageType >
PixelStatistics< typename ImageType::PixelType >
computeImagePixelStatistics( const typename ImageType::Pointer image )
{
    using StatisticsFilterType = ::itk::StatisticsImageFilter< ImageType >;
    using ImageToHistogramFilterType = itk::Statistics::ImageToHistogramFilter< ImageType >;
    using HistogramType = typename ImageToHistogramFilterType::HistogramType;

    typename StatisticsFilterType::Pointer statsImageFilter = StatisticsFilterType::New();

    statsImageFilter->SetInput( image );
    statsImageFilter->Update();


    static constexpr size_t sk_numComponents = 1;
    static constexpr size_t sk_numBins = 101;

    typename HistogramType::SizeType size( sk_numComponents );
    size.Fill( sk_numBins );

    typename ImageToHistogramFilterType::Pointer histogramFilter = ImageToHistogramFilterType::New();

    // Note: this is another way of setting the min and max histogram bounds:
    // using MeasType = typename ImageToHistogramFilterType::HistogramType::MeasurementVectorType;
    // MeasType lowerBound( sk_numBins );
    // MeasType upperBound( sk_numBins );
    // lowerBound.Fill( statsImageFilter->GetMinimum() );
    // lowerBound.Fill( statsImageFilter->GetMaximum() );
    // histogramFilter->SetHistogramBinMinimum( lowerBound );
    // histogramFilter->SetHistogramBinMaximum( upperBound );

    histogramFilter->SetInput( image );
    histogramFilter->SetAutoMinimumMaximum( true );
    histogramFilter->SetHistogramSize( size );
    histogramFilter->Update();


    PixelStatistics< typename ImageType::PixelType > stats;
    stats.m_minimum = statsImageFilter->GetMinimum();
    stats.m_maximum = statsImageFilter->GetMaximum();
    stats.m_mean = statsImageFilter->GetMean();
    stats.m_stdDeviation = statsImageFilter->GetSigma();
    stats.m_variance = statsImageFilter->GetVariance();
    stats.m_sum = statsImageFilter->GetSum();


    HistogramType* histogram = histogramFilter->GetOutput();

    typename HistogramType::ConstIterator itr = histogram->Begin();
    const typename HistogramType::ConstIterator end = histogram->End();

    stats.m_histogram.reserve( sk_numBins );

    while ( itr != end )
    {
        stats.m_histogram.push_back( itr.GetFrequency() );
        ++itr;
    }

    for ( size_t i = 0; i < sk_numBins; ++i )
    {
        stats.m_quantiles[i] = histogram->Quantile( 0, i / 100.0 );
    }

    return stats;
}


/// @todo Should these go in a different file? ImageIO?
namespace dicom
{

using NameGeneratorType = ::itk::GDCMSeriesFileNames;

std::pair< std::vector< std::string >, NameGeneratorType::Pointer >
seriesSearch( const char* directory );

itk::ImageIOBase::Pointer
createDicomImageIO( const char* firstFileName );

} // namespace dicom


itk::ImageIOBase::Pointer
createStandardImageIO( const char* fileName );


ImageFileType getImageFileType( const char* path );


template< class ComponentType,
          uint32_t NDim >
typename ::itk::Image< ComponentType, NDim >::Pointer
downcastImageBaseToImage( const typename ::itk::ImageBase< NDim >::Pointer& imageBase )
{
    typename ::itk::Image< ComponentType, NDim >::Pointer child =
            dynamic_cast< ::itk::Image< ComponentType, NDim > * >( imageBase.GetPointer() );

    if ( nullptr == child.GetPointer() || child.IsNull() )
    {
        std::cerr << "Unable to downcast ImageBase to Image with component type "
                  << typeid( ComponentType ).name() << std::endl;

        return nullptr;
    }

    return child;
}


template< class ComponentType, uint32_t NDim >
typename ::itk::VectorImage< ComponentType, NDim >::Pointer
downcastImageBaseToVectorImage( const typename ::itk::ImageBase< NDim >::Pointer& imageBase )
{
    typename ::itk::VectorImage< ComponentType, NDim >::Pointer child =
            dynamic_cast< ::itk::VectorImage< ComponentType, NDim > * >( imageBase.GetPointer() );

    if ( nullptr == child.GetPointer() || child.IsNull() )
    {
        std::cerr << "Unable to downcast ImageBase to VectorImage with component type "
                  << typeid( ComponentType ).name() << std::endl;

        return nullptr;
    }

    return child;
}


template< class ComponentType >
vtkSmartPointer< vtkImageData >
convertITKImageToVTKImageData(
        const typename ::itk::Image< ComponentType, 3 >::Pointer image )
{
    if ( image.IsNull() )
    {
        return nullptr;
    }

    using ConversionFilterType = ::itk::ImageToVTKImageFilter<
    ::itk::Image< ComponentType, 3 > >;

    typename ConversionFilterType::Pointer conversionFilter =
            ConversionFilterType::New();

    conversionFilter->SetInput( image );

    try
    {
        conversionFilter->Update();
    }
    catch ( const ::itk::ExceptionObject& e )
    {
        std::cerr << "Exception while converting ITK image to VTK image: "
                  << e.what() << std::endl;

        return nullptr;
    }
    catch ( ... )
    {
        std::cerr << "Exception while converting ITK image to VTK image." << std::endl;

        return nullptr;
    }

    vtkSmartPointer< vtkImageData > imageData =
            static_cast< vtkImageData* >( conversionFilter->GetOutput() );

    return imageData;
}


template< typename ComponentType >
bool rescaleIntensities(
        image3d::ImageBaseType::Pointer& imageBase,
        double outputMinValue,
        double outputMaxValue )
{
    if ( ! utility::equalTypes< ComponentType, float >() &&
         ! utility::equalTypes< ComponentType, double >() )
    {
        return false;
    }

    using ImageType = image3d::ImageType< ComponentType >;

    using RescaleFilterType = itk::RescaleIntensityImageFilter< ImageType, ImageType >;

    typename ImageType::Pointer image =
            utility::downcastImageBaseToImage< ComponentType, 3 >( imageBase );

    if ( image.IsNull() )
    {
        return false;
    }

    typename RescaleFilterType::Pointer rescaleFilter = RescaleFilterType::New();
    rescaleFilter->SetInput( image );
    rescaleFilter->SetOutputMinimum( outputMinValue );
    rescaleFilter->SetOutputMaximum( outputMaxValue );
    rescaleFilter->Update();

    imageBase = rescaleFilter->GetOutput();
    imageBase->DisconnectPipeline();

    return true;
}

} // namespace utility

} // namespace itkdetails


std::ostream& operator<< ( std::ostream&, const ::itkdetails::utility::PixelStatistics<double>& );
