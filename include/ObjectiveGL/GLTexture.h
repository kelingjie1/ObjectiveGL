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
class GLTexture : public GLShareObject {
    friend class GLContext;

protected:
    GLTexture(int backendTexture = -1) {
        if (backendTexture < 0) {
            OGL(glGenTextures(1, &textureID));
            checkError();
        }
        else {
            textureID = backendTexture;
            isBackend = true;
        }
        
    }

public:

    GLuint textureID;
    bool isBackend = false;
    static shared_ptr<GLTexture> create(int backendTexture = -1) {
        return shared_ptr<GLTexture>(new GLTexture(backendTexture));
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

