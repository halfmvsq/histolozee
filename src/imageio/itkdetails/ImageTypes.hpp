#pragma once

#include <itkImageIOBase.h>

#include <stdint.h>
#include <typeindex>
#include <unordered_map>

/* From ITK documentation:
ImageBase is the base class for the templated Image classes. ImageBase is templated over the dimension
of the image. It provides the API and ivars that depend solely on the dimension of the image.
ImageBase does not store any of the image (pixel) data. Storage for the pixels and the pixel access
methods are defined in subclasses of ImageBase, namely Image and ImageAdaptor.

ImageBase manages the geometry of an image. The geometry of an image is defined by its position,
orientation, spacing, and extent.

The position and orientation of an image is defined by its "Origin" and its "Directions".
The "Origin" is the physical position of the pixel whose "Index" is all zeros. The "Direction"
of an image is a matrix whose columns indicate the direction in physical space that each dimension
of the image traverses. The first column defines the direction that the fastest moving index in
the image traverses in physical space while the last column defines the direction that the slowest
moving index in the image traverses in physical space.

The extent of an image is defined by the pixel spacing and a set of regions. The "Spacing" is the
size of a pixel in physical space along each dimension. Regions describe a portion of an image grid
via a starting index for the image array and a size (or number of pixels) in each dimension.
The ivar LargestPossibleRegion defines the size and starting index of the image dataset.
The entire image dataset, however, may not be resident in memory. The region of the image that is
resident in memory is defined by the "BufferedRegion". The Buffer is a contiguous block of memory.
The third set of meta-data defines a region of interest, called the "RequestedRegion".
The RequestedRegion is used by the pipeline execution model to define what a filter is requested
to produce.

[RegionIndex, RegionSize] C [BufferIndex, BufferSize] C [ImageIndex, ImageSize]

ImageBase provides all the methods for converting between the physical space and index coordinate
frames. TransformIndexToPhysicalPoint() converts an Index in the pixel array into its coordinates
in physical space. TransformPhysicalPointToIndex() converts a position in physical space into an
Index into the pixel array (using rounding). Subpixel locations are supported by methods that
convert to and from ContinuousIndex types.

ImageBase also provides helper routines for the ImageIterators which convert an Index to an offset
in memory from the first pixel address as well as covert an offset in memory from the first pixel
address to an Index.
*/

namespace itk
{
/// Image base
template< uint32_t NumDim >
class ImageBase;

/// Image
template< typename PixelType, uint32_t NumDim >
class Image;

/// Vector image
template< typename PixelType, uint32_t NumDim >
class VectorImage;

/// Image region
template< uint32_t NumDim >
class ImageRegion;

/// Discrete image index coordinates
template< uint32_t NumDim >
class Index;

/// Continuous image index coordinates
template< typename CoordinateRep, uint32_t NumDim >
class ContinuousIndex;

/// Geometric point in n-Dimensional space
template< typename CoordinateRep, uint32_t NumDim >
class Point;
}


namespace itkdetails
{

namespace image3d
{

constexpr uint32_t NDIM = 3;

using ImageBaseType = ::itk::ImageBase< NDIM >;

template< class PixelType >
using ImageType = ::itk::Image< PixelType, NDIM >;

template< class PixelType >
using VectorImageType = ::itk::VectorImage< PixelType, NDIM >;

using ImageRegionType = ::itk::ImageRegion< NDIM >;

using IndexType = ::itk::Index< NDIM >;

using ContinuousIndexType = ::itk::ContinuousIndex< double, NDIM >;

using PointType = ::itk::Point< double, NDIM >;

} // namespace image3d


namespace io
{

extern
std::unordered_map< std::type_index, ::itk::ImageIOBase::IOComponentType > itkComponentTypeMap;

} // namespace io

} // namespace itkdetails
