//
//  GLVertexArray.h
//  ObjectiveGL
//
//  Created by lingtonke on 2018/10/10.
//  Copyright © 2018 tencent. All rights reserved.
//

#pragma once

#include "GLPlatform.h"
#include <string>
#include <memory>
#include <vector>
#include <map>
#include "GLBuffer.h"
#include "GLError.h"
#include "GLUtil.h"

namespace ObjectiveGL {
using namespace std;

class GLBaseVertex {
public:
    GLfloat x;
    GLfloat y;
    GLfloat u;
    GLfloat v;
};

class GLVertexArrayParams {
public:
    GLint size;//1、2、3、4
    GLenum type;//GL_FLOAT/GL_INT
    GLboolean normalized;

    GLVertexArrayParams(GLenum type,GLint size = 1,
                        GLboolean normalized = false) : type(type), size(size),
                                                        normalized(normalized) {}
};


class GLVertexArray : public GLObject {
    
    friend class GLFrameBuffer;
protected:
    
    GLenum mode;
    GLenum eboType;
    map<GLenum,shared_ptr<GLBuffer>> bufferMap;

    GLVertexArray():eboType(GL_UNSIGNED_INT) {
        glGenVertexArrays(1, &vao);
        checkError();
    }

    void draw(GLsizei count = 0) {
        check();
        glBindVertexArray(vao);
        auto it = bufferMap.find(GL_ELEMENT_ARRAY_BUFFER);
        if (it != bufferMap.end()) {
            auto elementBuffer = it->second;
            if (count == 0) {
                count = elementBuffer->count;
            }
            glDrawElements(mode, count, eboType, nullptr);
        } else {
            auto it = bufferMap.find(GL_ARRAY_BUFFER);
            auto vertexbuffer = it->second;
            if (count == 0) {
                count = vertexbuffer->count;
            }
            glDrawArrays(mode, 0, count);
        }
        
        glBindVertexArray(0);
    }
    
public:
    GLuint vao;
    
    static shared_ptr<GLVertexArray> create() {
        return shared_ptr<GLVertexArray>(new GLVertexArray());
    }

    ~GLVertexArray() {
        check();
        glDeleteVertexArrays(1, &vao);
        checkError();
    }
    
    void setBuffer(shared_ptr<GLBuffer> buffer) {
        check();
        glBindVertexArray(vao);
        glBindBuffer(buffer->bufferType, buffer->bufferID);
        glBindVertexArray(0);
        bufferMap[buffer->bufferType] = buffer;
    }
    
    


    void setParams(vector<GLVertexArrayParams> params) {
        check();
        glBindVertexArray(vao);
        GLuint offset = 0;
        auto vertexBuffer= bufferMap[GL_ARRAY_BUFFER];
        for (int i = 0; i < params.size(); i++) {
            glEnableVertexAttribArray(i);
            auto param = params[i];
            GLsizei stride = vertexBuffer->elementSize;
            GLvoid *of = reinterpret_cast<GLvoid *>(offset);
            glVertexAttribPointer(i, param.size, param.type, param.normalized, stride,
                                  of);
            offset += Util::sizeOfGLType(param.type) * param.size;
        }
        GLint maxAttributes = 0;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttributes);
        for (GLuint i = (GLuint) params.size(); i < maxAttributes; i++) {
            glDisableVertexAttribArray(i);
        }
        glBindVertexArray(0);
    }
    
    void setElementBufferType(GLenum type) {
        eboType = type;
    }

    void setDrawMode(GLenum mode) {
        this->mode = mode;
    }
};


}
