#ifndef _ENTITY_LIST_NODE_H_
#define _ENTITY_LIST_NODE_H_

#include "Entity.h"

class EntityListNode
{
  friend class EntityList;

  EntityListNode* next;
  Entity* ent;

  EntityListNode(Entity* entity);
  void SetNext(EntityListNode* ent);
  EntityListNode* GetNext() const;
public:
  Entity* GetEntity() const;
  ~EntityListNode();
};

#endif