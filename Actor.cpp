#include "Actor.h"
#include "StudentWorld.h"
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

// if appropriate, causes actor to be affected by holy water and returns true (overwritten by derived classes), default returns false
bool Actor::sprayIfAppropriate() {
    return false;
}

bool Actor::isAlive() {
    return m_isAlive;
}

// sets the actor's isAlive member to false
void Actor::die() {
    m_isAlive = false;
}

double Actor::getVertSpeed() {
    return m_vert_speed;
}

// returns whether actor is collision avoidance worthy
bool Actor::isCollisionAvoidanceWorthy() {
    return m_collision_avoidance_worthy;
}

// moves actor to correct new position every tick
void Actor::move() {
    double new_y = getY() + (getVertSpeed() - getWorld()->getGR()->getVertSpeed());
    double new_x = getX() + getHorizSpeed();
    moveTo(new_x, new_y);
    // if the actor is offscreen, die
    if (isOffscreen()) {
        die();
        return;
    }
}

// returns whether actor is offscreen
bool Actor::isOffscreen() {
    if (getX() < 0 || getX() > VIEW_WIDTH || getY() < 0 || getY() > VIEW_HEIGHT) {
        return true;
    }
    return false;
}

StudentWorld* Actor::getWorld() {
    return m_world;
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

DamageableActor::DamageableActor(StudentWorld* world, int health, double vert_speed, double horiz_speed, int imageID, double startX, double startY, int startDirection, double size)
    : Actor(world, vert_speed, horiz_speed, true, imageID, startX, startY, startDirection, size) {
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
: DamageableActor(world, GHOST_RACER_START_HEALTH, 0, 0, IID_GHOST_RACER, GHOST_RACER_X, GHOST_RACER_Y, 90, 4) {
    m_holy_water = 10;
}

GhostRacer::~GhostRacer() {
}

void GhostRacer::doSomething() {
    if (!isAlive()) {
        return;
    }
    // if Ghost Racer is off the road
    if (isOffscreen()) {
        // if it is off the left edge, change direction accordingly
        if (getX() <= LEFT_EDGE) {
            if (getDirection() > 90) {
                damage(10);
                setDirection(82);
                getWorld()->playSound(SOUND_VEHICLE_CRASH);
            }
        }
        // if it is off the right edge, change direction accordingly
        else if (getX() >= RIGHT_EDGE) {
            if (getDirection() < 90) {
                damage(10);
                setDirection(98);
                getWorld()->playSound(SOUND_VEHICLE_CRASH);
            }
        }
    }
    else {
        // handle input from user
        int ch;
        if (getWorld()->getKey(ch)) {
            switch(ch) {
                // spray holy water if possible
                case KEY_PRESS_SPACE:
                    if (m_holy_water > 0) {
                        getWorld()->addNewHolyWaterProjectile();
                        m_holy_water--;
                        getWorld()->playSound(SOUND_PLAYER_SPRAY);
                    }
                    break;
                // shift direction to left if possible
                case KEY_PRESS_LEFT:
                    if (getDirection() < 114) {
                        setDirection(getDirection() + 8);
                    }
                    break;
                // shift direction to right if possible
                case KEY_PRESS_RIGHT:
                    if (getDirection() > 66) {
                        setDirection(getDirection() - 8);
                    }
                    break;
                // speed up if possible
                case KEY_PRESS_UP:
                    if (getVertSpeed() < 5) {
                        setVertSpeed(getVertSpeed() + 1);
                    }
                    break;
                // slow down if possible
                case KEY_PRESS_DOWN:
                    if (getVertSpeed() > -1) {
                        setVertSpeed(getVertSpeed() - 1);
                    }
                    break;
            }
        }
    }
    // move Ghost Racer left or right on screen accordingly
    move();
}

int GhostRacer::getHolyWater() {
    return m_holy_water;
}

void GhostRacer::addHolyWater(int n_holy_water) {
    m_holy_water += n_holy_water;
}

// damages Ghost Racer with specified hit points, sets to dead if health is <= 0 as a result of damage
void GhostRacer::damage(int hit_damage) {
    setHealth(getHealth() - hit_damage);
    // if health < 0, die
    if (getHealth() <= 0) {
        die();
        getWorld()->playSound(SOUND_PLAYER_DIE);
    }
}

// heals Ghost Racer by specified amount of heal points, max health 100
void GhostRacer::heal(int heal_points) {
    int canBeHealedBy = GHOST_RACER_START_HEALTH - getHealth();
    if (canBeHealedBy >= heal_points) {
        setHealth(getHealth() + heal_points);
    }
    else {
        setHealth(getHealth() + canBeHealedBy);
    }
}

// redefines offscreen for Ghost Racer to be return whether it is off the road
bool GhostRacer::isOffscreen() {
    if (getX() <= LEFT_EDGE || getX() >= RIGHT_EDGE) {
        return true;
    }
    return false;
}

void GhostRacer::move() {
    // find amount to move left or right using direction
    double delta_x = cos(getDirection() * M_PI / 180) * 4;
    // move to new position
    moveTo(getX() + delta_x, getY());
}

IntelligentAgent::IntelligentAgent(StudentWorld* world, int health, double vert_speed, int imageID, double startX, double startY, int startDirection, double size)
    : DamageableActor(world, health, vert_speed, 0, imageID, startX, startY, startDirection, size) {
        m_movement_plan = 0;
}

IntelligentAgent::~IntelligentAgent() {
}

int IntelligentAgent::getMovementPlan() {
    return m_movement_plan;
}

void IntelligentAgent::setMovementPlan(int movement_plan) {
    m_movement_plan = movement_plan;
}

Pedestrian::Pedestrian(StudentWorld* world, int health, int imageID, double startX, double startY, int startDirection, double size)
: IntelligentAgent(world, health, -4, imageID, startX, startY, startDirection, size){
}

Pedestrian::~Pedestrian() {
}

// pick new movement plan for both human and zombie pedestrians
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
    // if hit Ghost Racer, tell Ghost Racer to die
    if (getWorld()->overlapping(this, getWorld()->getGR())) {
        getWorld()->getGR()->die();
        return;
    }
    move();
    // if not alive, return immediately
    if (!isAlive()) {
        return;
    }
    // decrement movement plan
    setMovementPlan(getMovementPlan() - 1);
    if (getMovementPlan() > 0) {
        return;
    }
    // pick new movement plan if it is <= 0
    pickNewMovementPlan();
}

bool HumanPed::sprayIfAppropriate() {
    // change speed to move in other direction
    setHorizSpeed(getHorizSpeed() * -1);
    // change direction accordingly
    if (getDirection() == 0) {
        setDirection(180);
    }
    else {
        setDirection(0);
    }
    getWorld()->playSound(SOUND_PED_HURT);
    return true;
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
    // if it is time to grunt, grunt
    if (m_till_next_grunt <= 0) {
        getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
        m_till_next_grunt = 20;
    }
    // if hit Ghost Racer, damage Ghost Racer by 5, die, and immediately return
    if (getWorld()->overlapping(this, getWorld()->getGR())) {
        getWorld()->getGR()->damage(5);
        setHealth(getHealth() - 2);
        die();
        getWorld()->playSound(SOUND_PED_DIE);
        getWorld()->increaseScore(ZOMBIE_PED_SCORE);
        return;
    }
    // if within 30 pixels left or right and is in front of Ghost Racer
    if (abs(getX() - getWorld()->getGR()->getX()) < 30 && getY() > getWorld()->getGR()->getY()) {
        // set direction to face down
        setDirection(270);
        // change speed to move towards Ghost Racer
        if (getX() < getWorld()->getGR()->getX()) {
            setHorizSpeed(1);
        }
        else if (getX() > getWorld()->getGR()->getX()) {
            setHorizSpeed(-1);
        }
        else {
            setHorizSpeed(0);
        }
        // decrement time until next grunt
        m_till_next_grunt--;
    }
    move();
    // if not alive, return immediately
    if (!isAlive()) {
        return;
    }
    // if movement plan >0, decrement movement plan, return
    if (getMovementPlan() > 0) {
        setMovementPlan(getMovementPlan() - 1);
        return;
    }
    // otherwise, pick new movement plan
    pickNewMovementPlan();
}

bool ZombiePed::sprayIfAppropriate() {
    // decrement health by 1
    setHealth(getHealth() - 1);
    // if health <= 0, die
    if (getHealth() <= 0) {
        die();
        getWorld()->playSound(SOUND_PED_DIE);
        // if it is not overlapping Ghost Racer at this time, there is a 1 in 5 chance to drop a healing goodie
        if(!(getWorld()->overlapping(this, getWorld()->getGR()))) {
            int chance = randInt(1, 5);
            if (chance == 1) {
                getWorld()->addNewHealingGoodie(getX(), getY());
            }
        }
        getWorld()->increaseScore(ZOMBIE_PED_SCORE);
    }
    else {
        getWorld()->playSound(SOUND_PED_HURT);
    }
    return true;
}

ZombieCab::ZombieCab(StudentWorld* world, double vert_speed, double startX, double startY)
: IntelligentAgent(world, 3, vert_speed, IID_ZOMBIE_CAB, startX, startY, 90, 4){
    hasDamagedGR = false;
}

ZombieCab::~ZombieCab() {
}

void ZombieCab::doSomething() {
    if (!isAlive()) {
        return;
    }
    // if hit Ghost Racer and hasn't damaged it yet
    if (getWorld()->overlapping(this, getWorld()->getGR()) && hasDamagedGR == false) {
        getWorld()->playSound(SOUND_VEHICLE_CRASH);
        // damage Ghost Racer by 20 hit points
        getWorld()->getGR()->damage(20);
        // change direction accordingly,
        if (getX() <= getWorld()->getGR()->getX()) {
            setHorizSpeed(-5);
            setDirection(120 + randInt(0, 19));
        }
        else {
            setHorizSpeed(5);
            setDirection(60 - randInt(0, 19));
        }
        // remember that it has damaged Ghost Racer
        hasDamagedGR = true;
    }
    move();
    // if speed > Ghost Racer speed
    if (getVertSpeed() > getWorld()->getGR()->getVertSpeed()) {
        // if the closest collision avoidance worthy actor in front of it is too close, slow down speed
        Actor* closestActor = getWorld()->closestCollisionAvoidanceWorthyActor(this, "front");
        if (closestActor != nullptr && closestActor->getY() - getY() < 96) {
            setVertSpeed(getVertSpeed() - 0.5);
            return;
        }
    }
    // if the closest collision avoidance worthy actor behind it is too close, speed up
    if (getVertSpeed() <= getWorld()->getGR()->getVertSpeed()) {
        Actor* closestActor = getWorld()->closestCollisionAvoidanceWorthyActor(this, "back");
        if (closestActor != nullptr && closestActor != getWorld()->getGR() && getY() - closestActor->getY() < 96) {
            setVertSpeed(getVertSpeed() + 0.5);
            return;
        }
    }
    // decrement movement plan
    setMovementPlan(getMovementPlan() - 1);
    // if movement plan > 0, return immediately
    if (getMovementPlan() > 0) {
        return;
    }
    // otherwise pick new movement plan
    pickNewMovementPlan();
}

bool ZombieCab::sprayIfAppropriate() {
    // decrement health by 1
    setHealth(getHealth() - 1);
    // if health <= 0, die
    if (getHealth() <= 0) {
        die();
        getWorld()->playSound(SOUND_VEHICLE_DIE);
        // there is a 1 in 5 chance of adding a new oil slick
        int chance = randInt(1, 5);
        if (chance == 1) {
            getWorld()->addNewOilSlick(getX(), getY());
        }
        getWorld()->increaseScore(ZOMBIE_CAB_SCORE);
        return true;
    }
    getWorld()->playSound(SOUND_VEHICLE_HURT);
    return true;
}

// pick new movement plan for zombie cabs
void ZombieCab::pickNewMovementPlan() {
    setMovementPlan(randInt(4, 32));
    setVertSpeed(getVertSpeed() + randInt(-2, 2));
}

Object::Object(StudentWorld* world, int imageID, double startX, double startY, int startDirection, double size, int depth)
    : Actor(world, -4, 0, false, imageID, startX, startY, startDirection, size, depth) {
}

Object::~Object() {
}

BorderLine::BorderLine(StudentWorld* world, int imageID, double startX, double startY)
    : Object(world, imageID, startX, startY, 0, 2) {
}

BorderLine::~BorderLine() {
}

void BorderLine::doSomething() {
    move();
}

HolyWaterProjectile::HolyWaterProjectile(StudentWorld* world, double startX, double startY, int startDirection)
: Object(world, IID_HOLY_WATER_PROJECTILE, startX, startY, startDirection, 1, 1) {
    m_pixels_moved = 0;
}

HolyWaterProjectile::~HolyWaterProjectile() {
}

void HolyWaterProjectile::doSomething() {
    if (!isAlive()) {
        return;
    }
    // call function to spray an overlapping holy water affected actor
    if (getWorld()->sprayOverlappingHolyWaterAffectedActor(this)) {
        // if it completed successfully and returned true, die
        die();
        return;
    }
    move();
}

void HolyWaterProjectile::move() {
    moveForward(SPRITE_HEIGHT);
    m_pixels_moved += SPRITE_HEIGHT;
    // if it has moved 160 pixels or is offscreen, die
    if (m_pixels_moved >= 160 || isOffscreen()) {
        die();
        return;
    }
}

GhostRacerActivatedObject::GhostRacerActivatedObject(StudentWorld* world, int imageID, double startX, double startY, int startDirection, double size, bool mustRotate)
: Object(world, imageID, startX, startY, startDirection, size) {
    m_mustRotate = mustRotate;
}

GhostRacerActivatedObject::~GhostRacerActivatedObject() {
}

void GhostRacerActivatedObject::doSomething() {
    move();
    // if not alive, return immediately
    if (!isAlive()) {
        return;
    }
    // if overlapping Ghost Racer, activate
    if (getWorld()->overlapping(this, getWorld()->getGR())) {
        activate();
    }
    // if object must rotate every tick, change direction
    if (m_mustRotate == true) {
        setDirection(getDirection() - 10);
    }
}

OilSlick::OilSlick(StudentWorld* world, double startX, double startY, double size)
: GhostRacerActivatedObject(world, IID_OIL_SLICK, startX, startY, 0, size){
}

OilSlick::~OilSlick() {
}

void OilSlick::activate() {
    getWorld()->playSound(SOUND_OIL_SLICK);
    // find random direction to change direction of Ghost Racer to
    int rand_spin = randInt(5, 20);
    int rand_sign = randInt(0, 1);
    if (rand_sign == 0) {
        rand_spin *= -1;
    }
    // if cannot move in that direction, find another random direction
    while ((getWorld()->getGR()->getDirection() + rand_spin) < 60 || (getWorld()->getGR()->getDirection() + rand_spin) > 120) {
        rand_spin = randInt(5, 20);
        rand_sign = randInt(0, 1);
        if (rand_sign == 0) {
            rand_spin *= -1;
        }
    }
    getWorld()->getGR()->setDirection(getWorld()->getGR()->getDirection() + rand_spin);
}

SoulGoodie::SoulGoodie(StudentWorld* world, double startX, double startY)
: GhostRacerActivatedObject(world, IID_SOUL_GOODIE, startX, startY, 0, 4, true) {
}

SoulGoodie::~SoulGoodie() {
}

void SoulGoodie::activate() {
    // increment count of souls saved in StudentWorld
    getWorld()->saveSoul();
    die();
    getWorld()->playSound(SOUND_GOT_SOUL);
    getWorld()->increaseScore(SOUL_GOODIE_SCORE);
}

HolyWaterAffectedObject::HolyWaterAffectedObject(StudentWorld* world, int imageID, double startX, double startY, int startDirection, double size)
: GhostRacerActivatedObject(world, imageID, startX, startY, startDirection, size) {
}

HolyWaterAffectedObject::~HolyWaterAffectedObject() {
}

// both holy water affected objects simply die and disappear when hit
bool HolyWaterAffectedObject::sprayIfAppropriate() {
    die();
    return true;
}

HealingGoodie::HealingGoodie(StudentWorld* world, double startX, double startY)
: HolyWaterAffectedObject(world, IID_HEAL_GOODIE, startX, startY, 0, 1){
}

HealingGoodie::~HealingGoodie() {
}

void HealingGoodie::activate() {
    // heal Ghost Racer with 10 points
    getWorld()->getGR()->heal(10);
    die();
    getWorld()->playSound(SOUND_GOT_GOODIE);
    getWorld()->increaseScore(HEALING_GOODIE_SCORE);
}

HolyWaterGoodie::HolyWaterGoodie(StudentWorld* world, double startX, double startY)
: HolyWaterAffectedObject(world, IID_HOLY_WATER_GOODIE, startX, startY, 90, 2) {
}

HolyWaterGoodie::~HolyWaterGoodie() {
}

void HolyWaterGoodie::activate() {
    // add holy water to Ghost Racer tank
    getWorld()->getGR()->addHolyWater(10);
    die();
    getWorld()->playSound(SOUND_GOT_GOODIE);
    getWorld()->increaseScore(HOLY_WATER_GOODIE_SCORE);
}
