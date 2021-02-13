#include "rendering/utility/containers/Uniforms.h"
#include "rendering/utility/gl/GLShaderProgram.h"

#include "common/HZeeException.hpp"

#include <iostream>
#include <sstream>


Uniforms::Decl::Decl()
    :
      m_type( UniformType::Undefined ),
      m_defaultValue( 0 ),
      m_value( 0 ),
      m_location( -1 ),
      m_isRequired( false ),
      m_isDirty( true )
{}

Uniforms::Decl::Decl( UniformType type, ValueType defaultValue, bool isRequired )
    :
      m_type( type ),
      m_defaultValue( defaultValue ),
      m_value( defaultValue ),
      m_location( -1 ),
      m_isRequired( isRequired ),
      m_isDirty( true )
{}

void Uniforms::Decl::set( const ValueType& value )
{
    m_value = value;
    m_isDirty = true;
}


Uniforms::Uniforms( const UniformsMap& map )
    :
      m_uniformsMap( map )
{}


bool Uniforms::insertUniform( const std::string& name, const Uniforms::Decl& uniform )
{
    auto result = m_uniformsMap.insert( { name, uniform } );
    return result.second;
}


bool Uniforms::insertUniform( const std::string& name, const UniformType& type,
                              ValueType defaultValue, bool isRequired )
{
    auto result = m_uniformsMap.emplace(
                std::piecewise_construct,
                std::forward_as_tuple( name ),
                std::forward_as_tuple( type, defaultValue, isRequired ) );

    return result.second;
}

void Uniforms::insertUniforms( const Uniforms& uniforms )
{
    m_uniformsMap.insert( std::begin( uniforms() ), std::end( uniforms() ) );
}

const Uniforms::Decl& Uniforms::operator()( const std::string& name ) const
{
    return m_uniformsMap.at( name );
}

const Uniforms::UniformsMap& Uniforms::operator()() const
{
    return m_uniformsMap;
}

bool Uniforms::containsKey( const std::string& name ) const
{
    auto itr = m_uniformsMap.find( name );
    if ( std::end( m_uniformsMap ) != itr )
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Uniforms::resetAllToDefaults()
{
    for ( auto& uniform : m_uniformsMap )
    {
        Decl& u = uniform.second;
        u.m_value = u.m_defaultValue;
        u.m_isDirty = true;
    }
}

void Uniforms::setValue( const std::string& name, const ValueType& value )
{
    Decl& u = m_uniformsMap.at( name );
    u.m_value = value;
    u.m_isDirty = true;
}

Uniforms::ValueType Uniforms::value( const std::string& name ) const
{
    return m_uniformsMap.at( name ).m_value;
}

void Uniforms::setLocation( const std::string& name, GLint loc )
{
    Decl& u = m_uniformsMap.at( name );
    u.m_location = loc;
    u.m_isDirty = true;
}

std::optional<GLint> Uniforms::location( const std::string& name ) const
{
    const auto itr = m_uniformsMap.find( name );
    if ( std::end( m_uniformsMap ) != itr )
    {
        return itr->second.m_location;
    }
    else
    {
        return std::nullopt;
    }
}

GLint Uniforms::queryAndSetLocation(
        const std::string& name,
        std::function< GLint ( const std::string& ) > locationGetter )
{
    const GLint loc = locationGetter( name );

    if ( -1 == loc )
    {
        std::ostringstream msg;
        msg << "Unrecognized uniform \"" << name << "\"" << std::ends;
        throw_debug( msg.str() );
    }

    setLocation( name, loc );
    return loc;
}

void Uniforms::queryAndSetAllLocations( std::function< GLint ( const std::string& ) > locationGetter )
{
    for ( auto& uniform : m_uniformsMap )
    {
        queryAndSetLocation( uniform.first, locationGetter );
    }
}

void Uniforms::setDirty( const std::string& name, bool dirty )
{
    m_uniformsMap.at( name ).m_isDirty = dirty;
}

bool Uniforms::isDirty( const std::string& name ) const
{
    return m_uniformsMap.at( name ).m_isDirty;
}


std::string Uniforms::getUniformTypeString( const GLenum type )
{
    switch ( type )
    {
    case GL_BOOL: return "bool";
    case GL_INT: return "int";
    case GL_UNSIGNED_INT: return "uint";
    case GL_FLOAT: return "float";
    case GL_DOUBLE: return "double";
    case GL_FLOAT_VEC2: return "vec2";
    case GL_FLOAT_VEC3: return "vec3";
    case GL_FLOAT_VEC4: return "vec4";
    case GL_FLOAT_MAT2: return "mat2";
    case GL_FLOAT_MAT3: return "mat3";
    case GL_FLOAT_MAT4: return "mat4";
    case 1: return "sampler";
    case 2: return "floatArray2";
    case 3: return "floatArray3";
    case 4: return "floatArray4";
    case 5: return "floatArray5";
    case 6: return "uintArray5";
    default: return "unknown";
    }
}
