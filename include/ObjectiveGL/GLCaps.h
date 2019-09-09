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

OGL_NAMESPACE_BEGIN(ObjectiveGL)
using namespace std;

class OGL_API GLCaps {
#ifdef GLES3
    static GLint MAJOR_VERSION() {
        GLint value;
        GLCHECK(glGetIntegerv(GL_MAJOR_VERSION, &value));
        return value;
    }
#endif
};
OGL_NAMESPACE_END(ObjectiveGL)
