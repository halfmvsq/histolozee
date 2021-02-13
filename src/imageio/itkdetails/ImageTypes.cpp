#include "itkdetails/ImageTypes.hpp"


namespace itkdetails
{

namespace io
{

std::unordered_map< std::type_index, ::itk::ImageIOBase::IOComponentType >
itkComponentTypeMap =
{
    { std::type_index( typeid(   int8_t ) ), ::itk::ImageIOBase::CHAR },
    { std::type_index( typeid(  uint8_t ) ), ::itk::ImageIOBase::UCHAR },
    { std::type_index( typeid(  int16_t ) ), ::itk::ImageIOBase::SHORT },
    { std::type_index( typeid( uint16_t ) ), ::itk::ImageIOBase::USHORT },
    { std::type_index( typeid(  int32_t ) ), ::itk::ImageIOBase::INT },
    { std::type_index( typeid( uint32_t ) ), ::itk::ImageIOBase::UINT },
    { std::type_index( typeid(  int64_t ) ), ::itk::ImageIOBase::LONG },
    { std::type_index( typeid( uint64_t ) ), ::itk::ImageIOBase::ULONG },
    { std::type_index( typeid(    float ) ), ::itk::ImageIOBase::FLOAT },
    { std::type_index( typeid(   double ) ), ::itk::ImageIOBase::DOUBLE }
};

} // namespace io

} // namespace itkdetails
