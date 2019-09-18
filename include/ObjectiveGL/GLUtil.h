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
#include <sstream>
#include <iostream>
#include "GLError.h"


OGL_NAMESPACE_BEGIN(ObjectiveGL)
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
                                                      GLCHECK(gl_Position = vec4(position, 0., 1.));
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

class OGL_API GLUtil {
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
        OGL_ERROR(ObjectiveGLError_InvalidType);
        return 0;
    }

    static inline const string shader2ToES3(const string &src, bool isVertexShader = false) {
        if (src.find("#version 300 es") != string::npos) {
            return src;

        } else if (src.find("#version 330") != string::npos) {
            auto dst = string(src);
            replaceAll(dst, "#version 330", "#version 300 es");
            return dst;

        } else return "#version 300 es\n" + shaderES2To3(src, isVertexShader);
    }

    static inline const string shader2To330(const string &src, bool isVertexShader = false) {
        if (src.find("#version 330") != string::npos) {
            return src;

        } else if (src.find("#version 300 es") != string::npos) {
            auto dst = string(src);
            replaceAll(dst, "#version 300 es", "#version 330");
            return dst;

        } else return "#version 330\n" + shaderES2To3(src, isVertexShader);
    }

    static inline const string shader330ToES3(const string &src, bool isVertexShader = false) {
        auto dst = string(src);
        replaceAll(dst, "#version 330\n", "#version 300 es\n");
        return dst;
    }

    static inline const string shaderES3To330(const string &src, bool isVertexShader = false) {
        auto dst = string(src);
        replaceAll(dst, "#version 300 es\n", "#version 330\n");
        return dst;
    }

    /**
     * @brief 将简单的300 es 或 300 shader 转换为GL2，注意空格的写法
     * @param src
     * @param isVertexShader
     * @return
     */
    static inline const string shader3To2(const string &src, bool isVertexShader = false) {
        if (src.find("#version 300 es") != string::npos || src.find("#version 330") != string::npos) {
            if (src.find("layout(") != string::npos) {
                return string("not support layout conversion");
            }

            auto dst = string(src);
            replaceAll(dst, "#version 300 es\n", "");
            replaceAll(dst, "#version 330\n", "");

            if (isVertexShader) {
                replaceAll(dst, "in ", "attribute ");
                replaceAll(dst, "out ", "varying ");

            } else {
                replaceAll(dst, "in ", "varying ");
                replaceAll(dst, "texture(", "texture2D(");

#define OUT_KEYWORD "out vec4 "
                auto outStart = dst.find(OUT_KEYWORD);
                if (outStart == string::npos) return string(src);

                auto outArgStart = outStart + strlen(OUT_KEYWORD);

                auto outEnd = dst.find(';', outStart);
                if (outEnd == string::npos) return string(src);

                auto outArg = dst.substr(outArgStart, outEnd - outArgStart);

                dst.replace(outStart, outEnd - outStart, "");
                replaceAll(dst, outArg, "gl_FragColor");
            }

            return dst;

        } else {
            return src;
        }
    }

private:
    static inline void replaceAll(string &src, const string& old, const string& rep) {
        auto pos = string::npos;
        auto oldLen = old.length();

        while ((pos = src.find(old)) != string::npos) {
            src.replace(pos, oldLen, rep);
        }
    }

    /**
     * @brief 将简单的ES2 shader转为300 es 或 330。
     *        简单shader先声明变量，再定义函数体，同时要注意空格的写法
     *        #变量1
     *        #变量2
     *        #函数体1
     *        #函数体2
     *
     * @param src
     * @param isVertexShader    是否是vertex shader
     * @return
     */
    static inline const string shaderES2To3(const string &src, bool isVertextShader) {
        auto dst = string(src);
        replaceAll(dst, "varying ", isVertextShader? "out " : "in ");

        if (isVertextShader) {
            replaceAll(dst, "attribute ", "in ");
        replaceAll(dst, "varying ", "out ");

        } else {
            replaceAll(dst, "varying ", "in ");
            replaceAll(dst, "texture2D(", "texture(");
            replaceAll(dst, "gl_FragColor", "glFragColor");

            auto s = dst.find("{");
            if (s == string::npos) return src;

            s = dst.rfind(';', s);
            if (s == string::npos) return src;

            dst.insert(s + 1, " out vec4 glFragColor;");
        }

        return dst;
    }
};

OGL_NAMESPACE_END(ObjectiveGL)
