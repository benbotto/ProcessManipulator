#include "Entity.h"

/*
 * Construct the Entity.
*/ 
Entity::Entity()
{
  life    = 100;
  x       = 0;
  y       = 0;
}

/*
 * Fire the weapon.
*/ 
void Entity::Fire() {}

/*
  * Take a hit.
*/ 
void Entity::TakeHit(int hit)
{
  if (life)
  {
    life -= hit;
    if (life < 0)
      life = 0;
  }
}

/*
 * Get the Entity's life.
*/ 
int Entity::GetLife() const
{
  return life;
}

/*
 * Describe the entity.
*/
string Entity::ToString() const
{
  ostringstream os;
  os << "Life: " << GetLife() << endl;
  return os.str();
}