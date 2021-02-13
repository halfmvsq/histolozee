#ifndef SLIDE_H
#define SLIDE_H

#include "rendering/drawables/DrawableBase.h"
#include "rendering/interfaces/ITexturable3D.h"
#include "rendering/common/MeshColorLayer.h"
#include "rendering/common/ShaderProviderType.h"

#include "common/ObjectCounter.hpp"
#include "common/PublicTypes.h"
#include "common/UID.h"

#include "logic/records/SlideRecord.h"

#include <memory>


class BlankTextures;
class MeshGpuRecord;
class TexturedMesh;
class Transformation;


class SlideBox :
        public DrawableBase,
        public ITexturable3d,
        public ObjectCounter<SlideBox>
{
public:

    SlideBox( std::string name,
              ShaderProgramActivatorType shaderProgramActivator,
              UniformsProviderType uniformsProvider,
              std::weak_ptr<BlankTextures> blankTextures,
              std::weak_ptr<MeshGpuRecord> boxMeshGpuRecord,
              std::weak_ptr<SlideRecord> slideRecord,
              QuerierType<bool, UID> activeSlideQuerier,
              GetterType<float> image3dLayerOpacityProvider );

    ~SlideBox() override = default;

    bool isOpaque() const override;

    DrawableOpacity opacityFlag() const override;

    void setImage3dRecord( std::weak_ptr<ImageRecord> ) override;
    void setParcellationRecord( std::weak_ptr<ParcellationRecord> ) override;
    void setImageColorMapRecord( std::weak_ptr<ImageColorMapRecord> ) override;
    void setLabelTableRecord( std::weak_ptr<LabelTableRecord> ) override;

    void setUseIntensityThresolding( bool );


private:

    void setupChildren();

    void doUpdate( double time, const Viewport&, const camera::Camera&, const CoordinateFrame& ) override;

    /// Function that returns true iff the provided UID is for the active slide
    QuerierType<bool, UID> m_activeSlideQuerier;

    /// Function that returns the opacity of the 3D image layer
    GetterType<float> m_image3dLayerOpacityProvider;

    std::weak_ptr<MeshGpuRecord> m_boxMeshGpuRecord;

    std::weak_ptr<SlideRecord> m_slideRecord;

    std::shared_ptr<Transformation> m_stack_O_slide_tx;
    std::shared_ptr<TexturedMesh> m_boxMesh;
};

#endif // SLIDE_H
