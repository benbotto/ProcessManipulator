#include "Entity.h"

/*
 * Construct the Entity.
*/ 
Entity::Entity()
{
  this->life = 100;
}

/*
  * Take a hit.
*/ 
void Entity::takeHit(unsigned hit)
{
  if (this->life)
  {
    this->life -= hit;
    if (this->life < 0)
      this->life = 0;
  }
}

/*
 * Get the Entity's life.
*/ 
unsigned Entity::getLife() const
{
  return this->life;
}

/*
 * Describe the entity.
*/
string Entity::toString() const
{
  ostringstream os;
  os << "Life: " << this->getLife() << endl;
  return os.str();
}