//
//  GLBuffer.h
//  ObjectiveGL
//
//  Created by lingtonke on 2018/10/10.
//  Copyright © 2018 tencent. All rights reserved.
//

#pragma once

#include "GLPlatform.h"
#include <string>
#include "GLError.h"
#include "GLObject.h"

namespace ObjectiveGL {
using namespace std;

class GLBuffer : public GLShareObject {
protected:
    GLBuffer(GLenum bufferType) : bufferID(0), size(0), bufferType(bufferType) {
        glGenBuffers(1, &bufferID);
        checkError();
    }


    
public:
    GLuint bufferID;
    const GLenum bufferType;
    GLsizei size;
    GLsizei elementSize;
    GLsizei count;
    
    ~GLBuffer() {
        check();
        glDeleteBuffers(1, &bufferID);
    }
    
    static shared_ptr<GLBuffer> create(GLenum bufferType) {
        return shared_ptr<GLBuffer>(new GLBuffer(bufferType));
    }
    
    void alloc(GLsizei elementSize, GLsizei count, void *data = nullptr, GLenum usage = GL_STREAM_DRAW) {
        check();
        this->count = count;
        this->elementSize = elementSize;
        size = elementSize*count;
        glBindBuffer(bufferType, bufferID);
        checkError();
        glBufferData(bufferType, size, data, usage);
        checkError();
        glBindBuffer(bufferType, 0);
        checkError();
    }

    void accessData(function<void(void *data)> func, GLuint offset = 0, GLuint length = 0,
                    GLbitfield access = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT) {
        check();
        auto data = lock(offset, length, access);
        func(data);
        unlock();
    }

    void *lock(GLuint offset = 0, GLuint length = 0,
            GLbitfield access = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT) {
        check();
        if (length == 0) {
            length = size - offset;
        }
        glBindBuffer(bufferType, bufferID);
        checkError();
        void *data = glMapBufferRange(bufferType, offset, length, access);
        checkError();
        glBindBuffer(bufferType, 0);
        checkError();
        return data;
    }

    void unlock() {
        check();
        glBindBuffer(bufferType, bufferID);
        checkError();
        glUnmapBuffer(bufferType);
        checkError();
        glBindBuffer(bufferType, 0);
        checkError();
        
    }
};

}
