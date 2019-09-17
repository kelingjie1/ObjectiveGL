//
//  GLRenderBuffer.hpp
//  ObjectiveGL
//
//  Created by lingtonke on 2018/10/8.
//  Copyright © 2018 tencent. All rights reserved.
//

#pragma once

#include "GLPlatform.h"
#include "GLObject.h"
#include "GLContext.h"
#include <memory>

OGL_NAMESPACE_BEGIN(ObjectiveGL)
using namespace std;

class OGL_API GLRenderBuffer : public GLShareObject {
    friend class GLContext;

protected:
    GLRenderBuffer(GLuint backendRenderBuffer) {
        if (backendRenderBuffer>=0) {
            renderBufferID = backendRenderBuffer;
        }
        else {
            GLCHECK(glGenRenderbuffers(1, &renderBufferID));
        }
        
    }

public:
    GLuint renderBufferID;
    int width = 0;
    int height = 0;
    static shared_ptr<GLRenderBuffer> create(int backendRenderBuffer = -1, function<void(GLRenderBuffer *renderbuffer)> deleter = nullptr) {
        if (deleter) {
            return shared_ptr<GLRenderBuffer>(new GLRenderBuffer(backendRenderBuffer),deleter);
        }
        else {
            return shared_ptr<GLRenderBuffer>(new GLRenderBuffer(backendRenderBuffer));
        }
        
    }
    
    void bind() {
        GLCHECK(glBindRenderbuffer(GL_RENDERBUFFER, renderBufferID));
    }

    ~GLRenderBuffer() {
        check();
        GLCHECK(glDeleteRenderbuffers(1, &renderBufferID));
    }
};
OGL_NAMESPACE_END(ObjectiveGL)
