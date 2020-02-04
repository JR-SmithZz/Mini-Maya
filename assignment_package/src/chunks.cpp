#include "chunks.h"
#include <iostream>

Chunk::Chunk(OpenGLContext* context): Drawable(context){
    BlockType type = EMPTY;
    for (int i = 0; i < 16*16*256; i++) {
        this->m_blocks[i] = type;
    }
    trunkPos = 0;
    posX = 0;
    posZ = 0;
}

Chunk::Chunk(OpenGLContext* context, glm::ivec2 position): Drawable(context)
{
    BlockType type = EMPTY;
    for (int i = 0; i < 16*16*256; i++) {
        this->m_blocks[i] = type;
    }
    //trunkPos =  ((int64_t)position[0]) << 32 | (position[1]);
    posX = position[0];
    posZ = position[1];
}

Chunk::~Chunk(){
    destroy();
}

void Chunk::destroy(){
    posV.clear();
    norV.clear();
    colV.clear();
    idxV.clear();
    uvV.clear();
    aniV.clear();
    powerV.clear();


}


BlockType Chunk::getBlockType(int x, int y, int z){
    if(x>=0&&x<16&&y>=0&&y<256&&z>=0&&z<16){
         //std::cout<<"not empty"<<std::endl;
        return this->m_blocks[16*256*z+16*y+x];
    }
    else
    {//std::cout<<"empty"<<std::endl;
        BlockType I = EMPTY;
        return I ;
}
}


void Chunk::setBlockType(int x, int y, int z, BlockType type){
    if(x>=0&&x<16&&y>=0&&y<256&&z>=0&&z<16)
    this->m_blocks[16*256*z+16*y+x] = type;
}

glm::vec4 Chunk::getColor(BlockType t){
    glm::vec4 color(1,1,1,1);
    switch(t)
    {
    case DIRT:
        color = (glm::vec4(121.f, 85.f, 58.f, 255.f) / 255.f);
        break;
    case GRASS:
        color = (glm::vec4(95.f, 159.f, 53.f, 255.f) / 255.f);
        break;
    case STONE:
        color = (glm::vec4(0.5f));
        break;
    default:
        // Other types are as of yet not defined
        break;
    }
    return color;
}

void Chunk::create()
{
    destroy();
    std::vector<glm::vec4> temp;
    for(int i = posX; i < posX + 16; i++){
        for(int j = 0; j < 256; j++){
            for(int k = posZ; k < posZ + 16; k++){
                  createHull(i,j,k);
            }
        }
    }
    //std::cout<<"tempSize1"<<posV.size()<<std::endl;
    for(int i = 0; i < posV.size(); i++){
        temp.push_back(posV.at(i));
        //std::cout<<posV.at(i).x<<std::endl;
        temp.push_back(norV.at(i));

        temp.push_back(colV.at(i));

    }
    this->count = idxV.size();
    generateIdx();
    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxV.size() * sizeof(GLuint), idxV.data(), GL_STATIC_DRAW);
    generateTotal();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufTotal);
    context->glBufferData(GL_ARRAY_BUFFER, temp.size() * sizeof(glm::vec4), temp.data(), GL_STATIC_DRAW);
    generateUV();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufUV);
    context->glBufferData(GL_ARRAY_BUFFER, uvV.size() * sizeof(glm::vec2), uvV.data(), GL_STATIC_DRAW);
    generatePower();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufPower);
    context->glBufferData(GL_ARRAY_BUFFER, powerV.size() * sizeof(float), powerV.data(), GL_STATIC_DRAW);
    generateAni();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufAni);
    context->glBufferData(GL_ARRAY_BUFFER, aniV.size() * sizeof(float), aniV.data(), GL_STATIC_DRAW);

}



void Chunk::createHull(int i, int j, int k){
    if(te->getBlockAt(i,j,k)==EMPTY){
        //std::cout<<i<<j<<k<<" didn't create the hull"<<std::endl;
        return;
    }
    //revert the local position value brfore adding posX and posZ in create()
    int blockI = i-posX;
    int blockJ = j;
    int blockK = k-posZ;

    glm::vec4 pos(blockI,blockJ,blockK,0);
    glm::vec4 pos0 = pos+ glm::vec4 (0,0,0,1);
    glm::vec4 pos1 = pos+ glm::vec4 (1,0,0,1);
    glm::vec4 pos2 = pos+ glm::vec4 (1,0,1,1);
    glm::vec4 pos3 = pos+ glm::vec4 (0,0,1,1);
    glm::vec4 pos4 = pos+ glm::vec4 (0,1,0,1);
    glm::vec4 pos5 = pos+ glm::vec4 (1,1,0,1);
    glm::vec4 pos6 = pos+ glm::vec4 (1,1,1,1);
    glm::vec4 pos7 = pos+ glm::vec4 (0,1,1,1);

    glm::vec4 nor0(0,-1,0,0);
    glm::vec4 nor1(0,1,0,0);
    glm::vec4 nor2(0,0,-1,0);
    glm::vec4 nor3(1,0,0,0);
    glm::vec4 nor4(0,0,1,0);
    glm::vec4 nor5(-1,0,0,0);
    BlockType type = getBlockType(blockI,blockJ,blockK);
    glm::vec4 col = getColor(type);

    //-x direction

    BlockType adjacentX = te->getBlockAt(i-1,j,k);
    BlockType adjacentXTrans = EMPTY;
    if(teTrans!= nullptr){
       adjacentXTrans = teTrans->getBlockAt(i-1,j,k);
    }
    glm::vec2 uv0 = getUVFromType(type,2);
    float ani = getPowerandAniFromType(type,2)[0];
    float power = getPowerandAniFromType(type,2)[1];
    if(adjacentX == EMPTY && adjacentXTrans == EMPTY){

        int posSize = posV.size();
        posV.push_back(pos0);
        posV.push_back(pos3);
        posV.push_back(pos7);
        posV.push_back(pos4);

           uvV.push_back(uv0);
        uvV.push_back(uv0+glm::vec2(15.f/256.f, 0));
        uvV.push_back(uv0+glm::vec2(15.f/256.f, 15.f/256.f));
        uvV.push_back(uv0+glm::vec2(0, 15.f/256.f));

        for(int i = 0; i < 4; i++){
            aniV.push_back(ani);
            powerV.push_back(power);
            norV.push_back(nor5);
            colV.push_back(col);
            if(i>2){
                idxV.push_back(posSize);
                idxV.push_back(posSize+i-1);
            }
            idxV.push_back(posSize+i);
        }
    }
    //std::cout<<"posSize1"<<posV.size()<<std::endl;
    //x direction
    uv0 = getUVFromType(type,1);
    ani = getPowerandAniFromType(type,1)[0];
    power = getPowerandAniFromType(type,1)[1];
    if(teTrans!= nullptr){
       adjacentXTrans = teTrans->getBlockAt(i+1,j,k);
    }
    adjacentX = te->getBlockAt(i+1,j,k);
    if(adjacentX == EMPTY && adjacentXTrans == EMPTY){

        int posSize = posV.size();
        posV.push_back(pos1);
        posV.push_back(pos5);
        posV.push_back(pos6);
        posV.push_back(pos2);


        uvV.push_back(uv0+glm::vec2(15.f/256.f, 0));
        uvV.push_back(uv0+glm::vec2(15.f/256.f, 15.f/256.f));
        uvV.push_back(uv0+glm::vec2(0, 15.f/256.f));
         uvV.push_back(uv0);
        for(int i = 0; i < 4; i++){
            aniV.push_back(ani);
            powerV.push_back(power);
            norV.push_back(nor3);
            colV.push_back(col);
            if(i>2){
                idxV.push_back(posSize);
                idxV.push_back(posSize+i-1);
            }
            idxV.push_back(posSize+i);
        }
    }

    //-y direction
    adjacentX = te->getBlockAt(i,j-1,k);
     uv0 = getUVFromType(type,4);
     ani = getPowerandAniFromType(type,4)[0];
     power = getPowerandAniFromType(type,4)[1];
     if(teTrans!= nullptr){
        adjacentXTrans = teTrans->getBlockAt(i,j-1,k);
     }
    if(adjacentX == EMPTY && adjacentXTrans == EMPTY){
        //if((j-1>=0&&j-1<256&&m_blocks[i*256*16+(j-1)*16+k] == EMPTY)||(j-1<0)){
        //std::cout<<"-y"<<std::endl;

        int posSize = posV.size();
        posV.push_back(pos0);
        posV.push_back(pos1);
        posV.push_back(pos2);
        posV.push_back(pos3);

        uvV.push_back(uv0+glm::vec2(15.f/256.f, 0));
        uvV.push_back(uv0+glm::vec2(15.f/256.f, 15.f/256.f));
        uvV.push_back(uv0+glm::vec2(0, 15.f/256.f));
         uvV.push_back(uv0);
        for(int i = 0; i < 4; i++){
            aniV.push_back(ani);
            powerV.push_back(power);
            norV.push_back(nor0);
            colV.push_back(col);
            if(i>2){
                idxV.push_back(posSize);
                idxV.push_back(posSize+i-1);
            }
            idxV.push_back(posSize+i);
        }
    }

    //y direction
    adjacentX = te->getBlockAt(i,j+1,k);
     uv0 = getUVFromType(type,3);
     ani = getPowerandAniFromType(type,3)[0];
     power = getPowerandAniFromType(type,3)[1];
     if(teTrans!= nullptr){
        adjacentXTrans = teTrans->getBlockAt(i,j+1,k);
     }
    if(adjacentX == EMPTY&& adjacentXTrans == EMPTY){
//        std::cout<<"y"<<std::endl;
//        std::cout<<pos4.x<<std::endl;
//        std::cout<<pos7.x<<std::endl;
//        std::cout<<pos6.x<<std::endl;
        //if((j+1>=0&&j+1<256&&m_blocks[i*256*16+(j+1)*16+k] == EMPTY)||(j+1<256)){
        //std::cout<<i<<" "<<j<<" " <<k<<std::endl;
        int posSize = posV.size();
        posV.push_back(pos4);
        posV.push_back(pos7);
        posV.push_back(pos6);
        posV.push_back(pos5);



        uvV.push_back(uv0+glm::vec2(15.f/256.f, 15.f/256.f));
        uvV.push_back(uv0+glm::vec2(0, 15.f/256.f));
       uvV.push_back(uv0);
uvV.push_back(uv0+glm::vec2(15.f/256.f, 0));
        for(int i = 0; i < 4; i++){
            aniV.push_back(ani);
            powerV.push_back(power);
            norV.push_back(nor1);
            colV.push_back(col);
            if(i>2){
                idxV.push_back(posSize);
                idxV.push_back(posSize+i-1);
            }
            idxV.push_back(posSize+i);
        }
    }

    //-z direction
    adjacentX = te->getBlockAt(i,j,k-1);
     uv0 = getUVFromType(type,6);
     ani = getPowerandAniFromType(type,6)[0];
     power = getPowerandAniFromType(type,6)[1];
     if(teTrans!= nullptr){
        adjacentXTrans = teTrans->getBlockAt(i,j,k-1);
     }
    if(adjacentX == EMPTY&& adjacentXTrans == EMPTY){
//        std::cout<<"-z"<<std::endl;
//        std::cout<<pos0.x<<std::endl;
//        std::cout<<pos4.x<<std::endl;
//        std::cout<<pos5.x<<std::endl;
        //if((k-1>=0&&k-1<16&&m_blocks[i*256*16+j*16+k-1] == EMPTY)||(k-1<0)){

        int posSize = posV.size();
        posV.push_back(pos0);
        posV.push_back(pos4);
        posV.push_back(pos5);
        posV.push_back(pos1);

        uvV.push_back(uv0+glm::vec2(15.f/256.f, 0));
        uvV.push_back(uv0+glm::vec2(15.f/256.f, 15.f/256.f));
        uvV.push_back(uv0+glm::vec2(0, 15.f/256.f));
        uvV.push_back(uv0);
        for(int i = 0; i < 4; i++){
            aniV.push_back(ani);
            powerV.push_back(power);
            norV.push_back(nor2);
            colV.push_back(col);
            if(i>2){
                idxV.push_back(posSize);
                idxV.push_back(posSize+i-1);
            }
            idxV.push_back(posSize+i);
        }
    }

    //z direction
    adjacentX = te->getBlockAt(i,j,k+1);
     uv0 = getUVFromType(type,5);
     ani = getPowerandAniFromType(type,5)[0];
     power = getPowerandAniFromType(type,5)[1];
     if(teTrans!= nullptr){
        adjacentXTrans = teTrans->getBlockAt(i,j,k+1);
     }
    if(adjacentX == EMPTY&& adjacentXTrans == EMPTY){
//        std::cout<<"z"<<std::endl;
//        std::cout<<pos2.x<<std::endl;
//        std::cout<<pos6.x<<std::endl;
//        std::cout<<pos7.x<<std::endl;
        //std::cout<<j<<std::endl;
        //if((k+1>=0&&k+1<16&&m_blocks[i*256*16+j*16+k+1] == EMPTY)||(k+1>=16)){

        int posSize = posV.size();
        posV.push_back(pos2);
        posV.push_back(pos6);
        posV.push_back(pos7);
        posV.push_back(pos3);

        uvV.push_back(uv0+glm::vec2(15.f/256.f, 0));
        uvV.push_back(uv0+glm::vec2(15.f/256.f, 15.f/256.f));
        uvV.push_back(uv0+glm::vec2(0, 15.f/256.f));
        uvV.push_back(uv0);
        for(int i = 0; i < 4; i++){
            aniV.push_back(ani);
            powerV.push_back(power);
            norV.push_back(nor4);
            colV.push_back(col);
            if(i>2){
                idxV.push_back(posSize);
                idxV.push_back(posSize+i-1);
            }
            idxV.push_back(posSize+i);
        }
    }
    //std::cout<<"posSize2"<<posV.size()<<std::endl;

}

glm::vec2 Chunk::getUVFromType(BlockType type, int face){
    glm::vec2 uv0(1.f,1.f);
    switch(type)
    {
    case DIRT:
        uv0 = glm::vec2(2/16.f, 15/16.f);
        break;
    case GRASS:
        switch(face){
        case 3:
            uv0 = glm::vec2(8.f/16.f, 13.f/16.f);
            break;
        case 4:
             uv0 = glm::vec2(2/16.f, 15.f/16.f);
             break;
        case 1:
             uv0 = glm::vec2(3/16.f, 15.f/16.f);
             break;
        case 2:
             uv0 = glm::vec2(3/16.f, 15.f/16.f);
             break;
        case 5:
             uv0 = glm::vec2(3/16.f, 15.f/16.f);
             break;
        case 6:
             uv0 = glm::vec2(3/16.f, 15.f/16.f);
             break;
        }
        break;
    case STONE:
        uv0 = glm::vec2(0/16.f, 15/16.f);
        break;
    case WOOD:
        uv0 = glm::vec2(7/16.f, 10/16.f);
        break;
    case LEAF:
        uv0 = glm::vec2(4/16.f, 12/16.f);
        break;
    case BEDROCK:
        uv0 = glm::vec2(1/16.f, 14/16.f);
        break;
    case LAVA:
        uv0 = glm::vec2(15/16.f, 1/16.f);
        break;
    case WATER:
        uv0 = glm::vec2(14/16.f, 3/16.f);
        break;
    case ICE:
        uv0 = glm::vec2(8/16.f, 14/16.f);
        break;
    case COAL:
        uv0 = glm::vec2(5/16.f, 13/16.f);
        break;
}
    return uv0;
}

glm::vec2 Chunk::getPowerandAniFromType(BlockType type, int face){
    glm::vec2 pa(1.f,1.f);
    switch(type)
    {
    case DIRT:
        pa = glm::vec2(0.f, 500.f);
        break;
    case GRASS:
        switch(face){
        case 3:
            pa = glm::vec2(0.f, 500.f);
            break;
        case 4:
             pa = glm::vec2(0.f, 500.f);
             break;
        case 1:
             pa = glm::vec2(0.f, 500.f);
             break;
        case 2:
             pa = glm::vec2(0.f, 500.f);
             break;
        case 5:
             pa = glm::vec2(0.f, 500.f);
             break;
        case 6:
             pa = glm::vec2(0.f, 500.f);
             break;
        }
        break;
    case STONE:
        pa = glm::vec2(0.f, 200.f);
        break;
    case WOOD:
        pa = glm::vec2(0.f, 300.f);
        break;
    case LEAF:
        pa = glm::vec2(0.f, 50.f);
        break;
    case BEDROCK:
        pa = glm::vec2(0.f, 400.f);
        break;
    case LAVA:
        pa = glm::vec2(1.f, 10.f);
        break;
    case WATER:
        pa = glm::vec2(1.f, 10.f);
        break;
    case ICE:
        pa = glm::vec2(0.f, 2.f);
        break;
    case COAL:
        pa = glm::vec2(0.f, 200.f);
        break;
}
    return pa;
}

