#ifndef IMAGEIO_IMAGE_SETTINGS_H
#define IMAGEIO_IMAGE_SETTINGS_H

#include "HZeeTypes.hpp"
#include "itkdetails/ImageUtility.hpp"

#include <boost/range/any_range.hpp>

#include <string>
#include <utility>
#include <vector>


/// @todo Take this out of imageio library.
/// @todo Throw exceptions if queried component index is out of range.

namespace imageio
{

class ImageSettings
{
private:

    /// @todo put in another header
    using statistics_range_t = boost::any_range<
        itkdetails::utility::PixelStatistics<double>,
        boost::forward_traversal_tag,
        itkdetails::utility::PixelStatistics<double>&,
        std::ptrdiff_t >;

public:

    enum class InterpolationMode
    {
        NearestNeighbor,
        Linear
    };


    /// Construct with vector of pixel statistics, one per image component
    explicit ImageSettings(
            std::string displayName,
            statistics_range_t statistics,
            ComponentType componentType,
            InterpolationMode interpMode );

    ImageSettings( const ImageSettings& ) = default;
    ImageSettings& operator=( const ImageSettings& ) = default;

    ImageSettings( ImageSettings&& ) = default;
    ImageSettings& operator=( ImageSettings&& ) = default;

    ~ImageSettings() = default;


    /// Set the short display name of the image
    void setDisplayName( std::string name );

    /// Get the short display of the image
    std::string displayName() const;


    /// Set window (in image intensity units) for a given component.
    void setWindow( uint32_t component, double window );

    /// Get window (in image intensity units) for a given component
    double window( uint32_t component ) const;


    /// Set level (in image intensity units) for a given component.
    void setLevel( uint32_t component, double level );

    /// Get level (in image intensity units) for a given component
    double level( uint32_t component ) const;


    /// Set low threshold (in image intensity units) for a given component
    void setThresholdLow( uint32_t component, double thresh );

    /// Get low threshold (in image intensity units) for a given component
    double thresholdLow( uint32_t component ) const;

    /// Get normalized low threshold for a given component
    double thresholdLowNormalized( uint32_t component ) const;


    /// Set high threshold (in image intensity units) for a given component
    void setThresholdHigh( uint32_t component, double thresh );

    /// Get high threshold (in image intensity units) for a given component
    double thresholdHigh( uint32_t component ) const;

    /// Get normalized high threshold for a given component
    double thresholdHighNormalized( uint32_t component ) const;


    /// Get whether the thresholds are active for a given component
    bool thresholdsActive( uint32_t component ) const;


    /// Set the image opacity (in [0, 1] range) for a given component
    void setOpacity( uint32_t component, double opacity );

    /// Get the image opacity (in [0, 1] range) of a given component
    double opacity( uint32_t component ) const;


    /// Set the interpolation mode for a given component
    void setInterpolationMode( uint32_t component, InterpolationMode mode );

    /// Get the interpolation mode of a given component
    InterpolationMode interpolationMode( uint32_t component ) const;


    /// Get window/level slope 'm' and intercept 'b' for a given component.
    /// These are used to map RAW image intensity units 'x' to normalized units 'y' in the
    /// normalized range [0, 1]: y = m*x + b
    std::pair<double, double> slopeIntercept( uint32_t component ) const;

    /// Get normalized window/level slope 'm' and intercept 'b' for a given component.
    /// These are used to map image TEXTURE intensity units 'x' to normalized units 'y' in the
    /// normalized range [0, 1]: y = m*x + b
    std::pair<double, double> slopeInterceptNormalized( uint32_t component ) const;

    /// Get window range (in image intensity units) for a given component
    std::pair<double, double> windowRange( uint32_t component ) const;

    /// Get level range (in image intensity units) for a given component
    std::pair<double, double> levelRange( uint32_t component ) const;

    /// Get threshold range (in image intensity units) for a given component
    std::pair<double, double> thresholdRange( uint32_t component ) const;


private:

    void updateInternals();

    std::string m_displayName;

    ComponentType m_componentType;

    /// Settings for one image component
    struct ComponentSettings
    {
        double m_level; //! Window center value in image units
        double m_window; //! Window width in image units

        // The following values of slope (m) and intercept (b) are used to map RAW image intensity
        // values (x) into the range [0.0, 1.0] via m*x + b
        double m_slope_old; //!< Slope (m) computed from window
        double m_intercept_old; //!< Intercept (b) computed from window and level

        // The following values of slope (m) and intercept (b) are used to map image TEXTURE intensity
        // values (x) into the range [0.0, 1.0] via m*x + b
        double m_slope; //!< Slope computed from window
        double m_intercept; //!< Intercept computed from window and level

        double m_thresholdLow;  //!< Values below threshold not displayed
        double m_thresholdHigh; //!< Values above threshold not displayed

        // The following threshold values are mapped to normalized range [0.0, 1.0]
        double m_thresholdLowNorm; //!< Normalized lower threshold
        double m_thresholdHighNorm; //!< Normalized upper threshold

        double m_opacity; //!< Opacity [0.0, 1.0]

        InterpolationMode m_interpolationMode;

        std::pair< double, double > m_minMaxWindowRange;    //!< Valid window size range
        std::pair< double, double > m_minMaxLevelRange;     //!< Valid level value range
        std::pair< double, double > m_minMaxThresholdRange; //!< Valid threshold range
    };


    /// Per-component settings for the image
    std::vector<ComponentSettings> m_settings;
};

} // namespace imageio

#endif // IMAGEIO_IMAGE_SETTINGS_H
