#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include <iostream>
#include "GameWorld.h"
#include <string>
#include "Actor.h"
#include <list>
#include <sstream>
#include <iomanip> 

using namespace std;


// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    int init();
    int move();
    void cleanUp();
    
    //update the string
    string updateString();
    
    int getNumCivilians();
    bool isComplete() const;
    void setcomplete();
    
    //check if overlap in any way
    //used for vomit, landmines, etc with zombies, civilians, penelope
    //NOT for wall
    bool overlap(int tempX, int tempY, Actor* curr, std::string& marker);
    //computes the distance between two actor objects
    int computeDistance(int tempX, int tempY, Actor* other2);
    
    //computes the distance between the object and the player
    int computeDistancePlayer(int tempX, int tempY);
    
    //computes the distance between the object and all the zombies
    int computeDistanceZombie(int tempX, int tempY);
    
    //computes the distance between the all the civilians
    int computeDistanceCivilian(int tempX, int tempY, int& xStore, int& yStore);
    //chcek if there is any overlap with any actor
    //used for a lot of the actors
    bool blocked(int tempX, int tempY, Direction dir, Actor* curr);
    bool checkBounds(int tempX, int tempY, Direction dir, Actor* curr);
    
    //accessors
    int getnLandMines() const;
    int getnFlamethrower() const;
    int getnVaccines() const;
    
    //setters;
    void incrLandMines(int amt);
    void incrFlamethrower(int amt);
    void incrVaccines(int amt);
    void incrNCivilians(int amt);
    
    //creates new flame objects
    bool createSingleFlame(int xCoord, int yCoord, Direction dir);
    void createFlames(Actor* curr, Direction dir);
    //deploys a new landmine
    void createLandmine(Penelope* player);
    //create a new pit
    void createPit(Actor* pit);
    //creates new vomit obejct
    bool createVomit(int xCoords, int yCoords, Direction dir);
    //creates new Zombie objects
    void createZombie(Actor* civil);
    
    //makes civilian come towards the player
    void comeToMe(int tempX, int tempY, Actor* curr);
    //make civilians flee from zombies
    void runAway(int tempX, int tempY, Actor* curr, int& minDistance);
    
    //gets the direction the zombie should travel in
    void MPplayer(int tempX, int tempY, Actor* curr, Direction& dir);
    void MPCivilian(int tempX, int tempY, Actor* curr, Direction& dir);
    
    //virtual destructor
    virtual ~StudentWorld();

private:
    //list of all of the actors in the world
    Penelope* m_player;
    list<Actor*> m_actors;
    int m_nCivilians;
    bool m_complete;
    int m_nLandmines;
    int m_nFlamethrower;
    int m_nVaccines;
};

#endif // STUDENTWORLD_H_
