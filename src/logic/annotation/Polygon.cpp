#include "logic/annotation/Polygon.h"

#include "common/HZeeException.hpp"

#include <glm/glm.hpp>


Polygon::Polygon()
    :
      m_vertices(),
      m_triangulation(),
      m_currentUid(),
      m_aabb( boost::none )
{}


void Polygon::setAllVertices( std::vector< std::vector<PointType> > vertices )
{
    m_vertices = std::move( vertices );
    m_triangulation.clear();
    m_currentUid = UID();

    computeAABBox();
}


const std::vector< std::vector<Polygon::PointType> >&
Polygon::getAllVertices() const
{
    return m_vertices;
}


void Polygon::setBoundaryVertices( size_t boundary, std::vector<PointType> vertices )
{
    m_vertices.at( boundary ) = std::move( vertices );
    m_triangulation.clear();
    m_currentUid = UID();

    if ( 0 == boundary )
    {
        computeAABBox();
    }
}


void Polygon::setOuterBoundary( std::vector<PointType> vertices )
{
    if ( m_vertices.size() >= 1 )
    {
        m_vertices[0] = std::move( vertices );
    }
    else
    {
        m_vertices.emplace_back( std::move( vertices ) );
    }

    m_triangulation.clear();
    m_currentUid = UID();

    computeAABBox();
}


void Polygon::addHole( std::vector<PointType> vertices )
{
    if ( m_vertices.size() >= 1 )
    {
        m_vertices.emplace_back( std::move( vertices ) );
    }

    m_triangulation.clear();
    m_currentUid = UID();
}


const std::vector< Polygon::PointType >&
Polygon::getBoundaryVertices( size_t boundary ) const
{
    return m_vertices.at( boundary );
}


size_t Polygon::numBoundaries() const
{
    return m_vertices.size();
}


size_t Polygon::numVertices() const
{
    size_t N = 0;

    for ( const auto& boundary : m_vertices )
    {
        N += boundary.size();
    }

    return N;
}


const Polygon::PointType& Polygon::getBoundaryVertex( size_t boundary, size_t i ) const
{
    return m_vertices.at( boundary ).at( i );
}


const Polygon::PointType& Polygon::getVertex( size_t i ) const
{
    size_t j = i;

    for ( const auto& boundary : m_vertices )
    {
        if ( j < boundary.size() )
        {
            return boundary[j];
        }
        else
        {
            j -= boundary.size();
        }
    }

    throw_debug( "Invalid vertex" );
}


void Polygon::setTriangulation( std::vector<IndexType> indices )
{
    m_triangulation = std::move( indices );
    m_currentUid = UID();
}


bool Polygon::hasTriangulation() const
{
    return ( ! m_triangulation.empty() );
}


const std::vector< Polygon::IndexType >&
Polygon::getTriangulation() const
{
    return m_triangulation;
}


std::tuple< Polygon::IndexType, Polygon::IndexType, Polygon::IndexType >
Polygon::getTriangle( size_t i ) const
{
    return std::make_tuple( m_triangulation.at( 3*i + 0 ),
                            m_triangulation.at( 3*i + 1 ),
                            m_triangulation.at( 3*i + 2 ) );
}


boost::optional< Polygon::AABBoxType > Polygon::getAABBox() const
{
    return m_aabb;
}


size_t Polygon::numTriangles() const
{
    // Every three indices make a triangle
    return m_triangulation.size() / 3;
}


UID Polygon::getCurrentUid() const
{
    return m_currentUid;
}


bool Polygon::equals( const UID& otherPolygonUid ) const
{
    return ( m_currentUid == otherPolygonUid );
}


void Polygon::computeAABBox()
{
    if ( m_vertices.empty() || m_vertices[0].empty() )
    {
        // There is no outer boundary or there are no vertices in the outer boundary.
        m_aabb = boost::none;
        return;
    }

    // Compute AABB of outer boundary vertices
    m_aabb = std::make_pair( PointType( std::numeric_limits<ComponentType>::max() ),
                             PointType( std::numeric_limits<ComponentType>::lowest() ) );

    for ( const auto& v : m_vertices[0] )
    {
        m_aabb->first = glm::min( m_aabb->first, v );
        m_aabb->second = glm::max( m_aabb->second, v );
    }
}
