#include "itkbridge/ImageDataFactory.hpp"
#include "itkbridge/ITKBridge.hpp"
#include "itkdetails/ImageData.hpp"


namespace
{

template< typename T >
imageio::ComponentType toHZeeType()
{
    return imageio::itkbridge::k_hzeeComponentTypeMap.at( std::type_index( typeid( T ) ) );
}

} // anonymous


namespace imageio
{

/**
 * @brief Register all known pixel component types with the \c ImageData<T> factory.
 * @todo There is probably some variadic template magic that could make this code more elegant.
 */
ImageDataFactory::ImageDataFactory( const ComponentTypeCastPolicy& policy )
{
    switch ( policy )
    {
    case ComponentTypeCastPolicy::Identity :
    {
        m_factory.registerType< ::itkdetails::ImageData<   int8_t > >( toHZeeType<int8_t>() );
        m_factory.registerType< ::itkdetails::ImageData<  uint8_t > >( toHZeeType<uint8_t>() );
        m_factory.registerType< ::itkdetails::ImageData<  int16_t > >( toHZeeType<int16_t>() );
        m_factory.registerType< ::itkdetails::ImageData< uint16_t > >( toHZeeType<uint16_t>() );
        m_factory.registerType< ::itkdetails::ImageData<  int32_t > >( toHZeeType<int32_t>() );
        m_factory.registerType< ::itkdetails::ImageData< uint32_t > >( toHZeeType<uint32_t>() );
        m_factory.registerType< ::itkdetails::ImageData<  int64_t > >( toHZeeType<int64_t>() );
        m_factory.registerType< ::itkdetails::ImageData< uint64_t > >( toHZeeType<uint64_t>() );
        m_factory.registerType< ::itkdetails::ImageData<    float > >( toHZeeType<float>() );
        m_factory.registerType< ::itkdetails::ImageData<   double > >( toHZeeType<double>() );

        m_componentTypeCastMap[     ComponentType::Int8 ] = ComponentType::Int8;
        m_componentTypeCastMap[    ComponentType::UInt8 ] = ComponentType::UInt8;
        m_componentTypeCastMap[    ComponentType::Int16 ] = ComponentType::Int16;
        m_componentTypeCastMap[   ComponentType::UInt16 ] = ComponentType::UInt16;
        m_componentTypeCastMap[    ComponentType::Int32 ] = ComponentType::Int32;
        m_componentTypeCastMap[   ComponentType::UInt32 ] = ComponentType::UInt32;
        m_componentTypeCastMap[    ComponentType::Int64 ] = ComponentType::Int64;
        m_componentTypeCastMap[   ComponentType::UInt64 ] = ComponentType::UInt64;
        m_componentTypeCastMap[  ComponentType::Float32 ] = ComponentType::Float32;
        m_componentTypeCastMap[ ComponentType::Double64 ] = ComponentType::Double64;

        break;
    }
    case ComponentTypeCastPolicy::ToFloat32 :
    {
        m_factory.registerType< ::itkdetails::ImageData< float > >( toHZeeType<int8_t>() );
        m_factory.registerType< ::itkdetails::ImageData< float > >( toHZeeType<uint8_t>() );
        m_factory.registerType< ::itkdetails::ImageData< float > >( toHZeeType<int16_t>() );
        m_factory.registerType< ::itkdetails::ImageData< float > >( toHZeeType<uint16_t>() );
        m_factory.registerType< ::itkdetails::ImageData< float > >( toHZeeType<int32_t>() );
        m_factory.registerType< ::itkdetails::ImageData< float > >( toHZeeType<uint32_t>() );
        m_factory.registerType< ::itkdetails::ImageData< float > >( toHZeeType<int64_t>() );
        m_factory.registerType< ::itkdetails::ImageData< float > >( toHZeeType<uint64_t>() );
        m_factory.registerType< ::itkdetails::ImageData< float > >( toHZeeType<float>() );
        m_factory.registerType< ::itkdetails::ImageData< float > >( toHZeeType<double>() );

        m_componentTypeCastMap[     ComponentType::Int8 ] = ComponentType::Float32;
        m_componentTypeCastMap[    ComponentType::UInt8 ] = ComponentType::Float32;
        m_componentTypeCastMap[    ComponentType::Int16 ] = ComponentType::Float32;
        m_componentTypeCastMap[   ComponentType::UInt16 ] = ComponentType::Float32;
        m_componentTypeCastMap[    ComponentType::Int32 ] = ComponentType::Float32;
        m_componentTypeCastMap[   ComponentType::UInt32 ] = ComponentType::Float32;
        m_componentTypeCastMap[    ComponentType::Int64 ] = ComponentType::Float32;
        m_componentTypeCastMap[   ComponentType::UInt64 ] = ComponentType::Float32;
        m_componentTypeCastMap[  ComponentType::Float32 ] = ComponentType::Float32;
        m_componentTypeCastMap[ ComponentType::Double64 ] = ComponentType::Float32;

        break;
    }
    case ComponentTypeCastPolicy::ToOpenGLCompatible :
    {
        // OpenGL 3.3 has incomplete support for 8-byte integer and floating-point textures,
        // so (u)int64_t and double components are cast to float.
        m_factory.registerType< ::itkdetails::ImageData<   int8_t > >( toHZeeType<int8_t>() );
        m_factory.registerType< ::itkdetails::ImageData<  uint8_t > >( toHZeeType<uint8_t>() );
        m_factory.registerType< ::itkdetails::ImageData<  int16_t > >( toHZeeType<int16_t>() );
        m_factory.registerType< ::itkdetails::ImageData< uint16_t > >( toHZeeType<uint16_t>() );
        m_factory.registerType< ::itkdetails::ImageData<  int32_t > >( toHZeeType<int32_t>() );
        m_factory.registerType< ::itkdetails::ImageData< uint32_t > >( toHZeeType<uint32_t>() );
        m_factory.registerType< ::itkdetails::ImageData<    float > >( toHZeeType<int64_t>() );
        m_factory.registerType< ::itkdetails::ImageData<    float > >( toHZeeType<uint64_t>() );
        m_factory.registerType< ::itkdetails::ImageData<    float > >( toHZeeType<float>() );
        m_factory.registerType< ::itkdetails::ImageData<    float > >( toHZeeType<double>() );

        m_componentTypeCastMap[     ComponentType::Int8 ] = ComponentType::Int8;
        m_componentTypeCastMap[    ComponentType::UInt8 ] = ComponentType::UInt8;
        m_componentTypeCastMap[    ComponentType::Int16 ] = ComponentType::Int16;
        m_componentTypeCastMap[   ComponentType::UInt16 ] = ComponentType::UInt16;
        m_componentTypeCastMap[    ComponentType::Int32 ] = ComponentType::Int32;
        m_componentTypeCastMap[   ComponentType::UInt32 ] = ComponentType::UInt32;
        m_componentTypeCastMap[    ComponentType::Int64 ] = ComponentType::Float32;
        m_componentTypeCastMap[   ComponentType::UInt64 ] = ComponentType::Float32;
        m_componentTypeCastMap[  ComponentType::Float32 ] = ComponentType::Float32;
        m_componentTypeCastMap[ ComponentType::Double64 ] = ComponentType::Float32;

        break;
    }
    case ComponentTypeCastPolicy::ToOpenGLCompatibleUInt :
    {
        // OpenGL 3.3 has incomplete support for 8-byte integer textures.
        m_factory.registerType< ::itkdetails::ImageData< uint16_t > >( toHZeeType<int8_t>() );
        m_factory.registerType< ::itkdetails::ImageData<  uint8_t > >( toHZeeType<uint8_t>() );
        m_factory.registerType< ::itkdetails::ImageData< uint32_t > >( toHZeeType<int16_t>() );
        m_factory.registerType< ::itkdetails::ImageData< uint16_t > >( toHZeeType<uint16_t>() );
        m_factory.registerType< ::itkdetails::ImageData< uint32_t > >( toHZeeType<int32_t>() );
        m_factory.registerType< ::itkdetails::ImageData< uint32_t > >( toHZeeType<uint32_t>() );
        m_factory.registerType< ::itkdetails::ImageData< uint32_t > >( toHZeeType<int64_t>() );
        m_factory.registerType< ::itkdetails::ImageData< uint32_t > >( toHZeeType<uint64_t>() );
        m_factory.registerType< ::itkdetails::ImageData< uint32_t > >( toHZeeType<float>() );
        m_factory.registerType< ::itkdetails::ImageData< uint32_t > >( toHZeeType<double>() );

        m_componentTypeCastMap[     ComponentType::Int8 ] = ComponentType::UInt16;
        m_componentTypeCastMap[    ComponentType::UInt8 ] = ComponentType::UInt8;
        m_componentTypeCastMap[    ComponentType::Int16 ] = ComponentType::UInt32;
        m_componentTypeCastMap[   ComponentType::UInt16 ] = ComponentType::UInt16;
        m_componentTypeCastMap[    ComponentType::Int32 ] = ComponentType::UInt32;
        m_componentTypeCastMap[   ComponentType::UInt32 ] = ComponentType::UInt32;
        m_componentTypeCastMap[    ComponentType::Int64 ] = ComponentType::UInt32;
        m_componentTypeCastMap[   ComponentType::UInt64 ] = ComponentType::UInt32;
        m_componentTypeCastMap[  ComponentType::Float32 ] = ComponentType::UInt32;
        m_componentTypeCastMap[ ComponentType::Double64 ] = ComponentType::UInt32;

        break;
    }
    }

    m_factory.registerIdentityType< ::itkdetails::ImageData<   int8_t > >( toHZeeType<int8_t>() );
    m_factory.registerIdentityType< ::itkdetails::ImageData<  uint8_t > >( toHZeeType<uint8_t>() );
    m_factory.registerIdentityType< ::itkdetails::ImageData<  int16_t > >( toHZeeType<int16_t>() );
    m_factory.registerIdentityType< ::itkdetails::ImageData< uint16_t > >( toHZeeType<uint16_t>() );
    m_factory.registerIdentityType< ::itkdetails::ImageData<  int32_t > >( toHZeeType<int32_t>() );
    m_factory.registerIdentityType< ::itkdetails::ImageData< uint32_t > >( toHZeeType<uint32_t>() );
    m_factory.registerIdentityType< ::itkdetails::ImageData<  int64_t > >( toHZeeType<int64_t>() );
    m_factory.registerIdentityType< ::itkdetails::ImageData< uint64_t > >( toHZeeType<uint64_t>() );
    m_factory.registerIdentityType< ::itkdetails::ImageData<    float > >( toHZeeType<float>() );
    m_factory.registerIdentityType< ::itkdetails::ImageData<   double > >( toHZeeType<double>() );
}


std::unique_ptr< ::itkdetails::ImageBaseData >
ImageDataFactory::createImageData(
        const ComponentType& componentType, bool forceIdentityCast ) const
{
    return m_factory.create( componentType, forceIdentityCast );
}

ComponentType ImageDataFactory::getComponentTypeCast(
        const ComponentType& inputComponentType ) const
{
    return m_componentTypeCastMap.at( inputComponentType );
}

} // namespace imageio
