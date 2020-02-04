#ifndef TURTLE_H
#define TURTLE_H
#pragma once

#include "la.h"
#include <QList>

#include <unordered_map>
#include <stack>

using namespace std;
using namespace glm;

#include <iostream>


class Turtle
{
public:
    vec2 pos;
    vec2 orient;
    int depth;

    Turtle();
    Turtle(glm::vec2 position, glm::vec2 orientation, int d);
};

class LSystem
{
public:
    typedef void (LSystem::*Rule)(void);
    QString axiom;
    QHash<QChar, QString> expand_map;
    QHash<QChar, Rule> draw_map;
    Turtle turtle;
    std::stack<Turtle> state;
    std::vector<glm::vec2> positions;

    LSystem();
    LSystem(QString s, glm::vec2 p, glm::vec2 o, int de);
    QString expand_string(const QString& s) const;

    void move_astep();
    void save_state();
    void pop_state();
    void delta_rotate1();
    void delta_rotate2();
    void linear_rotate();
    void linear_rotate1();
    void linear_rotate2();
    void tunnel_rotate1();
    void tunnel_rotate2();
    void building_rotate1();
    void building_rotate2();
    void test();
};

#endif // TURTLE_H
