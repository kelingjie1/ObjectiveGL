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

template<class T>
class GLBuffer : public GLShareObject {
protected:
    GLBuffer() : bufferID(0), size(0), count(0), bufferType(0) {
        glGenBuffers(1, &bufferID);
        checkError();
    }

    ~GLBuffer() {
        check();
        glDeleteBuffers(1, &bufferID);
    }

    GLenum bufferType;
public:
    GLuint bufferID;
    GLsizei size;
    GLsizei count;

    void alloc(GLsizei count, T *data = nullptr, GLenum usage = GL_STREAM_DRAW) {
        check();
        this->count = count;
        size = count * sizeof(T);
        glBindBuffer(bufferType, bufferID);
        checkError();
        glBufferData(bufferType, size, data, usage);
        checkError();
        glBindBuffer(bufferType, 0);
        checkError();
    }

    void accessData(function<void(T *data)> func, GLuint offset = 0, GLuint length = 0,
                    GLbitfield access = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT) {
        check();
        auto data = lock(offset, length, access);
        func(data);
        unlock();
    }

    T *lock(GLuint offset = 0, GLuint length = 0,
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
        return (T *) data;
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

template<class T>
class GLVertexBuffer : public GLBuffer<T> {
    friend class GLContext;

protected:
    GLVertexBuffer(shared_ptr<GLContext> context) : GLBuffer<T>() {
        this->bufferType = GL_ARRAY_BUFFER;
    }

public:

};

template<class T>
class GLElementBuffer : public GLBuffer<T> {
    friend class GLContext;

protected:
    GLElementBuffer(shared_ptr<GLContext> context) : GLBuffer<T>() {
        this->bufferType = GL_ELEMENT_ARRAY_BUFFER;
    }

public:

};
}
