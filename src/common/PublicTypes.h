#ifndef COMMON_PUBLIC_TYPES_H
#define COMMON_PUBLIC_TYPES_H

#include <functional>
#include <optional>
#include <string>


/// Function that enqueues re-render (update) calls for all views.
/// It is expected that the application's window system will take
/// care of actually calling "render" on the view at the correct time.
using AllViewsUpdaterType = std::function< void (void) >;

/// Function that aligns the crosshairs to the Slide Stack coordinate frame
using CrosshairsAlignerType = std::function< void () >;

/// Function that resets cameras of all views to their default states
using AllViewsResetterType = std::function< void () >;

/// Function that saves the project to an optional new file name
using ProjectSaverType = std::function< void ( const std::optional< std::string >& fileName ) >;


/// Shorthand for a function that returns an object
template< class T >
using GetterType = std::function< T (void) >;

/// Shorthand for a function that sets an object
template< class T >
using SetterType = std::function< void ( T ) >;

/// Function for querying an object by based on an ID
template< class ValueType, class IdType >
using QuerierType = std::function< ValueType ( const IdType& ) >;

#endif // COMMON_PUBLIC_TYPES_H
