#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

/*
 CLASS STRUCTURE
    GameWorld
        StudentWorld
    Actor
        DamagableActor
            Ghost Racer
            Intelligent Agent
                Human Pedestrian
                Zombie Pedestrian
                Zombie Cab
        Object
            Goodies
                Healing
                HolyWater
                Soul
            BorderLine
            OilSlick
            WaterProjectile
 */

class Actor:public GraphObject {
    public:
        Actor(StudentWorld* world, double vert_speed, double horiz_speed, bool collisionAvoidanceWorthy, int imageID, double startX, double startY, int startDirection, double size, int depth = 0);
        virtual ~Actor();
        virtual bool isOffscreen();
        virtual void doSomething() = 0;
        virtual void move() = 0;
        bool isAlive();
        void die();
        StudentWorld* getWorld();
        double getVertSpeed();
        void setVertSpeed(double speed);
        double getHorizSpeed();
        void setHorizSpeed(double speed);
        bool isCollisionAvoidanceWorthy();
    
    private:
        bool m_isAlive;
        StudentWorld* m_world;
        double m_vert_speed;
        double m_horiz_speed;
        bool m_collision_avoidance_worthy;
};

class Object:public Actor {
    public:
        Object(StudentWorld* world, double vert_speed, double horiz_speed, int imageID, double startX, double startY, int startDirection, double size, int depth = 0);
        virtual ~Object();
        virtual void doSomething() = 0;
        virtual void move();
};

class BorderLine:public Object {
    public:
        BorderLine(StudentWorld* world, int imageID, double startX, double startY);
        virtual ~BorderLine();
        virtual void doSomething();
};

class DamageableActor:public Actor {
    public:
        DamageableActor(StudentWorld* world, int health, double vert_speed, double horiz_speed, int imageID, double startX, double startY, int startDirection, double size, int depth = 0);
        virtual ~DamageableActor();
        int getHealth();
        void setHealth(int health);
        void damageGhostRacer(int hit_damage);
        virtual void doSomething() = 0;
        virtual void move() = 0;
    private:
        int m_health;
};

class GhostRacer:public DamageableActor {
    public:
        GhostRacer(StudentWorld* world);
        virtual ~GhostRacer();
        int getHolyWater();
        void addHolyWater(int n_holy_water);
        double getForwardSpeed();
        virtual bool isOffscreen();
        virtual void heal(int heal_points);
        virtual void doSomething();
        virtual void move();
        
    private:
        int m_holy_water;
        double m_forward_speed;
};

class IntelligentAgent:public DamageableActor {
    public:
        IntelligentAgent(StudentWorld* world, int health, int imageID, double startX, double startY, int startDirection, double size);
        virtual ~IntelligentAgent();
        virtual void doSomething() = 0;
        virtual void move();
        int getMovementPlan();
        void decrementMovementPlan();
        void pickNewMovementPlan();
        virtual void damage(int hit_damage) = 0;
        
    private:
        int m_movement_plan;
};

class HumanPed:public IntelligentAgent {
    public:
        HumanPed(StudentWorld* world, double startX, double startY);
        virtual ~HumanPed();
        virtual void doSomething();
        virtual void damage(int hit_damage);
};

class ZombiePed:public IntelligentAgent {
    public:
        ZombiePed(StudentWorld* world, double startX, double startY);
        virtual ~ZombiePed();
        virtual void doSomething();
        virtual void damage(int hit_damage);
        
    private:
        int m_till_next_grunt = 0;
};

#endif // ACTOR_H_
