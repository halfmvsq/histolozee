#ifndef SLICE_INTERSECTOR_H
#define SLICE_INTERSECTOR_H

#include "rendering/utility/gl/GLBufferObject.h"
#include "rendering/utility/gl/GLVertexArrayObject.h"

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <array>
#include <optional>
#include <utility>


/**
 * @brief Intersects a box (with vertices defined in local Modeling coordinate space)
 * against a plane.
 */
class SliceIntersector
{
public:

    /**
     * @brief Describes the method used for positioning slices
     */
    enum class PositioningMethod
    {
        OffsetFromCamera,
        FrameOrigin,
        UserDefined
    };


    /**
     * @brief Describes the method used for aligning slices
     */
    enum class AlignmentMethod
    {
        CameraZ,
        FrameX,
        FrameY,
        FrameZ,
        UserDefined
    };


    // There are up to six intersection points between a 3D plane and a 3D AABB.
    // We store the intersection polygon in vertex buffer using seven vertices:
    // Six are the intersection vertices themselves (included repeated ones);
    // plus one more hub vertex at the centroid of the intersection points.

    static constexpr int s_numIntersections = 6;
    static constexpr int s_numVertices = 7;

    using IntersectionVertices = std::array< glm::vec3, SliceIntersector::s_numVertices >;


    explicit SliceIntersector();

    SliceIntersector( const SliceIntersector& ) = default;
    SliceIntersector( SliceIntersector&& ) = default;

    SliceIntersector& operator= ( const SliceIntersector& ) = default;
    SliceIntersector& operator= ( SliceIntersector&& ) = default;

    ~SliceIntersector() = default;


    void setPositioningMethod(
            const PositioningMethod& method,
            const std::optional<glm::vec3>& p = std::nullopt );

    const PositioningMethod& positioningMethod() const;

    void setAlignmentMethod(
            const AlignmentMethod& method,
            const std::optional<glm::vec3>& worldNormal = std::nullopt );

    const AlignmentMethod& alignmentMethod() const;

    /// Compute and return the intersection vertices (if they exist) and the plane equation
    std::pair< std::optional< IntersectionVertices >, glm::vec4 >
    computePlaneIntersections(
            const glm::mat4& model_O_camera,
            const glm::mat4& model_O_frame,
            const std::array< glm::vec3, 8 >& modelBoxCorners );


private:

    void updatePlaneEquation( const glm::mat4& model_O_camera, const glm::mat4& model_O_frame );

    PositioningMethod m_positioningMethod;
    AlignmentMethod m_alignmentMethod;

    glm::vec3 m_cameraSliceOffset;
    glm::vec3 m_userSlicePosition;
    glm::vec3 m_userSliceNormal;

    glm::vec4 m_modelPlaneEquation;
};

#endif // SLICE_INTERSECTOR_H
