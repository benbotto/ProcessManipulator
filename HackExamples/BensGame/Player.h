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
  void Fire();
  unsigned int GetBullets() const;
  string GetName() const;
  void Reload();
  string ToString() const;
  void SetTarget(Entity* tar);
  Entity* GetTarget() const;
};

#endif