//
//  GLError.h
//  Starfall
//
//  Created by lingtonke on 2019/3/20.
//  Copyright © 2019 Starfall. All rights reserved.
//

#pragma once

#include <exception>

namespace ObjectiveGL {
    using namespace std;
    enum ObjectiveGLError {
        ObjectiveGLError_ContextCheckFailed,
        ObjectiveGLError_GLError,
        ObjectiveGLError_InvalidType,
        ObjectiveGLError_VertexShaderCompileFailed,
        ObjectiveGLError_FragmentShaderCompileFailed,
        ObjectiveGLError_ProgramLinkFailed,
        ObjectiveGLError_InvalidData,
    };
    
    class GLError : exception {
    public:
        ObjectiveGLError code;
        GLenum glcode;
        string log;
        GLError(ObjectiveGLError code,GLenum glcode = GL_NO_ERROR) noexcept {this->code = code;this->glcode = glcode;};
        GLError(ObjectiveGLError code,string log) noexcept {this->code = code;this->log = log;};
        
        virtual const char* what() const noexcept {return log.c_str();}
    };
}
