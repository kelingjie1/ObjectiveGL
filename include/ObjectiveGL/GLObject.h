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

class OGL_API GLObject {
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

    static void checkError() {
        auto error = OGL(glGetError());
        if (error) {
            OGLTHROW(GLError(ObjectiveGLError_GLError, error));
        }
    }

};

class OGL_API GLShareObject : public GLObject {
public:
    void check() {
        context->check(true);
    }
};
}
