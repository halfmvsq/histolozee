#include "ParcellationCpuRecord.h"

#include "util/HZeeException.hpp"

#include <algorithm>


namespace imageio
{

ParcellationCpuRecord::ParcellationCpuRecord(
        ImageCpuRecord imageCpuRecord,
        std::vector<int64_t> pixelValueMap )
    :
      ImageCpuRecord( std::move( imageCpuRecord ) ),
      m_labelValues( std::move( pixelValueMap ) )
{
    if ( m_labelValues.empty() )
    {
        throw_io_debug( "No pixel values provided." );
    }
}

ParcellationCpuRecord::label_value_range_t
ParcellationCpuRecord::labelValues() const
{
    return m_labelValues;
}

boost::optional<int64_t> ParcellationCpuRecord::labelValue( size_t index ) const
{
    if ( index < m_labelValues.size() )
    {
        return m_labelValues.at( index );
    }
    return boost::none;
}

size_t ParcellationCpuRecord::numLabels() const
{
    return m_labelValues.size();
}

size_t ParcellationCpuRecord::maxLabelIndex() const
{
    return m_labelValues.size() - 1;
}

std::pair<int64_t, int64_t> ParcellationCpuRecord::minMaxLabelValues() const
{
    // Sort the values in m_labelValues. It will not be sorted if there exist negative
    // lael values, since value 0 is forced to be first.

    std::vector<int64_t> sortedLabels = m_labelValues;
    std::sort( std::begin( sortedLabels ), std::end( sortedLabels ) );

    return { sortedLabels.front(), sortedLabels.back() };
}

} // namespace imageio
