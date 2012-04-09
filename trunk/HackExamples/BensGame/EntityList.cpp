#include "EntityList.h"

EntityList::EntityList()
{
  head    = NULL;
  tail    = NULL;
  numEnts = 0;
}

void EntityList::AddEntity(Entity* ent)
{
  if (!head)
    head = tail = new EntityListNode(ent);
  else
  {
    tail->SetNext(new EntityListNode(ent));
    tail = tail->GetNext();
  }

  ++numEnts;
}

EntityListNode* EntityList::GetHead() const { return head; }
EntityListNode* EntityList::GetNext(const EntityListNode* node) const 
{
  return (node->GetNext()) ? node->GetNext() : head;
}

EntityList::~EntityList()
{
  EntityListNode* hold;

  while (numEnts--)
  {
    hold = head;
    head = head->GetNext();
    delete hold;
  }
}