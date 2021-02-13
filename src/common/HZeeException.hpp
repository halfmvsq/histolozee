#ifndef HZEE_EXCEPTION
#define HZEE_EXCEPTION

#include <sstream>
#include <stdexcept>

/**
 * @brief A friendly wrapper around \c std::runtime_error that prints the file name,
 * function name, and line number on which the exception occurred to stdout.
 * The C-style definition \c throw_debug is to be used by clients of this class.
 */
class HZeeException : public std::runtime_error
{
public:

    HZeeException( const char* msg, const char* file, const char* function, int line )
        : std::runtime_error( msg )
    {
        std::ostringstream ss;
        ss << msg << " [in " << function << "; file '" << file << "' : " << line << "]";
        m_msg = ss.str();
    }

    HZeeException( const std::string& msg, const char* file, const char* function, int line )
        : HZeeException( msg.c_str(), file, function, line )
    {}

    virtual ~HZeeException() = default;

    const char* what() const noexcept
    {
        return m_msg.c_str();
    }


private:

    std::string m_msg;
};

#define throw_debug(msg) throw HZeeException( msg, __FILE__, __PRETTY_FUNCTION__, __LINE__ );

#endif // HZEE_EXCEPTION
