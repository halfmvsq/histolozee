#ifndef SLIDE_SLICE_H
#define SLIDE_SLICE_H

#include "rendering/common/MeshColorLayer.h"
#include "rendering/common/ShaderProviderType.h"
#include "rendering/drawables/DrawableBase.h"
#include "rendering/interfaces/ITexturable3D.h"
#include "rendering/utility/containers/Uniforms.h"
#include "rendering/utility/math/SliceIntersector.h"

#include "common/ObjectCounter.hpp"
#include "common/PublicTypes.h"
#include "common/UID.h"

#include "logic/records/SlideRecord.h"

#include <boost/optional.hpp>

#include <memory>


class BlankTextures;
class Line;
class MeshGpuRecord;
class TexturedMesh;
class Transformation;


class SlideSlice :
        public DrawableBase,
        public ITexturable3d,
        public ObjectCounter<SlideSlice>
{
public:

    SlideSlice( std::string name,
                ShaderProgramActivatorType shaderProgramActivator,
                UniformsProviderType uniformsProvider,
                std::weak_ptr<BlankTextures> blankTextures,
                std::weak_ptr<MeshGpuRecord> sliceMeshGpuRecord,
                std::weak_ptr<SlideRecord> slideRecord,
                QuerierType<bool, UID> activeSlideQuerier,
                GetterType<float> image3dLayerOpacityProvider );

    SlideSlice( const SlideSlice& ) = delete;
    SlideSlice& operator=( const SlideSlice& ) = delete;

    ~SlideSlice() override = default;

    bool isOpaque() const override;

    DrawableOpacity opacityFlag() const override;

    void setImage3dRecord( std::weak_ptr<ImageRecord> ) override;
    void setParcellationRecord( std::weak_ptr<ParcellationRecord> ) override;
    void setImageColorMapRecord( std::weak_ptr<ImageColorMapRecord> ) override;
    void setLabelTableRecord( std::weak_ptr<LabelTableRecord> ) override;

    void setPositioningMethod(
            const SliceIntersector::PositioningMethod& method,
            const boost::optional< glm::vec3 >& p = boost::none );

    void setAlignmentMethod(
            const SliceIntersector::AlignmentMethod& method,
            const boost::optional<glm::vec3>& worldNormal = boost::none );

    void setShowOutline( bool show );

    void setUseIntensityThresolding( bool );


private:

    using PositionType = glm::vec3;
    using NormalType = uint32_t;
    using TexCoord2DType = glm::vec2;
    using VertexIndexType = uint32_t;

    void doUpdate( double, const Viewport&, const camera::Camera&, const CoordinateFrame& ) override;

    void setupChildren();

    /// Function that returns true iff the provided UID is for the active slide
    QuerierType<bool, UID> m_activeSlideQuerier;

    /// Function that returns the opacity of the 3D image layer
    GetterType<float> m_image3dLayerOpacityProvider;

    std::weak_ptr<MeshGpuRecord> m_sliceMeshGpuRecord;
    std::weak_ptr<SlideRecord> m_slideRecord;

    /// Transformation from slide to slide stack
    std::shared_ptr<Transformation> m_stack_O_slide_tx;

    /// Textured slice mesh
    std::shared_ptr<TexturedMesh> m_sliceMesh;

    /// Line outline of slice
    std::shared_ptr<Line> m_sliceOutline;

    glm::vec3 m_modelPlaneNormal;

    glm::mat4 m_clip_O_camera;
    glm::mat4 m_camera_O_world;
    glm::vec3 m_worldCameraPos;

    bool m_showOutline;

    SliceIntersector m_sliceIntersector;
};

#endif // SLIDE_SLICE_H
