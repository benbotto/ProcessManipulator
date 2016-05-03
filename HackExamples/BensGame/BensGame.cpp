#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <Windows.h>
#include "Entity.h"
#include "Player.h"
using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;
using std::unique_ptr;

typedef vector<unique_ptr<Entity> > entList_t;

Player              p1("Target_Practice");
entList_t           enemies;
entList_t::iterator target;

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

void fire(Player& p1)
{
  cout << "Firing." << endl;
  p1.fire();
}

void reload(Player& p1)
{
  cout << "Reloading." << endl;
  p1.reload();
}

void printEntity(const Entity& ent)
{
  cout << ent.toString();
}

void takeHit(Entity& ent)
{
  cout << "Taking a hit." << endl;
  ent.takeHit(20);
}

void changeTarget(Player& p1, Entity* ent)
{
  cout << "Changing target." << endl;
  p1.setTarget(ent);
}

int main(int argc, char* argv)
{
  bool   quit = false;
  string command;

  // Change the window title.
  SetConsoleTitle("BensGame");

  // Create some enemies.
  for (int i = 0; i < 3; ++i)
    enemies.push_back(unique_ptr<Entity>(new Entity()));

  // Target the first Entity.
  target = enemies.begin();
  p1.setTarget(target->get());

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
        if (++target == enemies.end())
          target = enemies.begin();
        changeTarget(p1, target->get());
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
