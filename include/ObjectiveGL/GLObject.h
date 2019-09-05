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
#if OGL_CONTEXT_CHECK
        context->check(false);
#endif
    }

};

class OGL_API GLShareObject : public GLObject {
public:
    void check() {
#if OGL_CONTEXT_CHECK
        context->check(true);
#endif
    }
};
}
