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
#include "GLProgram.h"

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
    GLenum type;//GL_FLOAT/GL_INT
    GLint size;//1、2、3、4
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
    GLsizei drawCount;
    map<GLenum,shared_ptr<GLBuffer>> bufferMap;

    GLVertexArray():eboType(GL_UNSIGNED_INT),drawCount(0) {
        OGL(glGenVertexArrays(1, &vao));
        checkError();
    }


    void draw() {
        check();
        auto count = drawCount;
        OGL(glBindVertexArray(vao));
        auto it = bufferMap.find(GL_ELEMENT_ARRAY_BUFFER);
        if (it != bufferMap.end()) {
            auto elementBuffer = it->second;
            if (count == 0) {
                count = elementBuffer->count;
            }
            OGL(glDrawElements(mode, count, eboType, nullptr));
            checkError();
        } else {
            auto it = bufferMap.find(GL_ARRAY_BUFFER);
            auto vertexbuffer = it->second;
            if (count == 0) {
                count = vertexbuffer->count;
            }
            OGL(glDrawArrays(mode, 0, count));
            checkError();
        }
        
        OGL(glBindVertexArray(0));
    }
    
public:
    GLuint vao;
    
    static shared_ptr<GLVertexArray> create() {
        return shared_ptr<GLVertexArray>(new GLVertexArray());
    }

    ~GLVertexArray() {
        check();
        OGL(glDeleteVertexArrays(1, &vao));
        checkError();
    }
    
    void setBuffer(GLenum type,shared_ptr<GLBuffer> buffer) {
        check();
        OGL(glBindVertexArray(vao));
        checkError();
        if (type != GL_TRANSFORM_FEEDBACK_BUFFER) {
            OGL(glBindBuffer(type, buffer->bufferID));
        }
        checkError();
        OGL(glBindVertexArray(0));
        checkError();
        bufferMap[type] = buffer;
    }
    
    void setDrawCount(GLsizei drawCount) {
        this->drawCount = drawCount;
    }


    void setParams(vector<GLVertexArrayParams> params) {
        check();
        OGL(glBindVertexArray(vao));
        GLuint offset = 0;
        auto vertexBuffer= bufferMap[GL_ARRAY_BUFFER];
        for (int i = 0; i < (int)params.size(); i++) {
            OGL(glEnableVertexAttribArray(i));
            auto param = params[i];
            GLsizei stride = vertexBuffer->elementSize;
            GLvoid *of = reinterpret_cast<GLvoid *>(offset);
            glVertexAttribPointer(i, param.size, param.type, param.normalized, stride,
                                  of);
            offset += Util::sizeOfGLType(param.type) * param.size;
        }
        GLint maxAttributes = 0;
        OGL(glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttributes));
        for (GLint i = (GLint) params.size(); i < maxAttributes; i++) {
            OGL(glDisableVertexAttribArray(i));
        }
        OGL(glBindVertexArray(0));
    }
    
    void setElementBufferType(GLenum type) {
        eboType = type;
    }

    void setDrawMode(GLenum mode) {
        this->mode = mode;
    }
    
    void computeUsingTransformFeedback(shared_ptr<GLProgram> program) {
        program->use();
        OGL(glEnable(GL_RASTERIZER_DISCARD));
        checkError();
        auto buffer = bufferMap[GL_TRANSFORM_FEEDBACK_BUFFER];
        OGL(glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, buffer->bufferID));
        checkError();
        OGL(glBeginTransformFeedback(mode));
        checkError();
        draw();
        OGL(glEndTransformFeedback());
        checkError();
        OGL(glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0));
        checkError();
        OGL(glDisable(GL_RASTERIZER_DISCARD));
        checkError();
    }
};


}
