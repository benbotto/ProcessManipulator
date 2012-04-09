#ifndef _ENTITY_H_
#define _ENTITY_H_

#include <iostream>
#include <string>
#include <sstream>
using std::cout;
using std::endl;
using std::string;
using std::ostringstream;

class Entity
{
  int     life;
  double  x;
  double  y;

  friend class EntityList;
public:
  /*
   * Construct the Entity.
  */ 
  Entity();
  virtual void Fire();
  virtual void TakeHit(int hit);
  int GetLife() const;
  virtual string ToString() const;
};

#endif