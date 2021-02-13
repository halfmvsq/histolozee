#ifndef FACTORY_H
#define FACTORY_H

#include <functional>
#include <memory>
#include <unordered_map>


/**
 * @brief
 *
 * @tparam TBase Base class type to return
 * @tparam KeyType Key type
 */
template< typename TBase, typename KeyType >
class Factory
{
public:

    template< typename TDerived >
    void registerType( const KeyType& name )
    {
        static_assert( std::is_base_of< TBase, TDerived >::value,
                       "Type must derive from base class." );

        m_creationFunctions[name] = [] ()
        {
            return std::make_unique< TDerived >();
        };
    }

    template< typename TDerived >
    void registerIdentityType( const KeyType& name )
    {
        static_assert( std::is_base_of< TBase, TDerived >::value,
                       "Type must derive from base class." );

        m_identityCreationFunctions[name] = [] ()
        {
            return std::make_unique< TDerived >();
        };
    }

    std::unique_ptr<TBase> create( const KeyType& name, bool forceIdentity ) const
    {
        if ( ! forceIdentity )
        {
            const auto itr = m_creationFunctions.find( name );
            if ( std::end( m_creationFunctions ) != itr )
            {
                return itr->second();
            }
        }
        else
        {
            // User has chosen to force identity casting of components:
            const auto itr = m_identityCreationFunctions.find( name );
            if ( std::end( m_identityCreationFunctions ) != itr )
            {
                return itr->second();
            }
        }

        return nullptr;
    }


private:

    /**
     * @note Need for a hash function for enum classes is a defect in the standard that was
     * fixed in C++14
     *
     * @see http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-defects.html#2148
     */

    std::unordered_map< KeyType, std::function< std::unique_ptr<TBase>( void ) > >
    m_creationFunctions;

    std::unordered_map< KeyType, std::function< std::unique_ptr<TBase>( void ) > >
    m_identityCreationFunctions;
};

#endif // FACTORY_H
