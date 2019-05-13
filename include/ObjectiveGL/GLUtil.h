//
//  Util.h
//  GPUPower
//
//  Created by lingtonke on 2018/10/11.
//  Copyright © 2018 tencent. All rights reserved.
//

#pragma once

#define GLSTRING(x) #x

#include <string>
#include <fstream>
#include "GLError.h"


namespace ObjectiveGL {
using namespace std;
static const string passThroughVertexShader = string("#version 300 es\n") +
                                              GLSTRING(
                                                  precision
                                                  highp float;
                                                  layout(location = 0)
                                                  in
                                                  vec2 position;
                                                  layout(location = 1)
                                                  in
                                                  vec2 coord;
                                                  out
                                                  vec2 uv;
                                                  void main() {
                                                      OGL(gl_Position = vec4(position, 0., 1.));
                                                      uv = coord;
                                                  }
                                              );
static const string passThroughFragmentShader = string("#version 300 es\n") +
                                                GLSTRING(
                                                    precision
                                                    highp float;
                                                    layout(location = 0)
                                                    out
                                                    vec4 color;
                                                    uniform
                                                    sampler2D tex;
                                                    in
                                                    vec2 uv;
                                                    void main() {
                                                        color = texture(tex, uv);
                                                    }
                                                );

class Util {
public:
    static string readFile(string file) {
        char buf[1000];
        string str;
        fstream f(file);
        long getCount = 0;
        do {
            f.read(buf, 1000);
            getCount = f.gcount();
            str.append(buf, getCount);
        } while (getCount > 0);
        return str;
    }

    static GLuint sizeOfGLType(GLenum type) {
        switch (type) {
            case GL_BYTE:
                return sizeof(GLbyte);
            case GL_UNSIGNED_BYTE:
                return sizeof(GLubyte);
            case GL_SHORT:
                return sizeof(GLshort);
            case GL_UNSIGNED_SHORT:
                return sizeof(GLushort);
            case GL_INT:
                return sizeof(GLint);
            case GL_UNSIGNED_INT:
                return sizeof(GLuint);
            case GL_FLOAT:
                return sizeof(GLfloat);
            case GL_FIXED:
                return sizeof(GLfixed);
        }
        throw GLError(ObjectiveGLError_InvalidType);
        return 0;
    }
};
}
