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

class OGL_API GLBuffer : public GLShareObject {
protected:
    GLBuffer() : bufferID(0), size(0) {
        GLCHECK(glGenBuffers(1, &bufferID));
        
    }


    
public:
    GLuint bufferID;
    GLsizei size;
    GLsizei elementSize;
    GLsizei count;
    
    ~GLBuffer() {
        check();
        GLCHECK(glDeleteBuffers(1, &bufferID));
    }
    
    static shared_ptr<GLBuffer> create() {
        return shared_ptr<GLBuffer>(new GLBuffer());
    }
    
    void alloc(GLsizei elementSize, GLsizei count, const void *data = nullptr, GLenum usage = GL_STREAM_DRAW) {
        check();
        this->count = count;
        this->elementSize = elementSize;
        size = elementSize*count;
        GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, bufferID));
        
        GLCHECK(glBufferData(GL_ARRAY_BUFFER, size, data, usage));
        
        GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
        
    }
    
    void replaceData(GLintptr offset, GLsizei count, const GLvoid *data) {
        check();
        size = elementSize*count;
        GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, bufferID));
        
        GLCHECK(glBufferSubData(GL_ARRAY_BUFFER, offset, size, data));
        
        GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
        
    }
#ifdef ES3
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
        GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, bufferID));
        
        void *data = GLCHECK(glMapBufferRange(GL_ARRAY_BUFFER, offset, length, access));
        
        GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
        
        return data;
    }

    void unlock() {
        check();
        GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, bufferID));
        
        GLCHECK(glUnmapBuffer(GL_ARRAY_BUFFER));
        
        GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
        
        
    }
    
    void copyFromBuffer(shared_ptr<GLBuffer> buffer,GLuint readOffset=0,GLuint writeOffset=0,GLsizei size=0) {
        GLCHECK(glBindBuffer(GL_COPY_READ_BUFFER, buffer->bufferID));
        GLCHECK(glBindBuffer(GL_COPY_WRITE_BUFFER, bufferID));
        if (size == 0) {
            size = min(buffer->size,this->size);
        }
        GLCHECK(glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, readOffset, writeOffset, size));
    }
#endif
};

}
