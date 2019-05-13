//
//  GLObject.h
//  ObjectiveGL
//
//  Created by lingtonke on 2018/10/11.
//  Copyright © 2018 tencent. All rights reserved.
//
#pragma once

#include "GLContext.h"
#include "GLPlatform.h"
#include <memory>

namespace ObjectiveGL {
using namespace std;

class GLObject {
public:
    shared_ptr<GLContext> context;
protected:
    GLObject() : context(shared_ptr<GLContext>(GLContext::current())) {

    }

    virtual ~GLObject() {

    }

public:

    void check() {
        context->check(false);
    }

    void checkError() {
        auto error = OGL(glGetError());
        if (error) {
            throw GLError(ObjectiveGLError_GLError, error);
        }
    }

};

class GLShareObject : public GLObject {
public:
    void check() {
        context->check(true);
    }
};
}
