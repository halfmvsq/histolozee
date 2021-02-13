#ifndef IMAGE_COLOR_MAP_RECORD_H
#define IMAGE_COLOR_MAP_RECORD_H

#include "logic/RenderableRecord.h"
#include "logic/colormap/ImageColorMap.h"
#include "rendering/utility/gl/GLTexture.h"

/**
 * Record for an image color map. It is represented on GPU as a 2D texture.
 */
using ImageColorMapRecord = RenderableRecord< ImageColorMap, GLTexture >;

#endif // IMAGE_3D_COLOR_MAP_RECORD_H
