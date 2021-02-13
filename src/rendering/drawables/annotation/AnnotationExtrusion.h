#ifndef ANNOTATION_EXTRUSION_H
#define ANNOTATION_EXTRUSION_H

#include "rendering/drawables/DrawableBase.h"
#include "rendering/common/ShaderProviderType.h"
#include "rendering/records/MeshGpuRecord.h"

#include "common/ObjectCounter.hpp"
#include "common/PublicTypes.h"
#include "common/UID.h"

#include "logic/records/SlideAnnotationRecord.h"

#include <memory>


class BasicMesh;
class Transformation;


/**
 * @brief Render an extruded slide annotation.
 */
class AnnotationExtrusion :
        public DrawableBase,
        public ObjectCounter<AnnotationExtrusion>
{
public:

    AnnotationExtrusion(
            std::string name,
            ShaderProgramActivatorType shaderActivator,
            UniformsProviderType uniformsProvider,
            GetterType< boost::optional<glm::mat4> > annotToWorldTxProvider,
            GetterType< boost::optional<float> > thicknessProvider,
            std::weak_ptr<SlideAnnotationRecord> slideAnnotationRecord );

    ~AnnotationExtrusion() override = default;

    bool isOpaque() const override;

//    DrawableOpacity opacityFlag() const override;


private:

    void doUpdate( double, const Viewport&, const camera::Camera&, const CoordinateFrame& ) override;

    void setupChildren();

    ShaderProgramActivatorType m_shaderActivator;
    UniformsProviderType m_uniformsProvider;

    /// Function providing transformation from this annotation's Slide space to World space
    GetterType< boost::optional<glm::mat4> > m_annotToWorldTxProvider;

    /// Function providing the thickness of this annotation's slide in World space
    GetterType< boost::optional<float> > m_thicknessProvider;

    /// Slide annotation record that is rendered as a mesh
    std::weak_ptr<SlideAnnotationRecord> m_slideAnnotationRecord;

    /// GPU record of the mesh of the annotation slice
    std::shared_ptr<MeshGpuRecord> m_meshGpuRecord;

    /// Mesh drawable (a child of this object)
    std::shared_ptr<BasicMesh> m_mesh;

    /// Transformation atop the mesh that uses scale along z axis to account for layering
    std::shared_ptr<Transformation> m_scaleTx;
};

#endif // ANNOTATION_EXTRUSION_H
