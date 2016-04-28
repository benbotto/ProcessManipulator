#include "Player.h"

/*
  * Construct the Player.
*/ 
Player::Player(const string& playerName)
{
  this->name    = playerName;
  this->bullets = 20;
  this->target  = NULL;
}

/*
 * Fire and decrease bullets.
*/
void Player::fire()
{
  if (!this->bullets)
    cout << "Tick!" << endl;
  else
  {
    --this->bullets;

    if (this->target && this->target->getLife())
      this->target->takeHit(20);
  }
}

/*
 * Get the player's bullets.
*/
unsigned int Player::getBullets() const
{
  return this->bullets;
}

/*
 * Get the player's name.
*/
string Player::getName() const
{
  return this->name;
}

/*
 * Reload.
*/
void Player::reload()
{
  this->bullets = 20;
}

/*
 * Describe the Player.
*/
string Player::toString() const
{
  ostringstream os;
  os << "Name: "    << this->getName()    << '\n';
  os << "Bullets: " << this->getBullets() << '\n';
  os << Entity::toString()          << '\n';
  os << "Target"                    << '\n';
  os << target->toString()          << endl;
  return os.str();
}

/*
 * Set the player's target.
*/
void Player::setTarget(Entity* tar)
{
  this->target = tar;
}

/*
 * Get the player's target.
*/
Entity* Player::getTarget() const
{
  return this->target;
}
