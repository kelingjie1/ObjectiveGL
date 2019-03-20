//
//  GLTexture.h
//  ObjectiveGL
//
//  Created by lingtonke on 2018/10/8.
//  Copyright © 2018 tencent. All rights reserved.
//
#pragma once

#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#include "GLContext.h"
#include "GLObject.h"

namespace ObjectiveGL
{
    class GLTexture:public GLObject
    {
    protected:
        GLTexture(shared_ptr<GLContext> context):GLObject(context){}
    public:
        GLuint textureID;
        
        static shared_ptr<GLTexture> create(shared_ptr<GLContext> context)
        {
            return shared_ptr<GLTexture>(new GLTexture(context));
        }
        
        
        virtual void init()
        {
            auto c = context.lock();
            c->check(true);
            glGenTextures(1, &textureID);
            ready = true;
        }

        virtual void cleanup()
        {
            auto c = context.lock();
            c->check(true);
            GLuint tex = textureID;
            glDeleteTextures(1, &tex);
            
        }

        void setImageData(const GLvoid *pixels, GLsizei width,GLsizei height,GLenum internalformat=GL_RGBA,GLenum format=GL_BGRA)
        {
            check(true);
            glBindTexture(GL_TEXTURE_2D, textureID);
            checkError();
            if (pixels)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
            }
            else
            {
                glTexStorage2D(GL_TEXTURE_2D, 0, internalformat, width, height);
            }
            checkError();
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }

        void activeAndBind(GLuint index)
        {
            check(true);
            glActiveTexture(GL_TEXTURE0+index);
            checkError();
            glBindTexture(GL_TEXTURE_2D, textureID);
            checkError();
        }
    };
};

