#ifndef POLYDATAGENERATOR_H
#define POLYDATAGENERATOR_H

#include <glm/fwd.hpp>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

namespace vtkutils
{

// Generates triangles
vtkSmartPointer<vtkPolyData> generateCone();

// Generates triangles
vtkSmartPointer<vtkPolyData> generateCube();

// Generates triangles
vtkSmartPointer<vtkPolyData> generateCylinder( const glm::dvec3& center, double radius, double height );

// Generates triangles
vtkSmartPointer<vtkPolyData> generateSphere();

// Generates triangles
vtkSmartPointer<vtkPolyData> generatePointyCylinders( double coneToCylinderLengthRatio );

} // namespace vtkutils

#endif // POLYDATAGENERATOR_H
