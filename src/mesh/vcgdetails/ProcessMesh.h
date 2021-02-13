#ifndef PROCESSMESH_H
#define PROCESSMESH_H

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/type_precision.hpp>

#include <vector>

namespace vcgdetails
{

void test( const std::vector< glm::vec3 >& vertices,
           const std::vector< glm::u32vec3 >& triangles );

//    {
//        std::vector< glm::vec3 > vertices;
//        std::vector< glm::u32vec3 > triangles;

//        vertices.push_back( glm::vec3{ 0.0f, 0.0f, 0.0f } );
//        vertices.push_back( glm::vec3{ 1.0f, 0.0f, 0.0f } );
//        vertices.push_back( glm::vec3{ 0.0f, 1.0f, 0.0f } );

//        triangles.push_back( glm::u32vec3{ 0, 1, 2 } );

//        vcgdetails::test( vertices, triangles );
//    }

} // vcgdetails

#endif // PROCESSMESH_H
