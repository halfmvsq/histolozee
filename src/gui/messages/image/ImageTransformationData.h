#ifndef GUI_IMAGE_TX_DATA_H
#define GUI_IMAGE_TX_DATA_H

#include "common/Identity.h"
#include "common/UID.h"

#include <boost/optional.hpp>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>


namespace gui
{

/**
 * @brief Decomposed image affine transformation parameters.
 * Template type V can be either boost::optional (to denote all optional fields) or
 * Required (to denote all required fields).
 */
template< template<typename> class V >
struct AffineDecomposition
{
    V< glm::vec3 > m_translation;        //!< Translation vector
    V< glm::vec3 > m_center;             //!< Center of rotation and stretch
    V< glm::vec3 > m_rotationDeg;        //!< Rotations along all axes (in degrees)
    V< glm::vec3 > m_stretchRotationDeg; //!< Stretch rotations along all axes (in degrees)
    V< glm::vec3 > m_stretch;            //!< Stretches along all axes
};


/**
 * @brief Image transformation data sent from App to UI, including the complete decomposition
 * of world_O_subject into its parts
 */
struct ImageTransformation_msgToUi
{
    UID m_imageUid; //!< Image UID

    /// Affine transformation matrix mapping image Subject to World space
    glm::dmat4 m_world_O_subject;

    /// Decomposition of image Subject to World space matrix
    AffineDecomposition< Identity > m_world_O_subject_decomp;
};


/**
 * @brief Image transformation data sent from UI to App
 */
struct ImageTransformation_msgFromUi
{
    UID m_imageUid; //!< Image UID

    /// Decomposition of image Subject to World space matrix
    AffineDecomposition< boost::optional > m_world_O_subject_decomp;

    /// Flag to set the world_O_subject transformation to identity.
    boost::optional<bool> m_set_world_O_subject_identity;
};

}

#endif // GUI_IMAGE_TX_DATA_H
