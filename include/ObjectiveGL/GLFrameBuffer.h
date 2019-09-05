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
    bool enableBlend = false;
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

class GLSaver {
protected:
    bool saved = false;
    bool restored = false;
    virtual void doSave() {
        
    }
    virtual void doRestore() {
        
    }
public:
    virtual void save() {
        if (saved) {
            OGL_ERROR(ObjectiveGLError_StateAlreadyRestored);
        }
        doSave();
    }
    
    virtual void restore() {
        if (restored) {
            OGL_ERROR(ObjectiveGLError_StateAlreadyRestored);
        }
        doRestore();
    }
    
    GLSaver(bool saveNow = true) {
        if (saveNow) {
            save();
        }
    }
    virtual ~GLSaver() {
        if (!restored) {
            restore();
        }
    }
};

class GLDrawOptionSaver : public GLDrawOption,GLSaver {
public:
    virtual void doSave() {
        GLboolean enable;
        glGetBooleanv(GL_SCISSOR_TEST, &enable);
        enableScissorTest = enable;

        glGetIntegerv(GL_SCISSOR_BOX, scissorBox);
    }

    virtual void doRestore() {
        if (enableScissorTest) {
            glEnable(GL_SCISSOR_TEST);

        } else {
            glDisable(GL_SCISSOR_TEST);
        }

        glScissor(scissorBox[0], scissorBox[1], scissorBox[2], scissorBox[3]);
    }

    GLDrawOptionSaver(bool saveNow = true) : GLSaver(saveNow) {}
    virtual ~GLDrawOptionSaver() {}
};
    
class GLFrameBufferSaver : public GLSaver {
protected:
    GLint oldFrameBuffer;
public:
    virtual void doSave() {
        GLCHECK(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFrameBuffer));
    }
    
    virtual void doRestore() {
        GLCHECK(glBindFramebuffer(GL_FRAMEBUFFER, oldFrameBuffer));
    }
};

    
class OGL_API GLFrameBuffer : public GLShareObject {
    friend class GLContext;

protected:
    bool isBackend;
    
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

public:
    vector<shared_ptr<GLTexture>> colorTextures;
    GLuint frameBufferID;
    ~GLFrameBuffer() {
        if (!isBackend) {
            check();
            GLCHECK(glDeleteBuffers(1, &frameBufferID));
        }
        
    }
    
    static shared_ptr<GLFrameBuffer> create(int backendFrameBuffer = -1, function<void(GLFrameBuffer *framebuffer)> deleter = nullptr) {
        if (deleter) {
            return shared_ptr<GLFrameBuffer>(new GLFrameBuffer(backendFrameBuffer),deleter);
        }
        else {
            return shared_ptr<GLFrameBuffer>(new GLFrameBuffer(backendFrameBuffer));
        }
    }
    
    static shared_ptr<GLFrameBuffer> createWithCurrent() {
        GLint backendFrameBuffer = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &backendFrameBuffer);
        return shared_ptr<GLFrameBuffer>(new GLFrameBuffer(backendFrameBuffer));
    }
    
#ifdef ES3
    void setColorTextures(vector<shared_ptr<GLTexture> > textures) {
        colorTextures = textures;
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
        textures.clear();
        textures.push_back(texture);
        GLCHECK(glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID));
        GLCHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                       texture->textureID, 0));
#endif
    }

    void setRenderBuffer(shared_ptr<GLRenderBuffer> renderBuffer) {
        colorTextures.clear();
        GLFrameBufferSaver saver;
        GLCHECK(glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID));
        GLCHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderBuffer->renderBufferID));
#ifdef ES3
        GLenum buf = GL_COLOR_ATTACHMENT0;
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
    
    void clearColor(GLfloat red = 0, GLfloat green = 0, GLfloat blue = 0, GLfloat alpha = 1)
    {
        check();
        GLFrameBufferSaver saver;
        GLCHECK(glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID));
        GLCHECK(glClearColor(red, green, blue, alpha));
        GLCHECK(glClear(GL_COLOR_BUFFER_BIT));
    }
    
    void clearDepth(GLclampf depth = 1)
    {
        check();
        GLFrameBufferSaver saver;
        GLCHECK(glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID));
        GLCHECK(glClearDepthf(depth));
        GLCHECK(glClear(GL_DEPTH_BUFFER_BIT));
    }
    
    void clearStencil(GLint s = 0)
    {
        check();
        GLFrameBufferSaver saver;
        GLCHECK(glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID));
        GLCHECK(glClearStencil(s));
        GLCHECK(glClear(GL_STENCIL_BUFFER_BIT));
    }
    
    shared_ptr<GLTexture> getTexture() {
        if (colorTextures.size()>0) {
            return colorTextures[0];
        }
        else {
            GLFrameBufferSaver saver;
            GLCHECK(glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID));
            GLint viewport[4];
            GLCHECK(glGetIntegerv(GL_VIEWPORT, viewport));
            auto pbo = GLBuffer::create();
            pbo->alloc(sizeof(char)*4, viewport[2]*viewport[3]);
            pbo->bind(GL_PIXEL_PACK_BUFFER);
            GLCHECK(glReadPixels(viewport[0], viewport[1], viewport[2], viewport[3], GL_RGBA, GL_UNSIGNED_BYTE, 0));
            pbo->unbind(GL_PIXEL_PACK_BUFFER);
            
            
            pbo->bind(GL_PIXEL_UNPACK_BUFFER);
            auto texture = GLTexture::create();
            texture->setImageData(0, viewport[2], viewport[3]);
            pbo->unbind(GL_PIXEL_UNPACK_BUFFER);
            return texture;
        }
    }

};
}
