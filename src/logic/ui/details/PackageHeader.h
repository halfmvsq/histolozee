#include "imageio/ImageHeader.h"
#include "imageio/ImageSettings.h"

#include <string>
#include <utility>
#include <vector>

namespace details
{

/**
 * @brief Package relevant data from image header/settings for the UI into a vector of
 * (ordered) key-value pairs. The UI is to display the key-value pairs in the order
 * specified by the vector.
 *
 * @param header Image header
 *
 * @return Vector of key-value pairs to display in UI
 */
std::vector< std::pair< std::string, std::string > >
packageImageHeaderForUi( const imageio::ImageHeader& header,
                         const imageio::ImageSettings& settings );

} // namespace details
