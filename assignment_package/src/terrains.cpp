#include <terrains.h>
#include <scene/cube.h>
#include <iostream>


using namespace glm;
using namespace std;

Terrain::Terrain(OpenGLContext* context) : dimensions(64, 256, 64), context(context)
{
    delta_river = new LSystem("X", glm::vec2(32, 0), glm::vec2(0, 1), 1);
    linear_river = new LSystem("Y", glm::vec2(96, 0), glm::vec2(0, 1), 1);
    tunnel = new LSystem("Z", glm::vec2(240, 0), glm::vec2(0, 1), 1);
    building = new LSystem("W", glm::vec2(210, 128), glm::vec2(0, 1), 1);
}

BlockType Terrain::getBlockAt(int x, int y, int z)
{
    // TODO: Make this work with your new block storage!
    int xChunkPos = glm::floor((float)x/16) * 16;
    int zChunkPos = glm::floor((float)z/16) * 16;
    int xBlockPos = x - xChunkPos;
    int zBlockPos = z - zChunkPos;


    if(chunkMap.count(std::make_pair(xChunkPos,zChunkPos))!=0)
    {
        if(chunkMap[std::make_pair(xChunkPos,zChunkPos)] == nullptr)
        {

            return EMPTY;
        }
        else
        {
            Chunk* chunk = chunkMap[std::make_pair(xChunkPos,zChunkPos)];
            return chunk->getBlockType(xBlockPos, y, zBlockPos);
        }
    }
    else
    {
        return EMPTY;
    }
}


void Terrain::setBlockAt(int x, int y, int z, BlockType t)
{
    // TODO: Make this work with your new block storage!
    // m_blocks[x][y][z] = t;
    int xChunkPos = glm::floor((float)x/16) * 16;
    int zChunkPos = glm::floor((float)z/16) * 16;
    int xBlockPos = x - xChunkPos;
    int zBlockPos = z - zChunkPos;

    Chunk* chunk = chunkMap[std::make_pair(xChunkPos,zChunkPos)];

    if(chunk != nullptr)
    {
        chunk->setBlockType(xBlockPos,y,zBlockPos,t);
    }

//    else
//    {
//        Chunk* newChunk = new Chunk(context,glm::ivec2(xBlockPos,zBlockPos));
//        newChunk->te = this;
//        newChunk->setBlockType(xBlockPos,y,zBlockPos,t);

//        chunkMap[std::make_pair(xChunkPos,zChunkPos)] = newChunk;
//    }

}

float rand(vec2 n)
{
    return (fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453));
}

float interpNoise2D(float x, float y)
{
    float intX = floor(x);
    float fractX = fract(x);
    float intY = floor(y);
    float fractY = fract(y);

    float v1 = rand(vec2(intX, intY));
    float v2 = rand(vec2(intX + 1, intY));
    float v3 = rand(vec2(intX, intY + 1));
    float v4 = rand(vec2(intX + 1, intY + 1));

    float i1 = mix(v1, v2, fractX);
    float i2 = mix(v3, v4, fractX);
    return mix(i1, i2, fractY);
}

float fbm(float x, float y)
{
    x /= 64.f; y /= 64.f;
    float total = 0;
    float persistence = 0.45f;
    int octaves = 8;

    for(int i = 1; i<= octaves; i++) {
        float freq = pow(2.f, i);
        float amp = pow(persistence, i);

        total += interpNoise2D(x * freq, y * freq) * amp;
    }

    return total;
}

void Terrain::createChunk(glm::ivec2 position)
{
    Chunk* chunk = new Chunk(context, position);

    for(int x = 0 ; x < 16; x++)
    {
            for(int z = 0; z < 16; z++)
            {

                float height = fbm(x + position[0], z +position[1]) * 16 + 128;
                for(int y = 81; y < 256; ++y)
                {
                    if (y < height)
                    {
                        if(y == ceil(height) - 1)
                        {
                            chunk->setBlockType(x, y, z, GRASS);
                        }
                        else if(y >= 128)
                        {
                            chunk->setBlockType(x, y, z, DIRT);
                        }
                        else
                        {
                            chunk->setBlockType(x, y, z, STONE);
                        }
                    }
                    else
                    {
                        chunk->setBlockType(x, y, z, EMPTY);
                    }
                }
        }
    }
    chunk->te = this;
    chunkMap[std::make_pair(chunk->posX,chunk->posZ)] = chunk;
    chunk->create();
}

void Terrain::createChunk1(glm::ivec2 position)
{
    Chunk* chunk = new Chunk(context, position);

    for(int x = 0 ; x < 16; x++)
    {
            for(int z = 0; z < 16; z++)
            {

                float height = fbm(x + position[0], z +position[1]) * 16 + 128;
                for(int y = 81; y < 256; ++y)
                {
                    if (y < height)
                    {
                        if(y == ceil(height) - 1)
                        {
                            chunk->setBlockType(x, y, z, GRASS);
                        }
                        else if(y >= 128)
                        {
                            chunk->setBlockType(x, y, z, DIRT);
                        }
                        else
                        {
                            chunk->setBlockType(x, y, z, STONE);
                        }
                    }
                    else
                    {
                        chunk->setBlockType(x, y, z, EMPTY);
                    }
                }
        }
    }
    chunk->te = this;
    chunkMap[std::make_pair(chunk->posX,chunk->posZ)] = chunk;
//    chunk->create();
}

void Terrain::createChunkTrans(glm::ivec2 position)
{
    Chunk* chunk = new Chunk(context, position);

    for(int x = 0 ; x < 16; x++)
            for(int z = 0; z < 16; z++)
                for(int y = 0; y < 256; ++y)
                         chunk->setBlockType(x, y, z, EMPTY);


    for(int x = 0 ; x < 16; x++)
            for(int z = 0; z < 16; z++)
                  chunk->setBlockType(x, 80, z, STONE);


    chunk->te = this;
    chunkMap[std::make_pair(chunk->posX,chunk->posZ)] = chunk;
//    chunk->create();

}

std::pair<int, int> Terrain::getkey(glm::vec3 pos)
{
    int chunkx,chunkz;
    if(pos[0] >= 0)
        chunkx = ((int)pos[0] / 16) * 16;
    else
        chunkx = ((int)pos[0] / 16 - 1) * 16;
    if(pos[2] >= 0)
        chunkz = ((int)pos[2] / 16) * 16;
    else
        chunkz = ((int)pos[0] / 16 - 1) * 16;
//    printf("xxxxxxxxxx=%d\n",chunkx);
//    printf("zzzzzzzzzz=%d\n",chunkz);
    return std::make_pair(chunkx, chunkz);
}

void Terrain::destroychunk(glm::vec3 pos)
{
    std::pair<int, int> key = getkey(pos);
    if(chunkMap.find(key) != chunkMap.end())
    {
        Chunk* c = chunkMap[key];
        c->destroy();
    }
}

void Terrain::updatechunk(glm::vec3 pos)
{
    std::pair<int, int> key = getkey(pos);
    if(chunkMap.find(key) != chunkMap.end())
    {
        Chunk* c = chunkMap[key];
        c->create();
    }
}

bool Terrain::hasChunk(glm::vec3 pos)
{
    std::pair<int, int> key = getkey(pos);
    if(chunkMap.find(key) != chunkMap.end())
        return true;
    else
        return false;
}

void Terrain::createTerrain(glm::ivec2 position)
{
    glm::ivec2 worldpos;
    worldpos[0] = glm::floor((float)position[0] / 64);
    worldpos[1] = glm::floor((float)position[1] / 64);
    for(int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            glm::ivec2 pos;
            if(worldpos[0] >=0)
                pos[0] = ((int)worldpos[0] * 64 + i * 16);
            else
                pos[0] = ((int)(worldpos[0] + 1) * 64 - i * 16);
            if(worldpos[1] >=0)
                pos[1] = ((int)worldpos[1] * 64 + j * 16);
            else
                pos[1] = ((int)(worldpos[1] + 1) * 64 - j * 16);
            createChunk(pos);
        }
    }
}

void Terrain::createTerrain1(glm::ivec2 position)
{
    glm::ivec2 worldpos;
    worldpos[0] = glm::floor((float)position[0] / 64);
    worldpos[1] = glm::floor((float)position[1] / 64);
    for(int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            glm::ivec2 pos;
            if(worldpos[0] >=0)
                pos[0] = ((int)worldpos[0] * 64 + i * 16);
            else
                pos[0] = ((int)(worldpos[0] + 1) * 64 - i * 16);
            if(worldpos[1] >=0)
                pos[1] = ((int)worldpos[1] * 64 + j * 16);
            else
                pos[1] = ((int)(worldpos[1] + 1) * 64 - j * 16);
            createChunk1(pos);
        }
    }
}

void Terrain::createTerrainTrans(glm::ivec2 position)
{
    glm::ivec2 worldpos;
    worldpos[0] = glm::floor((float)position[0] / 64);
    worldpos[1] = glm::floor((float)position[1] / 64);
    for(int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            glm::ivec2 pos;
            if(worldpos[0] >=0)
                pos[0] = ((int)worldpos[0] * 64 + i * 16);
            else
                pos[0] = ((int)(worldpos[0] + 1) * 64 - i * 16);
            if(worldpos[1] >=0)
                pos[1] = ((int)worldpos[1] * 64 + j * 16);
            else
                pos[1] = ((int)(worldpos[1] + 1) * 64 - j * 16);
            createChunkTrans(pos);
        }
    }
}


void Terrain::remove_above(int x, int y, int z)
{
    for (int i = y; i < 256; i++)
        setBlockAt(x, i, z, EMPTY);
}


void Terrain::remove_below(int x, int z)
{
    for(int i = 124; i <= 127; i++)
    setBlockAt(x, i, z, EMPTY);
}

void Terrain::draw_deltariverbank(std::vector<glm::vec2> positions)
{
    int river_level = 128;
    int slope_distance = 6;

    for (glm::vec2 p : positions)
    {
        int x = (int)p[0];
        int z = (int)p[1];

        for(int i = x - 2; i <= x + 2; i++)
            remove_above(i, river_level + 1, z);

        for(int i = x - 2; i <= x + 2; i++)
            setBlockAt(i, river_level, z, EMPTY);

        for(int i = x - 2; i <= x + 2; i++)
            remove_below(i, z);

        for (int i = 0; i < slope_distance; i++)
        {
            int x_left = x - 2 - i;
            remove_above(x_left, river_level + i, z);
            if (getBlockAt(x_left, river_level, z) != EMPTY)
                setBlockAt(x_left, river_level + i, z, GRASS);

            int x_right = x + 2 + i;
            remove_above(x_right, river_level + i, z);
            if (getBlockAt(x_right, river_level, z) != EMPTY)
                setBlockAt(x_right, river_level + i, z, GRASS);

            int z_forward = z + i;
            for(int j = x - 2; j <= x+2; j++)
            {
                remove_above(j, river_level + i, z_forward);
                if (getBlockAt(j, river_level, z_forward) != EMPTY)
                {
                    setBlockAt(j, river_level + i, z_forward, GRASS);
                }
            }
        }
    }
}

void Terrain::draw_deltariver(std::vector<glm::vec2> positions)
{
    int river_level = 128;

    for (glm::vec2 p : positions)
    {
        int x = (int)p[0];
        int z = (int)p[1];

        for(int i = x - 2; i <= x + 2; i++)
            for(int y = river_level; y >= river_level - 4; y--)
                setBlockAt(i, y, z, WATER);
    }
}

void Terrain::draw_linearriver(std::vector<glm::vec2> positions)
{
    int river_level = 128;
//    int slope_distance = 6;

    for (glm::vec2 p : positions)
    {
        int x = (int)p[0];
        int z = (int)p[1];

        for(int i = x - 4; i <= x + 4; i++)
            for(int y = river_level; y >= river_level - 4; y--)
        setBlockAt(i, y, z, LAVA);
    }
}

void Terrain::draw_linearriverbank(std::vector<glm::vec2> positions)
{
    int river_level = 128;
    int slope_distance = 6;

    for (glm::vec2 p : positions)
    {
        int x = (int)p[0];
        int z = (int)p[1];

        for(int i = x - 4; i <= x + 4; i++)
            remove_above(i, river_level + 1, z);

        for(int i = x - 4; i <= x + 4; i++)
            setBlockAt(i, river_level, z, EMPTY);

        for(int i = x - 4; i <= x + 4; i++)
            remove_below(i, z);

        for (int i = 0; i < slope_distance; i++)
        {
            int x_left = x - 4 - i;
            remove_above(x_left, river_level + i, z);
            if (getBlockAt(x_left, river_level, z) != EMPTY)
                setBlockAt(x_left, river_level + i, z, GRASS);

            int x_right = x + 4 + i;
            remove_above(x_right, river_level + i, z);
            if (getBlockAt(x_right, river_level, z) != EMPTY)
                setBlockAt(x_right, river_level + i, z, GRASS);
        }
    }
}

std::vector<glm::vec2> Terrain::linear_positions()
{
    int times = 128;
    QString LString = linear_river->axiom;
    for (int i = 0; i < times; i++)
        LString = linear_river->expand_string(LString);

    for (QChar c : LString)
    {
        void (LSystem::*fptr)(void);
        fptr = linear_river->draw_map[c];
        (linear_river->*fptr)();
    }
    return linear_river->positions;
}

std::vector<glm::vec2> Terrain::delta_positions()
{

    int times = 12;
    QString LString = delta_river->axiom;
    for (int i = 0; i < times; i++)
        LString = delta_river->expand_string(LString);

    for (QChar c : LString)
    {
        void (LSystem::*fptr)(void);
        fptr = delta_river->draw_map[c];
        (delta_river->*fptr)();

    }
    return delta_river->positions;
}


void Terrain::remove_disc(int x, int y, int z, int radius)
{
    for(int i = x - radius; i <= x + radius; i++)
    {
        for(int j = y - radius; j <= y + radius; j++)
        {
            float dis = sqrt((i - x) * (i - x) + (j - y) * (j - y));
            if(dis < radius)
            {
                if(dis == radius - 1)
                    setBlockAt(i,j,z,COAL);
                else
                    setBlockAt(i, j, z, EMPTY);
            }
        }
    }
}

void Terrain::draw_rhombus(int x, int y, int z, int radius, BlockType b)
{
    for(int i = x - radius; i <= x + radius; i++)
    {
        for(int j = z - radius; j <= z + radius; j++)
        {
            float dis = abs(i - x) + abs(j - z);
            if(radius > 2)
            {
                if(dis <= radius)
                setBlockAt(i, y, j, b);
            }
            else if (radius == 2)
            {
                if(dis < radius)
                setBlockAt(i, y, j, b);
            }
            else
                setBlockAt(x, y, z, b);
        }
    }
}

void Terrain::draw_diamond(int x, int y, int z, int radius, BlockType b)
{
    for(int i = x - radius; i <= x + radius; i++)
    {
        for(int j = z - radius; j <= z + radius; j++)
        {
            float dis = abs(i - x) + abs(j - z);
            if(radius >= 2)
            {
                if(dis <= radius)
                setBlockAt(i, y, j, b);
            }
            else
                setBlockAt(x, y, z, b);
        }
    }
}

void Terrain::draw_disc(int x, int y, int z, int radius, BlockType b)
{
    for(int i = x - radius; i <= x + radius; i++)
    {
        for(int j = z - radius; j <= z + radius; j++)
        {
            float dis = sqrt((i - x) * (i - x) + (j - z) * (j - z));
            if(radius >= 2)
            {
                if(dis < radius)
                setBlockAt(i, y, j, b);
            }
            else
                setBlockAt(x, y, z, b);
        }
    }
}

void Terrain::make_cave(int x, int bottom, int z, int radius)
{
    for(int i = x - radius; i <= x + radius; i++)
    {
        for(int j = z - radius; j <= z + radius; j++)
        {
            for(int k = bottom; k < 256; k++)
            {
                float dis = sqrt((i - x) * (i - x) + (j - z) * (j - z));
                if(dis <= radius)
                    setBlockAt(i, k, j, EMPTY);
            }
        }
    }
}

void Terrain::make_lavapool(int x, int bottom, int z, int radius)
{
    for(int i = x - radius; i <= x + radius; i++)
    {
        for(int j = z - radius; j <= z + radius; j++)
        {
            for(int k = bottom; k < bottom + 5; k++)
            {
                float dis = sqrt((i - x) * (i - x) + (j - z) * (j - z));
                if(dis <= radius)
                    setBlockAt(i, k, j, LAVA);
            }
        }
    }
}

void Terrain::draw_tunnel(std::vector<glm::vec2> positions)
{
    int tunnel_level = 122;
    int tunnel_radius = 7;
    int cave_radius = 3;
    int lavapool_radius = 10;
    int i = 0;
    int j = 0;
    int size = positions.size();

    for (glm::vec2 p : positions)
    {
        int x = (int)p[0];
        int z = (int)p[1];
        i++;
        remove_disc(x, tunnel_level, z, tunnel_radius);

    }
    i = 0;
    for (glm::vec2 p : positions)
    {
        int x = (int)p[0];
        int z = (int)p[1];
        i++;

        if(i % 32 == 0)
        {
           if(i % 64 == 0)
                make_cave(x - 3, tunnel_level - tunnel_radius - 5, z, lavapool_radius);
           else
           {
                make_cave(x - 3, tunnel_level - tunnel_radius - 3, z, cave_radius + j * 2);
                j++;
           }
        }
    }
}




void Terrain::draw_lava(std::vector<glm::vec2> positions)
{
    int i = 0;
    int tunnel_level = 122;
    int tunnel_radius = 7;
    int lavapool_radius = 10;
    for (glm::vec2 p : positions)
    {
        i++;
        int x = (int)p[0];
        int z = (int)p[1];
        if(i % 64 == 0)
            make_lavapool(x - 3, tunnel_level - tunnel_radius - 5, z, lavapool_radius);
    }
}

std::vector<glm::vec2> Terrain::tunnel_positions()
{
    int times = 40;
    QString LString = tunnel->axiom;
    for (int i = 0; i < times; i++)
        LString = tunnel->expand_string(LString);

    for (QChar c : LString)
    {
        void (LSystem::*fptr)(void);
        fptr = tunnel->draw_map[c];
        (tunnel->*fptr)();
    }
    return tunnel->positions;
}

void Terrain::draw_building(std::vector<glm::vec2> positions)
{
    for (glm::vec2 p : positions)
    {
        int x = (int)p[0];
        int y = (int)p[1];

        if(y - 128 < 30)
            draw_rhombus(x, y, 30, 8, STONE);
        else if(y - 128 >=30 && y - 128 <= 50)
            draw_rhombus(x, y, 30, 7, STONE);
        else if(y - 128 > 50 && y - 128 <= 55)
            draw_rhombus(x, y, 30, 2, STONE);
        else if(y - 128 > 55 && y - 128 <= 70)
            draw_rhombus(x, y, 30, 4, STONE);
        else
            draw_rhombus(x, y, 30, 1, STONE);
    }
    for (glm::vec2 p : positions)
    {
        int z = (int)p[0];
        int y = (int)p[1];

        if(y - 128 < 30)
            draw_rhombus(210, y, z - 180, 8, ICE);
        else if(y - 128 >=30 && y - 128 <= 50)
            draw_rhombus(210, y, z - 180, 7, ICE);
        else if(y - 128 > 50 && y - 128 <= 55)
            draw_rhombus(210, y, z - 180, 2, ICE);
        else if(y - 128 > 55 && y - 128 <= 70)
            draw_rhombus(210, y, z - 180, 4, ICE);
        else
            draw_rhombus(210, y, z - 180, 1, ICE);
    }
}

std::vector<glm::vec2> Terrain::building_positions()
{
    int times = 20;
    QString LString = building->axiom;
    for (int i = 0; i < times; i++)
        LString = building->expand_string(LString);

    for (QChar c : LString)
    {
        void (LSystem::*fptr)(void);
        fptr = building->draw_map[c];
        (building->*fptr)();
    }

    return building->positions;
}

void Terrain::make_tree(int x, int z)
{
    int height;
    for(int i = 128; i < 256; ++i)
    {
        if(getBlockAt(x,i,z) != EMPTY && getBlockAt(x, i + 1, z) == EMPTY)
        {
            height = i;
            break;
        }
    }
    if(getBlockAt(x, height, z) == GRASS)
    {
        for (int i = 1; i<= 13;i++)
        {
            if(i >= 1 && i <=4)
                setBlockAt(x, height + i, z, GRASS);
            else if(i == 5)
                draw_diamond(x, height + i, z, 5, LEAF);
            else if(i == 6)
                draw_diamond(x, height + i, z, 4, LEAF);
            else if(i == 7)
                draw_diamond(x, height + i, z, 3, LEAF);
            else if(i == 8)
                draw_diamond(x, height + i, z, 3, LEAF);
            else if(i == 9)
                draw_diamond(x, height + i, z, 2, LEAF);
            else if(i == 10)
                draw_diamond(x, height + i, z, 2, LEAF);
            else if(i == 11)
                draw_rhombus(x, height + i, z, 2, LEAF);
            else if(i == 12)
                draw_rhombus(x, height + i, z, 2, LEAF);
            else if(i == 13)
                draw_diamond(x, height + i, z, 1, LEAF);
        }
    }
}
