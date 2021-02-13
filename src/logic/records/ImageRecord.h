#ifndef IMAGE_RECORD_H
#define IMAGE_RECORD_H

#include "logic/RenderableRecord.h"
#include "imageio/ImageCpuRecord.h"
#include "rendering/records/ImageGpuRecord.h"

/**
 * Record for an image. Nominally a 3D image, but could also be 1D or 2D.
 */
using ImageRecord = RenderableRecord< imageio::ImageCpuRecord, ImageGpuRecord >;

#endif // IMAGE_RECORD_H
