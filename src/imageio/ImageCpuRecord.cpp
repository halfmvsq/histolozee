#include "ImageCpuRecord.h"

namespace imageio
{

ImageCpuRecord::ImageCpuRecord(
        std::unique_ptr< ::itkdetails::ImageBaseData > data,
        ImageHeader info,
        ImageSettings settings,
        ImageTransformations tx )
    :
      m_data( std::move( data ) ),
      m_header( std::move( info ) ),
      m_settings( std::move( settings ) ),
      m_transformations( std::move( tx ) )
{}

const ::itkdetails::ImageBaseData* ImageCpuRecord::imageBaseData() const
{
    return m_data.get();
}

const uint8_t* ImageCpuRecord::buffer() const
{
    return m_data->bufferPointer();
}

const uint8_t* ImageCpuRecord::buffer( uint32_t componentIndex ) const
{
    return m_data->bufferPointer( componentIndex );
}

bool ImageCpuRecord::pixelValue( uint32_t componentIndex, const glm::uvec3& pixelIndex, double& value ) const
{
    return m_data->getPixelAsDouble( componentIndex, pixelIndex[0], pixelIndex[1], pixelIndex[2], value );
}

const ImageHeader& ImageCpuRecord::header() const
{
    return m_header;
}

const ImageSettings& ImageCpuRecord::settings() const
{
    return m_settings;
}

const ImageTransformations& ImageCpuRecord::transformations() const
{
    return m_transformations;
}

void ImageCpuRecord::setDisplayName( std::string name )
{
    m_settings.setDisplayName( std::move( name ) );
}

void ImageCpuRecord::setOpacity( uint32_t component, double opacity )
{
    m_settings.setOpacity( component, opacity );
}

void ImageCpuRecord::setWindowWidth( uint32_t component, double w )
{
    m_settings.setWindow( component, w );
}

void ImageCpuRecord::setLevel( uint32_t component, double l )
{
    m_settings.setLevel( component, l );
}

void ImageCpuRecord::setThresholdLow( uint32_t component, double t )
{
    m_settings.setThresholdLow( component, t );
}

void ImageCpuRecord::setThresholdHigh( uint32_t component, double t )
{
    m_settings.setThresholdHigh( component, t );
}

void ImageCpuRecord::setInterpolationMode( uint32_t component, const ImageSettings::InterpolationMode& mode )
{
    m_settings.setInterpolationMode( component, mode );
}

void ImageCpuRecord::setWorldSubjectOrigin( glm::vec3 worldSubjectOrigin )
{
    m_transformations.setWorldSubjectOrigin( std::move( worldSubjectOrigin ) );
}

void ImageCpuRecord::setSubjectToWorldRotation( glm::quat world_O_subject_rotation )
{
    m_transformations.setSubjectToWorldRotation( std::move( world_O_subject_rotation ) );
}

void ImageCpuRecord::resetSubjectToWorld()
{
    static const glm::vec3 sk_origin{ 0.0f, 0.0f, 0.0f };
    static const glm::quat sk_identRot{ 1.0f, 0.0f, 0.0f, 0.0f };

    m_transformations.setWorldSubjectOrigin( sk_origin );
    m_transformations.setSubjectToWorldRotation( sk_identRot );
}

} // namespace imageio
