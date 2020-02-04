#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>


MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      mp_geomCube(mkU<Cube>(this)), mp_worldAxes(mkU<WorldAxes>(this)),
      mp_progLambert(mkU<ShaderProgram>(this)), mp_progFlat(mkU<ShaderProgram>(this)),mp_geomQuad(new Quad(this)),
      mp_camera(mkU<Camera>()), mp_sky(mkU<ShaderProgram>(this)), mp_terrain(mkU<Terrain>(this)), mp_terrainTrans(mkU<Terrain>(this)),mp_texture(new Texture(this)), m_renderedTexture(-1)
{
    // Connect the timer to a function so that when the timer ticks the function is executed
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    // Tell the timer to redraw 60 times per second
    timer.start(16);
    setFocusPolicy(Qt::ClickFocus);

    setMouseTracking(true); // MyGL will track the mouse's movements even if a mouse button is not pressed
    setCursor(Qt::BlankCursor); // Make the cursor invisible
}

MyGL::~MyGL()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
    mp_geomCube->destroy();
    MoveMouseToCenter();
}


void MyGL::MoveMouseToCenter()
{
    QCursor::setPos(this->mapToGlobal(QPoint(width() / 2, height() / 2)));
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    //    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    //    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.37f, 0.74f, 1.0f, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);
    createRenderBuffers();

    // Create and set up the diffuse shader
    mp_progLambert->create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    mp_progFlat->create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");
    //mp_progLambert->setGeometryColor(glm::vec4(0,1,0,1));

    mp_texture->create(":/minecraft_textures_all.png");
    mp_texture->load(0);
    // Set a color with which to draw geometry since you won't have one
    // defined until you implement the Node classes.
    // This makes your geometry render green.

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);

    mp_terrain->createTerrain1(glm::ivec2(20,20));
    mp_terrain->createTerrain1(glm::ivec2(20,80));
//    mp_terrain->createTerrain1(glm::ivec2(20,130));

    mp_terrain->createTerrain1(glm::ivec2(80,20));
    mp_terrain->createTerrain1(glm::ivec2(80,80));
//    mp_terrain->createTerrain1(glm::ivec2(80,130));

    mp_terrain->createTerrain1(glm::ivec2(130,20));
    mp_terrain->createTerrain1(glm::ivec2(130,80));
//    mp_terrain->createTerrain1(glm::ivec2(130,130));

    mp_terrain->createTerrain1(glm::ivec2(200,20));
    mp_terrain->createTerrain1(glm::ivec2(200,80));
    mp_terrain->createTerrain1(glm::ivec2(200,130));
    mp_terrain->createTerrain1(glm::ivec2(270,20));
    mp_terrain->createTerrain1(glm::ivec2(270,80));
    mp_terrain->createTerrain1(glm::ivec2(270,130));

    std::vector<glm::vec2> delta_positions;
    delta_positions = mp_terrain->delta_positions();
    mp_terrain->draw_deltariverbank(delta_positions);

    std::vector<glm::vec2> linear_positions;
    linear_positions =  mp_terrain->linear_positions();
    mp_terrain->draw_linearriverbank(linear_positions);

    std::vector<glm::vec2> tunnel_positions;
    tunnel_positions =  mp_terrain->tunnel_positions();
    mp_terrain->draw_tunnel(tunnel_positions);

    std::vector<glm::vec2> building_positions;
    building_positions =  mp_terrain->building_positions();
    mp_terrain->draw_building(building_positions);

    for(int i = 0; i < 320; i+=16)
    {
        for(int j = 0; j < 192; j+=16)
        {
            int m = rand() % 16;
            int n = rand() % 16;
            mp_terrain->make_tree(i + m,j + n);
        }
    }

    for(auto iter=  mp_terrain->chunkMap.begin();iter!=mp_terrain->chunkMap.end();iter++)
    {
        Chunk* chunk = iter->second;
        if(mp_terrain->getBlockAt(iter->first.first, 81, iter->first.second) == STONE)
        {
            chunk->create();
        }
    }

    mp_terrainTrans->createTerrainTrans(glm::ivec2(20,20));
    mp_terrainTrans->createTerrainTrans(glm::ivec2(20,80));
    mp_terrainTrans->createTerrainTrans(glm::ivec2(80,20));
    mp_terrainTrans->createTerrainTrans(glm::ivec2(80,80));
    mp_terrainTrans->createTerrainTrans(glm::ivec2(130,20));
    mp_terrainTrans->createTerrainTrans(glm::ivec2(130,80));
    mp_terrainTrans->createTerrainTrans(glm::ivec2(200,20));
    mp_terrainTrans->createTerrainTrans(glm::ivec2(200,80));
    mp_terrainTrans->createTerrainTrans(glm::ivec2(200,130));
    mp_terrainTrans->createTerrainTrans(glm::ivec2(270,20));
    mp_terrainTrans->createTerrainTrans(glm::ivec2(270,80));
    mp_terrainTrans->createTerrainTrans(glm::ivec2(270,130));

    mp_terrainTrans->draw_deltariver(delta_positions);
    mp_terrainTrans->draw_linearriver(linear_positions);
    mp_terrainTrans->draw_lava(tunnel_positions);

    for(auto iter=  mp_terrainTrans->chunkMap.begin();iter!=mp_terrainTrans->chunkMap.end();iter++)
    {
        Chunk* chunk = iter->second;
        chunk->teTrans = mp_terrain.get();
        if(mp_terrainTrans->getBlockAt(iter->first.first, 80, iter->first.second) == STONE)
        {
            chunk->create();
        }
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glDisable(GL_BLEND);
    mp_sky->create(":/glsl/sky.vert.glsl", ":/glsl/sky.frag.glsl");
    mp_geomQuad->create();

}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    *mp_camera = Camera(w, h, glm::vec3(mp_terrain->dimensions.x, mp_terrain->dimensions.y * 0.75, mp_terrain->dimensions.z),
                        glm::vec3(mp_terrain->dimensions.x / 2, mp_terrain->dimensions.y / 2, mp_terrain->dimensions.z / 2), glm::vec3(0,1,0));
    glm::mat4 viewproj = mp_camera->getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    mp_progLambert->setViewProjMatrix(viewproj);
    mp_progFlat->setViewProjMatrix(viewproj);
    mp_sky->setViewProjMatrix(glm::inverse(viewproj));

    mp_sky->useMe();
    this->glUniform2i(mp_sky->unifDimensions, width(), height());
    this->glUniform3f(mp_sky->unifEye, mp_camera->eye.x, mp_camera->eye.y, mp_camera->eye.z);

    printGLErrorLog();
}


// MyGL's constructor links timerUpdate() to a timer that fires 60 times per second.
// We're treating MyGL as our game engine class, so we're going to use timerUpdate
void MyGL::timerUpdate()
{
    boundcheck();
    update();
}

// This function is called whenever update() is called.
// MyGL's constructor links update() to a timer that fires 60 times per second,
// so paintGL() called at a rate of 60 frames per second.
void MyGL::paintGL()
{

    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mp_progLambert->setViewProjMatrix(mp_camera->getViewProj());

    mp_sky->setViewProjMatrix(glm::inverse(mp_camera->getViewProj()));
    mp_sky->useMe();
    this->glUniform3f(mp_sky->unifEye, mp_camera->eye.x, mp_camera->eye.y, mp_camera->eye.z);
    this->glUniform1f(mp_sky->unifTime, (float)m_time);

    mp_sky->drawS(*mp_geomQuad);
    GLDrawScene();
    //  performPostprocessRenderPass();

    glDisable(GL_DEPTH_TEST);
    mp_progLambert->setTime(m_time);
    glEnable(GL_DEPTH_TEST);
    m_time++;
}

void MyGL::GLDrawScene()
{
    for(auto iter=  mp_terrain->chunkMap.begin();iter!=mp_terrain->chunkMap.end();iter++)
    {
        Chunk* chunk = iter->second;
        if(mp_terrain->getBlockAt(iter->first.first, 81, iter->first.second) == STONE)
        {
            mp_progLambert->setModelMatrix(glm::translate(glm::mat4(),glm::vec3(iter->first.first,0,iter->first.second)));
            mp_texture->bind(0);
            mp_progLambert->draw(*chunk);
        }
    }

    for(auto iter=  mp_terrainTrans->chunkMap.begin();iter!=mp_terrainTrans->chunkMap.end();iter++)
    {
        Chunk* chunkTrans = iter->second;
        if(mp_terrainTrans->getBlockAt(iter->first.first, 80, iter->first.second) == STONE)
        {
            mp_progLambert->setModelMatrix(glm::translate(glm::mat4(),glm::vec3(iter->first.first,0,iter->first.second)));
            mp_texture->bind(0);
            mp_progLambert->draw(*chunkTrans);
        }
    }
}

float MyGL::dis(vec3 p1, vec3 p2)
{
    int d1, d2, d3;
    d1 = (p2[0] - p1[0]) * (p2[0] - p1[0]);
    d2 = (p2[1] - p1[1]) * (p2[1] - p1[1]);
    d3 = (p2[2] - p1[2]) * (p2[2] - p1[2]);
    return std::sqrt(d1 + d2 + d3);
}

void MyGL::removeblock()
{
    vec3 pos = mp_camera->eye;
    vec3 direction = mp_camera->look;

    for(float t = 0.1f; t < 20; t = t + 0.1)
    {
        vec3 point = pos + t * direction;
        point.x = floor(point.x);
        point.y = floor(point.y);
        point.z = floor(point.z);
        if(mp_terrain->getBlockAt(point.x, point.y, point.z) != EMPTY)
        {
            mp_terrain->setBlockAt(point.x, point.y, point.z, EMPTY);
            mp_terrain->updatechunk(point);
            break;
        }
    }
}

void MyGL::addblock()
{
    vec3 pos = mp_camera->eye;
    vec3 direction = mp_camera->look;

    for(float t = 0.1f; t < 20; t = t + 0.1)
    {
        vec3 point = pos + t * direction;
        if(mp_terrain->getBlockAt(point.x, point.y, point.z) != EMPTY)
        {
            vec3 newpos;
            float min = FLT_MAX;
            for(int i = 0; i <= 2; i++)
            {
                for(int j = point[i] - 1; j <= point[i] + 1; j++)
                {
                    vec3 n = point;
                    n[i] = j;
                    if(mp_terrain->getBlockAt(n[0], n[1], n[2]) == EMPTY)
                    {

                        if(min > dis(pos, n))
                            min = dis(pos, n);
                        newpos = n;
                    }
                }
            }

            if(min < FLT_MAX)
            {
                mp_terrain->setBlockAt(newpos[0], newpos[1], newpos[2], STONE);
                mp_terrain->updatechunk(point);
            }
            break;
        }
    }
}

void MyGL::mousePressEvent(QMouseEvent *e)
{
    if(e->buttons() == Qt::LeftButton)
    {
        removeblock();
        update();
    }

    else if(e->buttons() == Qt::RightButton)
    {
        addblock();
        update();
    }
}


void MyGL::boundcheck()
{
    glm::vec3 pos = mp_camera->eye;
    int radius = 3;
    for(int ox = -radius; ox <= radius; ox += 2 * radius)
    {
        vec3 point(pos.x + ox, pos.y, pos.z);
        if(!mp_terrain->hasChunk(point))
        {
            vec2 newPos = vec2(pos.x + ox, pos.z);
            mp_terrain->createTerrain(newPos);
            glm::ivec2 worldpos;
            worldpos[0] = glm::floor((pos.x + ox)/ 64) * 64;
            worldpos[1] = glm::floor(pos.z / 64) * 64;

            for(int i = worldpos[0]; i < worldpos[0] + 64; i+=16)
            {
                for(int j = worldpos[1]; j < worldpos[1] + 64; j+=16)
                {
                    int m = rand() % 16;
                    int n = rand() % 16;
                    mp_terrain->make_tree(i + m,j + n);
                }
            }

            for(auto iter=  mp_terrain->chunkMap.begin();iter!=mp_terrain->chunkMap.end();iter++)
            {
                Chunk* chunk = iter->second;
                if(chunk->posX >= worldpos[0] && chunk->posX < worldpos[0] + 64 &&
                        chunk->posZ >= worldpos[1] && chunk->posZ < worldpos[1] + 64)
                {
                        chunk->create();
                }
            }
            return;
        }
    }
    for(int oz = -radius; oz <= radius; oz += 2 * radius)
    {
        vec3 point(pos.x, pos.y, pos.z + oz);
        if(!mp_terrain->hasChunk(point))
        {
            vec2 newPos = vec2(pos.x, pos.z + oz);
            mp_terrain->createTerrain1(newPos);

            glm::ivec2 worldpos;
            worldpos[0] = glm::floor(pos.x / 64) * 64;
            worldpos[1] = glm::floor((pos.z + oz)/ 64) * 64;

            for(int i = worldpos[0]; i < worldpos[0] + 64; i+=16)
            {
                for(int j = worldpos[1]; j < worldpos[1] + 64; j+=16)
                {
                    int m = rand() % 16;
                    int n = rand() % 16;
                    mp_terrain->make_tree(i + m,j + n);
                }
            }

            for(auto iter=  mp_terrain->chunkMap.begin();iter!=mp_terrain->chunkMap.end();iter++)
            {
                Chunk* chunk = iter->second;
                if(chunk->posX >= worldpos[0] && chunk->posX < worldpos[0] + 64 &&
                        chunk->posZ >= worldpos[1] && chunk->posZ < worldpos[1] + 64)
                {
                        chunk->create();
                }
            }
            return;
        }
    }

}
void MyGL::keyPressEvent(QKeyEvent *e)
{

    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used, but I really dislike their
    // syntax so I chose to be lazy and use a long
    // chain of if statements instead
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        mp_camera->RotateAboutUp(-amount);
    } else if (e->key() == Qt::Key_Left) {
        mp_camera->RotateAboutUp(amount);
    } else if (e->key() == Qt::Key_Up) {
        mp_camera->RotateAboutRight(-amount);
    } else if (e->key() == Qt::Key_Down) {
        mp_camera->RotateAboutRight(amount);
    } else if (e->key() == Qt::Key_1) {
        mp_camera->fovy += amount;
    } else if (e->key() == Qt::Key_2) {
        mp_camera->fovy -= amount;
    } else if (e->key() == Qt::Key_W) {
        mp_camera->TranslateAlongLook(amount);
    } else if (e->key() == Qt::Key_S) {
        mp_camera->TranslateAlongLook(-amount);
    } else if (e->key() == Qt::Key_D) {
        mp_camera->TranslateAlongRight(amount);
    } else if (e->key() == Qt::Key_A) {
        mp_camera->TranslateAlongRight(-amount);
        //        boundcheck();
    } else if (e->key() == Qt::Key_Q) {
        mp_camera->TranslateAlongUp(-amount);
    } else if (e->key() == Qt::Key_E) {
        mp_camera->TranslateAlongUp(amount);
    } else if (e->key() == Qt::Key_R) {
        *mp_camera = Camera(this->width(), this->height());
    }

    mp_camera->RecomputeAttributes();
}


void MyGL::createRenderBuffers()
{
    // Initialize the frame buffers and render textures
    glGenFramebuffers(1, &m_frameBuffer);
    glGenTextures(1, &m_renderedTexture);
    glGenRenderbuffers(1, &m_depthRenderBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    // Bind our texture so that all functions that deal with textures will interact with this one
    glBindTexture(GL_TEXTURE_2D, m_renderedTexture);
    // Give an empty image to OpenGL ( the last "0" )
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width() * this->devicePixelRatio(), this->height() * this->devicePixelRatio(), 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)0);

    // Set the render settings for the texture we've just created.
    // Essentially zero filtering on the "texture" so it appears exactly as rendered
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // Clamp the colors at the edge of our texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Initialize our depth buffer
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->width() * this->devicePixelRatio(), this->height() * this->devicePixelRatio());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderBuffer);

    // Set m_renderedTexture as the color output of our frame buffer
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_renderedTexture, 0);

    // Sets the color output of the fragment shader to be stored in GL_COLOR_ATTACHMENT0, which we previously set to m_renderedTextures[i]
    GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBuffers); // "1" is the size of drawBuffers

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Frame buffer did not initialize correctly..." << std::endl;
        printGLErrorLog();
    }
}

