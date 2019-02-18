#include "Actor.h"
#include "StudentWorld.h"
#include "GraphObject.h"

///////ACTOR FUNCTION IMPLEMENTATIONS////////////////
Actor::Actor(const int imageID, int startX, int startY, Direction startDirection, int depth, bool block, bool exit, bool flameEffect, StudentWorld* myWorld)
	:GraphObject(imageID, startX, startY, startDirection, depth), m_dead(false), m_world(myWorld), m_block(block), m_exit(exit), m_flame(flameEffect) {}
bool Actor::isDead()
{
	return m_dead;
}
void Actor::setDead()			//sets an actor state to dead and makes sure they can't block another actor
{
	m_dead = true;
	m_block = false;
	m_flame = false;
}
StudentWorld* Actor::getWorld()
{
	return m_world;
}
bool Actor::canBlock()
{
	return m_block;
}
bool Actor::canExit()
{
	return m_exit;
}
bool Actor::affectedByFlame()
{
	return m_flame;
}
////////////PENELOPE FUNCTION IMPLEMENTATIONS//////////////////////
Penelope::Penelope(int startX, int startY, StudentWorld* myWorld)
	:Actor(IID_PLAYER, startX, startY, right, 0, true, true, true, myWorld)
{
	m_landmine = 0; 
	m_flamethrower = 0;
	m_vaccine = 0;
	m_infected = false;
	m_infectionCount = 0;
	m_completed = false;
}
int Penelope::numLandmine()
{
	return m_landmine;
}
void Penelope::addNumLand(int add)
{
	m_landmine += add;
}
int Penelope::numFlames()
{
	return m_flamethrower;
}
void Penelope::addNumFlames(int add)
{
	m_flamethrower += add;
}
int Penelope::numVaccine()
{
	return m_vaccine;
}
void Penelope::addNumVaccine(int add)
{
	m_vaccine += add;
}
int Penelope::numInfection()
{
	return m_infectionCount;
}
void Penelope::setCompletion()
{
	m_completed = true;
}
bool Penelope::completion()
{
	return m_completed;
}
void Penelope::doSomething()
{
	if (isDead())
		return;
	if (m_infected)
		m_infectionCount++;
	if (m_infectionCount == 500)		//penelope turned into a zombie
	{
		setDead();
		getWorld()->decLives();
		//implement code to turn her into zombie;
		getWorld()->playSound(SOUND_PLAYER_DIE);
		return;
	}
	/*implement flamethrower, landmine, vaccine*/
	int ch;
	if (getWorld()->getKey(ch))
	{
		switch (ch)
		{
		case KEY_PRESS_LEFT:
			setDirection(left);
			if(getWorld()->canMove(getX() - 4.0, getY()))	//check if penelope can move to new spot, left
				moveTo(getX() - 4.0, getY());
			break;
		case KEY_PRESS_RIGHT:
			setDirection(right);
			if(getWorld()->canMove(getX() + 4.0, getY())) // check if penelope can move to new spot, right
				moveTo(getX() + 4.0, getY());
			break;
		case KEY_PRESS_UP:
			setDirection(up);
			if(getWorld()->canMove(getX(), getY()+4.0)) //check if penelope can move to new spot
				moveTo(getX(), getY()+4.0);
			break;
		case KEY_PRESS_DOWN:
			setDirection(down);
			if(getWorld()->canMove(getX(), getY()-4.0))	//check if penelope can move to new spot
				moveTo(getX(), getY()-4.0);
			break;
		case KEY_PRESS_SPACE:
			if (m_flamethrower > 0)
			{
				getWorld()->playSound(SOUND_PLAYER_FIRE);
				Direction dir = getDirection();
				switch (dir)
				{
				case up:
					for (double i = 1; i <= 3; i++)
					{
						if(getWorld()->blockFlame(getX(), getY() + (i*SPRITE_HEIGHT)))
							break;
						getWorld()->addActor('f', getX(), getY() + (i*SPRITE_HEIGHT), up, getWorld());
					}
					break;
				case down:
					for (double i = 1; i <= 3; i++)
					{
						if (getWorld()->blockFlame(getX(), getY() - (i*SPRITE_HEIGHT)))
							break;
						getWorld()->addActor('f', getX(), getY() - (i*SPRITE_HEIGHT), down, getWorld());
					}
					break;
				case left:
					for (double i = 1; i <= 3; i++)
					{
						if (getWorld()->blockFlame(getX() - (i*SPRITE_WIDTH), getY()))
							break;
						getWorld()->addActor('f', getX() - (i*SPRITE_WIDTH), getY(), left, getWorld());
					}
					break;
				case right:
					for (double i = 1; i <= 3; i++)
					{
						if (getWorld()->blockFlame(getX() + (i*SPRITE_WIDTH), getY()))
							break;
						getWorld()->addActor('f', getX() + (i*SPRITE_WIDTH), getY(), right, getWorld());
					}
					break;
				}
				m_flamethrower--;
			}
			break;
		case KEY_PRESS_TAB:
			if (m_landmine > 0)
			{
				getWorld()->addActor('l', getX(), getY(), right, getWorld());
				m_landmine--;
			}
			break;
		case KEY_PRESS_ENTER:
			break;
		}
	}
}
////////////CITIZEN FUNCTION IMPLEMENTATIONS///////////////////
Citizen::Citizen(int startX, int startY, StudentWorld* myWorld)
	: Actor(IID_CITIZEN, startX, startY, right, 0, true, true, true, myWorld)
{
}
void Citizen::doSomething()
{

}
///////////ZOMBIE FUNCTION IMPLEMENTATIONS///////////////////////
Zombie::Zombie(int startX, int startY, StudentWorld* myWorld)
	:Actor(IID_ZOMBIE, startX, startY, right, 0, true, false, true, myWorld)
{

}
void Zombie::doSomething() {};
DumbZombie::DumbZombie(int startX, int startY, StudentWorld* myWorld)
	:Zombie(startX, startY, myWorld)
{

}
void DumbZombie::doSomething() {};
SmartZombie::SmartZombie(int startX, int startY, StudentWorld* myWorld)
	:Zombie(startX, startY, myWorld)
{

}
void SmartZombie::doSomething() {};
////////////////LANDMINE IMPLEMENTATIONS////////////////////
Landmine::Landmine(int startX, int startY, StudentWorld* myWorld)
	: Actor(IID_LANDMINE, startX, startY, right, 1, false, false, true, myWorld)
{
	active = false;
	safety = 30;
}
void Landmine::doSomething() 
{
	if (isDead())
		return;
	if (safety > 0)
	{
		safety--;
		if (safety == 0)
		{
			active = true;
			return;
		}
	}
	if (active && getWorld()->dieByLandmine(getX(), getY()))
	{
		getWorld()->playSound(SOUND_LANDMINE_EXPLODE);
		for(int i = 0; i < 3; i++)
			getWorld()->addActor('f', getX() - SPRITE_WIDTH + (i*SPRITE_WIDTH), getY() + SPRITE_HEIGHT, up, getWorld());	//row 1 of fire
		for (int i = 0; i < 3; i++)
			getWorld()->addActor('f', getX() - SPRITE_WIDTH + (i*SPRITE_WIDTH), getY(), up, getWorld());   //row 2 of fire
		for (int i = 0; i < 3; i++)
			getWorld()->addActor('f', getX() - SPRITE_WIDTH + (i*SPRITE_WIDTH), getY() -SPRITE_HEIGHT, up, getWorld());	//row 3 of fire
		getWorld()->addActor('p', getX(), getY(), right, getWorld());
		setDead();
		active = false;
	}
};
////////////////PIT IMPLEMENTATIONS////////////////////
Pit::Pit(int startX, int startY, StudentWorld* myWorld)
	: Actor(IID_PIT, startX, startY, right, 0 , false, false, true, myWorld)
{
}
void Pit::doSomething() {};
////////////////FLAME IMPLEMENTATIONS////////////////////
Flame::Flame(int startX, int startY, Direction startDirection, StudentWorld* myWorld)
	: Actor(IID_FLAME, startX, startY, startDirection, 0, false, false, false, myWorld)
{
	m_lives = 2;
}
void Flame::doSomething()
{
	m_lives--;
	if (m_lives == 0)
		setDead();
};
////////////////VOMIT IMPLEMENTATIONS////////////////////
Vomit::Vomit(int startX, int startY, Direction startDirection, StudentWorld* myWorld)
	: Actor(IID_VOMIT, startX, startY, startDirection, 0, false, false, false, myWorld)
{

}
void Vomit::doSomething() {};
////////////////GOODIES IMPLEMENTATIONS////////////////////
Goodies::Goodies(const int imageID, int startX, int startY, StudentWorld* myWorld)
	: Actor(imageID, startX, startY, right, 1, false, false, true, myWorld)
{

}
void Goodies::doSomething() 
{
	if (isDead())
		return;
	if (getWorld()->overlapsPene(getX(), getY()))
	{
		getWorld()->increaseScore(50);
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		addCount();
	}
};
VaccineGoodies::VaccineGoodies(int const imageID, int startX, int startY, StudentWorld* myWorld)
	: Goodies(imageID, startX, startY, myWorld)
{

}
void VaccineGoodies::addCount() 
{
	getWorld()->increaseCount(1, 'v');	
};
GasCanGoodies::GasCanGoodies(int const imageID, int startX, int startY, StudentWorld* myWorld)
	: Goodies(imageID, startX, startY, myWorld)
{

}
void GasCanGoodies::addCount() 
{
	getWorld()->increaseCount(5, 'f');
};
LandmineGoodies::LandmineGoodies(int const imageID, int startX, int startY, StudentWorld* myWorld)
	: Goodies(imageID, startX, startY, myWorld)
{
}
void LandmineGoodies::addCount() 
{
	getWorld()->increaseCount(2, 'l');
}
////////////////WALL IMPLEMENTATIONS////////////////////
Wall::Wall(int startX, int startY, StudentWorld* myWorld)
	: Actor(IID_WALL, startX, startY, right, 0, true, false, false, myWorld)
{

}
void Wall::doSomething() {};
////////////////EXIT IMPLEMENTATIONS////////////////////
Exit::Exit(int startX, int startY, StudentWorld* myWorld)
	:Actor(IID_EXIT, startX, startY, right, 1, false, false, false, myWorld)
{

}
void Exit::doSomething() 
{		
	if (getWorld()->citizenCount() == 0)
	{
		getWorld()->canLeave(getX(), getY());
	}
	if (getWorld()->left(getX(), getY()))
	{
		getWorld()->increaseScore(500);
		getWorld()->playSound(SOUND_CITIZEN_SAVED);
	}	
};