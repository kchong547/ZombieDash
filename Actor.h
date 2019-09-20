#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <string>

//lets the actors know that the studentWorld class exists
class StudentWorld;

class Actor: public GraphObject
{
public:
    //constructor
    Actor(StudentWorld* arena, std::string marker, int imageID, double startX, double startY, Direction dir = right, int depth = 0, double size = 1.0)
    :GraphObject(imageID, startX, startY, dir, depth)
    {
        if(arena == nullptr)
        {
            exit(1);
        }
        
        m_arena = arena;
        m_dead = false;
        m_block = false;
        m_marker = marker;
        m_burnable = true;
        m_infectable = false;
        m_infectionStatus = false;
    }
    
    //every actor can do something during their "turn"
    virtual void doSomething() = 0;

    //all actors can die?
    virtual bool isDead() const;
    
    void setDead();
    
    bool doBlock() const;
    
    //conditions
    bool isBurnable() const;
    
    void notBurnable();

    bool isInfectable() const;

    void yesInfectable();
    
    void setInfected(bool val);
    
    bool getInfected() const;
    
    void yesBlock();
    
    std::string getMarker() const;
    
    //virtual destructor
    virtual ~Actor();
    
    //returns the current arena
    StudentWorld* getWorld();
    
private:
    StudentWorld* m_arena;
    bool m_dead;
    bool m_block;
    std::string m_marker;
    bool m_burnable;
    bool m_infectable;
    bool m_infectionStatus;
};

//=================Wall===========================
class Wall : public Actor
{
public:
    Wall(StudentWorld* arena, double startX, double startY);
    
    virtual void doSomething();
    
    virtual bool isDead() const;
    
    
    virtual ~Wall();
    
private:
    
};
//==================Person============================
class Person: public Actor
{
public:
    Person(StudentWorld* arena, std::string marker,  int imageID, double startX, double startY);
    
    virtual void doSomething() = 0;
    
    int getInfectionCount() const;
    
    void increInfectionCount();
    
    virtual ~Person()
    {
        
    }
    
private:
    int infectionCount;
    int m_complete;
};

//==================Penelope==========================
class Penelope: public Person
{
public:
    Penelope(StudentWorld* arena, double startX, double startY);
    
    virtual void doSomething();
    
    virtual ~Penelope();
    
private:
    int infectionCount;
    int m_complete;
};

//====================Civilian================
class Civilian: public Person
{
public:
    Civilian(StudentWorld* arena, double startX, double startY);
    
    virtual void doSomething();
    
    virtual ~Civilian()
    {
        
    }
private:
    int m_ticks;
};

//===============Exit=====================
class Exit: public Actor
{
public:
    Exit(StudentWorld* arena, double startX, double startY);

    virtual void doSomething();
    
    virtual ~Exit()
    {
        
    }
private:
    
};

//=================Pits====================
class Pit: public Actor
{
public:
    Pit(StudentWorld* arena, double startX, double startY);
    
    virtual void doSomething();
    
    virtual ~Pit()
    {
        
    }
};

//=================Goodies==================
class Goodies: public Actor
{
public:
    Goodies(StudentWorld* arena, double startX, double startY, int ID, std::string marker,  Direction dir, int depth);
    
    virtual void doSomething();
    
    virtual ~Goodies()
    {
        
    }
    
private:
    
};

//=================Vaccine Goodies=============
class Vaccine: public Goodies
{
public:
    Vaccine(StudentWorld* arena, double startX, double startY);
    
    virtual void doSomething();
    
    virtual ~Vaccine()
    {
        
    }
    
private:
};

//==================Gas Can Goodies=============
class GasCan: public Goodies
{
public:
    GasCan(StudentWorld* arena, double startX, double startY);
    
    virtual void doSomething();
    
    virtual ~GasCan()
    {
        
    }
};

//===================Landmine Goodies================
class LandMineGoodies: public Goodies
{
public:
    LandMineGoodies(StudentWorld* arena, double startX, double startY);
    
    virtual void doSomething();
    
    virtual ~LandMineGoodies()
    {
        
    }
private:
};

//===================Projectiles=====================
class Projectiles: public Actor
{
public:
    Projectiles(int ticks, StudentWorld* arena, double startX, double startY, int ID, std::string marker,  Direction dir, int depth);
    
    virtual void doSomething();
    
     int getTicks() const;
    
    void decreTicks();
    
    virtual ~Projectiles()
    {
        
    }
    
private:
    int m_ticks;
};

//===================Flames===========================
class Flame: public Projectiles
{
public:
    Flame(int ticks, StudentWorld* arena, double startX, double startY, Direction direction);
    
    virtual void doSomething();
    
    virtual ~Flame()
    {
        
    }
    
private:
    
};

//====================Landmines=======================
class Landmine: public Projectiles
{
public:
    Landmine(int ticks, StudentWorld* arena, double startX, double startY);
    
    virtual void doSomething();
    
    void setActive();
    
    bool isActive() const;
    
    virtual ~Landmine()
    {
        
    }
    
private:
    bool m_active;
};

//=====================Zombie===========================
class Zombie: public Actor
{
public:
    Zombie(StudentWorld* arena, double startX, double startY, std::string marker);
    
    virtual void doSomething();
    
    int getTicks() const;
    
    void incrTicks();
    
    virtual void createNewMovementPlan();
    
    void decrMovementDistance();
    
    int getMovementDistance() const;
    
    void setMovementDistance(int amt);
    
    virtual ~Zombie()
    {
        
    }
    
private:
    int m_ticks;
    int m_movementDistance;
};
//==================DumbZombie===========================
class DumbZombie: public Zombie
{
public:
    DumbZombie(StudentWorld* arena, double startX, double startY, std::string marker);
    
    virtual void doSomething();
    
    virtual ~DumbZombie()
    {
        
    }
    
private:
    
};

//======================SmartZombie========================
class SmartZombie: public Zombie
{
public:
    SmartZombie(StudentWorld* arena, double startX, double startY, std::string marker);
    
    virtual void doSomething();
    
    virtual void createNewMovementPlan();
    
    virtual ~SmartZombie()
    {
        
    }
private:
};


//==================Vomit===============================
class Vomit: public Projectiles
{
public:
    Vomit(int ticks, StudentWorld* arena, double startX, double startY, Direction direction);
    
    virtual void doSomething();
    
    virtual ~Vomit()
    {
        
    }
    
private:
};

#endif // ACTOR_H_

