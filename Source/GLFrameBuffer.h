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
#include <memory>
#include <vector>
#include <map>

namespace ObjectiveGL {
using namespace std;

class GLFrameBuffer : public GLShareObject {
    friend class GLContext;

protected:
    bool isBackend;
    GLFrameBuffer(int backendFrameBuffer) {
        if (backendFrameBuffer<0) {
            glGenBuffers(1, &frameBufferID);
            isBackend = false;
        }
        else {
            frameBufferID = backendFrameBuffer;
            isBackend = true;
        }
    }

public:
    GLuint frameBufferID;

    ~GLFrameBuffer() {
        if (!isBackend) {
            context->check();
            glDeleteBuffers(1, &frameBufferID);
        }
        
    }
    
    static shared_ptr<GLFrameBuffer> create(int backendFrameBuffer) {
        return shared_ptr<GLFrameBuffer>(new GLFrameBuffer(backendFrameBuffer));
    }

    void setColorTextures(vector<shared_ptr<GLTexture> > textures) {
        glBindBuffer(GL_FRAMEBUFFER, frameBufferID);
        vector<GLenum> bufs;
        for (int i = 0; i < textures.size(); i++) {
            bufs.push_back(GL_COLOR_ATTACHMENT0 + i);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D,
                                   textures[i]->textureID, 0);
        }
        glDrawBuffers((GLsizei) textures.size(), bufs.data());
    }

    void setColorTexture(shared_ptr<GLTexture> texture) {
        vector<shared_ptr<GLTexture>> vec;
        vec.push_back(texture);
        setColorTextures(vec);
    }

    void setRenderBuffer(shared_ptr<GLRenderBuffer> renderBuffer) {
        glBindBuffer(GL_FRAMEBUFFER, frameBufferID);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER,
                                  renderBuffer->renderBufferID);
        GLenum buf = GL_COLOR_ATTACHMENT0;
        glDrawBuffers(1, &buf);
    }

    template<class vboType, class eboType>
    void draw(shared_ptr<GLProgram> program, GLVertexArray<vboType, eboType> vao, GLsizei count = 0) {
        check();
        glBindBuffer(GL_FRAMEBUFFER, frameBufferID);
        program->use();
        vao->draw(count);
    }
    
    void clear(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
    {
        check();
        glBindBuffer(GL_FRAMEBUFFER, frameBufferID);
        glClearColor(red, green, blue, alpha);
        glClear(GL_COLOR_BUFFER_BIT);
    }

};
}
