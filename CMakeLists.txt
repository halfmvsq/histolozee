cmake_minimum_required( VERSION 3.1 )

cmake_policy( SET CMP0053 NEW )

project( HistoloZee2 )

# <REPLACE Qt5Root_DIR WITH THE PATH TO Qt 5.12.0+ ON YOUR SYSTEM, IF NECESSARY DUE TO MULTIPLE BUILDS>
# <REPLACE VTK_DIR WITH THE PATH TO VTK 8.0.0+ ON YOUR SYSTEM, IF NECESSARY DUE TO MULTIPLE BUILDS>


# Prevent compilation in-source
if( ${CMAKE_BINARY_DIR} STREQUAL ${PROJECT_SOURCE_DIR} )
    message( FATAL_ERROR "Source and build directories are the same." )
endif()


#--------------------------------------------------------------------------------
# Versioning
#--------------------------------------------------------------------------------

set( HZEE_APPNAME_FULL "HistoloZee" )
Set( HZEE_ORGNAME_FULL "Penn Image Computing and Science Lab, Department of Radiology, University of Pennsylvania" )

set( HZEE_VERSION_MAJOR 2 )
set( HZEE_VERSION_MINOR 0 )
set( HZEE_VERSION_PATCH 0 )

set( HZEE_VERSION_FULL "${HZEE_VERSION_MAJOR}.${HZEE_VERSION_MINOR}.${HZEE_VERSION_PATCH}" )


#--------------------------------------------------------------------------------
# Build configuration
#--------------------------------------------------------------------------------

set( CMAKE_BUILD_TYPE Release )
set( CMAKE_CONFIGURATION_TYPES "Debug;Release" )
set( CMAKE_VERBOSE_MAKEFILE ON )
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )

# In order for find_package to be successful, Qt 5 must be found below the CMAKE_PREFIX_PATH.
# Do this by setting the CMAKE_PREFIX_PATH environment variable to the install prefix of Qt 5.

# <REPLACE THIS WITH THE PATH TO QT 5.12.0+ ON YOUR SYSTEM, IF NECESSARY DUE TO MULTIPLE BUILDS>
set( CMAKE_PREFIX_PATH ${Qt5Root_DIR} )

# Instruct CMake to run moc automatically when needed
set( CMAKE_AUTOMOC ON )

# Create code from a list of Qt designer ui files
set( CMAKE_AUTOUIC ON )


# Find includes in corresponding build directories, since moc files are generated there
set( CMAKE_INCLUDE_CURRENT_DIR ON )

set( CMAKE_OSX_DEPLOYMENT_TARGET "10.10" CACHE STRING "Minimum OS X deployment version" )

# Install to "dist" directory in Windows for testing and as a staging directory
# for the installer.
#if (WIN32 AND NOT CMAKE_INSTALL_PREFIX)
#  set(CMAKE_INSTALL_PREFIX dist CACHE STRING "Install path prefix.")
#endif()


#--------------------------------------------------------------------------------
# Qt Libraries
#--------------------------------------------------------------------------------

# Find the Qt libraries. HistoloZee uses a subset of the Qt libraries:
find_package( Qt5Widgets REQUIRED )
find_package( Qt5Core REQUIRED )
find_package( Qt5Gui REQUIRED )
find_package( Qt5OpenGL REQUIRED )

# The OpenGL Extensions didn't end up being needed
# find_package( Qt5OpenGLExtensions REQUIRED )

if( Qt5Widgets_FOUND )
    if( Qt5Widgets_VERSION VERSION_LESS 5.10.0 )
        message( SEND_ERROR "Minimum supported Qt5Widgets version is 5.10.0.\nYou are using " ${Qt5Widgets_VERSION} "." )
    endif()
else()
    message( SEND_ERROR "The Qt5Widgets library could not be found!" )
endif( Qt5Widgets_FOUND )


#--------------------------------------------------------------------------------
# VTK Library
#--------------------------------------------------------------------------------

# Note: We could explicitly set the VTK components to link against here.
# e.g. COMPONENTS vtkCommonCore vtkRenderingOpenGL2 ...
find_package( VTK 8.0 REQUIRED )

if( VTK_FOUND )
    include( ${VTK_USE_FILE} )
else( VTK_FOUND )
    message( SEND_ERROR "Cannot build without VTK. Please set VTK_DIR." )
endif( VTK_FOUND )


#--------------------------------------------------------------------------------
# OpenCV Library
#--------------------------------------------------------------------------------

find_package( OpenCV 3.4.0 REQUIRED )


#--------------------------------------------------------------------------------
# Boost Library
#--------------------------------------------------------------------------------

find_package( Boost 1.66.0 COMPONENTS REQUIRED
    system filesystem program_options )

if( NOT Boost_FOUND )
    message( FATAL_ERROR "Boost was not found." )
endif()

set( Boost_USE_MULTITHREAD ON )


#--------------------------------------------------------------------------------
# OpenSlide Library
#--------------------------------------------------------------------------------

find_library( OPENSLIDE_LIB openslide )
find_path( OPENSLIDE_INC openslide )


#--------------------------------------------------------------------------------
# Header-only external libraries included as Git submodules
#--------------------------------------------------------------------------------

set( EXTERNALS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/externals )

# "JSON for C++11" by Niels Lohmann
set( JSONLOHMANN_DIR ${EXTERNALS_DIR}/nlohmann/include )

# OpenGL Mathematics (GLM)
set( GLM_DIR ${EXTERNALS_DIR}/glm )

# Common Toolkit from Kitware (CTK)
set( CTK_DIR ${EXTERNALS_DIR}/ctk )

# Earcut: A C++ port of earcut.js, a fast, header-only polygon triangulation library
set( EARCUT_DIR ${EXTERNALS_DIR}/earcut )

# VCG (Visualization and Computer Graphics) Library
# --> VCG may be used for mesh processing. It is the back-end for the tool MeshLab

#set( VCGLIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/externals/vcglib )
# Include: ${VCGLIB_DIR}


#--------------------------------------------------------------------------------
# HistoloZee's SOURCES
#--------------------------------------------------------------------------------

set( SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src )

# Configure a header file with the versioning information
configure_file(
    ${SRC_DIR}/defines.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/defines.h
    @ONLY )

# Image loading library directory:
add_subdirectory( ${SRC_DIR}/imageio )

set( HZEE_SOURCES
    ${SRC_DIR}/common/CoordinateFrame.cpp
    ${SRC_DIR}/common/UID.cpp
    ${SRC_DIR}/common/Utility.cpp
    ${SRC_DIR}/common/Viewport.cpp

    ${SRC_DIR}/gui/ActionsContainer.cpp
    ${SRC_DIR}/gui/MainWindow.cpp
    ${SRC_DIR}/gui/controls/QRealScrollBar.cpp
    ${SRC_DIR}/gui/docks/RefFrameEditorDock.cpp
    ${SRC_DIR}/gui/docks/SlideStackEditorDock.cpp
    ${SRC_DIR}/gui/docks/Utility.cpp
    ${SRC_DIR}/gui/docks/labels/DoubleSpinBoxDelegate.cpp
    ${SRC_DIR}/gui/docks/labels/LabelColorDialog.cpp
    ${SRC_DIR}/gui/docks/labels/LabelColorDialogDelegate.cpp
    ${SRC_DIR}/gui/docks/labels/LabelTableModel.cpp
    ${SRC_DIR}/gui/docks/labels/OpacitySpinBox.cpp
    ${SRC_DIR}/gui/docks/slides/PixmapDelegate.cpp
    ${SRC_DIR}/gui/docks/slides/SlideSorterTableModel.cpp
    ${SRC_DIR}/gui/layout/LayoutSerialization.cpp
    ${SRC_DIR}/gui/layout/LayoutConstruction.cpp
    ${SRC_DIR}/gui/layout/SplitterLayoutConstruction.cpp
    ${SRC_DIR}/gui/layout/ViewType.cpp
    ${SRC_DIR}/gui/toolbars/ToolBarCreation.cpp
    ${SRC_DIR}/gui/treemodel/TreeItem.cpp
    ${SRC_DIR}/gui/treemodel/TreeModel.cpp
    ${SRC_DIR}/gui/view/BorderPainter.cpp
    ${SRC_DIR}/gui/view/GLWidget.cpp
    ${SRC_DIR}/gui/view/ViewWidget.cpp
#    ${SRC_DIR}/gui/view/VTKWidget.cpp

    ${SRC_DIR}/logic/AppController.cpp
    ${SRC_DIR}/logic/AppInitializer.cpp
    ${SRC_DIR}/logic/ProgramOptions.cpp
    ${SRC_DIR}/logic/annotation/AnnotationHelper.cpp
    ${SRC_DIR}/logic/annotation/LandmarkGroupCpuRecord.cpp
    ${SRC_DIR}/logic/annotation/Polygon.cpp
    ${SRC_DIR}/logic/annotation/SlideAnnotationCpuRecord.cpp
    ${SRC_DIR}/logic/camera/OrthogonalProjection.cpp
    ${SRC_DIR}/logic/camera/PerspectiveProjection.cpp
    ${SRC_DIR}/logic/camera/Projection.cpp
    ${SRC_DIR}/logic/camera/Camera.cpp
    ${SRC_DIR}/logic/camera/CameraHelpers.cpp
    ${SRC_DIR}/logic/colormap/ImageColorMap.cpp
    ${SRC_DIR}/logic/colormap/ParcellationLabelTable.cpp
    ${SRC_DIR}/logic/data/DataHelper.cpp
    ${SRC_DIR}/logic/data/DataLoading.cpp
    ${SRC_DIR}/logic/data/DataMemoryUse.cpp
    ${SRC_DIR}/logic/data/details/DataLoadingDetails.cpp
    ${SRC_DIR}/logic/interaction/CameraInteractionHandler.cpp
    ${SRC_DIR}/logic/interaction/CrosshairsInteractionHandler.cpp
    ${SRC_DIR}/logic/interaction/InteractionHandlerBase.cpp
    ${SRC_DIR}/logic/interaction/InteractionPack.cpp
    ${SRC_DIR}/logic/interaction/RefImageInteractionHandler.cpp
    ${SRC_DIR}/logic/interaction/SlideInteractionHandler.cpp
    ${SRC_DIR}/logic/interaction/StackInteractionHandler.cpp
    ${SRC_DIR}/logic/interaction/WindowLevelInteractionHandler.cpp
    ${SRC_DIR}/logic/managers/ActionManager.cpp
    ${SRC_DIR}/logic/managers/AssemblyManager.cpp
    ${SRC_DIR}/logic/managers/ConnectionManager.cpp
    ${SRC_DIR}/logic/managers/DataManager.cpp
    ${SRC_DIR}/logic/managers/GuiManager.cpp
    ${SRC_DIR}/logic/managers/InteractionManager.cpp
    ${SRC_DIR}/logic/managers/LayoutManager.cpp
    ${SRC_DIR}/logic/managers/TransformationManager.cpp
    ${SRC_DIR}/logic/serialization/ProjectSerialization.cpp
    ${SRC_DIR}/logic/ui/ImageDataUiMapper.cpp
    ${SRC_DIR}/logic/ui/ParcellationDataUiMapper.cpp
    ${SRC_DIR}/logic/ui/SlideStackDataUiMapper.cpp
    ${SRC_DIR}/logic/ui/details/PackageHeader.cpp
    ${SRC_DIR}/logic/utility/DirectionMaps.cpp

#    ${SRC_DIR}/mesh/vcgdetails/ProcessMesh.cpp
    ${SRC_DIR}/mesh/MeshCpuRecord.cpp
    ${SRC_DIR}/mesh/MeshInfo.cpp
    ${SRC_DIR}/mesh/MeshLoading.cpp
    ${SRC_DIR}/mesh/MeshProperties.cpp
    ${SRC_DIR}/mesh/vtkdetails/MeshGeneration.cpp

    ${SRC_DIR}/rendering/ShaderNames.cpp
    ${SRC_DIR}/rendering/assemblies/AnnotationAssembly.cpp
    ${SRC_DIR}/rendering/assemblies/CameraLabelAssembly.cpp
    ${SRC_DIR}/rendering/assemblies/CrosshairsAssembly.cpp
    ${SRC_DIR}/rendering/assemblies/ImageSliceAssembly.cpp
    ${SRC_DIR}/rendering/assemblies/LandmarkAssembly.cpp
    ${SRC_DIR}/rendering/assemblies/MeshAssembly.cpp
    ${SRC_DIR}/rendering/assemblies/SlideStackAssembly.cpp
    ${SRC_DIR}/rendering/common/MeshPolygonOffset.cpp
    ${SRC_DIR}/rendering/common/ObjectIdHelper.cpp
    ${SRC_DIR}/rendering/computers/ComputerBase.cpp
    ${SRC_DIR}/rendering/computers/Polygonizer.cpp
    ${SRC_DIR}/rendering/drawables/BasicMesh.cpp
    ${SRC_DIR}/rendering/drawables/Crosshairs.cpp
    ${SRC_DIR}/rendering/drawables/DrawableBase.cpp
    ${SRC_DIR}/rendering/drawables/DynamicTransformation.cpp
    ${SRC_DIR}/rendering/drawables/ImageSlice.cpp
    ${SRC_DIR}/rendering/drawables/Line.cpp
    ${SRC_DIR}/rendering/drawables/TexturedMesh.cpp
    ${SRC_DIR}/rendering/drawables/Transformation.cpp
    ${SRC_DIR}/rendering/drawables/annotation/AnnotationExtrusion.cpp
    ${SRC_DIR}/rendering/drawables/annotation/AnnotationSlice.cpp
    ${SRC_DIR}/rendering/drawables/annotation/LandmarkGroup3d.cpp
#    ${SRC_DIR}/rendering/drawables/annotation/LandmarkGroupSlice.cpp
    ${SRC_DIR}/rendering/drawables/ddp/DdpBlendPassQuad.cpp
    ${SRC_DIR}/rendering/drawables/ddp/DdpFinalPassQuad.cpp
    ${SRC_DIR}/rendering/drawables/ddp/FullScreenDebugQuad.cpp
    ${SRC_DIR}/rendering/drawables/ddp/FullScreenQuad.cpp
    ${SRC_DIR}/rendering/drawables/overlays/CameraLabel.cpp
    ${SRC_DIR}/rendering/drawables/slides/SlideBox.cpp
    ${SRC_DIR}/rendering/drawables/slides/SlideSlice.cpp
    ${SRC_DIR}/rendering/drawables/slides/SlideStackArrow.cpp
    ${SRC_DIR}/rendering/renderers/DepthPeelRenderer.cpp
    ${SRC_DIR}/rendering/records/ImageGpuRecord.cpp
    ${SRC_DIR}/rendering/records/MeshGpuRecord.cpp
    ${SRC_DIR}/rendering/records/SlideAnnotationGpuRecord.cpp
    ${SRC_DIR}/rendering/records/SlideGpuRecord.cpp
    ${SRC_DIR}/rendering/utility/CreateGLObjects.cpp
    ${SRC_DIR}/rendering/utility/containers/BlankTextures.cpp
    ${SRC_DIR}/rendering/utility/containers/ShaderProgramContainer.cpp
    ${SRC_DIR}/rendering/utility/containers/Uniforms.cpp
    ${SRC_DIR}/rendering/utility/containers/VertexAttributeInfo.cpp
    ${SRC_DIR}/rendering/utility/containers/VertexIndicesInfo.cpp
    ${SRC_DIR}/rendering/utility/gl/GLBufferObject.cpp
    ${SRC_DIR}/rendering/utility/gl/GLBufferTexture.cpp
    ${SRC_DIR}/rendering/utility/gl/GLErrorChecker.cpp
    ${SRC_DIR}/rendering/utility/gl/GLFrameBufferObject.cpp
    ${SRC_DIR}/rendering/utility/gl/GLSamplerObject.cpp
    ${SRC_DIR}/rendering/utility/gl/GLShader.cpp
    ${SRC_DIR}/rendering/utility/gl/GLShaderInfo.cpp
    ${SRC_DIR}/rendering/utility/gl/GLShaderProgram.cpp
    ${SRC_DIR}/rendering/utility/gl/GLTexture.cpp
    ${SRC_DIR}/rendering/utility/gl/GLVersionChecker.cpp
    ${SRC_DIR}/rendering/utility/gl/GLVertexArrayObject.cpp
    ${SRC_DIR}/rendering/utility/math/MathUtility.cpp
    ${SRC_DIR}/rendering/utility/math/SliceIntersector.cpp
    ${SRC_DIR}/rendering/utility/vtk/PolyDataConversion.cpp
    ${SRC_DIR}/rendering/utility/vtk/PolyDataGenerator.cpp

    ${SRC_DIR}/slideio/SlideAssociatedImages.cpp
    ${SRC_DIR}/slideio/SlideCpuRecord.cpp
    ${SRC_DIR}/slideio/SlideHeader.cpp
    ${SRC_DIR}/slideio/SlideHelper.cpp
    ${SRC_DIR}/slideio/SlideProperties.cpp
    ${SRC_DIR}/slideio/SlideReading.cpp
    ${SRC_DIR}/slideio/SlideTransformation.cpp )

set( HZEE_HEADERS
    ${SRC_DIR}/common/AABB.h
    ${SRC_DIR}/common/CoordinateFrame.h
    ${SRC_DIR}/common/CoordinateFrameLinkingType.h
    ${SRC_DIR}/common/HZeeException.hpp
    ${SRC_DIR}/common/Identity.h
    ${SRC_DIR}/common/JSONSerializers.hpp
    ${SRC_DIR}/common/ObjectCounter.hpp
    ${SRC_DIR}/common/PublicTypes.h
    ${SRC_DIR}/common/RangeTypes.h
    ${SRC_DIR}/common/ThrowAssert.hpp
    ${SRC_DIR}/common/UID.h
    ${SRC_DIR}/common/UIDRange.h
    ${SRC_DIR}/common/Utility.hpp
    ${SRC_DIR}/common/Viewport.h

    ${SRC_DIR}/gui/ActionsContainer.h
    ${SRC_DIR}/gui/MainWindow.h
    ${SRC_DIR}/gui/controls/QRealScrollBar.h
    ${SRC_DIR}/gui/docks/RefFrameEditorDock.h
    ${SRC_DIR}/gui/docks/PublicSlideTypes.h
    ${SRC_DIR}/gui/docks/PublicTypes.h
    ${SRC_DIR}/gui/docks/SlideStackEditorDock.h
    ${SRC_DIR}/gui/docks/Utility.h
    ${SRC_DIR}/gui/docks/labels/DoubleSpinBoxDelegate.h
    ${SRC_DIR}/gui/docks/labels/LabelColorDialog.h
    ${SRC_DIR}/gui/docks/labels/LabelColorDialogDelegate.h
    ${SRC_DIR}/gui/docks/labels/LabelTableModel.h
    ${SRC_DIR}/gui/docks/labels/OpacitySpinBox.h
    ${SRC_DIR}/gui/docks/slides/PixmapDelegate.h
    ${SRC_DIR}/gui/docks/slides/SlideSorterTableModel.h
    ${SRC_DIR}/gui/layout/LayoutData.h
    ${SRC_DIR}/gui/layout/LayoutConstruction.h
    ${SRC_DIR}/gui/layout/LayoutSerialization.h
    ${SRC_DIR}/gui/layout/SplitterLayoutConstruction.h
    ${SRC_DIR}/gui/layout/ViewType.h
    ${SRC_DIR}/gui/layout/ViewTypeRange.h
    ${SRC_DIR}/gui/messages/image/ImageColorMapData.h
    ${SRC_DIR}/gui/messages/image/ImagePropertyData.h
    ${SRC_DIR}/gui/messages/image/ImageSelectionData.h
    ${SRC_DIR}/gui/messages/image/ImageTransformationData.h
    ${SRC_DIR}/gui/messages/landmark/LandmarkData.h
    ${SRC_DIR}/gui/messages/landmark/LandmarkGroupData.h
    ${SRC_DIR}/gui/messages/parcellation/ParcellationLabel.h
    ${SRC_DIR}/gui/messages/parcellation/ParcellationLabelData.h
    ${SRC_DIR}/gui/messages/parcellation/ParcellationPropertyData.h
    ${SRC_DIR}/gui/messages/parcellation/ParcellationSelectionData.h
    ${SRC_DIR}/gui/messages/slide/MoveToSlide.h
    ${SRC_DIR}/gui/messages/slide/SlideCommonProperties.h
    ${SRC_DIR}/gui/messages/slide/SlideData.h
    ${SRC_DIR}/gui/messages/slide/SlidePreview.h
    ${SRC_DIR}/gui/messages/slide/SlideStackData.h
    ${SRC_DIR}/gui/toolbars/ToolBarCreation.h
    ${SRC_DIR}/gui/treemodel/TreeItem.h
    ${SRC_DIR}/gui/treemodel/TreeModel.h
    ${SRC_DIR}/gui/view/BorderPainter.h
    ${SRC_DIR}/gui/view/GLWidget.h
    ${SRC_DIR}/gui/view/ViewSliderParams.h
    ${SRC_DIR}/gui/view/ViewWidget.h
#    ${SRC_DIR}/gui/view/VTKWidget.h

    ${SRC_DIR}/logic/AppController.h
    ${SRC_DIR}/logic/AppInitializer.h
    ${SRC_DIR}/logic/CrosshairsType.h
    ${SRC_DIR}/logic/ProgramOptions.h
    ${SRC_DIR}/logic/RenderableRecord.h
    ${SRC_DIR}/logic/TransformationState.h
    ${SRC_DIR}/logic/annotation/AnnotationHelper.h
    ${SRC_DIR}/logic/annotation/LandmarkGroupCpuRecord.h
    ${SRC_DIR}/logic/annotation/PointList.h
    ${SRC_DIR}/logic/annotation/PointRecord.h
    ${SRC_DIR}/logic/annotation/Polygon.h
    ${SRC_DIR}/logic/annotation/SlideAnnotationCpuRecord.h
    ${SRC_DIR}/logic/camera/Camera.h
    ${SRC_DIR}/logic/camera/CameraHelpers.h
    ${SRC_DIR}/logic/camera/CameraStartFrameType.h
    ${SRC_DIR}/logic/camera/CameraTypes.h
    ${SRC_DIR}/logic/camera/OrthogonalProjection.h
    ${SRC_DIR}/logic/camera/PerspectiveProjection.h
    ${SRC_DIR}/logic/camera/Projection.h
    ${SRC_DIR}/logic/colormap/ImageColorMap.h
    ${SRC_DIR}/logic/colormap/ParcellationLabelTable.h
    ${SRC_DIR}/logic/data/DataHelper.h
    ${SRC_DIR}/logic/data/DataLoading.h
    ${SRC_DIR}/logic/data/DataMemoryUse.h
    ${SRC_DIR}/logic/data/details/DataLoadingDetails.h
    ${SRC_DIR}/logic/interaction/CameraInteractionHandler.h
    ${SRC_DIR}/logic/interaction/CrosshairsInteractionHandler.h
    ${SRC_DIR}/logic/interaction/RefImageInteractionHandler.h
    ${SRC_DIR}/logic/interaction/StackInteractionHandler.h
    ${SRC_DIR}/logic/interaction/SlideInteractionHandler.h
    ${SRC_DIR}/logic/interaction/WindowLevelInteractionHandler.h
    ${SRC_DIR}/logic/interaction/InteractionHandlerBase.h
    ${SRC_DIR}/logic/interaction/InteractionHandlerType.h
    ${SRC_DIR}/logic/interaction/InteractionModes.h
    ${SRC_DIR}/logic/interaction/InteractionPack.h
    ${SRC_DIR}/logic/interfaces/IGestureHandler.h
    ${SRC_DIR}/logic/interfaces/IInteractionHandler.h
    ${SRC_DIR}/logic/interfaces/IMouseEventHandler.h
    ${SRC_DIR}/logic/interfaces/ITabletEventHandler.h
    ${SRC_DIR}/logic/interfaces/IWheelEventHandler.h
    ${SRC_DIR}/logic/managers/ActionManager.h
    ${SRC_DIR}/logic/managers/AssemblyManager.h
    ${SRC_DIR}/logic/managers/ConnectionManager.h
    ${SRC_DIR}/logic/managers/DataManager.h
    ${SRC_DIR}/logic/managers/GuiManager.h
    ${SRC_DIR}/logic/managers/InteractionManager.h
    ${SRC_DIR}/logic/managers/LayoutManager.h
    ${SRC_DIR}/logic/managers/TransformationManager.h
    ${SRC_DIR}/logic/records/ImageColorMapRecord.h
    ${SRC_DIR}/logic/records/ImageRecord.h
    ${SRC_DIR}/logic/records/LabelTableRecord.h
    ${SRC_DIR}/logic/records/LandmarkGroupRecord.h
    ${SRC_DIR}/logic/records/MeshRecord.h
    ${SRC_DIR}/logic/records/ParcellationRecord.h
    ${SRC_DIR}/logic/records/SlideAnnotationRecord.h
    ${SRC_DIR}/logic/records/SlideRecord.h
    ${SRC_DIR}/logic/serialization/ProjectSerialization.h
    ${SRC_DIR}/logic/ui/ImageDataUiMapper.h
    ${SRC_DIR}/logic/ui/ParcellationDataUiMapper.h
    ${SRC_DIR}/logic/ui/SlideStackDataUiMapper.h
    ${SRC_DIR}/logic/ui/details/PackageHeader.h
    ${SRC_DIR}/logic/utility/DirectionMaps.h

    ${SRC_DIR}/mesh/MeshCpuRecord.h
    ${SRC_DIR}/mesh/MeshInfo.hpp
    ${SRC_DIR}/mesh/MeshLoading.h
    ${SRC_DIR}/mesh/MeshProperties.h
    ${SRC_DIR}/mesh/MeshTypes.h
    ${SRC_DIR}/mesh/vtkdetails/ErrorObserver.hpp
    ${SRC_DIR}/mesh/vtkdetails/MeshGeneration.hpp

    ${SRC_DIR}/rendering/ShaderNames.h
    ${SRC_DIR}/rendering/assemblies/AnnotationAssembly.h
    ${SRC_DIR}/rendering/assemblies/CameraLabelAssembly.h
    ${SRC_DIR}/rendering/assemblies/CrosshairsAssembly.h
    ${SRC_DIR}/rendering/assemblies/ImageSliceAssembly.h
    ${SRC_DIR}/rendering/assemblies/LandmarkAssembly.h
    ${SRC_DIR}/rendering/assemblies/MeshAssembly.h
    ${SRC_DIR}/rendering/assemblies/RenderingProperties.h
    ${SRC_DIR}/rendering/assemblies/SlideStackAssembly.h
    ${SRC_DIR}/rendering/common/AccumulatedRenderingData.h
    ${SRC_DIR}/rendering/common/DrawableOpacity.h
    ${SRC_DIR}/rendering/common/DrawableScaling.h
    ${SRC_DIR}/rendering/common/MeshColorLayer.h
    ${SRC_DIR}/rendering/common/MeshPolygonOffset.h
    ${SRC_DIR}/rendering/common/NamedColors.h
    ${SRC_DIR}/rendering/common/ObjectIdHelper.h
    ${SRC_DIR}/rendering/common/SceneType.h
    ${SRC_DIR}/rendering/common/ShaderProviderType.h
    ${SRC_DIR}/rendering/common/ShaderStageTypes.h
    ${SRC_DIR}/rendering/common/TransformationTypes.h
    ${SRC_DIR}/rendering/interfaces/IComputer.h
    ${SRC_DIR}/rendering/interfaces/IDrawable.h
    ${SRC_DIR}/rendering/interfaces/IDrawableAssembly.h
    ${SRC_DIR}/rendering/interfaces/IRenderer.h
    ${SRC_DIR}/rendering/interfaces/ITexturable3D.h
    ${SRC_DIR}/rendering/computers/ComputerBase.h
    ${SRC_DIR}/rendering/computers/Polygonizer.h
    ${SRC_DIR}/rendering/drawables/BasicMesh.h
    ${SRC_DIR}/rendering/drawables/Crosshairs.h
    ${SRC_DIR}/rendering/drawables/DrawableBase.h
    ${SRC_DIR}/rendering/drawables/DynamicTransformation.h
    ${SRC_DIR}/rendering/drawables/ImageSlice.h
    ${SRC_DIR}/rendering/drawables/Line.h
    ${SRC_DIR}/rendering/drawables/TexturedMesh.h
    ${SRC_DIR}/rendering/drawables/Transformation.h
    ${SRC_DIR}/rendering/drawables/annotation/AnnotationExtrusion.h
    ${SRC_DIR}/rendering/drawables/annotation/AnnotationSlice.h
    ${SRC_DIR}/rendering/drawables/annotation/LandmarkGroup3d.h
#    ${SRC_DIR}/rendering/drawables/annotation/LandmarkGroupSlice.h
    ${SRC_DIR}/rendering/drawables/ddp/DdpBlendPassQuad.h
    ${SRC_DIR}/rendering/drawables/ddp/DdpFinalPassQuad.h
    ${SRC_DIR}/rendering/drawables/ddp/FullScreenDebugQuad.h
    ${SRC_DIR}/rendering/drawables/ddp/FullScreenQuad.h
    ${SRC_DIR}/rendering/drawables/overlays/CameraLabel.h
    ${SRC_DIR}/rendering/drawables/slides/SlideBox.h
    ${SRC_DIR}/rendering/drawables/slides/SlideSlice.h
    ${SRC_DIR}/rendering/drawables/slides/SlideStackArrow.h
    ${SRC_DIR}/rendering/records/EmptyGpuRecord.h
    ${SRC_DIR}/rendering/records/ImageGpuRecord.h
    ${SRC_DIR}/rendering/records/MeshGpuRecord.h
    ${SRC_DIR}/rendering/records/SlideAnnotationGpuRecord.h
    ${SRC_DIR}/rendering/records/SlideGpuRecord.h
    ${SRC_DIR}/rendering/renderers/DepthPeelRenderer.h
    ${SRC_DIR}/rendering/utility/CreateGLObjects.h
    ${SRC_DIR}/rendering/utility/UnderlyingEnumType.h
    ${SRC_DIR}/rendering/utility/containers/BlankTextures.h
    ${SRC_DIR}/rendering/utility/containers/ShaderProgramContainer.h
    ${SRC_DIR}/rendering/utility/containers/Uniforms.h
    ${SRC_DIR}/rendering/utility/containers/VertexAttributeInfo.h
    ${SRC_DIR}/rendering/utility/containers/VertexIndicesInfo.h
    ${SRC_DIR}/rendering/utility/gl/GLBufferObject.h
    ${SRC_DIR}/rendering/utility/gl/GLBufferTexture.h
    ${SRC_DIR}/rendering/utility/gl/GLBufferTypes.h
    ${SRC_DIR}/rendering/utility/gl/GLDrawTypes.h
    ${SRC_DIR}/rendering/utility/gl/GLErrorChecker.h
    ${SRC_DIR}/rendering/utility/gl/GLFBOAttachmentTypes.h
    ${SRC_DIR}/rendering/utility/gl/GLFrameBufferObject.h
    ${SRC_DIR}/rendering/utility/gl/GLSamplerObject.h
    ${SRC_DIR}/rendering/utility/gl/GLShader.h
    ${SRC_DIR}/rendering/utility/gl/GLShaderInfo.h
    ${SRC_DIR}/rendering/utility/gl/GLShaderProgram.h
    ${SRC_DIR}/rendering/utility/gl/GLShaderType.h
    ${SRC_DIR}/rendering/utility/gl/GLTexture.h
    ${SRC_DIR}/rendering/utility/gl/GLTextureTypes.h
    ${SRC_DIR}/rendering/utility/gl/GLUniformTypes.h
    ${SRC_DIR}/rendering/utility/gl/GLVersionChecker.h
    ${SRC_DIR}/rendering/utility/gl/GLVertexArrayObject.h
    ${SRC_DIR}/rendering/utility/math/MathUtility.h
    ${SRC_DIR}/rendering/utility/math/SliceIntersector.h
    ${SRC_DIR}/rendering/utility/vtk/PolyDataConversion.h
    ${SRC_DIR}/rendering/utility/vtk/PolyDataGenerator.h
    ${SRC_DIR}/rendering/utility/vtk/VectorArrayBuffer.h

    ${SRC_DIR}/slideio/SlideAssociatedImages.h
    ${SRC_DIR}/slideio/SlideCpuRecord.h
    ${SRC_DIR}/slideio/SlideHeader.h
    ${SRC_DIR}/slideio/SlideHelper.h
    ${SRC_DIR}/slideio/SlideLevel.h
    ${SRC_DIR}/slideio/SlideProperties.h
    ${SRC_DIR}/slideio/SlideReading.h
    ${SRC_DIR}/slideio/SlideTransformation.h )

set( HZEE_SHADERS
    ${SRC_DIR}/rendering/shaders/BasicMesh.vert
    ${SRC_DIR}/rendering/shaders/BasicMesh.frag
    ${SRC_DIR}/rendering/shaders/BasicMeshPeel.frag
    ${SRC_DIR}/rendering/shaders/Debug.vert
    ${SRC_DIR}/rendering/shaders/Debug.frag
    ${SRC_DIR}/rendering/shaders/fbo_screen.vert
    ${SRC_DIR}/rendering/shaders/fbo_screen.frag
    ${SRC_DIR}/rendering/shaders/Flat.vert
    ${SRC_DIR}/rendering/shaders/Flat.frag
    ${SRC_DIR}/rendering/shaders/FlatPeel.frag
    ${SRC_DIR}/rendering/shaders/Mesh.vert
    ${SRC_DIR}/rendering/shaders/Mesh.frag
    ${SRC_DIR}/rendering/shaders/MeshPeel.frag
    ${SRC_DIR}/rendering/shaders/Polygonizer.vert
    ${SRC_DIR}/rendering/shaders/Polygonizer.geom
    ${SRC_DIR}/rendering/shaders/Simple.vert
    ${SRC_DIR}/rendering/shaders/Simple.frag
    ${SRC_DIR}/rendering/shaders/ddp/blend.frag
    ${SRC_DIR}/rendering/shaders/ddp/blend.vert
    ${SRC_DIR}/rendering/shaders/ddp/final.frag
    ${SRC_DIR}/rendering/shaders/ddp/InitializeDepths.frag
    ${SRC_DIR}/rendering/shaders/ddp/init.vert )

set( EXTERNAL_SOURCES
    ${CTK_DIR}/ctkUtils.cpp
    ${CTK_DIR}/Widgets/ctkActionsWidget.cpp
    ${CTK_DIR}/Widgets/ctkAddRemoveComboBox.cpp
    ${CTK_DIR}/Widgets/ctkCollapsibleGroupBox.cpp
    ${CTK_DIR}/Widgets/ctkDoubleRangeSlider.cpp
    ${CTK_DIR}/Widgets/ctkDoubleSlider.cpp
    ${CTK_DIR}/Widgets/ctkDoubleSpinBox.cpp
    ${CTK_DIR}/Widgets/ctkMatrixWidget.cpp
    ${CTK_DIR}/Widgets/ctkPathLineEdit.cpp
    ${CTK_DIR}/Widgets/ctkRangeSlider.cpp
    ${CTK_DIR}/Widgets/ctkProxyStyle.cpp )

set( EXTERNAL_HEADERS
    ${CTK_DIR}/ctkUtils.h
    ${CTK_DIR}/Widgets/ctkActionsWidget.h
    ${CTK_DIR}/Widgets/ctkAddRemoveComboBox.h
    ${CTK_DIR}/Widgets/ctkCollapsibleGroupBox.h
    ${CTK_DIR}/Widgets/ctkDoubleRangeSlider.h
    ${CTK_DIR}/Widgets/ctkDoubleSlider.h
    ${CTK_DIR}/Widgets/ctkDoubleSpinBox.h
    ${CTK_DIR}/Widgets/ctkMatrixWidget.h
    ${CTK_DIR}/Widgets/ctkPathLineEdit.h
    ${CTK_DIR}/Widgets/ctkRangeSlider.h
    ${CTK_DIR}/Widgets/ctkProxyStyle.h
    ${EARCUT_DIR}/include/mapbox/earcut.hpp )

QT5_ADD_RESOURCES( HZEE_QT_RESOURCES
    ${SRC_DIR}/resources/colormaps/colormaps.qrc
    ${SRC_DIR}/resources/icons/icons.qrc
    ${SRC_DIR}/resources/letters/letters.qrc
    ${SRC_DIR}/resources/stylesheets/breeze.qrc
    ${SRC_DIR}/resources/toolbars/toolbars.qrc )

QT5_WRAP_UI( HZEE_UI_HEADERS
    ${CTK_DIR}/Widgets/ctkAddRemoveComboBox.ui )


#--------------------------------------------------------------------------------
# Build types and compile options
#--------------------------------------------------------------------------------

set( default_build_type "Release" )

if( NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES )
    message( STATUS "Setting build type to '${default_build_type}' as none was specified." )

    set( CMAKE_BUILD_TYPE "${default_build_type}" CACHE
        STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE )

    # Set the possible values of build type for cmake-gui
    set_property( CACHE CMAKE_BUILD_TYPE PROPERTY
        STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo" )
endif()


if( CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
    add_compile_options(
        -Wall -Wextra -Wfloat-equal -Wpointer-arith -Winit-self
        -Wunreachable-code -Werror-implicit-function-declaration -Wshadow
        -g -ftrapv -march=native
        #-Werror
        #-pedantic-errors
        #-fmax-errors=0
        )
endif()


# Todo: remove CMAKE_CXX_FLAGS here
set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O3 -fstack-protector-all" )

message( "CMAKE_CXX_FLAGS_DEBUG is ${CMAKE_CXX_FLAGS_DEBUG}" )
message( "CMAKE_CXX_FLAGS_RELEASE is ${CMAKE_CXX_FLAGS_RELEASE}" )


#--------------------------------------------------------------------------------
# Define main executable target
#--------------------------------------------------------------------------------

# ${OS_BUNDLE} Expands to WIN32 or MACOS_BUNDLE depending on OS
add_executable( ${PROJECT_NAME} WIN32 MACOSX_BUNDLE
    ${SRC_DIR}/main.cpp
    ${HZEE_UI_HEADERS} )

target_sources( ${PROJECT_NAME} PRIVATE
    ${HZEE_SOURCES}
    ${HZEE_HEADERS}
    ${HZEE_SHADERS}
    ${EXTERNAL_SOURCES}
    ${EXTERNAL_HEADERS}
    ${HZEE_QT_RESOURCES} )

target_link_libraries( ${PROJECT_NAME} PRIVATE
    Qt5::Widgets
    Qt5::OpenGL
    ${VTK_LIBRARIES}
    ${Boost_LIBRARIES}
    ${OPENSLIDE_LIB}
    ${OpenCV_LIBS}
    HZeeImageIO )

target_include_directories( ${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR} # defines.h is here
    ${SRC_DIR}
    ${SRC_DIR}/imageio
    ${Boost_INCLUDE_DIR}
    ${OpenCV_INCLUDE_DIRS}
    ${OPENSLIDE_INC}
    ${EXTERNALS_DIR}
    ${JSONLOHMANN_DIR}
    ${GLM_DIR}
    ${CTK_DIR} )

target_include_directories( ${PROJECT_NAME} PRIVATE SYSTEM
    ${VTK_INCLUDE_DIRS} )

target_compile_definitions( ${PROJECT_NAME} PRIVATE
    ${VTK_DEFINITIONS} )

set_target_properties( ${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
    AUTOMOC ON )


#--------------------------------------------------------------------------------
# Packaging
#--------------------------------------------------------------------------------

# This section is not yet complete.
# We need to run DeployQt5.cmake, CPack, and more.

# On Apple, the custom Info.plist file is needed
# Note: could modify MacOSXBundleInfo.plist.in

if( APPLE )
   set( MACOSX_BUNDLE_ICON_FILE HistoloZeeIcon.icns )

   set( HZEE_OSX_ICON ${SRC_DIR}/resources/icons/HistoloZeeIcons.icns )
   set_source_files_properties( ${HZEE_OSX_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION Resources )

   set_target_properties( ${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST
      ${SRC_DIR}/resources/macos/Info.plist )
endif( APPLE )
