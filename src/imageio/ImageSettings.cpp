#include "ImageSettings.h"

namespace imageio
{

ImageSettings::ImageSettings(
        std::string displayName,
        statistics_range_t statistics,
        ComponentType componentType,
        InterpolationMode interpMode )
    :
      m_displayName( std::move( displayName ) ),
      m_componentType( std::move( componentType ) )
{
    for ( const auto& stat : statistics )
    {
        ComponentSettings setting;

        const double minValue = stat.m_minimum;
        const double maxValue = stat.m_maximum;

        setting.m_minMaxWindowRange = std::make_pair( 0.0, maxValue - minValue );
        setting.m_minMaxLevelRange = std::make_pair( minValue, maxValue );
        setting.m_minMaxThresholdRange = std::make_pair( minValue, maxValue );

        // Default window covers 1st to 99th quantile intensity range of the first
        // image pixel component
        const double quantile01 = stat.m_quantiles[1];
        const double quantile99 = stat.m_quantiles[99];

        setting.m_window = quantile99 - quantile01;
        setting.m_level = 0.5 * ( quantile01 + quantile99 );

        // Default thresholds are the min and max image intensities
        setting.m_thresholdLow = minValue;
        setting.m_thresholdHigh = maxValue;

        // Default to maximum opacity
        setting.m_opacity = 1.0;

        setting.m_interpolationMode = interpMode;

        m_settings.emplace_back( std::move( setting ) );
    }

    updateInternals();
}

void ImageSettings::setDisplayName( std::string name )
{
    m_displayName = std::move( name );
}

std::string ImageSettings::displayName() const
{
    return m_displayName;
}


void ImageSettings::setLevel( uint32_t i, double level )
{
    if ( m_settings[i].m_minMaxLevelRange.first <= level &&
         level <= m_settings[i].m_minMaxLevelRange.second )
    {
        m_settings[i].m_level = level;
        updateInternals();
    }
}

void ImageSettings::setWindow( uint32_t i, double window )
{
    if ( m_settings[i].m_minMaxWindowRange.first <= window &&
         window <= m_settings[i].m_minMaxWindowRange.second )
    {
        m_settings[i].m_window = window;
        updateInternals();
    }
}

double ImageSettings::window( uint32_t i ) const
{
    return m_settings[i].m_window;
}

double ImageSettings::level( uint32_t i ) const
{
    return m_settings[i].m_level;
}

std::pair<double, double> ImageSettings::windowRange( uint32_t i ) const
{
    return m_settings[i].m_minMaxWindowRange;
}

std::pair<double, double> ImageSettings::levelRange( uint32_t i ) const
{
    return m_settings[i].m_minMaxLevelRange;
}

void ImageSettings::setThresholdLow( uint32_t i, double t )
{
    if ( m_settings[i].m_minMaxThresholdRange.first <= t &&
         t <= m_settings[i].m_minMaxThresholdRange.second )
    {
        m_settings[i].m_thresholdLow = t;
        updateInternals();
    }
}

double ImageSettings::thresholdLow( uint32_t i ) const
{
    return m_settings[i].m_thresholdLow;
}

double ImageSettings::thresholdLowNormalized( uint32_t i ) const
{
    return m_settings[i].m_thresholdLowNorm;
}

void ImageSettings::setThresholdHigh( uint32_t i, double t )
{
    if ( m_settings[i].m_minMaxThresholdRange.first <= t &&
         t <= m_settings[i].m_minMaxThresholdRange.second )
    {
        m_settings[i].m_thresholdHigh = t;
        updateInternals();
    }
}

double ImageSettings::thresholdHigh( uint32_t i ) const
{
    return m_settings[i].m_thresholdHigh;
}

double ImageSettings::thresholdHighNormalized( uint32_t i ) const
{
    return m_settings[i].m_thresholdHighNorm;
}

bool ImageSettings::thresholdsActive( uint32_t i ) const
{
    const auto& S = m_settings[i];

    return ( S.m_minMaxThresholdRange.first < S.m_thresholdLow ||
             S.m_thresholdHigh < S.m_minMaxThresholdRange.second );
}

void ImageSettings::setOpacity( uint32_t i, double o )
{
    if ( 0.0 <= o && o <= 1.0 )
    {
        m_settings[i].m_opacity = o;
    }
}

double ImageSettings::opacity( uint32_t i ) const
{
    return m_settings[i].m_opacity;
}


void ImageSettings::setInterpolationMode( uint32_t i, InterpolationMode mode )
{
    m_settings[i].m_interpolationMode = mode;
}

ImageSettings::InterpolationMode ImageSettings::interpolationMode( uint32_t i ) const
{
    return m_settings[i].m_interpolationMode;
}


std::pair<double, double> ImageSettings::thresholdRange( uint32_t i ) const
{
    return m_settings[i].m_minMaxThresholdRange;
}

std::pair<double, double> ImageSettings::slopeIntercept( uint32_t i ) const
{
    return { m_settings[i].m_slope_old, m_settings[i].m_intercept_old };
}

std::pair<double, double> ImageSettings::slopeInterceptNormalized( uint32_t i ) const
{
    return { m_settings[i].m_slope, m_settings[i].m_intercept };
}


void ImageSettings::updateInternals()
{
    for ( uint32_t i = 0; i < m_settings.size(); ++i )
    {
        auto& S = m_settings[i];

        const double imageMin = S.m_minMaxLevelRange.first;
        const double imageMax = S.m_minMaxLevelRange.second;

        const double imageRange = imageMax - imageMin;

        if ( imageRange <= 0.0 || S.m_window <= 0.0 )
        {
            // Resort to default slope/intercept and normalized threshold values
            // if either the image range or the window width are not positive:

            S.m_slope_old = 1.0;
            S.m_intercept_old = 0.0;

            S.m_slope = 1.0;
            S.m_intercept = 0.0;

            S.m_thresholdLowNorm = 0.0;
            S.m_thresholdHighNorm = 1.0;

            continue;
        }

        S.m_slope_old = 1.0 / S.m_window;
        S.m_intercept_old = 0.5 - S.m_level / S.m_window;

        /**
         * @note In OpenGL, unsigned normalized floats are computed as
         * float = int / MAX, where MAX = 2^B - 1 = 255 (e.g.)
         *
         * Signed normalized floats are computed as either
         * float = max( int / MAX, -1 ) where MAX = 2^(B-1) - 1 = 127 (e.g.)
         *   or alternatively (depending on implementation)
         * float = (2*int + 1) / (2^B - 1) = (2*int + 1) / 255 (e.g.)
         */

        double M = 0.0;

        switch ( m_componentType )
        {
        case ComponentType::Int8:
        case ComponentType::UInt8:
        {
            M = std::numeric_limits<uint8_t>::max();
            break;
        }
        case ComponentType::Int16:
        case ComponentType::UInt16:
        {
            M = std::numeric_limits<uint16_t>::max();
            break;
        }
        case ComponentType::Int32:
        case ComponentType::UInt32:
        {
            M = std::numeric_limits<uint32_t>::max();
            break;
        }
        case ComponentType::Int64:
        case ComponentType::UInt64:
        {
            M = std::numeric_limits<uint64_t>::max();
            break;
        }
        case ComponentType::Float32:
        case ComponentType::Double64:
        {
            break;
        }
        }


        switch ( m_componentType )
        {
        case ComponentType::Int8:
        case ComponentType::Int16:
        case ComponentType::Int32:
        case ComponentType::Int64:
        {
            S.m_slope = 0.5 * M / imageRange;
            S.m_intercept = -( imageMin + 0.5 ) / imageRange;
            break;
        }
        case ComponentType::UInt8:
        case ComponentType::UInt16:
        case ComponentType::UInt32:
        case ComponentType::UInt64:
        {
            S.m_slope = M / imageRange;
            S.m_intercept = -imageMin / imageRange;
            break;
        }
        case ComponentType::Float32:
        case ComponentType::Double64:
        {
            S.m_slope = 1.0 / imageRange;
            S.m_intercept = -imageMin / imageRange;
            break;
        }
        }


        const double a = 1.0 / imageRange;
        const double b = -imageMin / imageRange;

        // Normalized window and level:
        const double windowNorm = a * S.m_window;
        const double levelNorm = a * S.m_level + b;

        // Apply windowing and leveling to the slope and intercept:
        S.m_slope = S.m_slope / windowNorm;
        S.m_intercept = S.m_intercept / windowNorm + ( 0.5 - levelNorm / windowNorm );

        // Normalize thresholds to the range [0.0, 1.0]:
        S.m_thresholdLowNorm = a * S.m_thresholdLow + b;
        S.m_thresholdHighNorm = a * S.m_thresholdHigh + b;
    }
}

} // namespace imageio
