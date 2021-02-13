#ifndef IMAGE_DATA_FACTORY_H
#define IMAGE_DATA_FACTORY_H

#include "HZeeTypes.hpp"
#include "itkdetails/ImageBaseData.hpp"
#include "util/Factory.hpp"

#include <memory>
#include <unordered_map>


namespace imageio
{

class ImageDataFactory
{
public:

    ImageDataFactory( const ComponentTypeCastPolicy& policy =
            ComponentTypeCastPolicy::Identity );

    ImageDataFactory( const ImageDataFactory& ) = default;
    ImageDataFactory& operator=( const ImageDataFactory& ) = default;

    ImageDataFactory( ImageDataFactory&& ) = default;
    ImageDataFactory& operator=( ImageDataFactory&& ) = default;

    ~ImageDataFactory() = default;

    std::unique_ptr< ::itkdetails::ImageBaseData >
    createImageData( const ComponentType& componentType, bool forceIdentityCast ) const;

    ComponentType getComponentTypeCast( const ComponentType& inputComponentType ) const;


private:

    /// Factory to create dervied \c ::itkdetails::ImageData<T> classes of
    /// \c ::itkdetails::ImageBaseData.
    Factory< ::itkdetails::ImageBaseData, ComponentType > m_factory;

    std::unordered_map< ComponentType, ComponentType > m_componentTypeCastMap;
};

} // namespace imageio

#endif // IMAGE_DATA_FACTORY_H
