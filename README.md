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
    "referenceImages": [
        {
            "fileName": "data/template.nii.gz",
            "subjectToWorldRotation": {
                "w": 0.0,
                "x": 0.0,
                "y": 0.0,
                "z": 0.0
            },
            "worldSubjectOrigin": {
                "x": 0.0,
                "y": 0.0,
                "z": 0.0
            }

        }
    ],
    "parcellations": [
        {
            "displayName": "",
            "fileName": "data/template_parcellation.nii.gz",
            "subjectToWorldRotation": {
                "w": 0.0,
                "x": 0.0,
                "y": 0.0,
                "z": 0.0
            },
            "worldSubjectOrigin": {
                "x": 0.0,
                "y": 0.0,
                "z": 0.0
            }
        }
    ],
    "slides": [
        {
            "fileName": "data/slide1.tif"
        },
        {
            "fileName": "data/slide2.tif"
        },
        {
            "fileName": "data/slide3.tif"
        },
        {
            "fileName": "data/slide4.tif"
        }
    ]
}
```
