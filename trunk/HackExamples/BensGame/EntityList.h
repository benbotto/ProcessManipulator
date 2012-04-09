#ifndef _ENTITYLIST_H_
#define _ENTITYLIST_H_

#include "Entity.h"
#include "EntityListNode.h"

class EntityList
{
  EntityListNode* head;
  EntityListNode* tail;
  unsigned int    numEnts;
public:
  EntityList();
  void AddEntity(Entity* ent);
  EntityListNode* GetHead() const;
  EntityListNode* GetNext(const EntityListNode* node) const;
  ~EntityList();
};

#endif