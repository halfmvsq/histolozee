#include <nlohmann/json.hpp>

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <boost/optional.hpp>


namespace nlohmann
{

/**
 * Add JSON serializer for \c boost::optional to the \c nlohmann (JSON parser) namespace.
 * The default serializer for \c nlohmann::json is \c nlohmann::adl_serializer
 * (ADL means Argument-Dependent Lookup).
 */
template< typename T >
struct adl_serializer< boost::optional<T> >
{
    static void to_json( json& j, const boost::optional<T>& opt )
    {
        if ( opt == boost::none )
        {
            j = nullptr;
        }
        else
        {
            // Calls adl_serializer<T>::to_json, which will
            // find the free function to_json in T's namespace
            j = *opt;
        }
    }

    static void from_json( const json& j, boost::optional<T>& opt )
    {
        if ( j.is_null() )
        {
            opt = boost::none;
        }
        else
        {
            // Same as above, but with adl_serializer<T>::from_json
            opt = j.get<T>();
        }
    }
};


template <typename T>
struct adl_serializer< glm::vec<3, T> >
{
    static void to_json( json& j, const glm::vec<3, T>& p )
    {
        j = json{
            { "x", p.x },
            { "y", p.y },
            { "z", p.z }
        };
    }

    static void from_json( const json& j, glm::vec<3, T>& p )
    {
        p.x = j.at( "x" ).get<T>();
        p.y = j.at( "y" ).get<T>();
        p.z = j.at( "z" ).get<T>();
    }
};


template <typename T>
struct adl_serializer< glm::qua<T> >
{
    static void to_json( json& j, const glm::qua<T>& q )
    {
        j = json{
            { "w", q.w },
            { "x", q.x },
            { "y", q.y },
            { "z", q.z }
        };
    }

    static void from_json( const json& j, glm::qua<T>& q )
    {
        q.w = j.at( "w" ).get<T>();
        q.x = j.at( "x" ).get<T>();
        q.y = j.at( "y" ).get<T>();
        q.z = j.at( "z" ).get<T>();
    }
};

} // namespace nlohmann
