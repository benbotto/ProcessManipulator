#include "Entity.h"
#include <string>
using std::string;
#ifndef _PLAYER_H_
#define _PLAYER_H_

class Player : public Entity
{
  unsigned int bullets;
  string       name;
  Entity*      target;

public:
  Player(const string& name);
  void fire();
  unsigned int getBullets() const;
  string getName() const;
  void reload();
  string toString() const;
  void setTarget(Entity* tar);
  Entity* getTarget() const;
};

#endif