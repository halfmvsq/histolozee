#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>


/**
 * @note Geometry in Qt5 is specified in device-independent pixels.
 * This includes widget and item geometry, event geometry, desktop, window,
 * and screen geometry, and animation velocities. Rendered output is in
 * device pixels, which corresponds to the display resolution.
 *
 * This Viewport class stores left, bottom, width, and heigh in device
 * independent pixels.
 *
 * The ratio between the device independent (Pixels used by application
 * (user space), subject to scaling by the operating system or Qt)
 * and device pixel coordinate (Pixels of the display device) systems
 * is devicePixelRatio.
 */


/**
 * @brief Viewport class that follows the OpenGL convension:
 * - Viewport dimensions are measured in pixels.
 * - Pixel coordinate (0, 0) is the bottom left corner of the viewport.
 * - Left-to-right and bottom-to-top directions are both positive.
 *
 * @note Look at Qt 5.11's support for high resolution displays.
 */
class Viewport
{
public:

    /// Construct 1x1 viewport with bottom left at (0, 0)
    Viewport();

    /// Construct viewport with given bottom left coordiates and dimensions
    Viewport( float left, float bottom, float width, float height );

    Viewport( const Viewport& ) = default;
    Viewport& operator=( const Viewport& ) = default;

    Viewport( Viewport&& ) = default;
    Viewport& operator=( Viewport&& ) = default;

    ~Viewport() = default;

    /// Set the left coordinate in device-independent pixel units
    void setLeft( float left );

    /// Set the bottom coordinate in device-independent pixel units
    void setBottom( float bottom );

    /// Set the width in device-independent pixel units
    void setWidth( float width );

    /// Set the height in device-independent pixel units
    void setHeight( float height );

    /// Get the left coordinate in device-independent pixel units
    float left() const;

    /// Get the bottom coordinate in device-independent pixel units
    float bottom() const;

    /// Get the width in device-independent pixel units
    float width() const;

    /// Get the height in device-independent pixel units
    float height() const;

    /// Get the viewport area in device-independent pixel units
    float area() const;


    /// Set the viewport from a vec4: { left, bottom, width, height }
    void setAsVec4( const glm::vec4& viewport );

    /// Get the viewport as a vec4: {left, bottom, width, height }
    glm::vec4 getAsVec4() const;

    /// Get the left coordinate in device pixel units
    float deviceLeft() const;

    /// Get the bottom coordinate in device pixel units
    float deviceBottom() const;

    /// Get the width in device pixel units
    float deviceWidth() const;

    /// Get the height in device pixel units
    float deviceHeight() const;

    /// Get the area in device pixel units
    float deviceArea() const;


    /// Get the viewport aspect ratio: width / height
    float aspectRatio() const;


    /// Set the number of display device pixels per logical pixel
    void setDevicePixelRatio( float ratio );

    /// Get the number of device pixels per logical pixel
    float devicePixelRatio() const;


private:

    /// @note Left, bottom, width, and height are stored in device-independent pixel units

    float m_left;
    float m_bottom;
    float m_width;
    float m_height;

    /// Number of display device pixels per logical pixel
    float m_devicePixelRatio;
};

#endif // VIEWPORT_H
