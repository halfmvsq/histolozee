#ifndef PARCELLATION_RECORD_H
#define PARCELLATION_RECORD_H

#include "logic/RenderableRecord.h"
#include "imageio/ParcellationCpuRecord.h"
#include "rendering/records/ImageGpuRecord.h"

/**
 * Record for a parcellation image.
 */
using ParcellationRecord = RenderableRecord< imageio::ParcellationCpuRecord, ImageGpuRecord >;

#endif // PARCELLATION_RECORD_H
