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
    class GLTexture:public GLShareObject
    {
        friend class GLContext;
    protected:
        GLTexture()
        {
            glGenTextures(1, &textureID);
        }
    public:
        
        GLuint textureID;
        
        ~GLTexture()
        {
            check();
            glDeleteTextures(1, &textureID);
        }
        

        void setImageData(const GLvoid *pixels, GLsizei width,GLsizei height,GLenum internalformat=GL_RGBA,GLenum format=GL_BGRA)
        {
            check();
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

        void active(GLuint index)
        {
            check();
            glActiveTexture(GL_TEXTURE0+index);
            checkError();
            
        }
        
        void bind()
        {
            check();
            glBindTexture(GL_TEXTURE_2D, textureID);
            checkError();
        }
    };
};

