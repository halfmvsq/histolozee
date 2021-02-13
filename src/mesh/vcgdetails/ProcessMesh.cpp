#include "mesh/vcgdetails/ProcessMesh.h"

#include <vcg/complex/complex.h>
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/smooth.h>
#include <vcg/complex/algorithms/update/topology.h>
#include <vcg/complex/algorithms/update/normal.h>
#include <vcg/complex/algorithms/update/bounding.h>

#include <wrap/io_trimesh/import.h>
#include <wrap/io_trimesh/export_off.h>

#include <glm/glm.hpp>

#include <memory>


namespace
{

// based on buggy the vcg::tri::BuildMeshFromCoordVectorIndexVector

/// @see https://github.com/zarquon42b/trimesh-cxx/blob/master/vcglib/apps/trismooth/trismooth.cpp
template< class MeshType, class InCoordType, class InFaceIndexType >
void createVCGMesh(
        MeshType& mesh,
        const std::vector< InCoordType >& vertices,
        const std::vector< InFaceIndexType >& faces )
{
    using CoordType = typename MeshType::CoordType;
    using VertexIterator = typename MeshType::VertexIterator;
    using FaceIterator = typename MeshType::FaceIterator;

    mesh.Clear();

    VertexIterator vi = vcg::tri::Allocator< MeshType >::AddVertices( mesh, vertices.size() );
    FaceIterator fi = vcg::tri::Allocator< MeshType >::AddFaces( mesh, faces.size() );

    //  std::copy( std::begin(vertices), std::end(vertices), std::begin(mesh.vert) );
    //  std::copy( std::begin(faces), std::end(faces), std::begin(mesh.face) );

    for ( auto v : vertices )
    {
        vi->P() = CoordType( v[0], v[1], v[2] );
        ++vi;
    }

    for ( auto f : faces )
    {
        fi->V(0) = &( mesh.vert[ f[0] ] );
        fi->V(1) = &( mesh.vert[ f[1] ] );
        fi->V(2) = &( mesh.vert[ f[2] ] );
        ++fi;
    }

    vcg::tri::UpdateBounding<MeshType>::Box(mesh);
}

} // anonymous


namespace vcgdetails
{

class MyFace;
class MyVertex;

struct MyUsedTypes : public vcg::UsedTypes<
        vcg::Use< MyVertex >::AsVertexType,
        vcg::Use< MyFace >::AsFaceType > {};

// Per vertex Vertex-Face adjacency relation
class MyVertex : public vcg::Vertex< MyUsedTypes,
        //        vcg::vertex::VFAdj,
        vcg::vertex::Coord3f,
        vcg::vertex::Normal3f,
        vcg::vertex::BitFlags > {};

class MyFace : public vcg::Face< MyUsedTypes,
        vcg::face::VertexRef,
        vcg::face::Normal3f,
        vcg::face::FFAdj, // Per Face Face-Face adjacency relation
        vcg::face::BitFlags > {};

class MyMesh : public vcg::tri::TriMesh<
        std::vector< MyVertex >,
        std::vector< MyFace > > {};


void test( const std::vector< glm::vec3 >& vertices,
           const std::vector< glm::u32vec3 >& triangles )
{
    std::unique_ptr<MyMesh> mesh = std::make_unique<MyMesh>();

    //    std::vector<vcg::Point3f> coordVec;
    //    std::vector<vcg::Point3i> indexVec;
    //    coordVec.push_back( vcg::Point3f(0,0,0) );
    //    for(int i=0;i<36;++i)
    //    {
    //        float angleRad = float(i)*M_PI/18.0;
    //        coordVec.push_back( vcg::Point3f(sin(angleRad),cos(angleRad),0) );
    //        indexVec.push_back( vcg::Point3i( 0, i+1, 1+(i+1) % 36) );
    //    }

    createVCGMesh( *mesh, vertices, triangles );

    float lambda = 0.5f;
    float mu = -0.53f;
    int N = 50;

    //    vcg::CallBackPos* cb;
    vcg::tri::Smooth< MyMesh >::VertexCoordTaubin( *mesh, N, lambda, mu, false/*, cb*/ );

    // some cleaning to get rid of bad file formats like stl that duplicate vertexes..
    //    int dup = vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(m);
    //    int unref = tri::Clean<MyMesh>::RemoveUnreferencedVertex(m);
    //printf("Removed %i duplicate and %i unreferenced vertices from mesh %s\n",dup,unref,argv[1]);

    // compute the normal per-vertex -> update the value of v.N() for all v (vcg::vertex::Normal3f)
    //vcg::tri::UpdateNormal<MyMesh>::PerVertexPerFace(m);
    //    vcg::tri::UpdateNormal< MyMesh >::PerVertexNormalized( *mesh );
    //vcg::tri::UpdateTopology<MyMesh>::VertexFace(m);

    //    std::cout << "Mesh has " << vertices.size() << " vertices and " << triangles.size() << " faces " << std::endl;
    //    std::cout << "Mesh has " << mesh->VN() << " vertices and " << mesh->FN() << " faces " << std::endl;

    //LaplacianSmooth(m,atoi(argv[2]));

    vcg::tri::io::ExporterOFF< MyMesh >::Save( *mesh, "out.off" );

    //http://vcg.isti.cnr.it/vcglib/trimesh__normal_8cpp_source.html
    //http://vcg.isti.cnr.it/vcglib/trimesh__intersection__plane_8cpp.html

    // FP_RECOMPUTE_VERTEX_NORMAL (choice 3: Recompute vertex normals according to:
    // as an angle weighted sum of normals of the incident faces according to the paper
    // Computing Vertex Normals from Polygonal Facet</i>, G Thurmer, CA Wuthrich, JGT 1998"
    // Probably this is the best all-purpose choice. It could slightly bias the result for degenerate, fat triangles
}

} // vcgdetails
