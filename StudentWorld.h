#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <list>
#include <iterator>
#include <cmath>
using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

const int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
const int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    GhostRacer* getGR();
    void insertBorderLines();
    void addNewBorderLines();
    void addNewHumanPed();
    void addNewZombiePed();
    bool overlappingGR(Actor* ac);
    void saveSoul();
    void addToScore(int score);
    void updateStatusText();

private:
    list<Actor*> actorList;
    GhostRacer* m_gr;
    BorderLine* m_last_white_border_line;
    int m_nSouls;
    int m_soulsSaved;
    int m_bonus;
};

#endif // STUDENTWORLD_H_

