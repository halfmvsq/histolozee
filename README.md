# HistoloZee #

Welcome to the *next version* of [HistoloZee](http://picsl.upenn.edu/software/histolozee)! Check out the original version's [tutorials on YouTube](https://www.youtube.com/playlist?list=PL68v8FP_IVlg2tCgJXrsO3UHve1q9wLjI).

Copyright 2021 Penn Image Computing and Science Lab, University of Pennsylvania


### How do I get set up? ###

C++17, CMake, and the following libraries are required:

* Boost 1.66.0
* ITK 5.1.0
* OpenCV 3.4.0
* OpenGL 3.3
* OpenSlide 3.4.1
* Qt 5.12.0
* VTK 8.0.0

Flags used for compiling VTK:

* Module_vtkGUISupportQt=ON
* Module_vtkGUISupportQtOpenGL=ON
* Module_vtkGUISupportQtSQL=ON
* Module_vtkRenderingQt=ON
* Module_vtkViewsQt=ON
* VTK_RENDERING_BACKEND=OpenGL
* VTK_QT_VERSION=5

The following header-only libraries are included as Git submodules:

* Earcut.hpp (https://github.com/mapbox/earcut.hpp.git)
* GLM (https://github.com/g-truc/glm.git)
* "JSON for Modern C++" (https://github.com/nlohmann/json.git)

To get the external repositories, run`git submodule update --init --recursive` after cloning.

Components of the following libraries and resources are copied into this repository:

* CTK (https://github.com/commontk/CTK.git)
* "Library of Perceptually Uniform Colour Maps" by Peter Kovesi (https://colorcet.com)


### Running HistoloZee

HistoloZee project is currently building and running on macOS (clang-1100.0.33.12). There are known graphics problems when running on Linux that are most likely due to underlying differences between rendering implementations by Qt and/or the windowing systems across platforms. In particular, different QOpenGLWidget views seem to share the same OpenGL context on macOS, but not on Linux. We intend to address these issues by creating separate vertex array objects for each view.

A sample project file is shown below. Load the project file path as the first positional argument to HistoloZee.

```JSON
{
  "activeReferenceImage": 0,
  "referenceImages": [
    {
      "fileName": "average_template.nii.gz",

      "displaySettings": {
        "displayName": "average_template",
        "interpolation": "NearestNeighbor",
        "level": 125.0,
        "opacity": 1.0,
        "thresholdHigh": 516.0,
        "thresholdLow": 0.0,
        "window": 251.0
      },

      "world_T_subject": {
        "subjectToWorldQuaternion": {
          "w": 1.0,
          "x": 0.0,
          "y": 0.0,
          "z": 0.0
        },
        "worldOrigin": {
          "x": 0.0,
          "y": 0.0,
          "z": 0.0
        }
      }
    }
  ],

  "activeParcellation": 0,
  "parcellations": [
    {
      "fileName": "template_parcellation.nii.gz",

      "displaySettings": {
        "displayName": "template_parcellation",
        "opacity": 0.5
      },

      "world_T_subject": {
        "subjectToWorldQuaternion": {
          "w": 1.0,
          "x": 0.0,
          "y": 0.0,
          "z": 0.0
        },
        "worldOrigin": {
          "x": 0.0,
          "y": 0.0,
          "z": 0.0
        }
      }
    }
  ],

  "slides": [
    {
      "fileName": "slide1.tif",

      "displaySettings": {
        "borderColor": {
          "x": 0.0,
          "y": 0.5,
          "z": 1.0
        },
        "displayName": "slide1",
        "opacity": 1.0,
        "thresholdHigh": 255,
        "thresholdLow": 0,
        "visible": true
      },

      "slideStack_T_slide": {
        "normalizedRotationCenterXY": {
          "x": 0.5,
          "y": 0.5
        },
        "normalizedTranslationXY": {
          "x": 0.0,
          "y": 0.0
        },
        "rotationAngleZ": 0.0,
        "scaleFactorsXY": {
          "x": 1.0,
          "y": 1.0
        },
        "shearAnglesXY": {
          "x": 0.0,
          "y": 0.0
        },
        "stackTranslationZ": 0.353
      }
    },
    
    {
      "fileName": "slide2.tif",

      "slideStack_T_slide": {
        "normalizedRotationCenterXY": {
          "x": 0.5,
          "y": 0.5
        },
        "normalizedTranslationXY": {
          "x": 0.0,
          "y": 0.0
        },
        "rotationAngleZ": 0.0,
        "scaleFactorsXY": {
          "x": 1.0,
          "y": 1.0
        },
        "shearAnglesXY": {
          "x": 0.0,
          "y": 0.0
        },
        "stackTranslationZ": 0.529
      }
    },
    
    {
      "fileName": "slide3.tif",
      
      "slideStack_T_slide": {
        "stackTranslationZ": 0.706
      }
    },
    
    {
      "fileName": "slide4.tif",
      
      "slideStack_T_slide": {
        "stackTranslationZ": 0.882
      }
    }
  ],

  "world_T_slideStack": {
    "subjectToWorldQuaternion": {
      "w": 1.0,
      "x": 0.0,
      "y": 0.0,
      "z": 0.0
    },
    "worldOrigin": {
      "x": 0.0,
      "y": 0.0,
      "z": 0.0
    }
  }
}
```

The top-level fields `activeReferenceImage`, `activeParcellation`, `parcellations`, `slides`, and `world_T_slideStack` are optional. If the transformation for an image, parcellation, slide, or for the slide stack is not provided, then identity is assumed. Similarly, if any display setting for an image, parcellation, or slide is not defined, then the default is assumed.

All transformations are defined in "World" coordinates (usually millimeters). An origin position in World space and a quaternion rotation are used to defined the rigid-body transformation to World space for both images and the slide stack.


### Acknowledgements

The following toolbar icons are used from [Icons8](https://icons8.com/license):

* [3D Rotate icon](https://icons8.com/icons/set/3d-rotate)
* [Automatic Contrast icon](https://icons8.com/icons/set/automatic-contrast)
* [Customer icon](https://icons8.com/icons/set/gender-neutral-user)
* [Expand icon](https://icons8.com/icons/set/expand)
* [Fit to Page icon](https://icons8.com/icons/set/fit-to-page)
* [Hand icon](https://icons8.com/icons/set/hand)
* [Head Profile icon](https://icons8.com/icons/set/head-profile)
* [Manual Page Rotation icon](https://icons8.com/icons/set/manual-page-rotation)
* [Move icon](https://icons8.com/icons/set/move)
* [Move Selection To Previous icon](https://icons8.com/icons/set/move-selection-to-previous)
* [Portraits icon](https://icons8.com/icons/set/portraits)
* [Resize icon](https://icons8.com/icons/set/resize)
* [Rotate icon](https://icons8.com/icons/set/rotate)
* [Rotate Camera icon](https://icons8.com/icons/set/rotate-camera)
* [Rotate Left icon](https://icons8.com/icons/set/rotate-left)
* [Ruler Combined icon](https://icons8.com/icons/set/ruler-combined)
* [Save icon](https://icons8.com/icons/set/save--v1)
* [Save As icon](https://icons8.com/icons/set/save-as)
* [Stack Of Paper icon](https://icons8.com/icons/set/stack-of-paper)
* [Stretch Uniform to Fill icon](https://icons8.com/icons/set/stretch-uniform-to-fill)
* [Target icon](https://icons8.com/icons/set/define-location)
* [Zoom In icon](https://icons8.com/icons/set/zoom-in--v1)

