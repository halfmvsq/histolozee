#ifndef IMAGEIO_PARCELLATION_CPU_RECORD_H
#define IMAGEIO_PARCELLATION_CPU_RECORD_H

#include "ImageCpuRecord.h"

#include <boost/optional.hpp>
#include <boost/range/any_range.hpp>

#include <utility>
#include <vector>


namespace imageio
{

/**
 * @brief Record of a parcellation. It consists of the image record with pixel values
 * corresponding to label indices (unsigned integers). There is also a structure that maps label
 * indices to label values.
 *
 * @todo ImageCpuRecord and ParcellationCpuRecord do NOT belong in the imageio library.
 */
class ParcellationCpuRecord : public ImageCpuRecord
{
public:

    using label_value_range_t = boost::any_range<
        int64_t,
        boost::forward_traversal_tag,
        int64_t&,
        std::ptrdiff_t >;

public:

    /// There must be at least one label value!
    ParcellationCpuRecord( ImageCpuRecord imageCpuRecord,
                           std::vector<int64_t> labelValues );

    ParcellationCpuRecord( const ParcellationCpuRecord& ) = delete;
    ParcellationCpuRecord& operator=( const ParcellationCpuRecord& ) = delete;

    ParcellationCpuRecord( ParcellationCpuRecord&& ) = default;
    ParcellationCpuRecord& operator=( ParcellationCpuRecord&& ) = default;

    ~ParcellationCpuRecord() = default;


    /// Get all label values of the image in ascending order, with the exception of
    /// label value 0, which is always first. In other words, label index 0 always
    /// maps to label value 0.
    label_value_range_t labelValues() const;

    /// Get the label value at a given index. \c boost::none is returned if the index is invalid.
    boost::optional<int64_t> labelValue( size_t index ) const;

    /// Get the total number of labels in the parcellation
    size_t numLabels() const;

    /// Get the maximum label index in the parcellation (equals numLabels() - 1)
    size_t maxLabelIndex() const;

    /// Get the minimum and maximum label values in the parcellation
    std::pair<int64_t, int64_t> minMaxLabelValues() const;


private:

    /// Vector of label values: element at index i holds the i'th label value,
    /// with the excpetion of label value 0, which is always first.
    /// Therefore, this vector is sorted if all label values are non-negative.
    std::vector<int64_t> m_labelValues;
};

} // namespace imageio

#endif // IMAGEIO_PARCELLATION_CPU_RECORD_H
