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

class OGL_API GLBaseVertex {
public:
    GLfloat x;
    GLfloat y;
    GLfloat u;
    GLfloat v;
};

class OGL_API GLVertexArrayParams {
public:
    string name;
    int location = -1;
    GLenum type;//GL_FLOAT/GL_INT
    GLint size;//1、2、3、4
    GLboolean normalized;

    GLVertexArrayParams(GLenum type, GLint size = 1, GLboolean normalized = false) : name(""), type(type), size(size), normalized(normalized) {}
    GLVertexArrayParams(string name, GLenum type,GLint size = 1, GLboolean normalized = false) : name(name), type(type), size(size), normalized(normalized) {}
};

class OGL_API GLVertexArray : public GLObject {
    
    friend class GLFrameBuffer;
protected:
    
    GLenum mode;
    GLenum eboType;
    GLsizei drawCount;
    map<GLenum,shared_ptr<GLBuffer>> bufferMap;
    vector<GLVertexArrayParams> params;

    GLVertexArray():eboType(GL_UNSIGNED_INT),drawCount(0) {
#ifdef ES3
        GLCHECK(glGenVertexArrays(1, &vao));
        
#endif
    }


    void draw(shared_ptr<GLProgram> program) {
        check();
        auto count = drawCount;
#ifdef ES3
        GLCHECK(glBindVertexArray(vao));
#endif
        for (auto &buffer:bufferMap) {
            GLCHECK(glBindBuffer(buffer.first, buffer.second->bufferID));
            
        }
        useParams(program);
        auto it = bufferMap.find(GL_ELEMENT_ARRAY_BUFFER);
        if (it != bufferMap.end()) {
            auto elementBuffer = it->second;
            if (count == 0) {
                count = elementBuffer->count;
            }
            GLCHECK(glDrawElements(mode, count, eboType, nullptr));
            
        } else {
            auto it = bufferMap.find(GL_ARRAY_BUFFER);
            auto vertexbuffer = it->second;
            if (count == 0) {
                count = vertexbuffer->count;
            }
            GLCHECK(glDrawArrays(mode, 0, count));
            
        }
#ifdef ES3
        GLCHECK(glBindVertexArray(0));
#else
        for (auto &buffer:bufferMap) {
            GLCHECK(glBindBuffer(buffer.first, 0));
            
        }
#endif
    }
    
    void useParams(shared_ptr<GLProgram> program) {
        
        GLint maxAttributes = 0;
        GLCHECK(glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttributes));
        for (GLint i = 0; i < maxAttributes; i++) {
            GLCHECK(glDisableVertexAttribArray(i));
        }
        
        GLuint offset = 0;
        auto vertexBuffer= bufferMap[GL_ARRAY_BUFFER];
        for (int i = 0; i < (int)params.size(); i++) {
            auto param = params[i];
            GLsizei stride = vertexBuffer->elementSize;
            GLvoid *of = reinterpret_cast<GLvoid *>(offset);
            if (param.location < 0) {
                if (param.name.size()) {
                    param.location = GLCHECK(glGetAttribLocation(program->programID, param.name.c_str()));
                    
                    if (param.location < 0) {
                        OGL_ERROR(ObjectiveGLError_LocationNotFound);
                    }
                }
                else {
                    param.location = i;
                }
            }
            GLCHECK(glEnableVertexAttribArray(param.location));
            GLCHECK(glVertexAttribPointer(param.location, param.size, param.type, param.normalized, stride, of));
            
            offset += GLUtil::sizeOfGLType(param.type) * param.size;
        }
        
    }
    
public:
#ifdef ES3
    GLuint vao;
#endif
    
    static shared_ptr<GLVertexArray> create(function<void(GLVertexArray *vao)> deleter = nullptr) {
        if (deleter) {
            return shared_ptr<GLVertexArray>(new GLVertexArray(),deleter);
        }
        else {
            return shared_ptr<GLVertexArray>(new GLVertexArray());
        }
        
    }

    ~GLVertexArray() {
        check();
#ifdef ES3
        GLCHECK(glDeleteVertexArrays(1, &vao));
        
#endif
    }
    
    void setBuffer(GLenum type,shared_ptr<GLBuffer> buffer) {
        check();
        bufferMap[type] = buffer;
    }
    
    void setDrawCount(GLsizei drawCount) {
        this->drawCount = drawCount;
    }


    void setParams(vector<GLVertexArrayParams> params) {
        check();
        this->params = params;
#ifdef ES3
        GLCHECK(glBindVertexArray(vao));
        GLCHECK(glBindVertexArray(0));
#endif
    }
    
    void setElementBufferType(GLenum type) {
        eboType = type;
    }

    void setDrawMode(GLenum mode) {
        this->mode = mode;
    }
#ifdef ES3
    void computeUsingTransformFeedback(shared_ptr<GLProgram> program) {
        program->use();
        GLCHECK(glEnable(GL_RASTERIZER_DISCARD));
        
        auto buffer = bufferMap[GL_TRANSFORM_FEEDBACK_BUFFER];
        GLCHECK(glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, buffer->bufferID));
        
        GLCHECK(glBeginTransformFeedback(mode));
        
        draw(program);
        GLCHECK(glEndTransformFeedback());
        
        GLCHECK(glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0));
        
        GLCHECK(glDisable(GL_RASTERIZER_DISCARD));
        
    }
#endif
};


}
