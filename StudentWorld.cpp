//implement flame sounds

#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include "Level.h"

using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    m_player = nullptr;
    m_nCivilians = 0;
    m_complete = false;
    m_nLandmines = 0;
    m_nFlamethrower = 0;
    m_nVaccines = 0;
}

int StudentWorld::init()
{
    //reset the studentworld member variables
    m_player = nullptr;
    m_nCivilians = 0;
    m_complete = false;
    m_nLandmines = 0;
    m_nFlamethrower = 0;
    m_nVaccines = 0;
    
//    Your StudentWorld’s init() member function must:
//    1. Initialize the data structures used to keep track of your game’s world.
//    2. Allocate and insert a Penelope object into the game world as specified in the
//    current level’s data file.
//    3. Allocate and insert various wall, pit, goodie, zombie, and exit objects into the
//    game world as specified in the current level’s data file.
    ostringstream oss1;
    oss1 << "Score: ";
    if(this->getScore() < 0)
    {
        oss1 << "-";
        oss1.fill('0');
        oss1 << setw(6) << this->getScore() * -1;
    }
    else
    {
        oss1.fill('0');
        oss1 << setw(6) << this->getScore();
    }
    oss1 << "  Level:  ";
    oss1.fill(' ');
    oss1 << setw(2) << this->getLevel();
    oss1 << "  Lives: ";
    oss1 << setw(1) << this->getLives();
    oss1 << "  Vacc:  0  Flames:  0  Mines:  0  Infected: 0";
    string s = oss1.str();
    this->setGameStatText(s);
    
    ostringstream oss;
    oss << "level";
    oss.fill('0');
    oss << setw(2) << getLevel();
    oss << ".txt";

    Level lev(assetPath());
    string levelFile = oss.str();
//    cerr << levelFile << endl;

    //change this
//    Level lev(assetPath());
//    string levelFile = "level01.txt";
    Level::LoadResult result = lev.loadLevel(levelFile);
    if (result == Level::load_fail_file_not_found)
    {
        //change later?????
        return GWSTATUS_PLAYER_WON;
    }
    else if (result == Level::load_fail_bad_format)
    {
         return GWSTATUS_LEVEL_ERROR;
    }
    else if (result == Level::load_success)
    {
        cerr << "Successfully loaded level" << endl;
        
        for (int y = 0; y < LEVEL_HEIGHT; y++)
        {
            for (int x = 0; x < LEVEL_WIDTH; x++)
            {
                Level::MazeEntry ge = lev.getContentsOf(x,y); // level_x=5, level_y=10
                switch (ge) // so x=80 and y=160
                {
                    default:    //empty and everything else that isnt a player wall, or exit
                        break;
                    case Level::player:
                        //initialize m_player
                        m_player = new Penelope(this, x * 16, y * 16);
                        m_player->yesInfectable();
                        m_player->yesBlock();
                        break;
                    case Level::wall:
                        Actor* tempWall;
                        tempWall = new Wall(this, x * 16, y * 16);
                        tempWall->yesBlock();
                        tempWall->notBurnable();
                        m_actors.push_back(tempWall);
                        break;
                    case Level::exit:
                        Actor* tempExit;
                        tempExit = new Exit(this, x *16, y *16);
//                        tempExit->notBurnable();
                        m_actors.push_back(tempExit);
                        break;
                    case Level::pit:
                        Actor* tempPit;
                        tempPit = new Pit(this, x *16, y *16);
                        tempPit->notBurnable();
                        m_actors.push_back(tempPit);
                        break;
                    case Level::vaccine_goodie:
                        Actor* tempVacc;
                        tempVacc = new Vaccine(this, x *16, y *16);
                        m_actors.push_back(tempVacc);
                        break;
                    case Level::gas_can_goodie:
                        Actor* tempGas;
                        tempGas = new GasCan(this, x *16, y *16);
                        m_actors.push_back(tempGas);
                        break;
                    case Level::landmine_goodie:
                        Actor* tempLandmine;
                        tempLandmine = new LandMineGoodies(this, x *16, y *16);
                        m_actors.push_back(tempLandmine);
                        break;
                    case Level::dumb_zombie:
                        Actor* tempZombie;
                        tempZombie = new DumbZombie(this, x *16, y *16, "dz");
                        tempZombie->yesBlock();
                        m_actors.push_back(tempZombie);
                        break;
                    case Level::smart_zombie:
                        Actor* tempSmartZombie;
                        tempSmartZombie = new SmartZombie(this, x *16, y *16, "sz");
                        tempSmartZombie->yesBlock();
                        m_actors.push_back(tempSmartZombie);
                        break;
                    case Level::citizen:
                        Actor* tempCivil = new Civilian(this, x * 16, y * 16);
                        tempCivil->yesInfectable();
                        tempCivil->yesBlock();
                        m_actors.push_back(tempCivil);
                        this->incrNCivilians(1);
                        cerr << this->getNumCivilians();
                        break;
                }
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // The term "actors" refers to all zombies, Penelope, goodies, // pits, flames, vomit, landmines, etc.
    // Give each actor a chance to do something, including Penelope for each of the actors in the game world
    
    //first ask penelope to do something

    if(!(m_player->isDead()))
    {
        m_player->doSomething();
    }
    
    list<Actor*> :: iterator it;
    it = m_actors.begin();
    //for each of the actors in the list
    while(it != m_actors.end())
    {
        if (!((*it)->isDead()))
        {
            // tell each actor to do something (e.g. move) actor[i]->doSomething();
            (*it)->doSomething();
            if (m_player->isDead())
            {
                //decrease the lives of penelope
                this->decLives();
                this->playSound(SOUND_PLAYER_DIE);
                //tell the world that penelope has died
                return GWSTATUS_PLAYER_DIED;
            }
        }
        //if its dead then it wont do anything
        //move onto the next actor
        it++;
    }
    
    // Remove newly-dead actors after each tick Remove and delete dead game objects
    //for each of the actors in the list check if any are dead
    it = m_actors.begin();
    while(it != m_actors.end())
    {
        if((*it)->isDead())
        {
            delete *it;
            it = m_actors.erase(it);
        }
        //move onto the next actor
        it++;
    }
    
    //!!!// Update the game status line
    //Update Display Text // update the score/lives/level text at screen top
    string s = this->updateString();
    this->setGameStatText(s);
    
    //check if the game is complete
    if (this->isComplete())             //m_nCivilians == 0
    {
        this->playSound(SOUND_LEVEL_FINISHED);
        return GWSTATUS_FINISHED_LEVEL;
    }
    else if((m_player->isDead()))
    {
        //decrement penelope's lives
        this->decLives();
        this->playSound(SOUND_PLAYER_DIE);
        //if player is dead then stop
        return GWSTATUS_PLAYER_DIED;
    }
    else
    {
        // the player hasn’t completed the current level and hasn’t died, so // continue playing the current level
        return GWSTATUS_CONTINUE_GAME;
    }
    
//    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
//    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
//    decLives();
//    return GWSTATUS_PLAYER_DIED;
}

void StudentWorld::cleanUp()
{
    cerr << "clean-up" << endl;
    list<Actor*>::iterator it;
    it = m_actors.begin();
    //delete all actors in the list
    while(it != m_actors.end())
    {
        delete *it;
        it = m_actors.erase(it);
        //move onto the next actor
    }
    
    //delete penelope if she hasnt already been deleted
    if(m_player != nullptr)
    {
        cerr << "deleting player!" << endl;
        delete m_player;
        m_player = nullptr;
    }
    //does this cause a memory leak??
    //game should now be empty
    if(m_actors.empty())
    {
        cerr << "everything cleared" << endl;
    }
    else
    {
        cerr << "uh oh" << endl;
    }
}

//done when the player prematurely ends the game
StudentWorld::~StudentWorld()
{
    cerr << "destructor" << endl;
    this->cleanUp();
}

int StudentWorld::getNumCivilians()
{
    return m_nCivilians;
}

bool StudentWorld::isComplete() const
{
    return m_complete;
}

void StudentWorld::setcomplete()
{
    m_complete = true;
}

string StudentWorld::updateString()
{
    ostringstream oss;
    oss << "Score: ";
    if(this->getScore() < 0)
    {
        oss << "-";
      oss.fill('0');
      oss << setw(6) << this->getScore() * -1;
    }
    else
    {
        oss.fill('0');
        oss << setw(6) << this->getScore();
    }
    oss << "  Level: ";
    oss.fill(' ');
    oss << setw(2) << this->getLevel();
    oss << "  Lives: ";
    oss << setw(1) << this->getLives();
    oss << "  Vacc: ";
    oss << setw(2) << this->getnVaccines();
    oss << "  Flames: ";
    oss << setw(2) << this->getnFlamethrower();
    oss << "  Mines: ";
    oss << setw(2) << this->getnLandMines();
    oss << "  Infected: ";
    oss << m_player->getInfectionCount();
    
    string s = oss.str();
    return s;
//    Score: 000000  Level:  1  Lives: 3  Vaccines:  0  Flames:  0  Mines:  1  Infected: 0
//    Score: 004500  Level: 27  Lives: 3  Vaccines:  2  Flames: 16  Mines:  1  Infected: 0

}

bool StudentWorld::overlap(int tempX, int tempY, Actor* curr, std::string& marker)
{
    //check if the actor is alive???
    list<Actor*> ::iterator it;
    it = m_actors.begin();
    
    while(it != m_actors.end())
    {
        //for each item in actor
        //check if comparing the same object
        if(curr == *it || (*it)->isDead())
        {
            //if so, iterate to next
            //no point in comparing the same object
            it++;
            continue;
        }
        else
        {
            //if not, check if overlap
            //if so, return true
            int distance = this->computeDistance(tempX, tempY, *it);
            if(distance <= 10)
            {
                //goodies shouldnt be collected by ANY actor in the list. only penelope
                if(curr->getMarker() == "vacc" || curr->getMarker() == "gas" || curr->getMarker() == "lmg")
                {
                    if((*it)->getMarker() == "dz" || (*it)->getMarker() == "sz" ||(*it)->getMarker() == "c")
                    {
                        return false;
                    }
                }
                //if we're currently checking if anything overlaps with the would be vomit coordinates from a zombie
                if(curr->getMarker() == "dz" || curr->getMarker() == "sz")
                {
                    if((*it)->getMarker() == "c")
                    {
                        bool success = this->createVomit(tempX, tempY, curr->getDirection());
                        if (success)
                        {
                            (*it)->setInfected(true);
                        }
                    }
                }

                //if we/re currently checking if anything overlaps with a pit
                if(curr->getMarker() == "pit")
                {
                    if((*it)->getMarker() == "c")
                    {
                        //reduce 1000 points (studentWorld)
                        this->increaseScore(-1000);
                        marker = "c";
                        //set civilian to dead ( so it gets removed from student world)
                        (*it)->setDead();
                        //play CIVILIAN DEAD SOUND
                        this->playSound(SOUND_CITIZEN_DIE);
                        this->incrNCivilians(-1);
                        cerr << this->getNumCivilians() << endl;
                    }
                    if((*it)->getMarker() == "dz")
                    {
                        //increase 1000 points (studentWorld)
                        this->increaseScore(1000);
                        marker = "dz";
                        //set civilian to dead ( so it gets removed from student world)
                        (*it)->setDead();
                        //play CIVILIAN DEAD SOUND
                        this->playSound(SOUND_ZOMBIE_DIE);
                    }
                    if((*it)->getMarker() == "sz")
                    {
                        //increase 1000 points (studentWorld)
                        this->increaseScore(2000);
                        marker = "sz";
                        //set civilian to dead ( so it gets removed from student world)
                        (*it)->setDead();
                        //play CIVILIAN DEAD SOUND
                        this->playSound(SOUND_ZOMBIE_DIE);
                    }
                    if((*it)->getMarker() == "p")
                    {
                        m_player->setDead();
                    }
                }
                //if anything overlaps with an exit
                if(curr->getMarker() == "e")
                {
                    //    if overlap with civilian
                    if((*it)->getMarker() == "c")
                    {
                        //give user 500 points (studentWorld)
                        this->increaseScore(500);
                        marker = "c";
                        //        set civilian to dead ( so it gets removed from student world)
                        //        note that this is different from when a civilian gets killed by a zombie which deducts points instead
                        (*it)->setDead();
                        //        play CIVILIAN SAVED SOUND
                        this->playSound(SOUND_CITIZEN_SAVED);
//                       this->incrNCivilians(-1);
//                        cerr << this->getNumCivilians() << endl;
                    }
                }
                //if anything overlaps with landmine
                if(curr->getMarker() == "land")
                {
                    if((*it)->getMarker() == "c" || (*it)->getMarker() == "dz"|| (*it)->getMarker() == "sz"  || (*it)->getMarker() == "fla")
                    {
                        this->playSound(SOUND_LANDMINE_EXPLODE);
                        this->createFlames(curr, Actor::up);
                        this->createPit(curr);
                        curr->setDead();
                    }
                }
                //check if is flame and the object overlapping is burnable
                //if anything overlaps with flame
                if(curr->getMarker() == "fla")
                {
                    if((*it)->isBurnable() && (*it)->getMarker() != "land")
                    {
                        if((*it)->getMarker() == "c")
                        {
                            //reduce 1000 points (studentWorld)
                            this->increaseScore(-1000);
                            marker = "c";
                            //set civilian to dead ( so it gets removed from student world)
                            (*it)->setDead();
                            //play CIVILIAN DEAD SOUND
                            this->playSound(SOUND_CITIZEN_DIE);
//                            this->incrNCivilians(-1);
//                            cerr << this->getNumCivilians() << endl;
                        }
                        if((*it)->getMarker() == "dz")
                        {
                            //increase 1000 points (studentWorld)
                            this->increaseScore(1000);
                            marker = "dz";
                            //set civilian to dead ( so it gets removed from student world)
                            (*it)->setDead();
                            //play CIVILIAN DEAD SOUND
                            this->playSound(SOUND_ZOMBIE_DIE);
                        }
                        
                        if((*it)->getMarker() == "sz")
                        {
                            //increase 1000 points (studentWorld)
                            this->increaseScore(2000);
                            marker = "sz";
                            //set civilian to dead ( so it gets removed from student world)
                            (*it)->setDead();
                            //play CIVILIAN DEAD SOUND
                            this->playSound(SOUND_ZOMBIE_DIE);
                        }
                    }
                    //only exit and wall are not burnable
                    if((*it)->getMarker() == "w" || (*it)->getMarker() == "e")
                    {
                        //notifies the code that there is an overlapping object
                        marker = "block";
                    }
                }
                return true;
            }
            else
            {
                it++;
                
            }
        }
    }
    
    //if the object is not penelope/ compare addresses
    //if not, then check the overlap with penelope
    if(curr != m_player)
    {
        int distance = this->computeDistance(tempX, tempY, m_player);
        if(distance <= 10)
        {
            if(curr->getMarker() == "dz" || curr->getMarker() == "sz")
            {
                bool success = this->createVomit(tempX, tempY, curr->getDirection());
                if (success)
                {
                    m_player->setInfected(true);
                }
            }
            if(curr->getMarker() == "pit")
            {
                m_player->setDead();
            }
            if(curr->getMarker() == "e")
            {
                //if overlap with penelope
                //check if zero civilians remaining on the floor
                if(this->getNumCivilians() == 0)
                {
                    //if so, tell studentWorld that penelope has finished the level
                    this->setcomplete();
                }
            }
            //if the player overlaps with a vaccine
            if(curr->getMarker() == "vacc")
            {
                this->incrVaccines(1);
            }
            if(curr->getMarker() == "gas")
            {
                this->incrFlamethrower(5);
            }
            if(curr->getMarker() == "lmg")
            {
                this->incrLandMines(2);
            }
            //if player overlaps with a flame
            if(curr->getMarker() == "fla")
            {
                cerr << "flame overlap with player!" << endl;
                //player just die
                m_player->setDead();
            }
            if(curr->getMarker() == "land")
            {
                this->playSound(SOUND_LANDMINE_EXPLODE);
                this->createFlames(curr, Actor::up);
                this->createPit(curr);
                curr->setDead();
            }
            
            return true;
        }
    }
    //there was no overlap
    return false;
}

int StudentWorld::computeDistance(int tempX, int tempY, Actor* other2)
{
    int distance = 0;
    double curCenterX = tempX + static_cast<double>((SPRITE_HEIGHT-1) / 2);
    double curCenterY = tempY + static_cast<double>((SPRITE_WIDTH - 1) / 2);
    double otherCenterX = other2->getX() + static_cast<double>((SPRITE_HEIGHT-1) / 2);
    double otherCenterY = other2->getY() + static_cast<double>((SPRITE_HEIGHT-1) / 2);
    
    distance = sqrt(pow((curCenterX - otherCenterX), 2) + pow((curCenterY - otherCenterY), 2));
//    distance = sqrt(pow((tempX - other2->getX()), 2) + pow((tempY - other2->getY()), 2));
//
//    cerr << tempX << endl;
//    cerr << tempY << endl;
//    cerr << other2->getX() << endl;
//    cerr << other2->getY() << endl;

    return distance;
}

int StudentWorld::computeDistancePlayer(int tempX, int tempY)
{
    int x = this->computeDistance(tempX, tempY, m_player);
    return x;
}

int StudentWorld::computeDistanceZombie(int tempX, int tempY)
{
    int minDistance = 999;
    list<Actor*> ::iterator it;
    it = m_actors.begin();
    
    while(it != m_actors.end())
    {
        int x = 999;
        if((*it)->getMarker() == "dz" || (*it)->getMarker() == "sz" )
        {
            x = this->computeDistance(tempX, tempY, *it);
            it++;
        }
        //if the distance you just computed is less than the current min
        if(x < minDistance)
        {
            //then replace
            minDistance = x;
        }
        else
        {
            it++;
        }
        
    }
    return minDistance;
}

int StudentWorld::computeDistanceCivilian(int tempX, int tempY, int& xStore, int& yStore) 
{
    int minDistance = 999;
    list<Actor*> ::iterator it;
    it = m_actors.begin();
    
    while(it != m_actors.end())
    {
        int x = 999;
        if((*it)->getMarker() == "c")
        {
            x = this->computeDistance(tempX, tempY, *it);
            it++;
        }
        //if the distance you just computed is less than the current min
        if(x < minDistance)
        {
            //then replace
            minDistance = x;
            xStore = (*it)->getX();
            yStore = (*it)->getY();
        }
        else
        {
            it++;
        }
        
    }
    return minDistance;
}

bool StudentWorld::blocked(int tempX, int tempY, Direction dir, Actor* curr)
{
    list<Actor*> ::iterator it;
    it = m_actors.begin();
    
    while(it != m_actors.end())
    {
        //for each item in actor
        //check if comparing the same object
        if(curr == *it)
        {
            //if so, iterate to next
            //no point in comparing the same object
            it++;
            continue;
        }
        else
        {
            //if not, check if overlap
            //if so, return true
            bool blocked = this->checkBounds(tempX, tempY, dir, *it);
            if(blocked)
            {
                return true;
            }
            else
            {
                it++;

            }
        }
    }
    
    //if the object is not penelope/ compare addresses
    //if not, then check the overlap with penelope
    if(curr != m_player)
    {
        bool blocked = this->checkBounds(tempX, tempY, dir, m_player);
        if(blocked)
        {
            return true;
        }
    }
    return false;
}

bool StudentWorld::checkBounds(int tempX, int tempY, Direction dir, Actor* curr)
{
    //based on the direction the current object is facing, you need to check different bounds
    double tempXbounds = tempX;
    double tempYbounds = tempY;
    double currXBoundsLeft = curr->getX();
    double currXBoundsRight = curr->getX() + static_cast<double>((SPRITE_WIDTH - 1));
    double currYBoundsBottom = curr->getY();
    double currYBoundsTop = curr->getY() + static_cast<double>((SPRITE_HEIGHT - 1));
    
    switch(dir)
    {
        case(Actor::up):
            tempYbounds = tempY + static_cast<double>((SPRITE_HEIGHT - 1));
            if(curr->doBlock())
            {
                if(currYBoundsBottom <= tempYbounds && tempYbounds <= currYBoundsTop)
                {
                    if(currXBoundsLeft <= tempXbounds && tempXbounds <= currXBoundsRight)
                    {
                        return true;
                    }
                    
                    if(currXBoundsLeft <= (tempXbounds +  SPRITE_WIDTH - 1) && (tempXbounds +  SPRITE_WIDTH - 1) <= currXBoundsRight)
                    {
                        return true;
                    }
                }
            }
        
            break;
            
        case(Actor::down):
            if(curr->doBlock())
            {
                if(currYBoundsBottom <= tempYbounds && tempYbounds <= currYBoundsTop)
                {
                    if(currXBoundsLeft <= tempXbounds && tempXbounds <= currXBoundsRight)
                    {
                        return true;
                    }
                    
                    if(currXBoundsLeft <= (tempXbounds +  SPRITE_WIDTH - 1) && (tempXbounds +  SPRITE_WIDTH - 1) <= currXBoundsRight)
                    {
                        return true;
                    }
                }
            }
            break;
            
        case(Actor::right):
            tempXbounds = tempX + static_cast<double>((SPRITE_WIDTH - 1));
            if(curr->doBlock())
            {
                if(tempXbounds >= currXBoundsLeft && tempXbounds <= currXBoundsRight)
                {
                    if(currYBoundsBottom <= tempYbounds && tempYbounds <= currYBoundsTop)
                    {
                        return true;
                    }
                    
                    if(currYBoundsBottom <= (tempYbounds + SPRITE_HEIGHT - 1) && (tempYbounds + SPRITE_HEIGHT -1) <= currYBoundsTop)
                    {
                        return true;
                    }
                }
            }
            break;
            
        case(Actor::left):
            if(curr->doBlock())
            {
                if(tempXbounds >= currXBoundsLeft && tempXbounds <= currXBoundsRight)
                {
                    if(currYBoundsBottom <= tempYbounds && tempYbounds <= currYBoundsTop)
                    {
                        return true;
                    }
                    
                    if(currYBoundsBottom <= (tempYbounds + SPRITE_HEIGHT - 1) && (tempYbounds + SPRITE_HEIGHT -1) <= currYBoundsTop)
                    {
                        return true;
                    }
                }
            }
            break;
            
        default:
            //should never run but if it does, do nothing
            break;
    }
    //if nothing has returned up to this point then there is no blockage
    return false;
}

int StudentWorld::getnLandMines() const
{
    return m_nLandmines;
}

int StudentWorld::getnFlamethrower() const
{
    return m_nFlamethrower;
}

int StudentWorld::getnVaccines() const
{
    return m_nVaccines;
}

void StudentWorld::incrLandMines(int amt)
{
    m_nLandmines += amt;
}
void StudentWorld::incrFlamethrower(int amt)
{
    m_nFlamethrower += amt;
}
void StudentWorld::incrVaccines(int amt)
{
    m_nVaccines += amt;
}

void StudentWorld::incrNCivilians(int amt)
{
    m_nCivilians += amt;
}

void StudentWorld::createLandmine(Penelope* player)
{
    Actor* tempLandmine;
    tempLandmine = new Landmine(30, this, player->getX(), player->getY());
    m_actors.push_back(tempLandmine);
}

void StudentWorld::createPit(Actor* pit)
{
    Actor* tempPit;
    tempPit = new Pit(this, pit->getX(), pit->getY());
    tempPit->notBurnable();
    m_actors.push_back(tempPit);
}

bool StudentWorld::createVomit(int xCoords, int yCoords, Direction dir)
{
    Actor* tempVomit;
    tempVomit = new Vomit(2, this, xCoords, yCoords, dir);
    tempVomit->notBurnable();

    //calculate the chance
    int x = randInt(1, 3);

    switch(x)
    {
        case(1):
        case(2):
            cerr << "not this time" << endl;
            delete tempVomit;
            return false;
            break;
        case(3):
            //calculate the chance
            cerr << "vomitting" << endl;
            this->playSound(SOUND_ZOMBIE_VOMIT);
            m_actors.push_back(tempVomit);
            return true;
            break;
    }

    return false;
}

bool StudentWorld::createSingleFlame(int xCoord, int yCoord, Direction dir)
{
    Actor* tempFlame;
    tempFlame = new Flame(2, this, xCoord, yCoord, dir);
    tempFlame->notBurnable();

//    //If a flame at posi would overlap with a wall or exit object, then immediately stop the loop.
    std::string thingOverlap = "";

    bool overlapped = this->overlap(xCoord, yCoord, tempFlame, thingOverlap);
    if(overlapped)
    {
        //if overlap with a wall or exit or pit
        if(thingOverlap == "block")
        {
            delete tempFlame;   //prevent memory leak
            return false;
        }
    }

//    //otherwise, create a new flame object at new location
    m_actors.push_back(tempFlame);
    return true;
}

void StudentWorld::createZombie(Actor* civil)
{
//    int x = randInt(1, 10);
//    switch(x)
//    {
//        case 1:
//        case 2:
//        case 3:
//            Actor* tempZombie2;
//            tempZombie2 = new SmartZombie(this, civil->getX(), civil->getY(), "");
//            tempZombie2->yesBlock();
//            m_actors.push_back(tempZombie2);
//            break
//        default:
//
    this->increaseScore(-1000);
            Actor* tempZombie;
            tempZombie = new DumbZombie(this, civil->getX(), civil->getY(), "ds");
            tempZombie->yesBlock();
            m_actors.push_back(tempZombie);
//            break;
//    }
//    return;
}
void StudentWorld::createFlames(Actor* curr, Direction dir)
{
    //if penelope is the one creating the flames
    if(curr->getMarker() == "p")
    {
        //determine which direction penelope is facing and place 3 flames in front of her accordingly
        switch(dir)
        {
            case (Actor::up):
                for(int i = 1; i < 4; i++)
                {
                    int tempXFlame = curr->getX();
                    int tempYFlame = curr->getY() + (i * SPRITE_HEIGHT);
                    
                    bool notBlocked = this->createSingleFlame(tempXFlame, tempYFlame, dir);
                    if(!notBlocked)
                    {
                        break;
                    }

                }
                break;
            case (GraphObject::down):
                for(int i = 1; i < 4; i++)
                {
                    int tempXFlame = curr->getX();
                    int tempYFlame = curr->getY() - (i * SPRITE_HEIGHT);
                    
                    bool notBlocked = this->createSingleFlame(tempXFlame, tempYFlame, dir);
                    if(!notBlocked)
                    {
                        break;
                    }
                    
                }
                break;
            case(GraphObject::left):
                for(int i = 1; i < 4; i++)
                {
                    int tempXFlame = curr->getX() - (i * SPRITE_WIDTH);
                    int tempYFlame = curr->getY();
                    
                    bool notBlocked = this->createSingleFlame(tempXFlame, tempYFlame, dir);
                    if(!notBlocked)
                    {
                        break;
                    }
                }
                break;
            case(GraphObject::right):
                for(int i = 1; i < 4; i++)
                {
                    int tempXFlame = curr->getX() + (i * SPRITE_WIDTH);
                    int tempYFlame = curr->getY();
                    bool notBlocked = this->createSingleFlame(tempXFlame, tempYFlame, dir);
                    
                    if(!notBlocked)
                    {
                        break;
                    }
                }
                break;
            default:
                break;
        }
    }
    if(curr->getMarker() == "land")
    {
        //Introduce a flame object at the same (x,y) location as the landmine .
        //d. Introduce flame objects in the eight adjacent slots around the landmine (north, northeast, east, southeast, south, southwest, west, northwest). Each such adjacent spot must exactly SPRITE_WIDTH pixels away horizontally and/or SPRITE_HEIGHT pixels away vertically. (SPRITE_WIDTH and SPRITE_HEIGHT are both 16.) So if the landmine goodie were at position (100, 100), the northwest flame would be added at (84, 116), the east goodie at (116, 100), the southeast goodie at (116, 84), etc.
        
        this->createSingleFlame(curr->getX(), curr->getY(), Actor::up);
        //north
        this->createSingleFlame(curr->getX(), curr->getY() + SPRITE_HEIGHT, Actor::up);
        //south
        this->createSingleFlame(curr->getX(), curr->getY() - SPRITE_HEIGHT, Actor::up);
        //west
        this->createSingleFlame(curr->getX() - SPRITE_WIDTH, curr->getY(), Actor::up);
        //east
        this->createSingleFlame(curr->getX() + SPRITE_WIDTH, curr->getY(), Actor::up);
        //northeast
        this->createSingleFlame(curr->getX() + SPRITE_WIDTH, curr->getY() + SPRITE_HEIGHT, Actor::up);
        //northwest
        this->createSingleFlame(curr->getX() - SPRITE_WIDTH, curr->getY() + SPRITE_HEIGHT, Actor::up);
        //southwest
        this->createSingleFlame(curr->getX() - SPRITE_WIDTH, curr->getY() - SPRITE_HEIGHT, Actor::up);
        //southeast
        this->createSingleFlame(curr->getX() + SPRITE_WIDTH, curr->getY() - SPRITE_HEIGHT, Actor::up);
    }
}

void StudentWorld::comeToMe(int tempX, int tempY, Actor* curr)
{
    int wouldBeX;
    int wouldBeY;
    if(tempX == m_player->getX())
    {
        //determine if penelope is above or below the civilian
        int diff = tempY - m_player->getY();
        if(diff < 0)
        {
            //this means that civilian is below penelope
            //the civilian wants to go up
            int dir = Actor::up;
            //check if theres a wall, zombie, etc
            //if there is no overlap
            wouldBeX = tempX;
            wouldBeY = tempY + 2;
            if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
            {
                if(!(this->blocked(wouldBeX, wouldBeY, dir, curr)))
                {
                    //... move civilian to the up...;
                    curr->setDirection(dir);
                    curr->moveTo(wouldBeX, wouldBeY);
                    return;
                }
            }
            return;
        }
        else
        {
            //this means that the civilian is above Penelope
            //the civilian wants to go down
            int dir = Actor::down;
            //check if theres a wall, zombie, etc
            //if there is no overlap
            wouldBeX = tempX;
            wouldBeY = tempY - 2;
            if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
            {
                if(!(this->blocked(wouldBeX, wouldBeY, dir, curr)))
                {
                    //... move the civilian down ...;
                    curr->setDirection(dir);
                    curr->moveTo(wouldBeX, wouldBeY);
                    return;
                }
            }
            return;
        }
    }
    else if (tempY == m_player->getY())
    {
        int diff = tempX - m_player->getX();
        if(diff < 0)
        {
            //this means that civilian is to the left of penelope
            //the civilian wants to go right
            int dir = Actor::right;
            //check if theres a wall, zombie, etc
            //if there is no overlap
            wouldBeX = tempX + 2;
            wouldBeY = tempY;
            if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
            {
                if(!(this->blocked(wouldBeX, wouldBeY, dir, curr)))
                {
                    //... move civilian to the up...;
                    curr->setDirection(dir);
                    curr->moveTo(wouldBeX, wouldBeY);
                    return;
                }
            }
            return;
        }
        else
        {
            //this means that civilian is to the right of penelope
            //the civilian wants to go left
            int dir = Actor::left;
            //check if theres a wall, zombie, etc
            //if there is no overlap
            wouldBeX = tempX - 2;
            wouldBeY = tempY;
            if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
            {
                if(!(this->blocked(wouldBeX, wouldBeY, dir, curr)))
                {
                    //... move civilian to the left...;
                    curr->setDirection(dir);
                    curr->moveTo(wouldBeX, wouldBeY);
                    return;
                }
            }
            return;
        }
    }
    //penelope is nearby but not the same coordinates
    else
    {
        //determine the difference in x AND y
        int diffX = tempX - m_player->getX();
        int diffY = tempY - m_player->getY();
        
        int x = randInt(1,2);
        if( x == 1)
        {
            //go the x distance first
            if(diffX < 0)
            {
                //this means that civilian is to the left of penelope
                //the civilian wants to go right
                int dir = Actor::right;
                //check if theres a wall, zombie, etc
                //if there is no overlap
                wouldBeX = tempX + 2;
                wouldBeY = tempY;
                if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
                {
                    if(!(this->blocked(wouldBeX, wouldBeY, dir, curr)))
                    {
                        //... move civilian to the up...;
                        curr->setDirection(dir);
                        curr->moveTo(wouldBeX, wouldBeY);
                        return;
                    }
                    //if youre blocked then go the other way
                    else
                    {
                        if(diffY < 0)
                        {
                            //this means that civilian is below penelope
                            //the civilian wants to go up
                            int dir = Actor::up;
                            //check if theres a wall, zombie, etc
                            //if there is no overlap
                            wouldBeX = tempX;
                            wouldBeY = tempY + 2;
                            if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
                            {
                                if(!(this->blocked(wouldBeX, wouldBeY, dir, curr)))
                                {
                                    //... move civilian to the up...;
                                    curr->setDirection(dir);
                                    curr->moveTo(wouldBeX, wouldBeY);
                                    return;
                                }
                            }
                        }
                        else
                        {
                            //this means that the civilian is above Penelope
                            //the civilian wants to go down
                            int dir = Actor::down;
                            //check if theres a wall, zombie, etc
                            //if there is no overlap
                            wouldBeX = tempX;
                            wouldBeY = tempY - 2;
                            if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
                            {
                                if(!(this->blocked(wouldBeX, wouldBeY, dir, curr)))
                                {
                                    //... move the civilian down ...;
                                    curr->setDirection(dir);
                                    curr->moveTo(wouldBeX, wouldBeY);
                                    return;
                                }
                                else
                                {
                                    
                                }
                            }
                        }
                    }
                    return;
                }
            }
            else
            {
                //this means that civilian is to the right of penelope
                //the civilian wants to go left
                int dir = Actor::left;
                //check if theres a wall, zombie, etc
                //if there is no overlap
                wouldBeX = tempX - 2;
                wouldBeY = tempY;
                if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
                {
                    if(!(this->blocked(wouldBeX, wouldBeY, dir, curr)))
                    {
                        //... move civilian to the left...;
                        curr->setDirection(dir);
                        curr->moveTo(wouldBeX, wouldBeY);
                        return;
                    }
                    else
                    {
                        if(diffY < 0)
                        {
                            //this means that civilian is below penelope
                            //the civilian wants to go up
                            int dir = Actor::up;
                            //check if theres a wall, zombie, etc
                            //if there is no overlap
                            wouldBeX = tempX;
                            wouldBeY = tempY + 2;
                            if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
                            {
                                if(!(this->blocked(wouldBeX, wouldBeY, dir, curr)))
                                {
                                    //... move civilian to the up...;
                                    curr->setDirection(dir);
                                    curr->moveTo(wouldBeX, wouldBeY);
                                    return;
                                }
                            }
                        }
                        else
                        {
                            //this means that the civilian is above Penelope
                            //the civilian wants to go down
                            int dir = Actor::down;
                            //check if theres a wall, zombie, etc
                            //if there is no overlap
                            wouldBeX = tempX;
                            wouldBeY = tempY - 2;
                            if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
                            {
                                if(!(this->blocked(wouldBeX, wouldBeY, dir, curr)))
                                {
                                    //... move the civilian down ...;
                                    curr->setDirection(dir);
                                    curr->moveTo(wouldBeX, wouldBeY);
                                    return;
                                }
                                else
                                {
                                    
                                }
                            }
                        }
                    }
                    return;
                }
            }
        }
        //do the Y first
        else
        {
            if(diffY < 0)
            {
                //this means that civilian is below penelope
                //the civilian wants to go up
                int dir = Actor::up;
                //check if theres a wall, zombie, etc
                //if there is no overlap
                wouldBeX = tempX;
                wouldBeY = tempY + 2;
                if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
                {
                    if(!(this->blocked(wouldBeX, wouldBeY, dir, curr)))
                    {
                        //... move civilian to the up...;
                        curr->setDirection(dir);
                        curr->moveTo(wouldBeX, wouldBeY);
                        return;
                    }
                    else
                    {
                        if(diffX < 0)
                        {
                            //this means that civilian is to the left of penelope
                            //the civilian wants to go right
                            int dir = Actor::right;
                            //check if theres a wall, zombie, etc
                            //if there is no overlap
                            wouldBeX = tempX + 2;
                            wouldBeY = tempY;
                            if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
                            {
                                if(!(this->blocked(wouldBeX, wouldBeY, dir, curr)))
                                {
                                    //... move civilian to the up...;
                                    curr->setDirection(dir);
                                    curr->moveTo(wouldBeX, wouldBeY);
                                    return;
                                }
                            }
                        }
                        else
                        {
                            //this means that civilian is to the right of penelope
                            //the civilian wants to go left
                            int dir = Actor::left;
                            //check if theres a wall, zombie, etc
                            //if there is no overlap
                            wouldBeX = tempX - 2;
                            wouldBeY = tempY;
                            if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
                            {
                                if(!(this->blocked(wouldBeX, wouldBeY, dir, curr)))
                                {
                                    //... move civilian to the left...;
                                    curr->setDirection(dir);
                                    curr->moveTo(wouldBeX, wouldBeY);
                                    return;
                                }
                            }
                        }
                    }
                    return;
                }
            }
            else
            {
                //this means that the civilian is above Penelope
                //the civilian wants to go down
                int dir = Actor::down;
                //check if theres a wall, zombie, etc
                //if there is no overlap
                wouldBeX = tempX;
                wouldBeY = tempY - 2;
                if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
                {
                    if(!(this->blocked(wouldBeX, wouldBeY, dir, curr)))
                    {
                        //... move the civilian down ...;
                        curr->setDirection(dir);
                        curr->moveTo(wouldBeX, wouldBeY);
                        return;
                    }
                    else
                    {
                        if(diffX < 0)
                        {
                            //this means that civilian is to the left of penelope
                            //the civilian wants to go right
                            int dir = Actor::right;
                            //check if theres a wall, zombie, etc
                            //if there is no overlap
                            wouldBeX = tempX + 2;
                            wouldBeY = tempY;
                            if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
                            {
                                if(!(this->blocked(wouldBeX, wouldBeY, dir, curr)))
                                {
                                    //... move civilian to the up...;
                                    curr->setDirection(dir);
                                    curr->moveTo(wouldBeX, wouldBeY);
                                    return;
                                }
                            }
                        }
                        else
                        {
                            //this means that civilian is to the right of penelope
                            //the civilian wants to go left
                            int dir = Actor::left;
                            //check if theres a wall, zombie, etc
                            //if there is no overlap
                            wouldBeX = tempX - 2;
                            wouldBeY = tempY;
                            if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
                            {
                                if(!(this->blocked(wouldBeX, wouldBeY, dir, curr)))
                                {
                                    //... move civilian to the left...;
                                    curr->setDirection(dir);
                                    curr->moveTo(wouldBeX, wouldBeY);
                                    return;
                                }
                            }
                        }
                    }
                    return;
                }
            }
        }
    }
}

void StudentWorld::runAway(int tempX, int tempY, Actor* curr, int& minDistance) 
{
    //for a direction
        //if blocked
            //ignore path
        //if not
            //determines the new minDistance if travelled to this location
    int wouldBeX;
    int wouldBeY;
    
    int wouldBeExchX = tempX;
    int wouldBeExchY = tempY;
    int wouldBeDirection = curr->getDirection();
    int wouldBeMin = minDistance;
    
    
    //for up
    wouldBeX = tempX;
    wouldBeY = tempY + 2;
    
    int dir = Actor::up;
    
    if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
    {
        if(!(this->blocked(wouldBeX, wouldBeY, dir, curr)))
        {
            int diff = computeDistanceZombie(wouldBeX, wouldBeY);
            if(diff > minDistance)
            {
                wouldBeMin = diff;
                wouldBeExchX = wouldBeX;
                wouldBeExchY = wouldBeY;
                wouldBeDirection = dir;
            }
        }
    }
    //for down
    dir= Actor::down;
    wouldBeX = tempX;
    wouldBeY = tempY - 2;
    
    if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
    {
        if(!(this->blocked(wouldBeX, wouldBeY, dir, curr)))
        {
            int diff = computeDistanceZombie(wouldBeX, wouldBeY);
            if(diff > minDistance)
            {
                wouldBeMin = diff;
                wouldBeExchX = wouldBeX;
                wouldBeExchY = wouldBeY;
                wouldBeDirection = dir;
            }
        }
    }
    
    //for right
    dir= Actor::right;
    wouldBeX = tempX + 2;
    wouldBeY = tempY;
    if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
    {
        if(!(this->blocked(wouldBeX, wouldBeY, dir, curr)))
        {
            int diff = computeDistanceZombie(wouldBeX, wouldBeY);
            if(diff > minDistance)
            {
                wouldBeMin = diff;
                wouldBeExchX = wouldBeX;
                wouldBeExchY = wouldBeY;
                wouldBeDirection = dir;
            }
        }
    }
    
    dir= Actor::left;
    wouldBeX = tempX - 2;
    wouldBeY = tempY;
    if(wouldBeX >= 0 && wouldBeX <= 256 && wouldBeY >= 0 && wouldBeY <= 256)
    {
        if(!(this->blocked(wouldBeX, wouldBeY, dir, curr)))
        {
            int diff = computeDistanceZombie(wouldBeX, wouldBeY);
            if(diff >
               minDistance)
            {
                wouldBeMin = diff;
                wouldBeExchX = wouldBeX;
                wouldBeExchY = wouldBeY;
                wouldBeDirection = dir;
            }
        }
    }
    
    //all of the different paths would lead the civilian closer to a zombie
    if(wouldBeMin == minDistance)
    {
        //stay where you are
        return;
    }
    else
    {
        //... move civilian to whatever direction..;
        curr->setDirection(wouldBeDirection);
        curr->moveTo(wouldBeExchX, wouldBeExchY);
        return;
    }
}
void StudentWorld:: MPCivilian(int tempX, int tempY, Actor* curr, Direction& dir)
{
    if(tempX == curr->getX())
    {
        //determine if penelope is above or below the civilian
        int diff = tempY - curr->getY();
        if(diff < 0)
        {
            //this means that civilian is below penelope
            //the zombie wants to go up
            dir = Actor::up;
            return;
        }
        else
        {
            //this means that the civilian is above Penelope
            //the zombie wants to go down
            dir = Actor::down;
            return;
        }
    }
    else if (tempY == curr->getY())
    {
        int diff = tempX - curr->getX();
        if(diff < 0)
        {
            //this means that civilian is to the left of penelope
            //the civilian wants to go right
            dir = Actor::right;
            return;
        }
        else
        {
            //this means that civilian is to the right of penelope
            //the civilian wants to go left
            dir = Actor::left;
            return;
        }
    }
    //penelope is nearby but not the same coordinates
    else
    {
        //determine the difference in x AND y
        int diffX = tempX - curr->getX();
        int diffY = tempY - curr->getY();
        
        int x = randInt(1,2);
        if( x == 1)
        {
            //go the x distance first
            if(diffX < 0)
            {
                //this means that civilian is to the left of penelope
                //the zombie wants to go right
                dir = Actor::right;
                //check if theres a wall, zombie, etc
                //if there is no overlap
                return;
            }
            else
            {
                //this means that civilian is to the right of penelope
                //the civilian wants to go left
                dir = Actor::left;
                return;
            }
        }
        //do the Y first
        else
        {
            if(diffY < 0)
            {
                //this means that civilian is below penelope
                //the civilian wants to go up
                dir = Actor::up;
                
                return;
            }
            else
            {
                //this means that the civilian is above Penelope
                //the civilian wants to go down
                dir = Actor::down;
                
                return;
            }
        }
        
    }
}
void StudentWorld::MPplayer(int tempX, int tempY, Actor* curr, Direction& dir)
{
    if(tempX == m_player->getX())
    {
        //determine if penelope is above or below the civilian
        int diff = tempY - m_player->getY();
        if(diff < 0)
        {
            //this means that civilian is below penelope
            //the zombie wants to go up
            dir = Actor::up;
            return;
        }
        else
        {
            //this means that the civilian is above Penelope
            //the zombie wants to go down
            dir = Actor::down;
            return;
        }
    }
    else if (tempY == m_player->getY())
    {
        int diff = tempX - m_player->getX();
        if(diff < 0)
        {
            //this means that civilian is to the left of penelope
            //the civilian wants to go right
            dir = Actor::right;
            return;
        }
        else
        {
            //this means that civilian is to the right of penelope
            //the civilian wants to go left
            dir = Actor::left;
            return;
        }
    }
    //penelope is nearby but not the same coordinates
    else
    {
        //determine the difference in x AND y
        int diffX = tempX - m_player->getX();
        int diffY = tempY - m_player->getY();
        
        int x = randInt(1,2);
        if( x == 1)
        {
            //go the x distance first
            if(diffX < 0)
            {
                //this means that civilian is to the left of penelope
                //the zombie wants to go right
                dir = Actor::right;
                //check if theres a wall, zombie, etc
                //if there is no overlap
                return;
            }
            else
            {
                //this means that civilian is to the right of penelope
                //the civilian wants to go left
                dir = Actor::left;
                return;
            }
        }
        //do the Y first
        else
        {
            if(diffY < 0)
            {
                //this means that civilian is below penelope
                //the civilian wants to go up
                dir = Actor::up;
                
                return;
            }
            else
            {
                //this means that the civilian is above Penelope
                //the civilian wants to go down
                dir = Actor::down;
                
                return;
            }
        }
        
    }
}
