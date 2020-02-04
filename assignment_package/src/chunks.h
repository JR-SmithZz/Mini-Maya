#ifndef CHUNKS_H
#define CHUNKS_H

#include <la.h>
#include <drawable.h>
#include <QHash>
#include "terrains.h"
#include <utils.h>


class Terrain;

class Chunk : public Drawable{
public:
    Chunk(OpenGLContext* context);
    Chunk(OpenGLContext* context, glm::ivec2 position);

    ~Chunk();
    void destroy();
    BlockType m_blocks[16*256*16];
    //BlockType getBlockType(int x, int y, int z) const ;
    BlockType getBlockType(int x, int y, int z);
    glm::vec4 getColor(BlockType t);
    Terrain* te;
    Terrain* teTrans = nullptr;

    void setBlockType(int x, int y, int z, BlockType type);

    glm::vec2 getUVFromType(BlockType a, int face);
    glm::vec2 getPowerandAniFromType(BlockType type, int face);

    void create();
    void createHull(int i, int j, int k);

    BlockType m_chunkBlocks[16*256*16];

    int64_t trunkPos;
    int posX;
    int posZ;
    bool hasChanged = true;

    std::vector<glm::vec4> posV;
    std::vector<glm::vec4> norV;
    std::vector<glm::vec4> colV;
    std::vector<glm::vec2> uvV;
    std::vector<GLuint> idxV;
    std::vector<float> aniV;
    std::vector<float> powerV;

};
#endif // CHUNKS_H
