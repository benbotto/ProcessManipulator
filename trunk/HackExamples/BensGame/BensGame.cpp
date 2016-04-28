#include <iostream>
#include <string>
#include <Windows.h>
#include "Entity.h"
#include "Player.h"
#include "EntityList.h"
using std::cout;
using std::cin;
using std::endl;
using std::string;

void help()
{
  cout << '\n';
  cout << "h: This dialog.\n";
  cout << "f: Fire your gun.\n";
  cout << "r: Reload your gun.\n";
  cout << "p: Print the Entity's info.\n";
  cout << "t: Take a hit.\n";
  cout << "c: Change target.\n";
  cout << "q: Quit.\n";
  cout << endl;
}

void fire(Entity& ent)
{
  cout << "Firing." << endl;
  ent.Fire();
}

void reload(Player& p1)
{
  cout << "Reloading." << endl;
  p1.Reload();
}

void printEntity(const Entity& ent)
{
  cout << ent.ToString();
}

void takeHit(Entity& ent)
{
  cout << "Taking a hit." << endl;
  ent.TakeHit(20);
}

void changeTarget(Player& p1, Entity* ent)
{
  cout << "Changing target." << endl;
  p1.SetTarget(ent);
}

int main(int argc, char* argv)
{
  Player          p1("Target_practice");
  EntityList      ents;
  EntityListNode* target;
  bool            quit = false;
  string          command;

  // Change the window title.
  SetConsoleTitle("BensGame");

  // Create a bunch of Entities.
  for (int i = 0; i < 3; ++i) ents.AddEntity(new Entity());

  // Target the first Entity.
  target = ents.GetHead();
  
  p1.SetTarget(target->GetEntity());

  while (!quit)
  {
    do
    {
      cout << "Enter a command (h for help): ";
      getline(cin, command);
    }
    while (!command.length());

    switch (command.at(0))
    {
      case 'f':
        fire(p1);
        break;
      case 'r':
        reload(p1);
        break;
      case 'p':
        printEntity(p1);
        break;
      case 't':
        takeHit(p1);
        break;
      case 'c':
        target = ents.GetNext(target);
        changeTarget(p1, target->GetEntity());
        break;
      case 'q':
        quit = true;
        break;
      case 'h':
      default:
        help();
        break;
    }
  }
  return 0;
}
