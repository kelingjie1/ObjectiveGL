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

namespace ObjectiveGL {
using namespace std;

class GLRenderBuffer : public GLShareObject {
    friend class GLContext;

protected:
    GLRenderBuffer() {
        glGenRenderbuffers(1, &renderBufferID);
    }

public:
    GLuint renderBufferID;

    ~GLRenderBuffer() {
        glDeleteRenderbuffers(1, &renderBufferID);
    }
};
}
