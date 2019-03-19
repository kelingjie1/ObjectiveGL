//
//  GLVertexArray.h
//  ObjectiveGL
//
//  Created by lingtonke on 2018/10/10.
//  Copyright © 2018 tencent. All rights reserved.
//

#pragma once

#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#include <string>
#include <memory>
#include <vector>
#include "GLBuffer.h"
#include "../Error.h"
namespace ObjectiveGL
{
    using namespace std;
    class GLBaseVertex
    {
    public:
        GLfloat x;
        GLfloat y;
        GLfloat u;
        GLfloat v;
    };
    class GLVertexArrayParams
    {
    public:
        GLuint index;
        GLint size;//1、2、3、4
        GLenum type;//GL_FLOAT/GL_INT
        GLboolean normalized;
        GLVertexArrayParams(GLuint index,GLint size=1,GLenum type=GL_FLOAT,GLboolean normalized=false):index(index),size(size),type(type),normalized(normalized){}
    };
    template<class vboType=GLBaseVertex,class eboType = unsigned char>
    class GLVertexArray;
    
    
    
    template<class vboType,class eboType>
    class GLVertexArray:public GLObject
    {
        static shared_ptr<GLVertexArray<GLBaseVertex>> &basicVertexArrayInstance()
        {
            static thread_local shared_ptr<GLVertexArray<GLBaseVertex>> instance;
            return instance;
        }
        GLenum mode;
        shared_ptr<GLVertexBuffer<vboType>> vertexbuffer;
        shared_ptr<GLElementBuffer<eboType>> elementBuffer;
        GLVertexArray(shared_ptr<GLContext> context):GLObject(context){}
    public:
        GLuint vao;
        static shared_ptr<GLVertexArray<vboType,eboType>> create(shared_ptr<GLContext> context)
        {
            return shared_ptr<GLVertexArray<vboType,eboType>>(new GLVertexArray<vboType,eboType>(context));
        }
        virtual void init()
        {
            GLObject::init();
            glGenVertexArrays(1, &vao);
            if (!vao)
            {
                throw Error();
            }
        }
        
        void setVertexBuffer(shared_ptr<GLVertexBuffer<vboType>> vertexbuffer)
        {
            this->vertexbuffer = vertexbuffer;
            check();
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer->bufferID);
            glBindVertexArray(0);
        }
        void setElementBuffer(shared_ptr<GLElementBuffer<vboType>> elementbuffer)
        {
            this->elementBuffer = elementbuffer;
            check();
            glBindVertexArray(vao);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer->bufferID);
            glBindVertexArray(0);
        }
        
        
        void setParams(vector<GLVertexArrayParams> params)
        {
            check();
            glBindVertexArray(vao);
            GLuint offset = 0;
            for (int i=0; i<params.size(); i++)
            {
                glEnableVertexAttribArray(i);
                auto param = params[i];
                GLsizei stride = sizeof(vboType);
                GLvoid *of = reinterpret_cast<GLvoid*>(offset);
                glVertexAttribPointer(param.index, param.size, param.type, param.normalized, stride, of);
                offset += Util::sizeOfGLType(param.type)*param.size;
            }
            GLint maxAttributes = 0;
            glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttributes);
            for (GLuint i=(GLuint)params.size(); i<maxAttributes; i++)
            {
                glDisableVertexAttribArray(i);
            }
            glBindVertexArray(0);
        }
        
        void setDrawMode(GLenum mode)
        {
            this->mode = mode;
        }
        
        void draw(GLsizei count=0)
        {
            check();
            glBindVertexArray(vao);
            if (elementBuffer)
            {
                if (count == 0)
                {
                    count = elementBuffer->count;
                }
                GLenum type;
                if (sizeof(eboType) == sizeof(unsigned char))
                {
                    type = GL_UNSIGNED_BYTE;
                }
                else if (sizeof(eboType) == sizeof(unsigned short))
                {
                    type = GL_UNSIGNED_SHORT;
                }
                else if (sizeof(eboType) == sizeof(unsigned int))
                {
                    type = GL_UNSIGNED_INT;
                }
                else
                {
                    throw "";
                }
                glDrawElements(mode, count, type, nullptr);
            }
            else
            {
                if (count == 0)
                {
                    count = vertexbuffer->count;
                }
                glDrawArrays(mode, 0, count);
            }
            
            glBindVertexArray(0);
        }
        
        static shared_ptr<GLVertexArray<GLBaseVertex>> basicVertexArray()
        {
            if (!basicVertexArrayInstance()||!basicVertexArrayInstance()->context.lock().get())
            {
                basicVertexArrayInstance() = GLVertexArray<GLBaseVertex>::create(GLContext::current());
                
                auto buffer = GLVertexBuffer<GLBaseVertex>::create(GLContext::current());
                buffer->alloc(4);
                auto vertex = buffer->lock();
                vertex[0].x = -1.0f;
                vertex[0].y = -1.0f;
                vertex[0].u = 0.0f;
                vertex[0].v = 0.0f;
                
                vertex[1].x = 1.0f;
                vertex[1].y = -1.0f;
                vertex[1].u = 1.0f;
                vertex[1].v = 0.0f;
                
                vertex[2].x = -1.0f;
                vertex[2].y = 1.0f;
                vertex[2].u = 0.0f;
                vertex[2].v = 1.0f;
                
                vertex[3].x = 1.0f;
                vertex[3].y = 1.0f;
                vertex[3].u = 1.0f;
                vertex[3].v = 1.0f;
                buffer->unlock();
                basicVertexArrayInstance()->setVertexBuffer(buffer);
                
                vector<GLVertexArrayParams> params;
                params.push_back(GLVertexArrayParams(0,2));
                params.push_back(GLVertexArrayParams(1,2));
                basicVertexArrayInstance()->setParams(params);
                
                basicVertexArrayInstance()->setDrawMode(GL_TRIANGLE_STRIP);
                
            }
            return basicVertexArrayInstance();
        }
    };
    
    
}
