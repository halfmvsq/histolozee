#ifndef VIEW_TYPE_RANGE_H
#define VIEW_TYPE_RANGE_H

#include "common/UID.h"
#include "gui/layout/ViewType.h"

#include <boost/range/any_range.hpp>

#include <utility>

/**
 * @brief Convenience definition of a forward-traversable range of view UID/type pairs.
 *
 * @note (from online)
 * \c any_range() is a range object that can represent vectors, lists, or other
 * range objects. It can be used in interfaces to hide the type of the
 * underlying range, which decouples the interface from the implementation.
 * The different internal storage container types can be used without affecting
 * the public interface.

 * \c any_range() does not copy the input range. The range remains valid
 * only as long as the underlying containers exist. In other words, you can't
 * return an any_range() of a temporary variable!
 */

using view_type_range_t = boost::any_range<
    std::pair< const UID, gui::ViewType >,
    boost::forward_traversal_tag,
    std::pair< const UID, gui::ViewType >&,
    std::ptrdiff_t >;

#endif // VIEW_TYPE_RANGE_H
