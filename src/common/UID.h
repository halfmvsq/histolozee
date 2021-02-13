#ifndef UID_H
#define UID_H

#include <iostream>
#include <memory>
#include <string>


/**
 * @brief Wrapper around a RFC4122-compliant Universally Unique IDentifier (UUID)
 * that uses \c boost::uuids internally
 *
 * @note This class is based on SMTK's UUID class
 * @see https://github.com/Kitware/SMTK/blob/master/smtk/common/UUID.h
 * @todo Switch to uuids::uuid for C++17 instead
 */
class UID
{
public:

    /// Construct a UUID from a random seed
    UID();

    /// Construct a UUID from a string seed
    UID( const std::string& );

    UID( const UID& ) = default;
    UID& operator=( const UID& ) = default;

    UID( UID&& ) = default;
    UID& operator=( UID&& ) = default;

    ~UID() = default;

    bool operator!= ( const UID& ) const;
    bool operator== ( const UID& ) const;
    bool operator< ( const UID& ) const;

    std::string to_string() const;
    std::size_t hash() const;

    friend std::ostream& operator<< ( std::ostream&, const UID& uid );
    friend std::istream& operator>> ( std::istream&, UID& uid );


private:

    struct Impl;

    /// @note Implementation is declared as shared pointer so that we get the
    /// copy constructor for UIDs
    std::shared_ptr<Impl> m_impl;
};


namespace std
{

template<>
struct hash< UID >
{
    size_t operator() ( const UID& uid ) const
    {
        return uid.hash();
    }
};

} // namespace std

#endif // UID_H
