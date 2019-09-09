//
//  GLTexture.h
//  ObjectiveGL
//
//  Created by lingtonke on 2018/10/8.
//  Copyright © 2018 tencent. All rights reserved.
//
#pragma once

#include "GLPlatform.h"
#include "GLContext.h"
#include "GLObject.h"

OGL_NAMESPACE_BEGIN(ObjectiveGL)
class OGL_API GLTexture : public GLShareObject {
    friend class GLContext;

protected:
    GLTexture(int backendTexture = -1, int width = 0, int height = 0) {
        if (backendTexture < 0) {
            GLCHECK(glGenTextures(1, &textureID));
            
        }
        else {
            textureID = backendTexture;
            isBackend = true;
            this->width = width;
            this->height = height;
        }
        
    }

public:

    GLuint textureID;
    bool isBackend = false;
    GLsizei width = 0;
    GLsizei height = 0;
    static shared_ptr<GLTexture> create(function<void(GLTexture *texture)> deleter = nullptr) {
        if (deleter) {
            return shared_ptr<GLTexture>(new GLTexture(-1),deleter);
        }
        else {
            return shared_ptr<GLTexture>(new GLTexture(-1));
        }
    }

    static shared_ptr<GLTexture> create(int backendTexture, int width, int height, function<void(GLTexture *texture)> deleter = nullptr) {
        if (deleter) {
            return shared_ptr<GLTexture>(new GLTexture(backendTexture, width, height),deleter);
        }
        else {
            return shared_ptr<GLTexture>(new GLTexture(backendTexture, width, height));
        }
    }

    ~GLTexture() {
        check();
        if (!isBackend) {
            GLCHECK(glDeleteTextures(1, &textureID));
            
        }
        
    }


    void setImageData(const GLvoid *pixels, GLsizei width, GLsizei height,
                      GLenum internalformat = GL_RGBA, GLenum format = GL_RGBA) {
        check();
        GLCHECK(glBindTexture(GL_TEXTURE_2D, textureID));
        
        this->width = width;
        this->height = height;
        glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format,
                     GL_UNSIGNED_BYTE, pixels);
        
        GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCHECK(glBindTexture(GL_TEXTURE_2D, 0));
    }

    void active(GLuint index) {
        check();
        GLCHECK(glActiveTexture(GL_TEXTURE0 + index));
        

    }

    void bind() {
        check();
        GLCHECK(glBindTexture(GL_TEXTURE_2D, textureID));
        
    }
};
OGL_NAMESPACE_END(ObjectiveGL)

