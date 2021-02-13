#ifndef UID_RANGE_H
#define UID_RANGE_H

#include "common/UID.h"

#include <boost/range/any_range.hpp>

/// Convenience alias for a forward-traversable range of UIDs.
using uid_range_t = boost::any_range<
    const UID,
    boost::forward_traversal_tag,
    const UID&,
    std::ptrdiff_t >;

#endif // UID_RANGE
