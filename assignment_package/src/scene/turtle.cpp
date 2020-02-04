#include "turtle.h"

Turtle::Turtle(glm::vec2 position, glm::vec2 orientation, int d) :
    pos(position), orient(orientation), depth(d){}

Turtle::Turtle() :
    pos(glm::vec2(0,0)), orient(glm::vec2(0,0)), depth(1){}

LSystem::LSystem() : expand_map(), draw_map()
{
    expand_map.insert('X', QString("FFFFFFF[AFFFFFFFFFFFFFFFFF[BFFFFFFFFFFFFFFFFFFFFFF]FFFFFFFFFFF]X"));
    draw_map.insert('F', move_astep);
    draw_map.insert('[', save_state);
    draw_map.insert(']', pop_state);
    draw_map.insert('A', delta_rotate1);
    draw_map.insert('B', delta_rotate2);
}

LSystem::LSystem(QString s, glm::vec2 p, glm::vec2 o, int de) : expand_map(), draw_map(), axiom(s),
  turtle(Turtle(p, o, de)), state()
{
//    printf("11111111111111111");
    expand_map.insert('X', QString("FFFFFFF[AFFFFFFFFFFFFFFFFFFFFFFFFFFF[BFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF]FFFFFFFFFFF]X"));
    expand_map.insert('Y', QString("[PF-F+FY"));
    expand_map.insert('Z', QString("[LFFRFFPFFZ"));
    expand_map.insert('W', QString("FFFF[IFFFF][JFFFF]W"));
    draw_map.insert('F', &move_astep);
    draw_map.insert('[', &save_state);
    draw_map.insert(']', &pop_state);
    draw_map.insert('A', &delta_rotate1);
    draw_map.insert('B', &delta_rotate2);
    draw_map.insert('P', &linear_rotate);
    draw_map.insert('+', &linear_rotate1);
    draw_map.insert('-', &linear_rotate2);
    draw_map.insert('L', &tunnel_rotate1);
    draw_map.insert('R', &tunnel_rotate2);
    draw_map.insert('I', &building_rotate1);
    draw_map.insert('J', &building_rotate2);
    draw_map.insert('W', &test);
    draw_map.insert('X', &test);
    draw_map.insert('Y', &test);
    draw_map.insert('Z', &test);
}

QString LSystem::expand_string(const QString &s) const
{
    QString r("");
    for (QChar c : s)
    {
        if (expand_map.contains(c))
            r.push_back(expand_map.value(c));
        else
            r.push_back(c);
    }
    return r;
}

void LSystem::move_astep()
{
    positions.push_back(turtle.pos);
    turtle.pos += turtle.orient;
}

void LSystem::save_state()
{
    turtle.depth ++;
    state.push(turtle);
}

void LSystem::pop_state()
{
    turtle = state.top();
    state.pop();
}

void LSystem::delta_rotate1()
{
    float range = 75.0;
    float angle = ((float)rand()) / RAND_MAX * range;
    float cos = glm::cos(glm::radians(angle));
    float sin = glm::sin(glm::radians(angle));
    float x = cos * turtle.orient.x - sin * turtle.orient.y;
    float y = sin * turtle.orient.x + cos * turtle.orient.y;
    turtle.orient.x = x;
    turtle.orient.y = y;
}
void LSystem::delta_rotate2()
{
    float range = -75.0;
    float angle = ((float)rand()) / RAND_MAX * range;
    float cos = glm::cos(glm::radians(angle));
    float sin = glm::sin(glm::radians(angle));
    float x = cos * turtle.orient.x - sin * turtle.orient.y;
    float y = sin * turtle.orient.x + cos * turtle.orient.y;
    turtle.orient.x = x;
    turtle.orient.y = y;
}

void LSystem::linear_rotate()
{
    float range = 40.0;
    float angle = ((float)rand()) / RAND_MAX * range - range / 2;
    float cos = glm::cos(glm::radians(angle));
    float sin = glm::sin(glm::radians(angle));
    float x = cos * turtle.orient.x - sin * turtle.orient.y;
    float y = sin * turtle.orient.x + cos * turtle.orient.y;
    turtle.orient.x = x;
    turtle.orient.y = y;
}

void LSystem::linear_rotate1()
{
    float range = 30.0;
    float angle = ((float)rand()) / RAND_MAX * range;
    float cos = glm::cos(glm::radians(angle));
    float sin = glm::sin(glm::radians(angle));
    float x = cos * turtle.orient.x - sin * turtle.orient.y;
    float y = sin * turtle.orient.x + cos * turtle.orient.y;
    turtle.orient.x = x;
    turtle.orient.y = y;
}

void LSystem::tunnel_rotate1()
{
    float range = 20.0;
    float angle = ((float)rand()) / RAND_MAX * range;
    float cos = glm::cos(glm::radians(angle));
    float sin = glm::sin(glm::radians(angle));
    float x = cos * turtle.orient.x - sin * turtle.orient.y;
    float y = sin * turtle.orient.x + cos * turtle.orient.y;
    turtle.orient.x = x;
    turtle.orient.y = y;
}

void LSystem::tunnel_rotate2()
{
    float range = -20.0;
    float angle = ((float)rand()) / RAND_MAX * range;
    float cos = glm::cos(glm::radians(angle));
    float sin = glm::sin(glm::radians(angle));
    float x = cos * turtle.orient.x - sin * turtle.orient.y;
    float y = sin * turtle.orient.x + cos * turtle.orient.y;
    turtle.orient.x = x;
    turtle.orient.y = y;
}

void LSystem::building_rotate1()
{
    float range = 60.0;
    float angle = ((float)rand()) / RAND_MAX * range;
    float cos = glm::cos(glm::radians(angle));
    float sin = glm::sin(glm::radians(angle));
    float x = cos * turtle.orient.x - sin * turtle.orient.y;
    float y = sin * turtle.orient.x + cos * turtle.orient.y;
    turtle.orient.x = x;
    turtle.orient.y = y;
}

void LSystem::building_rotate2()
{
    float range = -60.0;
    float angle = ((float)rand()) / RAND_MAX * range;
    float cos = glm::cos(glm::radians(angle));
    float sin = glm::sin(glm::radians(angle));
    float x = cos * turtle.orient.x - sin * turtle.orient.y;
    float y = sin * turtle.orient.x + cos * turtle.orient.y;
    turtle.orient.x = x;
    turtle.orient.y = y;
}

void LSystem::linear_rotate2()
{
    float range = -30.0;
    float angle = ((float)rand()) / RAND_MAX * range;
    float cos = glm::cos(glm::radians(angle));
    float sin = glm::sin(glm::radians(angle));
    float x = cos * turtle.orient.x - sin * turtle.orient.y;
    float y = sin * turtle.orient.x + cos * turtle.orient.y;
    turtle.orient.x = x;
    turtle.orient.y = y;
}

void LSystem::test()
{
    ;
}
