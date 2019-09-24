//
// Created by willisdai on 2019/4/22.
//
#include "jni.h"
#include "android/log.h"
#include "ObjectiveGL.h"
#include <string>
#include "Platform/android/GLAndContext.h"

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

const string feedbackShader =
    string("#version 300 es\n") +
    SHADER_STRING (
        layout(location = 0) in int position;
        out int out1;
        void main() {
            out1 = position * 2;
        }
    );

const string feedbackGrayScaleShader =
    string("#version 300 es\n") +
    SHADER_STRING (
        layout(location = 0) in float color;
        const vec4 shift = vec4(16777216., 65536., 256., 1.);
        out float grayColor;
        void main() {
            grayColor = color;
        }
    );

static int _count = 0;
static long _cost0 = 0;
static long _cost1 = 0;

static shared_ptr<GLContext> context;
static shared_ptr<GLProgram> program;
static shared_ptr<GLProgram> grayProgram;
static shared_ptr<GLProgram> fastGrayProgram;
static shared_ptr<GLFrameBuffer> fbo;
static shared_ptr<GLFrameBuffer> fbo1;
static shared_ptr<GLFrameBuffer> fbo2;
static shared_ptr<GLBuffer> vboTri;
static shared_ptr<GLBuffer> vboRec;
static shared_ptr<GLBuffer> vboPt;
static shared_ptr<GLBuffer> vboFeedback;
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

static inline void drawInit(JNIEnv* env,
                            jobject pixel_buf) {
    context = GLAndContext::create();
    context->setCurrent();

    fbo = GLFrameBuffer::create(0);
    fbo1 = GLFrameBuffer::create(-1);
    fbo2 = GLFrameBuffer::create(-1);

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

static inline void feedbackInit() {
    vao = GLVertexArray::create();

    vboFeedback = GLBuffer::create();
    vboFeedback->alloc(sizeof(int), 4);
    vao->setBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, vboFeedback);

    int index[4] = {1, 3, 5, 7};
    vboPt = GLBuffer::create();
    vboPt->alloc(sizeof(int), 4, index);

    vao->setBuffer(GL_ARRAY_BUFFER, vboPt);
    vector<GLVertexArrayParams> params;
    params.emplace_back(GL_INT, 1);
    vao->setParams(params);

    vao->setDrawMode(GL_POINTS);

    program = GLProgram::create();
    program->setTransformFeedbackShader(feedbackShader, {"out1"});

//    opt.rasterizerDiscard = true;
}

static inline void feedbackDraw() {
    glFinish();
    vao->computeUsingTransformFeedback(program);
    glFinish();

//    int *feedback = static_cast<int *>(vboFeedback->lock(0, 0, GL_MAP_READ_BIT));
//    __android_log_print(ANDROID_LOG_WARN, "feedbackDraw", "feedback={%d, %d, %d, %d}", feedback[0], feedback[1], feedback[2], feedback[3]);
//    vboFeedback->unlock();

//    vao->setBuffer(GL_ARRAY_BUFFER, vboPt);
}

static GLsizei pixelCount = 12/*80 * 720 * 16*/;
static GLvoid *pixels = new float[pixelCount];

static inline void feedbackGrayScaleInit(JNIEnv *env, jobject pixel_buf) {
    vao = GLVertexArray::create();

//    GLvoid *pixels = env->GetDirectBufferAddress(pixel_buf);
//    __android_log_print(ANDROID_LOG_WARN, "feedbackGrayScaleInit", "first 1 color %u, %u %u, %u", ((unsigned char *)pixels)[0], ((unsigned char *)pixels)[1], ((unsigned char *)pixels)[2], ((unsigned char *)pixels)[3]);
//    jlong pixelCount = env->GetDirectBufferCapacity(pixel_buf) / sizeof(int);

    vboFeedback = GLBuffer::create();
    vboFeedback->alloc(sizeof(float), pixelCount);
//    vboFeedback->alloc(sizeof(int), 1);
    vao->setBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, vboFeedback);

    vboPt = GLBuffer::create();
    vboPt->alloc(sizeof(float), pixelCount, pixels, GL_STATIC_DRAW);

    vao->setBuffer(GL_ARRAY_BUFFER, vboPt);
    vector<GLVertexArrayParams> params;
    params.emplace_back(GL_FLOAT, 1);
    vao->setParams(params);

    vao->setDrawMode(GL_POINTS);

    program = GLProgram::create();
    program->setTransformFeedbackShader(feedbackGrayScaleShader, {"grayColor"});
}

static inline void feedbackGrayScaleDraw() {
    glFinish();
    auto now = ms();
    vao->computeUsingTransformFeedback(program);
    glFinish();
    _cost0 += ms() - now;

//    unsigned int *feedback = static_cast<unsigned int*>(vboFeedback->lock(0, 0, GL_MAP_READ_BIT));
//    __android_log_print(ANDROID_LOG_WARN, "feedbackDraw", "first 4 color %u, %u %u, %u", feedback[0], feedback[1], feedback[2], feedback[3]);
//    vboFeedback->unlock();

//    vao->setBuffer(GL_ARRAY_BUFFER, vboPt);
}

static inline void normalDraw() {
    glViewport(0, 0, 720, 1280);
//    grayProgram->setTexture("tex", texImage);
//    fbo1->draw(grayProgram, vao, opt);

//    auto now = ms();

//    glFinish();
    for (int i = 0; i < 50; ++i) {
        grayProgram->setTexture("tex", texImage);
        fbo1->setColorTexture(texImage1);
        fbo1->draw(grayProgram, vao, opt);

        grayProgram->setTexture("tex", texImage1);
        fbo1->setColorTexture(texImage);
        fbo1->draw(grayProgram, vao, opt);

        /*grayProgram->setTexture("tex", texImage1);
        fbo2->draw(grayProgram, vao, opt);

        grayProgram->setTexture("tex", texImage2);
        fbo1->draw(grayProgram, vao, opt);*/
    }
//    glFinish();

//    _cost0 += ms() - now;
}

static inline void fbDraw() {
//    auto now = ms();

    glViewport(0, 0, 720, 1280);
    grayProgram->setTexture("tex", texImage);
    fbo1->draw(grayProgram, vao, opt);

//    glFinish();
    for (int i = 0; i < 100; ++i) {
        fbo1->draw(fastGrayProgram, vao, opt);
    }
//    glFinish();

//    _cost1 += ms() - now;
}

extern "C" JNIEXPORT void JNICALL
Java_com_objectivegl_MainActivity_runTest(
        JNIEnv* env,
        jobject jobj,
        jint c) {

    context->setCurrent();

    if (c == 0) normalDraw();
    else if (c == 1) fbDraw();
    else if (c == 2) feedbackDraw();
    else if (c == 3) feedbackGrayScaleDraw();

    /*++_count;
    if (_count % 200 == 0) {
        __android_log_print(ANDROID_LOG_WARN, "drawTest", "cost0=%f, cost1=%f", (float)_cost0 /_count, (float)_cost1 /_count);
        _cost0 = 0;
        _cost1 = 0;
        _count = 0;
    }*/
}

extern "C" JNIEXPORT void JNICALL
Java_com_objectivegl_MainActivity_init (
    JNIEnv* env,
    jobject jobj,
    jobject pixel_buf,
    jint c) {

    if (c == 0) drawInit(env, pixel_buf);
    else if (c == 1) feedbackInit();
    else if (c == 2) feedbackGrayScaleInit(env, pixel_buf);
}
