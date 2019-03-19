//
//  GLBuffer.h
//  ObjectiveGL
//
//  Created by lingtonke on 2018/10/10.
//  Copyright © 2018 tencent. All rights reserved.
//

#pragma once

#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#include <string>
#include "../Error.h"
#include "GLObject.h"

namespace ObjectiveGL
{
    using namespace std;
    template<class T>
    class GLBuffer:public GLObject
    {
    protected:
        GLBuffer(shared_ptr<GLContext> context):GLObject(context),bufferID(0),size(0),count(0),bufferType(0){}
        GLenum bufferType;
    public:
        GLuint bufferID;
        GLsizei size;
        GLsizei count;
        

        virtual void init()
        {
            GLObject::init();
            glGenBuffers(1, &bufferID);
            checkError();
            if (!bufferID)
            {
                throw Error();
            }
        }
        
        void alloc(GLsizei count,T *data=nullptr,GLenum usage=GL_STREAM_DRAW)
        {
            check();
            this->count = count;
            size = count*sizeof(T);
            glBindBuffer(bufferType, bufferID);
            checkError();
            glBufferData(bufferType, size, data, usage);
            checkError();
            glBindBuffer(bufferType, 0);
            checkError();
        }
        void accessData(function<void(T *data)> func,GLuint offset=0,GLuint length=0,GLbitfield access=GL_MAP_READ_BIT|GL_MAP_WRITE_BIT)
        {
            auto c = context.lock();
            c->checkAndAsyncTask([&]{
                auto data = lock(offset,length,access);
                func(data);
                unlock();
            });
        }
        T *lock(GLuint offset=0,GLuint length=0,GLbitfield access=GL_MAP_READ_BIT|GL_MAP_WRITE_BIT)
        {
            check();
            if (length == 0)
            {
                length = size-offset;
            }
            glBindBuffer(bufferType, bufferID);
            checkError();
            void *data = glMapBufferRange(bufferType, offset, length, access);
            if (!data)
            {
                GLenum error = glGetError();
                throw Error(ObjectiveGLError_MapBufferFailed,error);
            }
            glBindBuffer(bufferType, 0);
            checkError();
            return (T*)data;
        }
        void unlock()
        {
            check();
            glBindBuffer(bufferType, bufferID);
            checkError();
            if(!glUnmapBuffer(bufferType))
            {
                GLenum error = glGetError();
                throw Error(ObjectiveGLError_UnMapBufferFailed,error);
            }
            glBindBuffer(bufferType, 0);
            checkError();
        }
    };
    template<class T>
    class GLVertexBuffer:public GLBuffer<T>
    {
    protected:
        GLVertexBuffer(shared_ptr<GLContext> context):GLBuffer<T>(context)
        {
            this->bufferType = GL_ARRAY_BUFFER;
        }
    public:
        static shared_ptr<GLVertexBuffer> create(shared_ptr<GLContext> context)
        {
            return shared_ptr<GLVertexBuffer>(new GLVertexBuffer(context));
        }
    };
    template<class T>
    class GLElementBuffer:public GLBuffer<T>
    {
    protected:
        GLElementBuffer(shared_ptr<GLContext> context):GLBuffer<T>(context)
        {
            this->bufferType = GL_ELEMENT_ARRAY_BUFFER;
        }
    public:
        static shared_ptr<GLElementBuffer> create(shared_ptr<GLContext> context)
        {
            return shared_ptr<GLElementBuffer>(new GLElementBuffer(context));
        }
    };
}
