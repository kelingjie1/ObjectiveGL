//
// Created by willisdai on 2019/4/22.
//
#include "jni.h"
#include "android/log.h"
#include "ObjectiveGL.h"
#include <string>

using namespace ObjectiveGL;
using namespace std;

const string vertexShader =
    string("#version 300 es\n") +
    SHADER_STRING (
        layout(location = 0) in vec4 aPosition;
        void main() {
            gl_Position = aPosition;
        }
    );

const string fragShader =
    string("#version 300 es\n") +
    SHADER_STRING (
        precision mediump float;
        out vec4 color;
        void main() {
            color = vec4(1.0, 1.0, 0.0, 1.0);
        }
    );

static shared_ptr<GLContext> context;
static shared_ptr<GLProgram> program;
static shared_ptr<GLFrameBuffer> fbo;
static shared_ptr<GLBuffer> vbo;
static shared_ptr<GLVertexArray> vao;
static GLDrawOption opt;

extern "C" JNIEXPORT void JNICALL
Java_com_objectivegl_MainActivity_init (
    JNIEnv* env,
    jobject /* this */) {

    context = GLContext::create();
    context->setCurrent();

    program = GLProgram::create();
    program->setRenderShader(vertexShader, fragShader);

    fbo = GLFrameBuffer::create(0);

    GLfloat vertex[] = {0.0, 0.5f, 0.5f, 0.0, -0.5f, 0.0};
    vbo = GLBuffer::create();
    vbo->alloc(2 * sizeof(GLfloat), 3, vertex, GL_STATIC_DRAW);

    vector<GLVertexArrayParams> params;
    params.emplace_back(GL_FLOAT, 2);

    vao = GLVertexArray::create();
    vao->setDrawMode(GL_TRIANGLES);
    vao->setBuffer(GL_ARRAY_BUFFER, vbo);
    vao->setParams(params);

    opt.enableBlend = false;
}

extern "C" JNIEXPORT void JNICALL
Java_com_objectivegl_MainActivity_runTest(
        JNIEnv* env,
        jobject /* this */) {

    fbo->clear(1.0f, 0.0f, 0.0f, 0.0f);
    fbo->draw(program, vao, opt);
}
