#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <sstream>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    m_gr = nullptr;
    m_last_white_border_y = 0;
    m_nSouls = getLevel() * 2 + 5;
    m_soulsSaved = 0;
    m_bonus = START_BONUS;
}

// calls cleanUp method
StudentWorld::~StudentWorld() {
    cleanUp();
}

// initiates Ghost Racer and border lines
int StudentWorld::init()
{
    m_nSouls = getLevel() * 2 + 5;
    m_soulsSaved = 0;
    m_bonus = START_BONUS;
    // adds Ghost Racer
    m_gr = new GhostRacer(this);
    actorList.push_back(m_gr);
    // adds border lines
    insertBorderLines();
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    list<Actor*>::iterator it;
    it = actorList.begin();
    // for every alive actor
    while (it != actorList.end() && (*it)->isAlive()) {
        // call the doSomething function
        (*it)->doSomething();
        // if Ghost Racer is dead now, decrement lives
        if (!m_gr->isAlive()) {
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
        // if correct number of souls to save has been fulfilled
        if (m_nSouls - m_soulsSaved <= 0) {
            // increment score by bonus
            increaseScore(m_bonus);
            return GWSTATUS_FINISHED_LEVEL;
        }
        it++;
    }
    it = actorList.begin();
    // delete all dead actors
    while (it != actorList.end()) {
        if (!(*it)->isAlive()) {
            delete *it;
            it = actorList.erase(it);
            it--;
        }
        it++;
    }
    // adjust last white border's y-position
    m_last_white_border_y += (-4 - m_gr->getVertSpeed());
    // add actors necessary
    addNewBorderLines();
    addNewHumanPed();
    addNewZombiePed();
    addNewZombieCabs();
    addNewOilSlick();
    addNewHolyWaterGoodie();
    addNewLostSoulGoodie();
    // decrement bonus each tick
    if (m_bonus > 0) {
        m_bonus--;
    }
    // update status text
    updateStatusText();
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    // delete every actor in list
    list<Actor*>::iterator it;
    it = actorList.begin();
    while (!actorList.empty()) {
        delete *it;
        it = actorList.erase(it);
    }
}

GhostRacer* StudentWorld::getGR() {
    return m_gr;
}

// adds new oil slick at specified location where zombie cab is destroyed
void StudentWorld::addNewOilSlick(double posX, double posY) {
    actorList.push_back(new OilSlick(this, posX, posY, randInt(2, 5)));
}

// adds new healing goodie at specified location where zombie pedestrian is destroyed
void StudentWorld::addNewHealingGoodie(double posX, double posY) {
    actorList.push_back(new HealingGoodie(this, posX, posY));
}

// adds new holy water projectile at specified location with specified direction
void StudentWorld::addNewHolyWaterProjectile() {
    double posX;
    double posY;
    m_gr->getPositionInThisDirection(m_gr->getDirection(), SPRITE_HEIGHT, posX, posY);
    actorList.push_back(new HolyWaterProjectile(this, posX, posY, m_gr->getDirection()));
}

// returns true if the two actors are overlapping, false if not
bool StudentWorld::overlapping(Actor* ac1, Actor* ac2) {
    double delta_x = abs(ac1->getX() - ac2->getX());
    double delta_y = abs(ac1->getY() - ac2->getY());
    double radius_sum = ac1->getRadius() + ac2->getRadius();
    if (delta_x < (radius_sum * 0.25) && delta_y < (radius_sum * 0.6)) {
        return true;
    }
    return false;
}

// will spray a holy water affected actor if it is overlapping the holy water projectile and return true, false if there is no actor to be sprayed
bool StudentWorld::sprayOverlappingHolyWaterAffectedActor(HolyWaterProjectile* hw) {
    list<Actor*>::iterator it;
    it = actorList.begin();
    // for every actor
    while (it != actorList.end()) {
        // if it is alive and is overlapping the holy water projectile
        if ((*it)->isAlive() && overlapping((*it), hw)) {
            // return true if it is sprayed
            if ((*it)->sprayIfAppropriate()) {
                return true;
            }
        }
        it++;
    }
    // otherwise return false if nothing is sprayed
    return false;
}

// find the closest collision avoidance worthy actor in front or or behind the zombie cab
Actor* StudentWorld::closestCollisionAvoidanceWorthyActor(ZombieCab* zc, string front_or_back) {
    list<Actor*>::iterator it;
    it = actorList.begin();
    Actor* closestFront = nullptr;
    Actor* closestBack = nullptr;
    // for every actor
    while (it != actorList.end()) {
        // if it is not the zombie cab we are looking at and it is collision avoidance worthy
        if ((*it) != zc && (*it)->isCollisionAvoidanceWorthy()) {
            // if it is in the same lane as the zombie cab
            if (abs((*it)->getX() - zc->getX()) < ROAD_WIDTH/6) {
                // if we are looking at the front
                if (front_or_back == "front") {
                    // if the actor is in front of the zombie cab and is the closest one, set closestFront to that actor
                    if ((*it)->getY() - zc->getY() > 0) {
                        if (closestFront == nullptr) {
                            closestFront = (*it);
                        }
                        else {
                            if ((*it)->getY() < closestFront->getY()) {
                                closestFront = (*it);
                            }
                        }
                    }
                }
                // else if we are looking at the back
                else if (front_or_back == "back") {
                    // if the actor is behind the zombie cab and is the closest one, set closestBack to that actor
                    if ((*it)->getY() - zc->getY() < 0) {
                        if (closestBack == nullptr) {
                            closestBack = (*it);
                        }
                        else {
                            if ((*it)->getY() > closestBack->getY()) {
                                closestBack = (*it);
                            }
                        }
                    }
                }
            }
        }
        it++;
    }
    // return the closest collision avoidance worthy actor in the front or back depending on which we are looking for, return nullptr if there isn't one
    if (front_or_back == "front") {
        return closestFront;
    }
    return closestBack;
}

// increment number of souls saved
void StudentWorld::saveSoul() {
    m_soulsSaved++;
}

// insert border lines called fron init(), fill screen with border lines
void StudentWorld::insertBorderLines() {
    // add yellow border lines
    for (int i = 0; i < VIEW_HEIGHT/SPRITE_HEIGHT; i++) {
        actorList.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, LEFT_EDGE, i * SPRITE_HEIGHT));
        actorList.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, i * SPRITE_HEIGHT));
    }
    double last_y = 0;
    for (int i = 0; i < (VIEW_HEIGHT / (4 * SPRITE_HEIGHT)); i++) {
        actorList.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, LEFT_WHITE_BORDER_LINE, i * (4 * SPRITE_HEIGHT)));
        actorList.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, RIGHT_WHITE_BORDER_LINE, i * (4 * SPRITE_HEIGHT)));
        // keep track of y position of last white border line added
        last_y = i * (4 * SPRITE_HEIGHT);
    }
    // keep y position of last white border line added in member variable
    m_last_white_border_y = last_y;
}

// checks if new border lines need to be added, then adds if necessary
void StudentWorld::addNewBorderLines() {
    int new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    int delta_y = new_border_y - m_last_white_border_y;
    // if yellow border lines should be added, add them
    if (delta_y >= SPRITE_HEIGHT) {
        actorList.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, LEFT_EDGE, new_border_y));
        actorList.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, new_border_y));
    }
    // if white border lines should be added, add them
    if (delta_y >= (4 * SPRITE_HEIGHT)) {
        actorList.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, LEFT_WHITE_BORDER_LINE, new_border_y));
        actorList.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, RIGHT_WHITE_BORDER_LINE, new_border_y));
        // keep y position of last white border line added in member variable
        m_last_white_border_y = new_border_y;
    }
}

// randomly adds new human pedestrian
void StudentWorld::addNewHumanPed() {
    int chance = max(200 - getLevel() * 10, 30);
    int rand = randInt(0, chance - 1);
    if (rand == 0) {
        int rand_x = randInt(0, VIEW_WIDTH);
        actorList.push_back(new HumanPed(this, rand_x, VIEW_HEIGHT));
    }
}

// randomly adds new zombie pedestrian
void StudentWorld::addNewZombiePed() {
    int chance = max(100 - getLevel() * 10, 20);
    int rand = randInt(0, chance - 1);
    if (rand == 0) {
        int rand_x = randInt(0, VIEW_WIDTH);
        actorList.push_back(new ZombiePed(this, rand_x, VIEW_HEIGHT));
    }
}

// randomly adds new zombie cabs
void StudentWorld::addNewZombieCabs() {
    int chance = max(100 - getLevel() * 10, 20);
    int rand = randInt(0, chance - 1);
    if (rand == 0) {
        // pick random lane (0 is left, 1 is middle, 2 is right)
        int cur_lane = randInt(0, 2);
        // for each lane
        for (int i = 0; i < 3; i++) {
            // find the closest collision avoidance worthy actor to the bottom of the screen
            Actor* ac_bottom = closestCollisionAvoidanceWorthyActor("bottom", LEFT_EDGE + (ROAD_WIDTH/3 * cur_lane), LEFT_EDGE + (ROAD_WIDTH/3 * (1 + cur_lane)));
            // if there is none or if it is towards the top of the screen, add a zombie cab at specified location at bottom of the lane, break
            if (ac_bottom == nullptr || ac_bottom->getY() > (VIEW_HEIGHT / 3)) {
                actorList.push_back(new ZombieCab(this, m_gr->getVertSpeed() + randInt(2, 4), FIRST_LANE_CENTER + (ROAD_WIDTH/3 * cur_lane), SPRITE_HEIGHT/2));
                break;
            }
            // if there is a collision avoidance worthy actor at the bottom of the lane, find the closest collision avoidance worthy actor to the top of the screen
            Actor* ac_top = closestCollisionAvoidanceWorthyActor("top", LEFT_EDGE + (ROAD_WIDTH/3 * cur_lane), LEFT_EDGE + (ROAD_WIDTH/3 * (1 + cur_lane)));
            // if there is none or if it is towards the bottom of the screen, add a zombie cab at specified location at top of the lane, break
            if (ac_top == nullptr || ac_top->getY() < (VIEW_HEIGHT * 2 / 3)) {
                actorList.push_back(new ZombieCab(this, m_gr->getVertSpeed() - randInt(2, 4), FIRST_LANE_CENTER + (ROAD_WIDTH/3 * cur_lane), VIEW_HEIGHT - SPRITE_HEIGHT/2));
                break;
            }
            // otherwise try another lane
            if (cur_lane < 2) {
                cur_lane++;
            }
            else {
                cur_lane = 0;
            }
        }
    }
}

// randomly adds new oil slick
void StudentWorld::addNewOilSlick() {
    int chance = max(150 - getLevel() * 10, 40);
    int rand = randInt(0, chance - 1);
    if (rand == 0) {
        int rand_x = randInt(LEFT_EDGE, RIGHT_EDGE);
        actorList.push_back(new OilSlick(this, rand_x, VIEW_HEIGHT, randInt(2, 5)));
    }
}

// randomly adds new holy water goodie
void StudentWorld::addNewHolyWaterGoodie() {
    int chance = 100 + 10 * getLevel();
    int rand = randInt(0, chance - 1);
    if (rand == 0) {
        int rand_x = randInt(LEFT_EDGE, RIGHT_EDGE);
        actorList.push_back(new HolyWaterGoodie(this, rand_x, VIEW_HEIGHT));
    }
}

// randomly adds new lost soul goodie
void StudentWorld::addNewLostSoulGoodie() {
    int chance = 100;
    int rand = randInt(0, chance - 1);
    if (rand == 0) {
        int rand_x = randInt(LEFT_EDGE, RIGHT_EDGE);
        actorList.push_back(new SoulGoodie(this, rand_x, VIEW_HEIGHT));
    }
}

Actor* StudentWorld::closestCollisionAvoidanceWorthyActor(string top_or_bottom, double left_boundary, double right_boundary) {
    list<Actor*>::iterator it;
    it = actorList.begin();
    Actor* closestTop = nullptr;
    Actor* closestBottom = nullptr;
    // for every actor
    while (it != actorList.end()) {
        // if it is collision avoidance worthy
        if ((*it)->isCollisionAvoidanceWorthy()) {
            // if it is within the x bounds that are specified (lane bounds)
            if ((*it)->getX() > left_boundary && (*it)->getX() < right_boundary) {
                // if we are looking at the top of the screen
                if (top_or_bottom == "top") {
                    // if the actor is the closest to the top, set closestTop to that actor
                    if (closestTop == nullptr) {
                        closestTop = (*it);
                    }
                    else {
                        if ((*it)->getY() > closestTop->getY()) {
                            closestTop = (*it);
                        }
                    }
                }
                // else if we are looking at the bottom of the screen
                else if (top_or_bottom == "bottom") {
                    // if the actor is the closest to the bottom, set closestTop to that actor
                    if (closestBottom == nullptr) {
                        closestBottom = (*it);
                    }
                    else {
                        if ((*it)->getY() < closestBottom->getY()) {
                            closestBottom = (*it);
                        }
                    }
                }
            }
        }
        it++;
    }
    // return the closest collision avoidance worthy actor to the top or bottom depending on which we are looking for, return nullptr if there isn't one
    if (top_or_bottom == "top") {
        return closestTop;
    }
    return closestBottom;
}

// update status text
void StudentWorld::updateStatusText() {
    ostringstream status;
    status << "Score: " << getScore() << "  ";
    status << "Lvl: " << getLevel() << "  ";
    status << "Souls2Save: " << m_nSouls - m_soulsSaved << "  ";
    status << "Lives: " << getLives() << "  ";
    status << "Health: " << m_gr->getHealth() << "  ";
    status << "Sprays: " << m_gr->getHolyWater() << "  ";
    status << "Bonus: " << m_bonus;
    setGameStatText(status.str());
}
