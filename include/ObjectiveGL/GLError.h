//
//  GLError.h
//  Starfall
//
//  Created by lingtonke on 2019/3/20.
//  Copyright © 2019 Starfall. All rights reserved.
//

#pragma once

#include "GLPlatform.h"
#include <exception>
#include <string>
#include <assert.h>

#ifndef _LIBCPP_NO_EXCEPTIONS

#define OGLTHROW(x) assert(!#x)

#else

#define OGLTHROW(x) throw x

#endif

namespace ObjectiveGL {
using namespace std;
enum ObjectiveGLError {
    ObjectiveGLError_ContextCheckFailed,
    ObjectiveGLError_GLError,
    ObjectiveGLError_InvalidType,
    ObjectiveGLError_ShaderCompileFailed,
    ObjectiveGLError_ProgramLinkFailed,
    ObjectiveGLError_InvalidData,
    ObjectiveGLError_LocationNotFound,
};

class OGL_API GLError : exception {
public:
    ObjectiveGLError code;
    GLenum glcode;
    string log;

    GLError(ObjectiveGLError code, GLenum glcode = GL_NO_ERROR) noexcept {
        this->code = code;
        this->glcode = glcode;
    };

    GLError(ObjectiveGLError code, string log) noexcept {
        this->code = code;
        this->log = log;
    };

    virtual const char *what() const noexcept { return log.c_str(); }
};
}
