//
//  GLContext.h
//  ObjectiveGL
//
//  Created by lingtonke on 2018/10/9.
//  Copyright © 2018 tencent. All rights reserved.
//
#pragma once

#include "GLPlatform.h"
#include "GLError.h"
#include <iostream>

namespace ObjectiveGL {
using namespace std;

class GLTexture;

class GLFrameBuffer;

class GLRenderBuffer;

class GLProgram;

class GLBaseVertex;

class GLVertexArray;

class GLBuffer;

class GLShareGroup {
    friend class GLPlatform;
protected:
    GLShareGroup(){};
public:
    void *sharegroup;
    static shared_ptr<GLShareGroup> create() {
        return GLPlatform::createShareGroup();
    }
};

class GLContext;

class GLContext : public enable_shared_from_this<GLContext> {

    static shared_ptr<GLContext> &currentContext() {
        static thread_local shared_ptr<GLContext> context;
        return context;
    }

    GLContext(shared_ptr<GLShareGroup> sharegroup) {
        context = GLPlatform::createContext(sharegroup.get());
    }

public:
    shared_ptr<GLShareGroup> sharegroup;
    void *context;
    
    ~GLContext() {
        GLPlatform::releaseContext(this->context);
    }

    static shared_ptr<GLContext> current() {
        return currentContext();
    }

    static shared_ptr<GLContext> create(shared_ptr<GLShareGroup> sharegroup = nullptr) {
        auto context = shared_ptr<GLContext>(new GLContext(sharegroup));
        return context;
    }

    void setCurrent() {
        auto s = shared_from_this();
        GLPlatform::setContext(this->context);
        currentContext() = s;
    }

    void check(bool share = false) {
        bool failed = false;
        if (share && share && sharegroup && GLContext::current()->sharegroup == sharegroup) {
            //sharegroup
        } else if (GLContext::current().get() != this) {
            failed = true;
        }
        
        if (failed) {
            throw GLError(ObjectiveGLError_ContextCheckFailed);
        }
    }
};
}
