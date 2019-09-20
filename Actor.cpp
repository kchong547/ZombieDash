//delete actor overlap!!!!!

#include "Actor.h"
#include "StudentWorld.h"
#include <string>
//================Actor=======================
bool Actor::isDead() const
{
    return m_dead;
}

void Actor::setDead()
{
    m_dead = true;
}

bool Actor::doBlock() const
{
    return m_block;
}

void Actor::yesBlock()
{
    m_block = true;
}

std::string Actor::getMarker() const
{
    return m_marker;
}

Actor::~Actor()
{
    
}

StudentWorld* Actor::getWorld()
{
    return m_arena;
}

bool Actor::isBurnable() const
{
    return m_burnable;
}

void Actor::notBurnable()
{
    m_burnable = false;
}

bool Actor::isInfectable() const
{
    return m_infectable;
}

void Actor::yesInfectable()
{
    m_infectable = true;
}

void Actor::setInfected(bool val)
{
    m_infectionStatus = val;
}

bool Actor::getInfected() const
{
    return m_infectionStatus;
}

//================Wall========================
Wall::Wall(StudentWorld* arena, double startX, double startY)
    :Actor(arena, "w", IID_WALL, startX, startY, right)   //direction, depth, and size set to default
{
    
}

void Wall::doSomething()
{
    //do nothing but exist
//    return GWSTATUS_CONTINUE_GAME;
    return;
}

//walls never die
bool Wall::isDead() const
{
    return false;
}



Wall::~Wall()
{
    
}

//===============Person=======================
Person::Person(StudentWorld* arena, string marker, int imageID,  double startX, double startY)
:Actor(arena, marker , imageID, startX, startY)    //direction, depth, and size set to default
{
    if (arena == nullptr)
    {
        exit(1);
    }
    if (startX < 0 || startX > 256 || startY < 0 || startY > 256)
    {
        exit(1);
    }
    
    m_complete = false;
    infectionCount = 0;
}

int Person::getInfectionCount() const
{
    return infectionCount;
}

void Person:: increInfectionCount()
{
    infectionCount++;
}

//===============Penelope=====================
Penelope::Penelope(StudentWorld* arena, double startX, double startY)
:Person(arena, "p" ,IID_PLAYER, startX, startY)    //direction, depth, and size set to default
{
    
}

void Penelope::doSomething()
{
    if(!(this->isDead()))
    {
        if(this->getInfected() == true)
        {
            this->increInfectionCount();
            if(this->getInfectionCount() == 500)
            {
                //set status to dead
                this->setDead();
                //play SOUND_PLAYER_DIE
                getWorld()->playSound(SOUND_PLAYER_DIE);
                //doSomething returns immediately
//                return GWSTATUS_PLAYER_DIED;
                return;
                //StudentWorld Object must detect that penelope is now dead and the current level ends
            }
        }
        
        int ch;
        int wouldBeX;
        int wouldBeY;
        
        if (getWorld()->getKey(ch))
        {
            // user hit a key during this tick!
            switch (ch)
            {
                default:    //no key was pressed
                    //do nothing
                    break;
                case KEY_PRESS_LEFT:
                    //check if theres a wall, zombie, etc
                    //if there is no overlap
                    wouldBeX = this->getX() - 4;
                    wouldBeY = this->getY();
                    if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
                    {
                        if(!(this->getWorld()->blocked(wouldBeX, wouldBeY, left, this)))
                        {
                            //... move Penelope to the left ...;
                            this->setDirection(left);
                            this->moveTo(this->getX() - 4,this->getY());
                        }
                    }
                    break;
                case KEY_PRESS_RIGHT:
                    
                    //check if theres a wall, zombie, etc
                    //if there is no overlap
                    wouldBeX = this->getX() + 4;
                    wouldBeY = this->getY();
                    if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
                    {
                        if(!(this->getWorld()->blocked(wouldBeX, wouldBeY, right, this)))
                        {
                            //... move Penelope to the right ...;
                            this->setDirection(right);
                            this->moveTo(this->getX() + 4,this->getY());
                        }
                    }
                    break;
                case KEY_PRESS_UP:
                    //check if theres a wall, zombie, etc
                    //if there is no overlap
                    wouldBeX = this->getX();
                    wouldBeY = this->getY() + 4;
                    if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
                    {
                       if(!(this->getWorld()->blocked(wouldBeX, wouldBeY, up, this)))
                       {
                            //move penelope up;
                            this->setDirection(up);
                            this->moveTo(this->getX(),this->getY() + 4);
                           
                       }
                    }
                    break;
                case KEY_PRESS_DOWN:
                    //check if theres a wall, zombie, etc
                    //if there is no overlap
                    wouldBeX = this->getX();
                    wouldBeY = this->getY() - 4;
                    if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
                    {
                        if(!(this->getWorld()->blocked(wouldBeX, wouldBeY, down, this)))
                        {
                            
                            ///move Penelope down;
                            this->setDirection(down);
                            this->moveTo(this->getX(),this->getY() - 4);
                        }
                    }
                    break;
                case KEY_PRESS_SPACE:
                    //check if penelope has any flame charges left
                    //if so, reduce nFlameThrower count by one
                    //deploy flames in front of penelope
                    if(this->getWorld()->getnFlamethrower() > 0)
                    {
                        getWorld()->playSound(SOUND_PLAYER_FIRE);
                        this->getWorld()->incrFlamethrower(-1);
                        this->getWorld()->createFlames(this, this->getDirection());
                        
                    }
                    break;
                case KEY_PRESS_TAB:
                    //check if penelope has any landmines left
                    //if so, reduce nLandmine by one
                    //deploy landmine right where penelope is standing
                    //make sure she doesnt die when its deployed...
                    
                    if(this->getWorld()->getnLandMines() > 0)
                    {
                        this->getWorld()->incrLandMines(-1);
                        this->getWorld()->createLandmine(this);
                    }
                    break;
                case KEY_PRESS_ENTER:
                    //check if penelope has any vaccines left
                    //if so, reduce nVaccines by one
                    //set penelope's m_infection status to false
                    if(this->getWorld()->getnVaccines() > 0)
                    {
                        this->getWorld()->incrVaccines(-1);
                        this->setInfected(false);;
                    }
                    break;
            }
        }
//        return GWSTATUS_CONTINUE_GAME;
        return;
    }
    else
    {
        //if 
//        return GWSTATUS_PLAYER_DIED;
        return;
    }
}

Penelope::~Penelope()
{
}

//===============Civilain class=================
Civilian::Civilian(StudentWorld* arena, double startX, double startY)
:Person(arena, "c" ,IID_CITIZEN, startX, startY)    //direction, depth, and size set to default
{
    m_ticks = 0;
}

void Civilian::doSomething()
{
    if(this->isDead())
    {
        return;
    }
    
    if(this->getInfected() == true)
    {
        this->increInfectionCount();
        if(this->getInfectionCount() == 500)
        {
            //set status to dead
            this->setDead();
            //play zombie born sound
            getWorld()->playSound(SOUND_ZOMBIE_BORN);
            getWorld()->increaseScore(-1000);

            //create a new zombie object
            this->getWorld()->createZombie(this);
            //doSomething returns immediately
            return;
        }
    }
    
    m_ticks++;
    if(m_ticks % 2 == 0)
    {
        return;
    }
    
    //compute its distance to Penelope
    int dist_p = this->getWorld()->computeDistancePlayer(this->getX(), this->getY());
    int dist_z = this->getWorld()->computeDistanceZombie(this->getX(), this->getY());
    
    //checks if the distance between penelope and citizen is less than distance between the civilian and a zombie
    if(dist_p < dist_z || dist_z == 999)
    {
        if(dist_p <= 80)
        {
            this->getWorld()->comeToMe(this->getX(), this->getY(), this);
        }
    }
    
    if(dist_z <= 80)
    {
        this->getWorld()->runAway(this->getX(), this->getY(), this, dist_z);
    }
    
    return;
    
    
}

//===============Exit Class=====================
Exit::Exit(StudentWorld* arena, double startX, double startY)
:Actor(arena, "e" , IID_EXIT, startX, startY)
{
}

void Exit::doSomething()
{
    std::string marker = "";
    this->getWorld()->overlap(this->getX(), this->getY(), this , marker);
    
    if(marker == "c")
    {
        this->getWorld()->increaseScore(500);
        this->getWorld()->incrNCivilians(-1);
    }
}


//=================Pits============================
Pit::Pit(StudentWorld* arena, double startX, double startY)
:Actor(arena, "pit" , IID_PIT, startX, startY)
{
}
    
void Pit::doSomething()
{
    //the pit will cause any person or zombie that overlaps with it to be destroyed (they fall into the pit).
    
    //if a dumb zombie falls into a pit, the player gets 1000 points, the game plays a dying noise,
    //if Penelope falls into a pit the current level will end
    //a citizen falling into a pit dies, and the player loses 1000 points, etc.
    std::string marker = "";
    this->getWorld()->overlap(this->getX(), this->getY(), this , marker);
    
    if(marker == "c")
    {
        this->getWorld()->increaseScore(-1000);
    }
    if(marker == "dz")
    {
        this->getWorld()->increaseScore(1000);
    }
    if(marker == "sz")
    {
        this->getWorld()->increaseScore(2000);
    }
}

//====================Goodies=====================
//change the constructor=========================================
Goodies::Goodies(StudentWorld* arena, double startX, double startY, int ID, std::string marker, Direction dir, int depth)
:Actor(arena, marker , ID, startX, startY, dir, depth)
{
    
}

void Goodies::doSomething()
{
    //1. It must check to see if it is currently alive. If not, then doSomething() must return immediately – none of the following steps should be performed.
    if(this->isDead())
    {
        return;
    }
    //2. The vaccine goodie must determine if it overlaps with Penelope. If so, then the vaccine goodie must:
        //a. Inform the StudentWorld object that the user is to receive 50 points.
        //b. Set its state to dead (so that it will be removed from the game by the StudentWorld object at the end of the current tick).
        //c. Play a sound effect to indicate that Penelope picked up the goodie: SOUND_GOT_GOODIE.
        //d. Inform the StudentWorld object that Penelope is to receive one dose of vaccine.
    
    //this will ONLY run if a subclass of goodie overlaps with penelope
        this->getWorld()->increaseScore(50);
        this->setDead();
        getWorld()->playSound(SOUND_GOT_GOODIE);
    
}

//======================Vaccines Goodies=========================
Vaccine::Vaccine(StudentWorld* arena, double startX, double startY)
:Goodies(arena, startX, startY, IID_VACCINE_GOODIE, "vacc" , right, 1)
{
    
}

void Vaccine::doSomething()
{
    std::string doNothing = "";
    bool collected = this->getWorld()->overlap(this->getX(), this->getY(), this, doNothing);
    
    if (collected)
    {
        Goodies::doSomething();
    }
}

//=======================Gas Can Goodies==========================
GasCan::GasCan(StudentWorld* arena, double startX, double startY)
:Goodies(arena, startX, startY, IID_GAS_CAN_GOODIE, "gas" , right, 1)
{
    
}

void GasCan::doSomething()
{
    std::string doNothing = "";
    bool collected = this->getWorld()->overlap(this->getX(), this->getY(), this, doNothing);
    
    if (collected)
    {
        Goodies::doSomething();
    }
}

//=======================Landmine Goodies==========================
LandMineGoodies::LandMineGoodies(StudentWorld* arena, double startX, double startY)
:Goodies(arena, startX, startY, IID_LANDMINE_GOODIE , "lmg" , right, 1)
{
    
}

void LandMineGoodies::doSomething()
{
    std::string doNothing = "";
    bool collected = this->getWorld()->overlap(this->getX(), this->getY(), this, doNothing);
    
    if (collected)
    {
        Goodies::doSomething();
    }

}

//========================Projectiles==========================
Projectiles::Projectiles(int ticks, StudentWorld* arena, double startX, double startY, int ID, std::string marker,  Direction dir, int depth)
:Actor(arena, marker , ID, startX, startY, dir, depth)
{
    m_ticks = ticks;
}

void Projectiles::doSomething()
{
    //. It must check to see if it is currently alive. If not, then doSomething() must return immediately – none of the following steps should be performed.
    if(this->isDead())
    {
        cerr << "dead now" << endl;
        return;
    }
    //2. After exactly two ticks from its creation, the flame must set its state to dead so it can be destroyed and removed from the level by the StudentWorld object. The doSomething() method must return immediately, doing nothing more during this tick.
    m_ticks--;
    cerr<<"tick"<<endl;
    if(m_ticks == 0)
    {
        cerr << "dying" << endl;
        this->setDead();
        return;
    }

//    //do something if overlapping something
//    std::string doNothing = "";
//    this->getWorld()->overlap(this->getX(), this->getY(), this, doNothing);
}

int Projectiles::getTicks() const
{
    return m_ticks;
}

void Projectiles::decreTicks()
{
    m_ticks--;
}

//=========================Flame Class=============================
Flame::Flame(int ticks, StudentWorld* arena, double startX, double startY, Direction direction) 
:Projectiles(ticks, arena, startX, startY, IID_FLAME , "fla" , direction, 0)
{
    
}

void Flame::doSomething()
{
    Projectiles::doSomething();

    //Otherwise, the flame will damage all damageable objects that overlap with the flame. The following objects are all damageable and must react to being damaged in the appropriate way: Penelope, citizens, all types of goodies, landmines, and all types of zombies.
    std::string marker = "";
    this->getWorld()->overlap(this->getX(), this->getY(), this, marker);
    cerr << marker << endl;
    if(marker == "c")
    {
        this->getWorld()->increaseScore(-1000);
        this->getWorld()->incrNCivilians(-1);
    }
    if(marker == "dz")
    {
        this->getWorld()->increaseScore(1000);
    }
    if(marker == "sz")
    {
        this->getWorld()->increaseScore(2000);
    }
}

//=========================Landmine==================================
Landmine::Landmine(int ticks, StudentWorld* arena, double startX, double startY)
:Projectiles(ticks, arena, startX, startY, IID_LANDMINE , "land" , right, 1)
{
    m_active = false;
}

void Landmine::doSomething()
{
    //It must check to see if it is currently alive. If not, then doSomething() must return immediately – none of the following steps should be performed.
    if(this->isDead())
    {
        cerr << "dead now" << endl;
        return;
    }
    
    
    //If the landmine is not yet active then:
        //a. It must decrement the number of safety ticks left.
        //b. If the number of safety ticks is zero, the landmine becomes active.
        //c. The doSomething() method must return immediately, doing nothing more
            //during this tick.
    if(!this->isActive())
    {
        this->decreTicks();
        if(this->getTicks() == 0)
        {
            cerr << "im dangerous!!!" << endl;
            this->setActive();
            return;
        }
    }
    
    //The landmine must determine if it overlaps with Penelope, a citizen, or zombie. If so, then the landmine must:
        //a. Set its state to dead (so that it will be removed from the game by the StudentWorld object at the end of the current tick).
        //b. Play a sound effect to indicate that the landmine exploded: SOUND_LANDMINE_EXPLODE.
        //c. Introduce a flame object at the same (x,y) location as the landmine .
        //d. Introduce flame objects in the eight adjacent slots around the landmine (north, northeast, east, southeast, south, southwest, west, northwest). Each such adjacent spot must exactly SPRITE_WIDTH pixels away horizontally and/or SPRITE_HEIGHT pixels away vertically. (SPRITE_WIDTH and SPRITE_HEIGHT are both 16.) So if the landmine goodie were at position (100, 100), the northwest flame would be added at (84, 116), the east goodie at (116, 100), the southeast goodie at (116, 84), etc.
        //e. Introduce a pit object at the same (x,y) location as the landmine.
    
    if(this->isActive())
    {
        std::string marker = "";
        this->getWorld()->overlap(this->getX(), this->getY(), this, marker);
    }
}

void Landmine::setActive()
{
    m_active = true;
}

bool Landmine::isActive() const
{
    return m_active;
}

//=====================Zombie===========================
Zombie::Zombie(StudentWorld* arena, double startX, double startY, string marker)
:Actor(arena, marker , IID_ZOMBIE, startX, startY)    //direction, depth, and size set to default
{
    m_ticks = 0;
    m_movementDistance = 0;
}

int Zombie::getTicks() const
{
    return m_ticks;
}

void Zombie::incrTicks()
{
    m_ticks++;
}

int Zombie::getMovementDistance() const
{
    return m_movementDistance;
}

void Zombie::decrMovementDistance()
{
    m_movementDistance--;
}

void Zombie::setMovementDistance(int amt)
{
    m_movementDistance = amt;
}

void Zombie::doSomething()
{
    //check if the zombie is dead in the first place
    if(this->isDead())
    {
        return;
    }
    
    //paralysis ticks
    //every even tick, dont do anything
    this->incrTicks();
    if(m_ticks % 2 == 0)
    {
        return;
    }
    
    if(m_movementDistance == 0)
    {
        this->createNewMovementPlan();
    }
}

void Zombie::createNewMovementPlan()
{
        //Pick a new random movement plan distance in the range 3 through 10 inclusive.
        m_movementDistance = randInt(3, 10);
        //Set its direction to a random direction (up, down, left, or right).
        int x = randInt(1, 4);
        switch(x)
        {
            case(1):
                this->setDirection(up);
                break;
            case(2):
                this->setDirection(down);
                break;
            case(3):
                this->setDirection(left);
                break;
            case(4):
                this->setDirection(right);
                break;
        }
    

}

//====================DumbZombie=======================
DumbZombie::DumbZombie(StudentWorld* arena, double startX, double startY, string marker)
:Zombie(arena, startX, startY, marker)
{
    
}

void DumbZombie::doSomething()
{
    Zombie::doSomething();
    
    if (this->getTicks() % 2 == 0)
    {
        return;
    }
    
    int wouldBeX;
    int wouldBeY;
    
    //based on the zombie's direction
    switch (this->getDirection())
    {
        default:    //no key was pressed
            //do nothing
            break;
        case left:
            //check if theres a wall, zombie, etc
            //if there is no overlap
            wouldBeX = this->getX() - 1;
            wouldBeY = this->getY();
            if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
            {
                if(!(this->getWorld()->blocked(wouldBeX, wouldBeY, left, this)))
                {
                    //... move Penelope to the left ...;
                    this->setDirection(left);
                    this->moveTo(this->getX() - 1,this->getY());
                    this->decrMovementDistance();
                    return;
                }
                else
                {
                    //check if would be coordinates by penelope or civilian
                    //if so, create new vomit object at that location
                    //set the object to INFECTED
                    int tempVomX = this->getX() - SPRITE_WIDTH;
                    int tempVomY = this->getY();
                    std::string thingOverlap = "";
                    this->getWorld()->overlap(tempVomX, tempVomY, this, thingOverlap);
                    
                    this->setMovementDistance(0);
                    return;
                }
            }
            break;
        case right:
            
            //check if theres a wall, zombie, etc
            //if there is no overlap
            wouldBeX = this->getX() + 1;
            wouldBeY = this->getY();
            if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
            {
                if(!(this->getWorld()->blocked(wouldBeX, wouldBeY, right, this)))
                {
                    //... move Penelope to the right ...;
                    this->moveTo(this->getX() + 1,this->getY());
                    this->decrMovementDistance();
                    return;
                }
                else
                {
                    //check if would be coordinates by penelope or civilian
                    //if so, create new vomit object at that location
                    //set the object to INFECTED
                    int tempVomX = this->getX() + SPRITE_WIDTH;
                    int tempVomY = this->getY();
                    std::string thingOverlap = "";
                    this->getWorld()->overlap(tempVomX, tempVomY, this, thingOverlap);
                    
                    this->setMovementDistance(0);
                    return;
                }
            }
            break;
        case up:
            //check if theres a wall, zombie, etc
            //if there is no overlap
            wouldBeX = this->getX();
            wouldBeY = this->getY() + 1;
            if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
            {
                if(!(this->getWorld()->blocked(wouldBeX, wouldBeY, up, this)))
                {
                    //move penelope up;
                    this->moveTo(this->getX(),this->getY() + 1);
                    this->decrMovementDistance();
                    return;
                    
                }
                else
                {
                    //check if would be coordinates by penelope or civilian
                    //if so, create new vomit object at that location
                    //set the object to INFECTED
                    int tempVomX = this->getX();
                    int tempVomY = this->getY() + SPRITE_HEIGHT;
                    std::string thingOverlap = "";
                    this->getWorld()->overlap(tempVomX, tempVomY, this, thingOverlap);

                    this->setMovementDistance(0);
                    return;
                }
            }
            break;
        case down:
            //check if theres a wall, zombie, etc
            //if there is no overlap
            wouldBeX = this->getX();
            wouldBeY = this->getY() - 1;
            if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
            {
                if(!(this->getWorld()->blocked(wouldBeX, wouldBeY, down, this)))
                {
                    //move penelope down
                    this->moveTo(this->getX(),this->getY() - 1);
                    this->decrMovementDistance();
                    return;
                    
                }
                else
                {
                    //check if would be coordinates by penelope or civilian
                    //if so, create new vomit object at that location
                    //set the object to INFECTED
                    int tempVomX = this->getX();
                    int tempVomY = this->getY() - SPRITE_HEIGHT;
                    std::string thingOverlap = "";
                    this->getWorld()->overlap(tempVomX, tempVomY, this, thingOverlap);
                    this->setMovementDistance(0);
                    return;
                }
            }
    }
}

//====================SmartZombie======================
SmartZombie::SmartZombie(StudentWorld* arena, double startX, double startY, string marker)
:Zombie(arena, startX, startY, marker)
{

}

void SmartZombie::doSomething()
{
    Zombie::doSomething();
    
    
    if (this->getTicks() % 2 == 0)
    {
        return;
    }
    
    int wouldBeX;
    int wouldBeY;
    
    //based on the zombie's direction
    switch (this->getDirection())
    {
        default:    //no key was pressed
            //do nothing
            break;
        case left:
            //check if theres a wall, zombie, etc
            //if there is no overlap
            wouldBeX = this->getX() - 1;
            wouldBeY = this->getY();
            if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
            {
                if(!(this->getWorld()->blocked(wouldBeX, wouldBeY, left, this)))
                {
                    //... move Penelope to the left ...;
                    this->setDirection(left);
                    this->moveTo(this->getX() - 1,this->getY());
                    this->decrMovementDistance();
                    return;
                }
                else
                {
                    //check if would be coordinates by penelope or civilian
                    //if so, create new vomit object at that location
                    //set the object to INFECTED
                    int tempVomX = this->getX() - SPRITE_WIDTH;
                    int tempVomY = this->getY();
                    std::string thingOverlap = "";
                    this->getWorld()->overlap(tempVomX, tempVomY, this, thingOverlap);
                    
                    this->setMovementDistance(0);
                    return;
                }
            }
            break;
        case right:
            
            //check if theres a wall, zombie, etc
            //if there is no overlap
            wouldBeX = this->getX() + 1;
            wouldBeY = this->getY();
            if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
            {
                if(!(this->getWorld()->blocked(wouldBeX, wouldBeY, right, this)))
                {
                    //... move Penelope to the right ...;
                    this->moveTo(this->getX() + 1,this->getY());
                    this->decrMovementDistance();
                    return;
                }
                else
                {
                    //check if would be coordinates by penelope or civilian
                    //if so, create new vomit object at that location
                    //set the object to INFECTED
                    int tempVomX = this->getX() + SPRITE_WIDTH;
                    int tempVomY = this->getY();
                    std::string thingOverlap = "";
                    this->getWorld()->overlap(tempVomX, tempVomY, this, thingOverlap);
                    
                    this->setMovementDistance(0);
                    return;
                }
            }
            break;
        case up:
            //check if theres a wall, zombie, etc
            //if there is no overlap
            wouldBeX = this->getX();
            wouldBeY = this->getY() + 1;
            if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
            {
                if(!(this->getWorld()->blocked(wouldBeX, wouldBeY, up, this)))
                {
                    //move penelope up;
                    this->moveTo(this->getX(),this->getY() + 1);
                    this->decrMovementDistance();
                    return;
                    
                }
                else
                {
                    //check if would be coordinates by penelope or civilian
                    //if so, create new vomit object at that location
                    //set the object to INFECTED
                    int tempVomX = this->getX();
                    int tempVomY = this->getY() + SPRITE_HEIGHT;
                    std::string thingOverlap = "";
                    this->getWorld()->overlap(tempVomX, tempVomY, this, thingOverlap);
                    
                    this->setMovementDistance(0);
                    return;
                }
            }
            break;
        case down:
            //check if theres a wall, zombie, etc
            //if there is no overlap
            wouldBeX = this->getX();
            wouldBeY = this->getY() - 1;
            if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
            {
                if(!(this->getWorld()->blocked(wouldBeX, wouldBeY, down, this)))
                {
                    //move penelope down
                    this->moveTo(this->getX(),this->getY() - 1);
                    this->decrMovementDistance();
                    return;
                    
                }
                else
                {
                    //check if would be coordinates by penelope or civilian
                    //if so, create new vomit object at that location
                    //set the object to INFECTED
                    int tempVomX = this->getX();
                    int tempVomY = this->getY() - SPRITE_HEIGHT;
                    std::string thingOverlap = "";
                    this->getWorld()->overlap(tempVomX, tempVomY, this, thingOverlap);
                    this->setMovementDistance(0);
                    return;
                }
            }
    }

}

void SmartZombie::createNewMovementPlan()
{
//    //Pick a new random movement plan distance in the range 3 through 10 inclusive.
this->setMovementDistance(randInt(3, 10));
    int xtemp = 0;
    int ytemp = 0;
    Direction togo;
    int p_dist = this->getWorld()->computeDistancePlayer(this->getX(), this->getY());
    int c_dist = this->getWorld()->computeDistanceCivilian(this->getX(), this->getY(), xtemp, ytemp);

    if(c_dist >= p_dist)
    {
        if (p_dist < 80)
        {
            //draws the zombie closer to the player
//            this->getWorld()->comeToMe(this->getX(), this->getY(), this);
            this->getWorld()->MPplayer(this->getX(), this->getY(), this, togo);

            this->setDirection(togo);
            return;
        }
    }
    else if(c_dist < p_dist)
    {
        if(c_dist < 80)
        {
            this->getWorld()->MPCivilian(xtemp, ytemp, this, togo);

            this->setDirection(togo);
            return;
        }
    }
//Set its direction to a random direction (up, down, left, or right).
        int x = randInt(1, 4);
        switch(x)
        {
            case(1):
                this->setDirection(up);
                break;
            case(2):
                this->setDirection(down);
                break;
            case(3):
                this->setDirection(left);
                break;
            case(4):
                this->setDirection(right);
                break;
        }
}

//=====================Vomit============================
Vomit::Vomit(int ticks, StudentWorld* arena, double startX, double startY, Direction direction)
:Projectiles(ticks, arena, startX, startY, IID_VOMIT , "vom" , direction, 0)
{
    
}

void Vomit::doSomething()
{
    Projectiles::doSomething();
}



