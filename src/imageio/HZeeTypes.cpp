#include "HZeeTypes.hpp"

namespace imageio
{

bool isIntegerType( const ComponentType& type )
{
    return ( ! isFloatingType( type ) );
}

bool isFloatingType( const ComponentType& type )
{
    if ( imageio::ComponentType::Float32 == type ||
         imageio::ComponentType::Double64 == type )
    {
        return true;
    }

    return false;
}

bool isSignedIntegerType( const ComponentType& type )
{
    if ( imageio::ComponentType::Int8 == type ||
         imageio::ComponentType::Int16 == type ||
         imageio::ComponentType::Int32 == type ||
         imageio::ComponentType::Int64 == type )
    {
        return true;
    }

    return false;
}

bool isUnsignedIntegerType( const ComponentType& type )
{
    return ( isIntegerType( type ) && ! isSignedIntegerType( type ) );
}

} // namespace imageio
