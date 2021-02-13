#ifndef IMAGEIO_ITKBRIDGE_H
#define IMAGEIO_ITKBRIDGE_H

#include "ImageHeader.h"
#include "itkdetails/ImageIOInfo.hpp"

#include <boost/optional.hpp>

#include <functional>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>


namespace itkdetails
{
namespace io
{
struct ImageIoInfo;
}

namespace utility
{
template< class PixelType >
struct PixelStatistics;
}
}


namespace imageio
{

namespace itkbridge
{

extern const std::unordered_map< std::type_index, ComponentType > k_hzeeComponentTypeMap;

extern const std::unordered_map< ComponentType, uint32_t > k_bytesPerComponentMap;

extern const std::unordered_map< ComponentType, std::string > k_componentStringMap;

boost::optional< ::imageio::ComponentType >
fromITKComponentType( const ::itk::ImageIOBase::IOComponentType& componentType );

bool createImageHeader(
        const ::itkdetails::io::ImageIoInfo& itkImageIOInfo,
        const std::function< ComponentType ( ComponentType ) >& componentTypeCaster,
        ImageHeader& header );

bool createImageHeader(
        const ::itkdetails::io::ImageIoInfo& itkImageIOInfo,
        const ComponentType& fileComponentType,
        const ComponentType& bufferComponentType,
        ImageHeader& header );

boost::optional<ComponentType> sniffComponentType( const char* fileName );

} // namespace itkbridge

} // namespace imageio

#endif // IMAGEIO_ITKBRIDGE_H
