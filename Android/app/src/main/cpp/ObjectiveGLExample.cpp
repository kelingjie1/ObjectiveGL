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
        layout(location = 0) in vec2 position;
        layout(location = 1) in vec2 texCoords;
        out vec2 uv;
        void main() {
            gl_Position = vec4(position.x, position.y, 1.0, 1.0);
            uv = texCoords;
        }
    );

const string fragShader =
    string("#version 300 es\n") +
    SHADER_STRING (
        in vec2 uv;
        out vec4 color;
        uniform sampler2D tex;
        void main() {
            color = texture(tex, uv);
        }
    );

const string grayFragShader =
    string("#version 300 es\n") +
    SHADER_STRING (
        in highp vec2 uv;
        out vec4 color;
        uniform sampler2D tex;
        const highp float alpha = 1.0;
        const highp vec3 grayWeight = vec3(0.299, 0.587, 0.114);

        void main() {
            highp vec4 textureColor = texture(tex, uv);
            highp float luminance = dot(textureColor.rgb, grayWeight);
            highp vec4 grayTextureColor = vec4(vec3(luminance), textureColor.a);
            color = vec4(mix(textureColor.rgb, grayTextureColor.rgb, alpha), textureColor.a);
        }
    );

const string fastGrayFragShader =
    string("#version 300 es\n") +
    string("#extension GL_EXT_shader_framebuffer_fetch : require\n") +
    SHADER_STRING (
        inout vec4 color;
        const highp float alpha = 1.0;
        const highp vec3 grayWeight = vec3(0.299, 0.587, 0.114);

        void main() {
            highp vec4 textureColor = color;
            highp float luminance = dot(textureColor.rgb, grayWeight);
            highp vec4 grayTextureColor = vec4(vec3(luminance), textureColor.a);
            color = vec4(mix(textureColor.rgb, grayTextureColor.rgb, alpha), textureColor.a);
        }
    );

static shared_ptr<GLContext> context;
static shared_ptr<GLProgram> program;
static shared_ptr<GLProgram> grayProgram;
static shared_ptr<GLProgram> fastGrayProgram;
static shared_ptr<GLFrameBuffer> fbo;
static shared_ptr<GLFrameBuffer> fbo1;
static shared_ptr<GLFrameBuffer> fbo2;
static shared_ptr<GLBuffer> vboTri;
static shared_ptr<GLBuffer> vboRec;
static shared_ptr<GLVertexArray> vao;
static shared_ptr<GLTexture> texImage;
static shared_ptr<GLTexture> texImage1;
static shared_ptr<GLTexture> texImage2;
static GLDrawOption opt;

typedef struct {
    GLfloat x;
    GLfloat y;
    GLfloat u;
    GLfloat v;

} Vertex;

extern "C" JNIEXPORT void JNICALL
Java_com_objectivegl_MainActivity_init (
    JNIEnv* env,
    jobject jobj,
    jobject pixel_buf) {

    context = GLContext::create();
    context->setCurrent();

    fbo = GLFrameBuffer::create(0);
    fbo1 = GLFrameBuffer::create(-1);
    fbo2 = GLFrameBuffer::create(-1);

    Vertex tri[] = {{0.0f, 0.5f, 0.0, 0.0}, {0.5f, 0.0, 0.0, 1.0}, {-0.5f, 0.0, 1.0, 0.0}};
    vboTri = GLBuffer::create();
    vboTri->alloc(sizeof(Vertex), 3, tri, GL_STATIC_DRAW);

    Vertex rec[] = {
        {-1.0f, -1.0f, 0.0f, 0.0f},
        {-1.0f, 1.0, 0.0f, 1.0f},
        {1.0f, -1.0, 1.0, 0.0},


        {-1.0f, 1.0, 0.0, 1.0},
        {1.0f, -1.0, 1.0, 0.0},
        {1.0f, 1.0, 1.0, 1.0}
    };
    vboRec = GLBuffer::create();
    vboRec->alloc(sizeof(Vertex), 6, rec, GL_STATIC_DRAW);

    vao = GLVertexArray::create();
    vao->setDrawMode(GL_TRIANGLES);

    vector<GLVertexArrayParams> params;
    params.emplace_back(GL_FLOAT, 2);
    params.emplace_back(GL_FLOAT, 2);

    vao->setBuffer(GL_ARRAY_BUFFER, vboRec);
    vao->setParams(params);

    texImage = GLTexture::create();
    GLvoid *pixels = env->GetDirectBufferAddress(pixel_buf);
    texImage->setImageData(pixels, 720, 1280);

    texImage1 = GLTexture::create();
    texImage1->setImageData(pixels, 720, 1280);
    fbo1->setColorTexture(texImage1);

    texImage2 = GLTexture::create();
    texImage2->setImageData(pixels, 720, 1280);
    fbo2->setColorTexture(texImage2);

    program = GLProgram::create();
    program->setRenderShader(vertexShader, fragShader);

    grayProgram = GLProgram::create();
    grayProgram->setRenderShader(vertexShader, grayFragShader);

    fastGrayProgram = GLProgram::create();
    fastGrayProgram->setRenderShader(vertexShader, fastGrayFragShader);
//    program->setTexture("tex", texImage);

    opt.enableBlend = false;
}

static inline long ms(void) {
    struct timespec res;
    clock_gettime(CLOCK_BOOTTIME, &res);
    return static_cast<long>(1000 * res.tv_sec + res.tv_nsec / 1e6);
}

//static int _count = 0;
//static long _cost0 = 0;
//static long _cost1 = 0;

void normalDraw() {
    glFinish();

//    auto now = ms();

    glViewport(0, 0, 720, 1280);
    grayProgram->setTexture("tex", texImage);
    fbo1->draw(grayProgram, vao, opt);

    grayProgram->setTexture("tex", texImage1);
    fbo2->draw(grayProgram, vao, opt);

    grayProgram->setTexture("tex", texImage2);
    fbo1->draw(grayProgram, vao, opt);

    grayProgram->setTexture("tex", texImage1);
    fbo2->draw(grayProgram, vao, opt);

    grayProgram->setTexture("tex", texImage2);
    fbo1->draw(grayProgram, vao, opt);

    grayProgram->setTexture("tex", texImage1);
    fbo2->draw(grayProgram, vao, opt);

    grayProgram->setTexture("tex", texImage2);
    fbo2->draw(grayProgram, vao, opt);

    grayProgram->setTexture("tex", texImage2);
    fbo1->draw(grayProgram, vao, opt);

    grayProgram->setTexture("tex", texImage1);
    fbo2->draw(grayProgram, vao, opt);

    grayProgram->setTexture("tex", texImage2);
    fbo1->draw(grayProgram, vao, opt);

    glFinish();

//    _cost0 += ms() - now;
}

void fbDraw() {
    glFinish();

//    auto now = ms();

    glViewport(0, 0, 720, 1280);
    grayProgram->setTexture("tex", texImage);
    fbo1->draw(grayProgram, vao, opt);

    fbo1->draw(fastGrayProgram, vao, opt);
    fbo1->draw(fastGrayProgram, vao, opt);
    fbo1->draw(fastGrayProgram, vao, opt);
    fbo1->draw(fastGrayProgram, vao, opt);
    fbo1->draw(fastGrayProgram, vao, opt);
    fbo1->draw(fastGrayProgram, vao, opt);
    fbo1->draw(fastGrayProgram, vao, opt);
    fbo1->draw(fastGrayProgram, vao, opt);
    fbo1->draw(fastGrayProgram, vao, opt);

    glFinish();

//    _cost1 += ms() - now;
}

extern "C" JNIEXPORT void JNICALL
Java_com_objectivegl_MainActivity_runTest(
        JNIEnv* env,
        jobject jobj,
        jint c) {

    if (c == 0) normalDraw();
    else if  (c == 1) fbDraw();
    /*fbDraw();
    normalDraw();

    ++_count;
    if (_count % 200 == 0) {
        __android_log_print(ANDROID_LOG_WARN, "drawTest", "cost0=%f, cost1=%f", (float)_cost0 /_count, (float)_cost1 /_count);
        _cost0 = 0;
        _cost1 = 0;
        _count = 0;
    }

    glViewport(0, 0, 1440, 2112);
    grayProgram->setTexture("tex", texImage1);
    fbo->draw(grayProgram, vao, opt);*/
}
