#ifndef POLYDATACONVERSION_H
#define POLYDATACONVERSION_H

#include "rendering/utility/vtk/VectorArrayBuffer.h"

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

namespace vtkconvert
{

std::unique_ptr< VectorArrayBuffer< float > >
extractPointsToFloatArrayBuffer( const vtkSmartPointer<vtkPolyData> polyData );

std::unique_ptr< VectorArrayBuffer< uint32_t > >
extractNormalsToUIntArrayBuffer( const vtkSmartPointer<vtkPolyData> polyData );

std::unique_ptr< VectorArrayBuffer< uint32_t > >
extractTexCoordsToUIntArrayBuffer( const vtkSmartPointer<vtkPolyData> polyData );

std::unique_ptr< VectorArrayBuffer< float > >
extractTexCoordsToFloatArrayBuffer( const vtkSmartPointer<vtkPolyData> polyData );

std::unique_ptr< VectorArrayBuffer< uint32_t > >
extractIndicesToUIntArrayBuffer( const vtkSmartPointer<vtkPolyData> polyData );

} // namespace vtkconvert

#endif // POLYDATACONVERSION_H
