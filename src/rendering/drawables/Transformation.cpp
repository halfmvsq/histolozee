#include "rendering/drawables/Transformation.h"

Transformation::Transformation( std::string name, const glm::mat4& parent_O_this )
    :
      DrawableBase( std::move( name ), DrawableType::Transformation )
{
    set_parent_O_this( parent_O_this );
    setPickable( true );
}

void Transformation::setMatrix( glm::mat4 parent_O_this )
{
    set_parent_O_this( std::move( parent_O_this ) );
}

const glm::mat4& Transformation::matrix() const
{
    return parent_O_this();
}
