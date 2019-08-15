//
//  GLFrameBuffer.h
//  ObjectiveGL
//
//  Created by lingtonke on 2018/10/8.
//  Copyright © 2018 tencent. All rights reserved.
//

#pragma once

#include "GLTexture.h"
#include "GLRenderBuffer.h"
#include "GLObject.h"
#include "GLProgram.h"
#include "GLVertexArray.h"
#include <memory>
#include <vector>
#include <map>
#include <functional>

namespace ObjectiveGL {
using namespace std;
    
    
class OGL_API GLDrawOption {
public:
    GLDrawOption(){}
    bool enableBlend = true;
    GLenum blendSrcFactor = GL_SRC_ALPHA;
    GLenum blendDstFactor = GL_ONE_MINUS_SRC_ALPHA;
    
    bool enableDepthTest = false;
    bool enableStencilTest = false;

    bool enableScissorTest = false;
    GLint scissorBox[4];

    shared_ptr<GLDrawOption> savedOpt;

    function<void()> stencilOperations;
    void use() const {
        if (enableBlend) {
            OGL(glEnable(GL_BLEND));
            OGL(glBlendFunc(blendSrcFactor, blendDstFactor));
        }
        else {
            OGL(glDisable(GL_BLEND));
        }
        if (enableDepthTest) {
            OGL(glEnable(GL_DEPTH_TEST));
        }
        else {
            OGL(glDisable(GL_DEPTH_TEST));
        }
        if (enableStencilTest) {
            OGL(glEnable(GL_STENCIL_TEST));
            stencilOperations();
        }
        else {
            OGL(glDisable(GL_STENCIL_TEST));
        }

        if (enableScissorTest) {
            glEnable(GL_SCISSOR_TEST);
            glScissor(scissorBox[0], scissorBox[1], scissorBox[2], scissorBox[3]);
        }
    }

    void save() {
        savedOpt = make_shared<GLDrawOption>();

        GLboolean enable;
        glGetBooleanv(GL_SCISSOR_TEST, &enable);
        savedOpt->enableScissorTest = enable;

        glGetIntegerv(GL_SCISSOR_BOX, &(savedOpt->scissorBox[0]));
    }

    void restore() {
        if (savedOpt == nullptr) return;

        if (savedOpt->enableScissorTest) {
            glEnable(GL_SCISSOR_TEST);

        } else {
            glDisable(GL_SCISSOR_TEST);
        }

        glScissor(savedOpt->scissorBox[0], savedOpt->scissorBox[1], savedOpt->scissorBox[2], savedOpt->scissorBox[3]);

        savedOpt = nullptr;
    }
};

class OGL_API GLFrameBuffer : public GLShareObject {
    friend class GLContext;

protected:
    bool isBackend;

public:
    GLuint frameBufferID;

    GLFrameBuffer(int backendFrameBuffer) {
        if (backendFrameBuffer<0) {
            OGL(glGenBuffers(1, &frameBufferID));
            isBackend = false;
        }
        else {
            frameBufferID = backendFrameBuffer;
            isBackend = true;
        }
    }

    ~GLFrameBuffer() {
        if (!isBackend) {
            context->check();
            OGL(glDeleteBuffers(1, &frameBufferID));
        }
        
    }
    
    static shared_ptr<GLFrameBuffer> create(int backendFrameBuffer = -1) {
        return shared_ptr<GLFrameBuffer>(new GLFrameBuffer(backendFrameBuffer));
    }
    
    static shared_ptr<GLFrameBuffer> createWithCurrent() {
        GLint backendFrameBuffer = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &backendFrameBuffer);
        return shared_ptr<GLFrameBuffer>(new GLFrameBuffer(backendFrameBuffer));
    }
    
#ifdef ES3
    void setColorTextures(vector<shared_ptr<GLTexture> > textures) {
        OGL(glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID));
        vector<GLenum> bufs;
        for (int i = 0; i < (int)textures.size(); i++) {
            bufs.push_back(GL_COLOR_ATTACHMENT0 + i);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D,
                                   textures[i]->textureID, 0);
        }
        OGL(glDrawBuffers((GLsizei) textures.size(), bufs.data()));
    }
#endif

    void setColorTexture(shared_ptr<GLTexture> texture) {
#ifdef ES3
        vector<shared_ptr<GLTexture>> vec;
        vec.push_back(texture);
        setColorTextures(vec);
#else
        OGL(glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID));
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                               texture->textureID, 0);
#endif
    }

    void setRenderBuffer(shared_ptr<GLRenderBuffer> renderBuffer) {
        OGL(glBindBuffer(GL_FRAMEBUFFER, frameBufferID));
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER,
                                  renderBuffer->renderBufferID);
        GLenum buf = GL_COLOR_ATTACHMENT0;
#ifdef ES3
        OGL(glDrawBuffers(1, &buf));
#endif
    }

    void draw(shared_ptr<GLProgram> program, shared_ptr<GLVertexArray> vao, GLDrawOption &option) {
        check();
        OGL(glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID));
        check();
        program->use();
        option.save();
        option.use();
        vao->draw(program);
        option.restore();
    }
    
    void clearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
    {
        check();
        OGL(glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID));
        checkError();
        OGL(glClearColor(red, green, blue, alpha));
        OGL(glClear(GL_COLOR_BUFFER_BIT));
        checkError();
    }
    
    void clearStencil(GLint s)
    {
        check();
        OGL(glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID));
        checkError();
        OGL(glClearStencil(s));
        OGL(glClear(GL_STENCIL_BUFFER_BIT));
        checkError();
    }

};
}
