#ifndef SLIDE_READING_H
#define SLIDE_READING_H

#include "logic/records/SlideRecord.h"

#include <glm/vec2.hpp>

#include <memory>
#include <string>


namespace slideio
{

class SlideCpuRecord;

using AssociatedImage = std::pair< std::shared_ptr< std::vector<uint32_t> >, glm::i64vec2 >;

std::unique_ptr<SlideCpuRecord> readSlide(
        const std::string& fileName,
        const glm::vec2& pixelSize,
        float thickness );

} // namespace slideio

#endif // SLIDE_READING_H

