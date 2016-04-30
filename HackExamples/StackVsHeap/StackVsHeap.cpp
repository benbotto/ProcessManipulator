#include <iostream>
#include <iomanip>
#include "../BensGame/Entity.h"
#include "../BensGame/Player.h"
using std::cout;
using std::cin;
using std::endl;
using std::hex;

int main(int argc, char* argv[])
{
  Player player1("Target_Practice");
  cout << "Address of player1: 0x" << hex << &player1 << endl;

  Entity* pEnemy = new Player("Enemy1");
  cout << "Address of Enemy1: 0x" << hex << pEnemy
       << ".  Address of pEnemy (pointer): 0x" << &pEnemy
       << endl;
  delete pEnemy;
  cin.get();

  return 0;
}
