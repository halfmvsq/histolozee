#include "itkdetails/ImageBaseData.hpp"
#include "itkdetails/ImageReading.hpp"
#include "itkdetails/ImageUtility.hpp"

#include <itkContinuousIndex.h>
#include <itkIndex.h>
#include <itkPoint.h>


namespace itkdetails
{

ImageBaseData::ImageBaseData()
    :
      m_imageIOInfo(),
      m_pixelStatistics(),
      m_imageBasePtr( ITK_NULLPTR )
{}

ImageBaseData::ImageBaseData( io::ImageIoInfo ioInfo )
    :
      m_imageIOInfo( std::move( ioInfo ) ),
      m_pixelStatistics(),
      m_imageBasePtr( ITK_NULLPTR )
{}

ImageBaseData::statistics_range_t
ImageBaseData::pixelStatistics() const
{
    return m_pixelStatistics;
}

boost::optional< utility::PixelStatistics<double> >
ImageBaseData::pixelStatistics( uint32_t componentIndex ) const
{
    if ( componentIndex < m_pixelStatistics.size() )
    {
        return m_pixelStatistics.at( componentIndex );
    }
    return boost::none;
}

const io::ImageIoInfo& ImageBaseData::imageIOInfo() const
{
    return m_imageIOInfo;
}


bool ImageBaseData::isVectorImage() const
{
    return ( m_imageBasePtr.IsNotNull() )
            ? ( m_imageBasePtr->GetNumberOfComponentsPerPixel() > 1 )
            : false;
}


image3d::ImageBaseType::Pointer
ImageBaseData::imageBase() const
{
    return m_imageBasePtr;
}


typename image3d::ImageRegionType::SizeValueType
ImageBaseData::numPixels() const
{
    return ( m_imageBasePtr.IsNotNull() )
            ? m_imageBasePtr->GetBufferedRegion().GetNumberOfPixels()
            : 0u;
}


bool ImageBaseData::isFullyBuffered() const
{
    return ( m_imageBasePtr.IsNotNull() )
            ? ( m_imageBasePtr->GetBufferedRegion() ==
                m_imageBasePtr->GetLargestPossibleRegion() )
            : false;
}


bool ImageBaseData::transformPhysicalPointToIndex(
        const image3d::PointType& point,
        image3d::IndexType& index ) const
{
    return ( m_imageBasePtr.IsNotNull() )
            ? m_imageBasePtr->TransformPhysicalPointToIndex( point, index )
            : false;
}


bool ImageBaseData::transformPhysicalPointToContinuousIndex(
        const image3d::PointType& point,
        image3d::ContinuousIndexType& index ) const
{
    return ( m_imageBasePtr.IsNotNull() )
            ? m_imageBasePtr->TransformPhysicalPointToContinuousIndex( point, index )
            : false;
}


bool ImageBaseData::transformIndexToPhysicalPoint(
        const image3d::IndexType& index,
        image3d::PointType& point ) const
{
    if ( m_imageBasePtr.IsNull() )
    {
        return false;
    }

    m_imageBasePtr->TransformIndexToPhysicalPoint( index, point );
    return true;
}


bool ImageBaseData::transformContinuousIndexToPhysicalPoint(
        const image3d::ContinuousIndexType& index,
        image3d::PointType& point ) const
{
    if ( m_imageBasePtr.IsNull() )
    {
        return false;
    }

    m_imageBasePtr->TransformContinuousIndexToPhysicalPoint( index, point );
    return true;
}

} // namespace itkdetails
