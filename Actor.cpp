#include "Actor.h"
#include "StudentWorld.h"
#include <math.h>
using namespace std;

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(StudentWorld* world, double vert_speed, double horiz_speed, bool collisionAvoidanceWorthy, int imageID, double startX, double startY, int startDirection, double size, int depth)
    : GraphObject(imageID, startX, startY, startDirection, size, depth) {
    m_isAlive = true;
    m_world = world;
    m_vert_speed = vert_speed;
    m_horiz_speed = horiz_speed;
    m_collision_avoidance_worthy = collisionAvoidanceWorthy;
}
             
Actor::~Actor() {
}

bool Actor::isOffscreen() {
    if (getX() < 0 || getX() > VIEW_WIDTH || getY() < 0 || getY() > VIEW_HEIGHT) {
        return true;
    }
    return false;
}

bool Actor::isAlive() {
    return m_isAlive;
}

void Actor::die() {
    m_isAlive = false;
}

StudentWorld* Actor::getWorld() {
    return m_world;
}

double Actor::getVertSpeed() {
    return m_vert_speed;
}

void Actor::setVertSpeed(double speed) {
    m_vert_speed = speed;
}

double Actor::getHorizSpeed() {
    return m_horiz_speed;
}

void Actor::setHorizSpeed(double speed) {
    m_horiz_speed = speed;
}

bool Actor::isCollisionAvoidanceWorthy() {
    return m_collision_avoidance_worthy;
}

Object::Object(StudentWorld* world, double vert_speed, double horiz_speed, int imageID, double startX, double startY, int startDirection, double size, int depth)
    : Actor(world, vert_speed, horiz_speed, false, imageID, startX, startY, startDirection, size, 1) {
}

Object::~Object() {
}

void Object::move() {
    moveTo(getX(), getY() + (getVertSpeed() - getWorld()->getGR()->getVertSpeed()));
}

BorderLine::BorderLine(StudentWorld* world, int imageID, double startX, double startY)
    : Object(world, -4, 0, imageID, startX, startY, 0, 2, 2) {
}

BorderLine::~BorderLine() {
}

void BorderLine::doSomething() {
    move();
    if (isOffscreen()) {
        die();
    }
}

DamageableActor::DamageableActor(StudentWorld* world, int health, double vert_speed, double horiz_speed, int imageID, double startX, double startY, int startDirection, double size, int depth)
    : Actor(world, vert_speed, horiz_speed, true, imageID, startX, startY, startDirection, size, depth) {
    m_health = health;
}

DamageableActor::~DamageableActor() {
}

int DamageableActor::getHealth() {
    return m_health;
}

void DamageableActor::setHealth(int health) {
    m_health = health;
}

GhostRacer::GhostRacer(StudentWorld* world)
: DamageableActor(world, 100, 0, 0, IID_GHOST_RACER, 128, 32, 90, 4) {
    m_holy_water = 10;
}

GhostRacer::~GhostRacer() {
}

bool GhostRacer::isOffscreen() {
    if (getX() <= LEFT_EDGE || getX() >= RIGHT_EDGE) {
        return true;
    }
    return false;
}

int GhostRacer::getHolyWater() {
    return m_holy_water;
}

void GhostRacer::addHolyWater(int n_holy_water) {
    m_holy_water += n_holy_water;
}

double GhostRacer::getForwardSpeed() {
    return m_forward_speed;
}

void GhostRacer::damage(int hit_damage) {
    setHealth(getHealth() - hit_damage);
    if (getHealth() <= 0) {
        die();
        getWorld()->playSound(SOUND_PLAYER_DIE);
    }
}

void GhostRacer::heal(int heal_points) {
    if (getHealth() < 100) {
        setHealth(getHealth() + heal_points);
    }
}

void GhostRacer::doSomething() {
    if (!isAlive()) {
        return;
    }
    if (isOffscreen()) {
        if (getX() <= LEFT_EDGE) {
            if (getDirection() > 90) {
                damage(10);
                setDirection(82);
                getWorld()->playSound(SOUND_VEHICLE_CRASH);
            }
        }
        else if (getX() >= RIGHT_EDGE) {
            if (getDirection() < 90) {
                damage(10);
                setDirection(98);
                getWorld()->playSound(SOUND_VEHICLE_CRASH);
            }
        }
    }
    else {
        int ch;
        if (getWorld()->getKey(ch)) {
            switch(ch) {
                // TODO
                case KEY_PRESS_SPACE:
                    break;
                case KEY_PRESS_LEFT:
                    if (getDirection() < 114) {
                        setDirection(getDirection() + 8);
                    }
                    break;
                case KEY_PRESS_RIGHT:
                    if (getDirection() > 66) {
                        setDirection(getDirection() - 8);
                    }
                    break;
                case KEY_PRESS_UP:
                    if (m_forward_speed < 5) {
                        m_forward_speed++;
                    }
                    break;
                case KEY_PRESS_DOWN:
                    if (m_forward_speed > -1) {
                        m_forward_speed--;
                    }
                    break;
            }
        }
    }
    setVertSpeed(m_forward_speed * sin(getDirection() * M_PI / 180));
    move();
}

void GhostRacer::move() {
    // TODO
    double delta_x = cos(getDirection() * M_PI / 180) * 4;
    moveTo(getX() + delta_x, getY());
}

IntelligentAgent::IntelligentAgent(StudentWorld* world, int health, int imageID, double startX, double startY, int startDirection, double size)
    : DamageableActor(world, health, -4, 0, imageID, startX, startY, startDirection, size) {
        m_movement_plan = 0;
}

IntelligentAgent::~IntelligentAgent() {
}

void IntelligentAgent::move() {
    double new_y = getY() + (getVertSpeed() - getWorld()->getGR()->getVertSpeed());
    double new_x = getX() + getHorizSpeed();
    moveTo(new_x, new_y);
    if (isOffscreen()) {
        die();
        return;
    }
}

int IntelligentAgent::getMovementPlan() {
    return m_movement_plan;
}

void IntelligentAgent::setMovementPlan(int movement_plan) {
    m_movement_plan = movement_plan;
}

Pedestrian::Pedestrian(StudentWorld* world, int health, int imageID, double startX, double startY, int startDirection, double size)
: IntelligentAgent(world, health, imageID, startX, startY, startDirection, size){
}

Pedestrian::~Pedestrian() {
}

void Pedestrian::pickNewMovementPlan() {
    int new_horiz_speed = randInt(-3, 3);
    while (new_horiz_speed == 0) {
        new_horiz_speed = randInt(-3, 3);
    }
    setHorizSpeed(new_horiz_speed);
    setMovementPlan(randInt(4, 32));
    if (getHorizSpeed() < 0) {
        setDirection(180);
    }
    else {
        setDirection(0);
    }
}

HumanPed::HumanPed(StudentWorld* world, double startX, double startY)
: Pedestrian(world, 2, IID_HUMAN_PED, startX, startY, 0, 2) {
}

HumanPed::~HumanPed() {
}

void HumanPed::doSomething() {
    if (!isAlive()) {
        return;
    }
    if (getWorld()->overlappingGR(this)) {
        getWorld()->getGR()->die();
        return;
    }
    move();
    if (!isAlive()) {
        return;
    }
    setMovementPlan(getMovementPlan() - 1);
    if (getMovementPlan() > 0) {
        return;
    }
    pickNewMovementPlan();
}

// called by holy water projectile damage(0)
void HumanPed::damage(int hit_damage) {
    setHealth(getHealth() - hit_damage);
    setHorizSpeed(getHorizSpeed() * -1);
    if (getDirection() == 0) {
        setDirection(180);
    }
    else {
        setDirection(0);
    }
    getWorld()->playSound(SOUND_PED_HURT);
}

ZombiePed::ZombiePed(StudentWorld* world, double startX, double startY)
: Pedestrian(world, 2, IID_ZOMBIE_PED, startX, startY, 0, 3) {
    m_till_next_grunt = 0;
}

ZombiePed::~ZombiePed() {
}

void ZombiePed::doSomething() {
    if (!isAlive()) {
        return;
    }
    if (m_till_next_grunt <= 0) {
        getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
        m_till_next_grunt = 20;
    }
    if (getWorld()->overlappingGR(this)) {
        getWorld()->getGR()->damage(5);
        damage(2);
        return;
    }
    if (abs(getX() - getWorld()->getGR()->getX()) < 30 && getY() > getWorld()->getGR()->getY()) {
        setDirection(270);
        if (getX() < getWorld()->getGR()->getX()) {
            setHorizSpeed(1);
        }
        else if (getX() > getWorld()->getGR()->getX()) {
            setHorizSpeed(-1);
        }
        else {
            setHorizSpeed(0);
        }
        m_till_next_grunt--;
    }
    move();
    if (!isAlive()) {
        return;
    }
    if (getMovementPlan() > 0) {
        setMovementPlan(getMovementPlan() - 1);
        return;
    }
    pickNewMovementPlan();
}

// called by doSomething and holy water projectile
void ZombiePed::damage(int hit_damage) {
    setHealth(getHealth() - hit_damage);
    if (getHealth() <= 0) {
        die();
        getWorld()->playSound(SOUND_PED_DIE);
        if(!getWorld()->overlappingGR(this)) {
            // TODO chance add healing goodie to world
        }
        getWorld()->increaseScore(150);
    }
    else {
        getWorld()->playSound(SOUND_PED_HURT);
    }
}

ZombieCab::ZombieCab(StudentWorld* world, double startX, double startY)
: IntelligentAgent(world, 3, IID_ZOMBIE_CAB, startX, startY, 90, 4){
    hasDamagedGR = false;
}

ZombieCab::~ZombieCab() {
}

void ZombieCab::doSomething() {
    if (!isAlive()) {
        return;
    }
    if (getWorld()->overlappingGR(this) && hasDamagedGR == false) {
        getWorld()->playSound(SOUND_VEHICLE_CRASH);
        getWorld()->getGR()->damage(20);
        if (getX() <= getWorld()->getGR()->getX()) {
            setHorizSpeed(-5);
            setDirection(120 + randInt(0, 19));
        }
        else {
            setHorizSpeed(5);
            setDirection(60 - randInt(0, 19));
        }
        hasDamagedGR = true;
    }
    move();
    if (getVertSpeed() > getWorld()->getGR()->getVertSpeed()) {
        Actor* closestActor = getWorld()->closestCollisionAvoidanceWorthyActor(this, "front");
        if (closestActor->getY() - getY() < 96) {
            setVertSpeed(getVertSpeed() - 0.5);
            return;
        }
    }
    if (getVertSpeed() <= getWorld()->getGR()->getVertSpeed()) {
        Actor* closestActor = getWorld()->closestCollisionAvoidanceWorthyActor(this, "back");
        if (getY() - closestActor->getY() < 96) {
            setVertSpeed(getVertSpeed() + 0.5);
            return;
        }
    }
    setMovementPlan(getMovementPlan() - 1);
    if (getMovementPlan() > 0) {
        return;
    }
    pickNewMovementPlan();
}

void ZombieCab::damage(int hit_damage) {
    setHealth(getHealth() - hit_damage);
    if (getHealth() <= 0) {
        die();
        getWorld()->playSound(SOUND_VEHICLE_DIE);
        // TODO chance to drop oil slick
        getWorld()->increaseScore(200);
        return;
    }
    getWorld()->playSound(SOUND_VEHICLE_HURT);
}

void ZombieCab::pickNewMovementPlan() {
    setMovementPlan(randInt(4, 32));
    setVertSpeed(getVertSpeed() + randInt(-2, 2));
}
