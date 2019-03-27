//
//  GLProgram.h
//  ObjectiveGL
//
//  Created by lingtonke on 2018/10/8.
//  Copyright © 2018 tencent. All rights reserved.
//

#pragma once

#include "GLPlatform.h"
#include "GLUtil.h"
#include "GLError.h"
#include "GLObject.h"
#include <string>
#include <memory>
#include <fstream>
#include <vector>
#include <functional>
#include <map>
#define SHADER_STRING(x) #x

namespace ObjectiveGL {
using namespace std;
    
enum GLProgramType {
    GLProgramTypeUnknow,
    GLProgramTypeRender,
    GLProgramTypeTransformFeedback,
};

class GLProgram : public GLObject {
    friend class GLContext;

protected:
    GLProgramType type;
    map <GLuint, function<void()>> uniformFunc;
    vector<pair<GLuint, shared_ptr<GLTexture>>> textures;

    GLProgram() : programID(glCreateProgram()), vertexShaderID(0), fragmentShaderID(0) {}

    void setUniform(string name, function<void()> func) {
        auto location = getUniformLocation(name);
        setUniform(location, func);
    }

    void setUniform(GLuint location, function<void()> func) {
        check();
        uniformFunc[location] = func;
        func();
        checkError();
    }


    void setUniformToGL() {
        for (auto &uniform:uniformFunc) {
            uniform.second();
        }
        for (int i = 0; i < textures.size(); i++) {
            auto texture = textures[i].second;
            auto location = textures[i].first;
            texture->active(i);
            texture->bind();
            glUniform1i(location, i);
        }
    }
    GLuint compileShader(string str,GLenum type) {
        
        GLint success;
        GLchar infoLog[512];
        auto c = str.c_str();
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &c, nullptr);
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            throw GLError(ObjectiveGLError_VertexShaderCompileFailed, infoLog);
        }
        checkError();
        return shader;
    }

    void link() {
        GLint linked;
        GLchar infoLog[512];
        glLinkProgram(programID);
        glGetProgramiv(programID, GL_LINK_STATUS, &linked);
        if (!linked) {
            glGetProgramInfoLog(programID, 512, nullptr, infoLog);
            throw GLError(ObjectiveGLError_ProgramLinkFailed, infoLog);
        }
    }

public:
    GLuint programID;
    GLuint vertexShaderID;
    GLuint fragmentShaderID;
    
    static shared_ptr<GLProgram> create() {
        return shared_ptr<GLProgram>(new GLProgram());
    }

    ~GLProgram() {
        check();
        GLuint vs = vertexShaderID;
        GLuint fs = fragmentShaderID;
        GLuint p = programID;
        if (vs) {
            glDeleteShader(vs);
        }
        if (fs) {
            glDeleteShader(fs);
        }
        if (p) {
            glDeleteProgram(p);
        }
    }
    
    GLProgramType getType() {
        return type;
    }

    void setRenderShader(string vs, string fs) {
        vertexShaderID = compileShader(vs, GL_VERTEX_SHADER);
        glAttachShader(programID, vertexShaderID);
        checkError();
        fragmentShaderID = compileShader(fs, GL_FRAGMENT_SHADER);
        glAttachShader(programID, fragmentShaderID);
        checkError();
        link();
        type = GLProgramTypeRender;
    }
    
    void setTransformFeedbackShader(string vs,vector<const GLchar*> varyings,GLenum bufferMode = GL_INTERLEAVED_ATTRIBS) {
        vertexShaderID = compileShader(vs, GL_VERTEX_SHADER);
        glAttachShader(programID, vertexShaderID);
        checkError();
        fragmentShaderID = compileShader("#version 300 es\nvoid main(){}", GL_FRAGMENT_SHADER);
        glAttachShader(programID, fragmentShaderID);
        checkError();
        glTransformFeedbackVaryings(programID, (GLsizei)varyings.size(), varyings.data(), bufferMode);
        checkError();
        auto c = vs.c_str();
        link();
        type = GLProgramTypeTransformFeedback;
    }


    void use() {
        check();
        glUseProgram(programID);
    }

    void setUniform(GLuint location, GLfloat x) {
        setUniform(location, [=] {
            glUniform1f(location, x);
        });
    }

    void setUniform(GLuint location, GLfloat x, GLfloat y) {
        setUniform(location, [=] {
            glUniform2f(location, x, y);
        });

    }

    void setUniform(GLuint location, GLfloat x, GLfloat y, GLfloat z) {
        setUniform(location, [=] {
            glUniform3f(location, x, y, z);
        });
    }

    void setUniform(GLuint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
        setUniform(location, [=] {
            glUniform4f(location, x, y, z, w);
        });
    }

    void setUniform(GLuint location, GLint x) {
        setUniform(location, [=] {
            glUniform1i(location, x);
        });

    }

    void setUniform(GLuint location, GLint x, GLint y) {
        setUniform(location, [=] {
            glUniform2i(location, x, y);
        });

    }

    void setUniform(GLuint location, GLint x, GLint y, GLint z) {
        setUniform(location, [=] {
            glUniform3i(location, x, y, z);
        });
    }

    void setUniform(GLuint location, GLint x, GLint y, GLint z, GLint w) {
        setUniform(location, [=] {
            glUniform4i(location, x, y, z, w);
        });
        check();

    }

    void setUniform(GLuint location, GLuint x) {
        setUniform(location, [=] {
            glUniform1ui(location, x);
        });
    }

    void setUniform(GLuint location, GLuint x, GLuint y) {
        setUniform(location, [=] {
            glUniform2ui(location, x, y);
        });
    }

    void setUniform(GLuint location, GLuint x, GLuint y, GLuint z) {
        setUniform(location, [=] {
            glUniform3ui(location, x, y, z);
        });
    }

    void setUniform(GLuint location, GLuint x, GLuint y, GLuint z, GLuint w) {
        setUniform(location, [=] {
            glUniform4ui(location, x, y, z, w);
        });
    }

    void setUniform(GLuint location, vector<GLfloat> matrix) {
        if (matrix.size() == 4) {
            setUniform(location, [=] {
                glUniformMatrix2fv(location, (GLsizei) matrix.size(), false, matrix.data());
            });
        } else if (matrix.size() == 9) {
            setUniform(location, [=] {
                glUniformMatrix3fv(location, (GLsizei) matrix.size(), false, matrix.data());
            });
        } else if (matrix.size() == 16) {
            setUniform(location, [=] {
                glUniformMatrix4fv(location, (GLsizei) matrix.size(), false, matrix.data());
            });
        } else {
            throw GLError(ObjectiveGLError_InvalidData);
        }
    }

    GLuint getUniformLocation(string name) {
        check();
        auto location = glGetUniformLocation(programID, name.c_str());
        checkError();
        return location;
    }

    void setUniform(string name, GLfloat x) {
        auto location = getUniformLocation(name);
        setUniform(location, x);
    }

    void setUniform(string name, GLfloat x, GLfloat y) {
        auto location = getUniformLocation(name);
        setUniform(location, x, y);
    }

    void setUniform(string name, GLfloat x, GLfloat y, GLfloat z) {
        auto location = getUniformLocation(name);
        setUniform(location, x, y, z);
    }

    void setUniform(string name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
        auto location = getUniformLocation(name);
        setUniform(location, x, y, z, w);
    }

    void setUniform(string name, GLint x) {
        auto location = getUniformLocation(name);
        setUniform(location, x);
    }

    void setUniform(string name, GLint x, GLint y) {
        auto location = getUniformLocation(name);
        setUniform(location, x, y);
    }

    void setUniform(string name, GLint x, GLint y, GLint z) {
        auto location = getUniformLocation(name);
        setUniform(location, x, y, z);
    }

    void setUniform(string name, GLint x, GLint y, GLint z, GLint w) {
        auto location = getUniformLocation(name);
        setUniform(location, x, y, z, w);
    }

    void setUniform(string name, GLuint x) {
        auto location = getUniformLocation(name);
        setUniform(location, x);
    }

    void setUniform(string name, GLuint x, GLuint y) {
        auto location = getUniformLocation(name);
        setUniform(location, x, y);
    }

    void setUniform(string name, GLuint x, GLuint y, GLuint z) {
        auto location = getUniformLocation(name);
        setUniform(location, x, y, z);
    }

    void setUniform(string name, GLuint x, GLuint y, GLuint z, GLuint w) {
        auto location = getUniformLocation(name);
        setUniform(location, x, y, z, w);
    }

    void setUniform(string name, vector<GLfloat> matrix) {
        auto location = getUniformLocation(name);
        setUniform(location, matrix);
    }

    void setTexture(string name, shared_ptr<GLTexture> texture) {
        auto location = getUniformLocation(name);
        bool found = false;
        for (int i = 0; i < textures.size(); i++) {
            if (textures[i].first == location) {
                found = true;
                textures[i].second = texture;
                texture->active(i);
                texture->bind();
                glUniform1i(location, i);
            }
        }
        if (!found) {
            textures.push_back(pair<GLuint, shared_ptr<GLTexture>>(location, texture));
        }
    }
};
}
