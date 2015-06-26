#ifndef EDVSRIFTAPP_H
#define EDVSRIFTAPP_H

#include "Common.h"
#include "EdvsImage.hpp"

#include <chrono>
#include <glm/gtc/matrix_transform.hpp>
#include <oglplus/shapes/obj_mesh.hpp>
#include <oglplus/opt/resources.hpp>


struct MeshInputFile
{
    std::ifstream stream;

    MeshInputFile(void)
    {
        oglplus::OpenResourceFile(stream, "resources", "spherical_calotte", ".obj");
    }
};


//class EdvsRiftApp : public RiftApp
class EdvsRiftApp : public GlfwApp
{
public:
    EdvsRiftApp(EdvsImage (*images)[7])
        : images_(images)
    {

    }

    void initGl();
    void update();
    void renderScene();
    void onKey(int key, int scancode, int action, int mods);

private:
    float projection_scale = 1.0f;
    float trans = 0.0f;
    float rotation = 0.0f;

    EdvsImage (*images_)[7];

    void drawSphere(int camera);

    MeshInputFile mesh_input;
    BufferPtr vbo;
};


#endif // EDVSRIFTAPP_H
