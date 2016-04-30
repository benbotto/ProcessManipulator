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
  unsigned life;

public:
  /*
   * Construct the Entity.
  */ 
  Entity();
  virtual void takeHit(unsigned hit);
  virtual unsigned getLife() const;
  virtual string toString() const;
};

#endif