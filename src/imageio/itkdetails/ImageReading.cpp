#include "itkdetails/ImageReading.hpp"


namespace itkdetails
{

namespace reader
{

/**
 * Template specialization for void:
 * Output component type is to be the same as the input component type.
 */
template<>
image3d::ImageBaseType::Pointer
read< void >( const itk::ImageIOBase::Pointer imageIO,
              const std::vector< std::string >& fileNames )
{
    using IOB = ::itk::ImageIOBase;

    try
    {
        switch ( const IOB::IOComponentType componentType = imageIO->GetComponentType() )
        {
        case IOB::CHAR:   return details::readDispatchOnComponentType<   int8_t >( imageIO, fileNames );
        case IOB::UCHAR:  return details::readDispatchOnComponentType<  uint8_t >( imageIO, fileNames );
        case IOB::SHORT:  return details::readDispatchOnComponentType<  int16_t >( imageIO, fileNames );
        case IOB::USHORT: return details::readDispatchOnComponentType< uint16_t >( imageIO, fileNames );
        case IOB::INT:    return details::readDispatchOnComponentType<  int32_t >( imageIO, fileNames );
        case IOB::UINT:   return details::readDispatchOnComponentType< uint32_t >( imageIO, fileNames );
        case IOB::LONG:   return details::readDispatchOnComponentType<  int64_t >( imageIO, fileNames );
        case IOB::ULONG:  return details::readDispatchOnComponentType< uint64_t >( imageIO, fileNames );
        case IOB::FLOAT:  return details::readDispatchOnComponentType<    float >( imageIO, fileNames );
        case IOB::DOUBLE: return details::readDispatchOnComponentType<   double >( imageIO, fileNames );

        case IOB::UNKNOWNCOMPONENTTYPE:
        {
            std::cerr << "Unknown and unsupported component type: "
                      << IOB::GetComponentTypeAsString( componentType ) << std::endl;

            return ITK_NULLPTR;
        }
        }
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
