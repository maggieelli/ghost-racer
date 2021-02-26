#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;

const double GHOST_RACER_X = 128;
const double GHOST_RACER_Y = 32;
const int ZOMBIE_PED_SCORE = 150;
const int ZOMBIE_CAB_SCORE = 200;
const int HEALING_GOODIE_SCORE = 250;
const int HOLY_WATER_GOODIE_SCORE = 50;
const int SOUL_GOODIE_SCORE = 100;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

/*
 CLASS STRUCTURE
    GameWorld
        StudentWorld
    Actor
        DamagableActor
            * Ghost Racer
            Intelligent Agent
                Pedestrian
                    * Human Pedestrian
                    * Zombie Pedestrian
                * Zombie Cab
        Object
            GhostRacerActivatedObject
                HolyWaterAffectedObject
                    * Healing
                    * Holy Water
                * Soul
                * Oil Slick
            * Border Line
            * Water Projectile
 */

class Actor:public GraphObject {
    public:
        Actor(StudentWorld* world, double vert_speed, double horiz_speed, bool collisionAvoidanceWorthy, int imageID, double startX, double startY, int startDirection, double size, int depth = 0);
        virtual ~Actor();
        virtual void doSomething() = 0;
        virtual bool sprayIfAppropriate();
        bool isAlive();
        void die();
        double getVertSpeed();
        bool isCollisionAvoidanceWorthy();
    
    protected:
        virtual void move();
        virtual bool isOffscreen();
        StudentWorld* getWorld();
        void setVertSpeed(double speed);
        double getHorizSpeed();
        void setHorizSpeed(double speed);
    
    private:
        bool m_isAlive;
        StudentWorld* m_world;
        double m_vert_speed;
        double m_horiz_speed;
        bool m_collision_avoidance_worthy;
};

class Object:public Actor {
    public:
        Object(StudentWorld* world, int imageID, double startX, double startY, int startDirection, double size, int depth = 2);
        virtual ~Object();
};

class BorderLine:public Object {
    public:
        BorderLine(StudentWorld* world, int imageID, double startX, double startY);
        virtual ~BorderLine();
        virtual void doSomething();
};

class DamageableActor:public Actor {
    public:
        DamageableActor(StudentWorld* world, int health, double vert_speed, double horiz_speed, int imageID, double startX, double startY, int startDirection, double size);
        virtual ~DamageableActor();
        int getHealth();
    
    protected:
        void setHealth(int health);
    
    private:
        int m_health;
};

class GhostRacer:public DamageableActor {
    public:
        GhostRacer(StudentWorld* world);
        virtual ~GhostRacer();
        virtual void doSomething();
        int getHolyWater();
        void addHolyWater(int n_holy_water);
        void damage(int hit_damage);
        void heal(int heal_points);
        
    protected:
        virtual bool isOffscreen();
        virtual void move();
    
    private:
        int m_holy_water;
};

class IntelligentAgent:public DamageableActor {
    public:
        IntelligentAgent(StudentWorld* world, int health, double vert_speed, int imageID, double startX, double startY, int startDirection, double size);
        virtual ~IntelligentAgent();
    
    protected:
        int getMovementPlan();
        void setMovementPlan(int movement_plan);
        virtual void pickNewMovementPlan() = 0;
        
    private:
        int m_movement_plan;
};

class Pedestrian:public IntelligentAgent {
    public:
        Pedestrian(StudentWorld* world, int health, int imageID, double startX, double startY, int startDirection, double size);
        virtual ~Pedestrian();
    
    protected:
        virtual void pickNewMovementPlan();
};

class HumanPed:public Pedestrian {
    public:
        HumanPed(StudentWorld* world, double startX, double startY);
        virtual ~HumanPed();
        virtual void doSomething();
        virtual bool sprayIfAppropriate();
};

class ZombiePed:public Pedestrian {
    public:
        ZombiePed(StudentWorld* world, double startX, double startY);
        virtual ~ZombiePed();
        virtual void doSomething();
        virtual bool sprayIfAppropriate();
        
    private:
        int m_till_next_grunt = 0;
};

class ZombieCab:public IntelligentAgent {
    public:
        ZombieCab(StudentWorld* world, double vert_speed, double startX, double startY);
        virtual ~ZombieCab();
        virtual void doSomething();
        virtual bool sprayIfAppropriate();
    
    protected:
        virtual void pickNewMovementPlan();
    
    private:
        bool hasDamagedGR;
};

class GhostRacerActivatedObject:public Object {
    public:
        GhostRacerActivatedObject(StudentWorld* world, int imageID, double startX, double startY, int startDirection, double size, bool mustRotate = false);
        virtual ~GhostRacerActivatedObject();
        virtual void doSomething();
    
    protected:
        virtual void activate() = 0;
    
    private:
        bool m_mustRotate;
};

class OilSlick:public GhostRacerActivatedObject {
    public:
        OilSlick(StudentWorld* world, double startX, double startY, double size);
        virtual ~OilSlick();
    
    protected:
        virtual void activate();
};

class HolyWaterAffectedObject:public GhostRacerActivatedObject {
    public:
        HolyWaterAffectedObject(StudentWorld* world, int imageID, double startX, double startY, int startDirection, double size);
        virtual ~HolyWaterAffectedObject();
        virtual bool sprayIfAppropriate();
};

class HealingGoodie:public HolyWaterAffectedObject {
    public:
        HealingGoodie(StudentWorld* world, double startX, double startY);
        virtual ~HealingGoodie();
    
    protected:
        virtual void activate();
};

class HolyWaterGoodie:public HolyWaterAffectedObject {
    public:
        HolyWaterGoodie(StudentWorld* world, double startX, double startY);
        virtual ~HolyWaterGoodie();
    
    protected:
        virtual void activate();
};

class SoulGoodie:public GhostRacerActivatedObject {
    public:
        SoulGoodie(StudentWorld* world, double startX, double startY);
        ~SoulGoodie();
    
    protected:
        virtual void activate();
};

class HolyWaterProjectile:public Object {
    public:
        HolyWaterProjectile(StudentWorld* world, double startX, double startY, int startDirection);
        ~HolyWaterProjectile();
        virtual void doSomething();
    
    protected:
        virtual void move();
        
    private:
        int m_pixels_moved;
};

#endif // ACTOR_H_
