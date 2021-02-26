#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <list>
using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

const int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
const int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;
const int FIRST_LANE_CENTER = ROAD_CENTER - ROAD_WIDTH / 3;
const int LEFT_WHITE_BORDER_LINE = LEFT_EDGE + ROAD_WIDTH / 3;
const int RIGHT_WHITE_BORDER_LINE = RIGHT_EDGE - ROAD_WIDTH / 3;
const int START_BONUS = 5000;

class StudentWorld : public GameWorld
{
    public:
        StudentWorld(std::string assetPath);
        virtual ~StudentWorld();
        virtual int init();
        virtual int move();
        virtual void cleanUp();
        GhostRacer* getGR();
        void addNewOilSlick(double posX, double posY);
        void addNewHealingGoodie(double posX, double posY);
        void addNewHolyWaterProjectile();
        bool overlapping(Actor* ac1, Actor* ac2);
        bool sprayOverlappingHolyWaterAffectedActor(HolyWaterProjectile* hw);
        Actor* closestCollisionAvoidanceWorthyActor(ZombieCab* zc, string front_or_back);
        void saveSoul();
        
    protected:
        void insertBorderLines();
        void addNewBorderLines();
        void addNewHumanPed();
        void addNewZombiePed();
        void addNewZombieCabs();
        void addNewOilSlick();
        void addNewHolyWaterGoodie();
        void addNewLostSoulGoodie();
        Actor* closestCollisionAvoidanceWorthyActor(string top_or_bottom, double left_boundary, double right_boundary);
        void updateStatusText();

    private:
        list<Actor*> actorList;
        GhostRacer* m_gr;
        double m_last_white_border_y;
        int m_nSouls;
        int m_soulsSaved;
        int m_bonus;
};

#endif // STUDENTWORLD_H_
