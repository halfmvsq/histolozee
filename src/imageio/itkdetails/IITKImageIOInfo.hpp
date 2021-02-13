#ifndef I_ITK_IMAGE_IO_INFO_H
#define I_ITK_IMAGE_IO_INFO_H

#include <itkImageIOBase.h>

namespace itkdetails
{

namespace io
{

/**
 * @brief Interface for classes intended hold information from ITK ImageIO objects
 */
class IItkImageIoInfo
{
public:

    virtual ~IItkImageIoInfo() = default;

    virtual bool set( const ::itk::ImageIOBase::Pointer imageIO ) = 0;
    virtual bool validate() = 0;
};

} // namespace io

} // namespace itkdetails

#endif // I_ITK_IMAGE_IO_INFO_H
