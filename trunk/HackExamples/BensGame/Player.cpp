#include "Player.h"

/*
  * Construct the Player.
*/ 
Player::Player(const string& playerName)
{
  name    = playerName;
  bullets = 20;
  target  = NULL;
}

/*
 * Fire and decrease bullets.
*/
void Player::Fire()
{
  if (!bullets)
    cout << "Tick!" << endl;
  else
  {
    --bullets;
    Entity::Fire();

    if (target && target->GetLife()) target->TakeHit(20);
  }
}

/*
 * Get the player's bullets.
*/
unsigned int Player::GetBullets() const
{
  return bullets;
}

/*
 * Get the player's name.
*/
string Player::GetName() const
{
  return name;
}

/*
 * Reload.
*/
void Player::Reload()
{
  bullets = 20;
}

/*
 * Describe the Player.
*/
string Player::ToString() const
{
  ostringstream os;
  os << "Name: "    << GetName()    << '\n';
  os << "Bullets: " << GetBullets() << '\n';
  os << Entity::ToString()          << '\n';
  os << "Target"                    << '\n';
  os << target->ToString()          << endl;
  return os.str();
}

/*
 * Set the player's target.
*/
void Player::SetTarget(Entity* tar)
{
  target = tar;
}

/*
 * Get the player's target.
*/
Entity* Player::GetTarget() const
{
  return target;
}
