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

    function<void()> stencilOperations;
    void use() const {
        if (enableBlend) {
            GLCHECK(glEnable(GL_BLEND));
            GLCHECK(glBlendFunc(blendSrcFactor, blendDstFactor));
        }
        else {
            GLCHECK(glDisable(GL_BLEND));
        }
        if (enableDepthTest) {
            GLCHECK(glEnable(GL_DEPTH_TEST));
        }
        else {
            GLCHECK(glDisable(GL_DEPTH_TEST));
        }
        if (enableStencilTest) {
            GLCHECK(glEnable(GL_STENCIL_TEST));
            stencilOperations();
        }
        else {
            GLCHECK(glDisable(GL_STENCIL_TEST));
        }

        if (enableScissorTest) {
            glEnable(GL_SCISSOR_TEST);
            glScissor(scissorBox[0], scissorBox[1], scissorBox[2], scissorBox[3]);
        }
    }
};

class GLDrawOptionSaver : public GLDrawOption {
protected:
    void save() {

        GLboolean enable;
        glGetBooleanv(GL_SCISSOR_TEST, &enable);
        enableScissorTest = enable;

        glGetIntegerv(GL_SCISSOR_BOX, scissorBox);
    }

    void restore() {

        if (enableScissorTest) {
            glEnable(GL_SCISSOR_TEST);

        } else {
            glDisable(GL_SCISSOR_TEST);
        }

        glScissor(scissorBox[0], scissorBox[1], scissorBox[2], scissorBox[3]);
    }
public:
    GLDrawOptionSaver() {
        save();
    }
    ~GLDrawOptionSaver() {
        restore();
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
            GLCHECK(glGenFramebuffers(1, &frameBufferID));
            isBackend = false;
        }
        else {
            frameBufferID = backendFrameBuffer;
            isBackend = true;
        }
    }

    ~GLFrameBuffer() {
        if (!isBackend) {
            check();
            GLCHECK(glDeleteBuffers(1, &frameBufferID));
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
        GLCHECK(glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID));
        vector<GLenum> bufs;
        for (int i = 0; i < (int)textures.size(); i++) {
            bufs.push_back(GL_COLOR_ATTACHMENT0 + i);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D,
                                   textures[i]->textureID, 0);
        }
        GLCHECK(glDrawBuffers((GLsizei) textures.size(), bufs.data()));
    }
#endif

    void setColorTexture(shared_ptr<GLTexture> texture) {
#ifdef ES3
        vector<shared_ptr<GLTexture>> vec;
        vec.push_back(texture);
        setColorTextures(vec);
#else
        GLCHECK(glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID));
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                               texture->textureID, 0);
#endif
    }

    void setRenderBuffer(shared_ptr<GLRenderBuffer> renderBuffer) {
        GLCHECK(glBindBuffer(GL_FRAMEBUFFER, frameBufferID));
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER,
                                  renderBuffer->renderBufferID);
        GLenum buf = GL_COLOR_ATTACHMENT0;
#ifdef ES3
        GLCHECK(glDrawBuffers(1, &buf));
#endif
    }

    void draw(shared_ptr<GLProgram> program, shared_ptr<GLVertexArray> vao, const GLDrawOption &option) {
        check();
        GLCHECK(glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID));
        check();
        program->use();
        GLDrawOptionSaver saver;
        option.use();
        vao->draw(program);
    }
    
    void clearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
    {
        check();
        GLCHECK(glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID));
        
        GLCHECK(glClearColor(red, green, blue, alpha));
        GLCHECK(glClear(GL_COLOR_BUFFER_BIT));
        
    }
    
    void clearStencil(GLint s)
    {
        check();

        GLint old;
        GLCHECK(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &old));

        GLCHECK(glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID));
        

        GLCHECK(glClearStencil(s));
        

        GLCHECK(glClear(GL_STENCIL_BUFFER_BIT));
        

        GLCHECK(glBindFramebuffer(GL_FRAMEBUFFER, old));
        
    }

};
}
