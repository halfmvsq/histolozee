#include "common/UID.h"

#include <boost/functional/hash.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>


namespace
{

/// @note equivalent to boost::uuids::basic_random_generator<boost::mt19937>:
static boost::uuids::random_generator s_randomGen;
static boost::uuids::string_generator s_stringGen;

} // anonymous


struct UID::Impl
{
    Impl() : m_data( s_randomGen() ) {}
    Impl( const std::string& s ) : m_data( s_stringGen(s) ) {}

    boost::uuids::uuid m_data;
};


UID::UID()
    : m_impl( std::make_unique<Impl>() )
{}

UID::UID( const std::string& s )
    : m_impl( std::make_shared<Impl>( s ) )
{}

bool UID::operator!= ( const UID& other ) const
{
    return ( m_impl->m_data != other.m_impl->m_data );
}

bool UID::operator== ( const UID& other ) const
{
    return ( m_impl->m_data == other.m_impl->m_data );
}

bool UID::operator< ( const UID& other ) const
{
    return ( m_impl->m_data < other.m_impl->m_data );
}

std::string UID::to_string() const
{
    return boost::uuids::to_string( m_impl->m_data );
}

std::size_t UID::hash() const
{
    return boost::hash< boost::uuids::uuid >()( m_impl->m_data );
}

std::ostream& operator<< ( std::ostream& stream, const UID& uid )
{
    stream << uid.to_string().c_str();
    return stream;
}

std::istream& operator>> ( std::istream& stream, UID& uid )
{
    std::string s;
    stream >> s;
    uid = UID( s );
    return stream;
}
