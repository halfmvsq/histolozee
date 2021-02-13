#ifndef UNIFORMS_H
#define UNIFORMS_H

#include "rendering/utility/gl/GLUniformTypes.h"

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <boost/optional.hpp>
#include <boost/variant.hpp>

#include <QOpenGLFunctions_3_3_Core>

#include <array>
#include <functional>
#include <memory>
#include <unordered_map>


class GLShaderProgram;


/**
 * @brief Collection of uniform variables for a GLSL shader program.
 */
class Uniforms
{
public:

    // To avoid ambiguity, we define a type used to specifically encapsulate sampler indices.
    // Note that OpenGL expects sampler indices to be set with int32_t (signed) in glUniform1i.
    // However, the signed integer type conflicts with other OpenGL function calls that expect
    // sampler indices to be unsigned. Let's just use unsigned uint32_t, with the understanding
    // that sampler indices will never exceed the maximum signed value.
    struct SamplerIndexType{ uint32_t index; };

    using ValueType = boost::variant<
        bool,
        int,
        unsigned int,
        float,
        glm::vec2, glm::vec3, glm::vec4,
        glm::mat2, glm::mat3, glm::mat4,
        SamplerIndexType,
        std::array< float, 2 >,
        std::array< float, 3 >,
        std::array< float, 4 >,
        std::array< float, 5 >,
        std::array< uint32_t, 5 >,
        std::array< glm::vec3, 8 > >;


    /// Declaration of a single uniform variable
    struct Decl
    {
        Decl();
        Decl( UniformType type, ValueType defaultValue, bool isRequired );

        Decl( const Decl& ) = default;
        Decl& operator=( const Decl& ) = default;

        Decl( Decl&& ) = default;
        Decl& operator=( Decl&& ) = default;

        ~Decl() = default;

        void set( const ValueType& value );

        UniformType m_type;
        ValueType m_defaultValue;
        ValueType m_value;
        GLint m_location;
        bool m_isRequired;
        bool m_isDirty;
    };


    /// Hash map of uniforms, keyed by uniform name used in the GLSL code.
    using UniformsMap = std::unordered_map< std::string, Decl >;


    explicit Uniforms() = default;
    explicit Uniforms( const UniformsMap& map );

    Uniforms( const Uniforms& ) = default;
    Uniforms& operator=( const Uniforms& ) = default;

    Uniforms( Uniforms&& ) = default;
    Uniforms& operator=( Uniforms&& ) = default;

    ~Uniforms() = default;


    /// Insert a single uniform. Ignores uniforms that already exist.
    bool insertUniform( const std::string& name, const Decl& uniform );

    bool insertUniform( const std::string& name, const UniformType& type,
                        ValueType defaultValue, bool isRequired );


    /// Insert another set of uniforms. Ignores uniforms that already exist.
    void insertUniforms( const Uniforms& uniforms );

    void resetAllToDefaults();


    /// @throws If uniform with given name doesn't exist
    void setValue( const std::string& name, const ValueType& value );
    ValueType value( const std::string& name ) const;

    void setLocation( const std::string& name, GLint loc );
    boost::optional<GLint> location( const std::string& name ) const;

    GLint queryAndSetLocation(
            const std::string& name,
            std::function< GLint ( const std::string& ) > locationGetter );

    void queryAndSetAllLocations( std::function< GLint ( const std::string& ) > locationGetter );

    void setDirty( const std::string& name, bool set );
    bool isDirty( const std::string& name ) const;

    const Decl& operator() ( const std::string& name ) const;
    const UniformsMap& operator() () const;

    bool containsKey( const std::string& name ) const;

    static std::string getUniformTypeString( const GLenum type );


private:

    UniformsMap m_uniformsMap;
};

#endif // UNIFORMS_H
