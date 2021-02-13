# HistoloZee 2 #

Welcome to the next generation [HistoloZee](http://picsl.upenn.edu/software/histolozee)!

Check out the [tutorials on YouTube](https://www.youtube.com/playlist?list=PL68v8FP_IVlg2tCgJXrsO3UHve1q9wLjI)

### How do I get set up? ###

C++14 and CMake are required. Below are the minimum versions of required libraries:

* Boost 1.66.0
* OpenCV 3.4.0
* OpenGL 3.3
* OpenSlide 3.4.1
* ITK 5.1.0
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

The following libraries are included as Git submodules:

* Earcut.hpp (https://github.com/mapbox/earcut.hpp.git)
* GLM (https://github.com/g-truc/glm.git)
* JSON for C++11 (https://github.com/nlohmann/json.git)
* vcglib (https://github.com/cnr-isti-vclab/vcglib.git)

To clone the external repositories:

1. git clone --recurse-submodules
2. git submodule foreach git pull

Or, alternatively:

1. git submodule update --init --recursive
2. git submodule update --recursive

Components of the following libraries and resources are copied into this repository:

* CTK (https://github.com/commontk/CTK.git)
* "Library of Perceptually Uniform Colour Maps" by Peter Kovesi (https://colorcet.com)

This project is working on macOS. There are known graphics problems when running on Linux that are most likely due to underlying differences between rendering implementations by QOpenGLWidget across platforms. We intend to address these issues.

### Running HistoloZee

A sample project file is shown below. Load the project file path as the first positional argument to HistoloZee.

```JSON
{
    "activeImage": 0,
    "activeParcellation": 0,
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
