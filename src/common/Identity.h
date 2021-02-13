#ifndef IDENTITY_H
#define IDENTITY_H

#include <utility>


/**
 * @brief Template class defining an identity wrapper around any object.
 * Its members are self-explanatory.
 */
template< typename T >
class Identity
{
public:

    Identity() = default;
    ~Identity() = default;

    Identity( T x ) : m_x( std::move( x ) ) {}

    Identity( const Identity<T>& ) = default;
    Identity( Identity<T>&& ) = default;

    Identity& operator= ( const Identity<T>& ) = default;
    Identity& operator= ( Identity<T>&& ) = default;

    const T& operator() () const noexcept { return m_x; }
    T& operator() () noexcept { return m_x; }


private:

    T m_x;
};


/**
 * @brief Alias for a required type.
 */
template< typename T >
using Required = Identity<T>;

#endif // IDENTITY_H
