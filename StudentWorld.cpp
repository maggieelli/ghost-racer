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
    m_last_white_border_line = nullptr;
    m_nSouls = getLevel() * 2 + 5;
    m_soulsSaved = 0;
    m_bonus = 5000;
}

StudentWorld::~StudentWorld() {
    cleanUp();
}

int StudentWorld::init()
{
    m_nSouls = getLevel() * 2 + 5;
    m_soulsSaved = 0;
    m_bonus = 5000;
    m_gr = new GhostRacer(this);
    actorList.push_back(m_gr);
    insertBorderLines();
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    list<Actor*>::iterator it;
    it = actorList.begin();
    while (it != actorList.end() && (*it)->isAlive()) {
        (*it)->doSomething();
        if (!m_gr->isAlive()) {
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
        if (m_nSouls - m_soulsSaved <= 0) {
            increaseScore(m_bonus);
            return GWSTATUS_FINISHED_LEVEL;
        }
        it++;
    }
    // TODO delete dead actors
    it = actorList.begin();
    while (it != actorList.end()) {
        if (!(*it)->isAlive()) {
            delete *it;
            it = actorList.erase(it);
            it--;
        }
        it++;
    }
    // TODO add new actors
    addNewBorderLines();
    addNewHumanPed();
    addNewZombiePed();
    // TODO bonus and update status text
    if (m_bonus > 0) {
        m_bonus--;
    }
    updateStatusText();
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
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

void StudentWorld::insertBorderLines() {
    for (int i = 0; i < VIEW_HEIGHT/SPRITE_HEIGHT; i++) {
        actorList.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, LEFT_EDGE, i * SPRITE_HEIGHT));
        actorList.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, i * SPRITE_HEIGHT));
    }
    for (int i = 0; i < (VIEW_HEIGHT / (4 * SPRITE_HEIGHT)); i++) {
        actorList.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, LEFT_EDGE + ROAD_WIDTH / 3, i * (4 * SPRITE_HEIGHT)));
        m_last_white_border_line = new BorderLine(this, IID_WHITE_BORDER_LINE, RIGHT_EDGE - ROAD_WIDTH / 3, i * (4 * SPRITE_HEIGHT));
        actorList.push_back(m_last_white_border_line);
    }
}

void StudentWorld::addNewBorderLines() {
    int new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    int delta_y = new_border_y - m_last_white_border_line->getY();
    if (delta_y >= SPRITE_HEIGHT) {
        actorList.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, LEFT_EDGE, new_border_y));
        actorList.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, new_border_y));
    }
    if (delta_y >= (4 * SPRITE_HEIGHT)) {
        actorList.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, LEFT_EDGE + ROAD_WIDTH / 3, new_border_y));
        m_last_white_border_line = new BorderLine(this, IID_WHITE_BORDER_LINE, RIGHT_EDGE - ROAD_WIDTH / 3, new_border_y);
        actorList.push_back(m_last_white_border_line);
    }
}

void StudentWorld::addNewHumanPed() {
    int chance = max(200 - getLevel() * 10, 30);
    int rand = randInt(0, chance - 1);
    if (rand == 0) {
        int rand_x = randInt(0, VIEW_WIDTH);
        actorList.push_back(new HumanPed(this, rand_x, VIEW_HEIGHT));
    }
}

void StudentWorld::addNewZombiePed() {
    int chance = max(100 - getLevel() * 10, 20);
    int rand = randInt(0, chance - 1);
    if (rand == 0) {
        int rand_x = randInt(0, VIEW_WIDTH);
        actorList.push_back(new ZombiePed(this, rand_x, VIEW_HEIGHT));
    }
}

void StudentWorld::addNewZombieCabs() {
    int cur_lane = randInt(1, 3);
    for (int i = 0; i < 3; i++) {
        
        if (cur_lane < 3) {
            cur_lane++;
        }
        else {
            cur_lane = 1;
        }
    }
}

bool StudentWorld::overlappingGR(Actor* ac) {
    double delta_x = abs(m_gr->getX() - ac->getX());
    double delta_y = abs(m_gr->getY() - ac->getY());
    double radius_sum = m_gr->getRadius() + ac->getRadius();
    if (delta_x < (radius_sum * 0.25) && delta_y < (radius_sum * 0.6)) {
        return true;
    }
    return false;
}

Actor* StudentWorld::closestCollisionAvoidanceWorthyActor(ZombieCab* zc, string front_or_back) {
    list<Actor*>::iterator it;
    it = actorList.begin();
    Actor* closestFront = nullptr;
    Actor* closestBack = nullptr;
    while (it != actorList.end()) {
        if ((*it) != zc && (*it)->isCollisionAvoidanceWorthy()) {
            if (abs((*it)->getX() - zc->getX()) < ROAD_WIDTH/6) {
                if (front_or_back == "front") {
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
                else if (front_or_back == "back") {
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
    }
    if (front_or_back == "front") {
        return closestFront;
    }
    return closestBack;
}

Actor* closestCollisionAvoidanceWorthyActor(string top_or_bottom, double left_boundary, double right_boundary) {
    // TODO
}

void StudentWorld::saveSoul() {
    m_soulsSaved++;
}

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

