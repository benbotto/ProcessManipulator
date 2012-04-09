#include "EntityListNode.h"

EntityListNode::EntityListNode(Entity* entity) 
{ 
  ent = entity;
  next = NULL;
}

Entity* EntityListNode::GetEntity() const { return ent; }
void EntityListNode::SetNext(EntityListNode* ent) { next = ent; }
EntityListNode* EntityListNode::GetNext() const { return next; }
EntityListNode::~EntityListNode() { delete ent; }