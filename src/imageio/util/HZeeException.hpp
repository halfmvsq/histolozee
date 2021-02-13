#ifndef HZEEIO_EXCEPTION
#define HZEEIO_EXCEPTION

#include <sstream>
#include <stdexcept>

class HZeeIOException : public std::runtime_error
{
public:

    HZeeIOException( const std::string& msg,
                     const char* file,
                     const char* function,
                     int line )
        : std::runtime_error( msg )
    {
        std::ostringstream ss;
        ss << msg << " [in " << function << "; file '" << file << "' : " << line << "]" << std::ends;
        m_msg = ss.str();
    }

    ~HZeeIOException() throw() {}

    const char* what() const throw()
    {
        return m_msg.c_str();
    }


private:

    std::string m_msg;
};

#define throw_io_debug(msg) throw HZeeIOException( msg, __FILE__, __PRETTY_FUNCTION__, __LINE__ );

#endif // HZEEIO_EXCEPTION
