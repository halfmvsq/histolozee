#pragma once

#include "itkdetails/IITKImageIOInfo.hpp"

#include <itkImageBase.h>

#include <boost/variant.hpp>

#include <string>
#include <unordered_map>


namespace itkdetails
{

/// @brief Definitions in this namespace are for ITK image IO.
/// With the exception of SPIRAL codes, nothing is specific to image type or dimension.
namespace io
{

class FileInfo : public IItkImageIoInfo
{
public:

    FileInfo();
    FileInfo( const ::itk::ImageIOBase::Pointer imageIO );

    FileInfo( const FileInfo& ) = default;
    FileInfo& operator=( const FileInfo& ) = default;

    FileInfo( FileInfo&& ) = default;
    FileInfo& operator=( FileInfo&& ) = default;

    ~FileInfo() override = default;

    bool set( const ::itk::ImageIOBase::Pointer imageIO ) override;
    bool validate() override;

    std::string m_fileName;

    ::itk::ImageIOBase::ByteOrder m_byteOrder;
    std::string m_byteOrderString;
    bool m_useCompression;

    ::itk::ImageIOBase::IOFileEnum m_fileType;
    std::string m_fileTypeString;

    std::vector< std::string > m_supportedReadExtensions;
    std::vector< std::string > m_supportedWriteExtensions;
};


class ComponentInfo : public IItkImageIoInfo
{
public:

    ComponentInfo();
    ComponentInfo( const ::itk::ImageIOBase::Pointer imageIO );

    ComponentInfo( const ComponentInfo& ) = default;
    ComponentInfo& operator=( const ComponentInfo& ) = default;

    ComponentInfo( ComponentInfo&& ) = default;
    ComponentInfo& operator=( ComponentInfo&& ) = default;

    ~ComponentInfo() override = default;

    bool set( const ::itk::ImageIOBase::Pointer imageIO ) override;
    bool validate() override;

    ::itk::ImageIOBase::IOComponentType m_componentType;
    std::string m_componentTypeString;
    uint32_t m_componentSizeInBytes;
};


class PixelInfo : public IItkImageIoInfo
{
public:

    PixelInfo();
    PixelInfo( const ::itk::ImageIOBase::Pointer imageIO );

    PixelInfo( const PixelInfo& ) = default;
    PixelInfo& operator=( const PixelInfo& ) = default;

    PixelInfo( PixelInfo&& ) = default;
    PixelInfo& operator=( PixelInfo&& ) = default;

    ~PixelInfo() override = default;

    bool set( const ::itk::ImageIOBase::Pointer imageIO ) override;
    bool validate() override;

    ::itk::ImageIOBase::IOPixelType m_pixelType;
    std::string m_pixelTypeString;
    uint32_t m_numComponents;
    ::itk::ImageIOBase::SizeType m_pixelStrideInBytes;
};


class SizeInfo : public IItkImageIoInfo
{
public:

    SizeInfo();
    SizeInfo( const ::itk::ImageIOBase::Pointer imageIO );

    SizeInfo( const SizeInfo& ) = default;
    SizeInfo& operator=( const SizeInfo& ) = default;

    SizeInfo( SizeInfo&& ) = default;
    SizeInfo& operator=( SizeInfo&& ) = default;

    ~SizeInfo() override = default;

    bool set( const ::itk::ImageIOBase::Pointer imageIO ) override;
    bool set( const typename ::itk::ImageBase< 3 >::Pointer imageBase,
              const uint32_t componentSizeInBytes );
    bool validate() override;

    ::itk::ImageIOBase::SizeType m_imageSizeInComponents;
    ::itk::ImageIOBase::SizeType m_imageSizeInPixels;
    ::itk::ImageIOBase::SizeType m_imageSizeInBytes;
};


class SpaceInfo : public IItkImageIoInfo
{
public:

    SpaceInfo();
    SpaceInfo( const ::itk::ImageIOBase::Pointer imageIO );

    SpaceInfo( const SpaceInfo& ) = default;
    SpaceInfo& operator=( const SpaceInfo& ) = default;

    SpaceInfo( SpaceInfo&& ) = default;
    SpaceInfo& operator=( SpaceInfo&& ) = default;

    ~SpaceInfo() override = default;

    bool set( const ::itk::ImageIOBase::Pointer imageIO ) override;
    bool set( const typename ::itk::ImageBase< 3 >::Pointer imageBase );
    bool validate() override;

    uint32_t m_numDimensions;
    std::vector< uint64_t > m_dimensions;
    std::vector< double > m_origin;
    std::vector< double > m_spacing;
    std::vector< std::vector< double > > m_directions;
};


using MetaDataMap = std::unordered_map< std::string,
        boost::variant< std::string,
                        int8_t, uint8_t,
                        int16_t, uint16_t,
                        int32_t, uint32_t,
                        int64_t, uint64_t,
                        float, double > >;


class ImageIoInfo : public IItkImageIoInfo
{
public:

    ImageIoInfo() = default;
    ImageIoInfo( const ::itk::ImageIOBase::Pointer imageIO );

    ImageIoInfo( const ImageIoInfo& ) = default;
    ImageIoInfo& operator=( const ImageIoInfo& ) = default;

    ImageIoInfo( ImageIoInfo&& ) = default;
    ImageIoInfo& operator=( ImageIoInfo&& ) = default;

    ~ImageIoInfo() override = default;

    bool set( const ::itk::ImageIOBase::Pointer imageIO ) override;
    bool validate() override;

    FileInfo m_fileInfo;
    ComponentInfo m_componentInfo;
    PixelInfo m_pixelInfo;
    SizeInfo m_sizeInfo;
    SpaceInfo m_spaceInfo;
    MetaDataMap m_metaData;
};

} // namespace io

} // namespace itkdetails
