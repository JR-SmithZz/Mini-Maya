#ifndef TERRAINS_H
#define TERRAINS_H
#pragma once

#include <QList>
#include <la.h>
#include <drawable.h>
#include <QHash>
#include <chunks.h>
#include <utils.h>
#include <unordered_map>
#include "scene\turtle.h"
#include <stack>

// C++ 11 allows us to define the size of an enum. This lets us use only one byte
// of memory to store our different block types. By default, the size of a C++ enum
// is that of an int (so, usually four bytes). This *does* limit us to only 256 different
// block types, but in the scope of this project we'll never get anywhere near that many.

class Chunk;

struct pair_hash
{
    template<class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& p) const
    {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2;
    }
};

class Terrain
{
public:
    Terrain(OpenGLContext* context);
    OpenGLContext* context;
    BlockType m_blocks[64][256][64];                    // A 3D list of the blocks in the world.
    // You'll need to replace this with a far more
    // efficient system of storing terrain.
    void CreateTestScene();

    glm::ivec3 dimensions;

    // Given a world-space coordinate (which may have negative
    BlockType getBlockAt(int x, int y, int z);

    // values) return the block stored at that point in space.
    void setBlockAt(int x, int y, int z, BlockType t); // Given a world-space coordinate (which may have negative
    // values) set the block at that point in space to the
    // given type.
    std::unordered_map<std::pair<int,int>, Chunk*, pair_hash> chunkMap;

    void createChunk(glm::ivec2 position);
    void createChunk1(glm::ivec2 position);
    void createChunkTrans(glm::ivec2 position);

    float interpNoise(glm::vec2 p);
    std::pair<int, int> getkey(glm::vec3 pos);
    void destroychunk(glm::vec3 pos);
    void updatechunk(glm::vec3 pos);
    void createTerrain(glm::ivec2 position);
    void createTerrain1(glm::ivec2 position);
    void createTerrainTrans(glm::ivec2 position);
    bool hasChunk(glm::vec3 pos);

    LSystem* delta_river;
    LSystem* linear_river;
    LSystem* tunnel;
    LSystem* building;

    void draw_linearriver(std::vector<glm::vec2> positions);
    void draw_deltariver(std::vector<glm::vec2> positions);
    void draw_linearriverbank(std::vector<glm::vec2> positions);
    void draw_deltariverbank(std::vector<glm::vec2> positions);
    void draw_tunnel(std::vector<glm::vec2> positions);
    void draw_caves(std::vector<glm::vec2> positions);
    void draw_lava(std::vector<glm::vec2> positions);
    void draw_building(std::vector<glm::vec2> positions);

    std::vector<glm::vec2> linear_positions();
    std::vector<glm::vec2> delta_positions();
    std::vector<glm::vec2> tunnel_positions();
    std::vector<glm::vec2> building_positions();

    void remove_above(int x, int y, int z);
    void remove_below(int x, int z);
    void remove_disc(int x, int y, int z, int radius);
    void make_cave(int x, int bottom, int z, int radius);
    void make_lavapool(int x, int bottom, int z, int radius);
    void draw_rhombus(int x, int y, int z, int radius, BlockType b);
    void draw_diamond(int x, int y, int z, int radius, BlockType b);
    void draw_disc(int x, int y, int z, int radius, BlockType b);
    void make_tree(int x, int z);
};



#endif // TERRAINS_H
