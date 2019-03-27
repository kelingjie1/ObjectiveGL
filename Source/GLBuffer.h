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
    GLBuffer() : bufferID(0), size(0) {
        glGenBuffers(1, &bufferID);
        checkError();
    }


    
public:
    GLuint bufferID;
    GLsizei size;
    GLsizei elementSize;
    GLsizei count;
    
    ~GLBuffer() {
        check();
        glDeleteBuffers(1, &bufferID);
    }
    
    static shared_ptr<GLBuffer> create() {
        return shared_ptr<GLBuffer>(new GLBuffer());
    }
    
    void alloc(GLsizei elementSize, GLsizei count, void *data = nullptr, GLenum usage = GL_STREAM_DRAW) {
        check();
        this->count = count;
        this->elementSize = elementSize;
        size = elementSize*count;
        glBindBuffer(GL_ARRAY_BUFFER, bufferID);
        checkError();
        glBufferData(GL_ARRAY_BUFFER, size, data, usage);
        checkError();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
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
        glBindBuffer(GL_ARRAY_BUFFER, bufferID);
        checkError();
        void *data = glMapBufferRange(GL_ARRAY_BUFFER, offset, length, access);
        checkError();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        checkError();
        return data;
    }

    void unlock() {
        check();
        glBindBuffer(GL_ARRAY_BUFFER, bufferID);
        checkError();
        glUnmapBuffer(GL_ARRAY_BUFFER);
        checkError();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        checkError();
        
    }
};

}
