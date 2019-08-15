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

namespace ObjectiveGL {
class OGL_API GLTexture : public GLShareObject {
    friend class GLContext;

protected:
    GLTexture(int backendTexture = -1, int width = 0, int height = 0) {
        if (backendTexture < 0) {
            OGL(glGenTextures(1, &textureID));
            checkError();
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
    static shared_ptr<GLTexture> create() {
        return shared_ptr<GLTexture>(new GLTexture(-1));
    }

    static shared_ptr<GLTexture> create(int backendTexture, int width, int height) {
        return shared_ptr<GLTexture>(new GLTexture(backendTexture, width, height));
    }

    ~GLTexture() {
        check();
        if (!isBackend) {
            OGL(glDeleteTextures(1, &textureID));
            checkError();
        }
        
    }


    void setImageData(const GLvoid *pixels, GLsizei width, GLsizei height,
                      GLenum internalformat = GL_RGBA, GLenum format = GL_RGBA) {
        check();
        OGL(glBindTexture(GL_TEXTURE_2D, textureID));
        checkError();
        this->width = width;
        this->height = height;
        glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format,
                     GL_UNSIGNED_BYTE, pixels);
        checkError();
        OGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        OGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        OGL(glBindTexture(GL_TEXTURE_2D, 0));
    }

    void active(GLuint index) {
        check();
        OGL(glActiveTexture(GL_TEXTURE0 + index));
        checkError();

    }

    void bind() {
        check();
        OGL(glBindTexture(GL_TEXTURE_2D, textureID));
        checkError();
    }
};
};

