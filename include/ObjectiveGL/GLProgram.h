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
#include "GLTexture.h"
#include <string>
#include <memory>
#include <fstream>
#include <vector>
#include <functional>
#include <map>
#define SHADER_STRING(x) #x

OGL_NAMESPACE_BEGIN(ObjectiveGL)
using namespace std;
    
enum GLProgramType {
    GLProgramTypeUnknow,
    GLProgramTypeRender,
    GLProgramTypeTransformFeedback,
};

class OGL_API GLProgram : public GLObject {
    friend class GLContext;

protected:
    GLProgramType type;
    map <GLuint, function<void()>> uniformFunc;
    vector<pair<GLuint, vector<shared_ptr<GLTexture>>>> textureList;

    GLProgram() : vertexShaderID(0), fragmentShaderID(0) {
        programID = GLCHECK(glCreateProgram());
    }

    void setUniform(string name, function<void()> func) {
        auto location = getUniformLocation(name);
        setUniform(location, func);
    }

    void setUniform(GLuint location, function<void()> func) {
        check();
        uniformFunc[location] = func;
    }


    void setUniformToGL() {
        for (auto &uniform:uniformFunc) {
            uniform.second();
        }
        int index = 0;
        for (int i = 0; i < (int)textureList.size(); i++) {
            auto textures = textureList[i].second;
            auto location = textureList[i].first;
            vector<GLint> v;
            for (int i=0; i<(int)textures.size(); i++) {
                auto texture = textures[i];
                if (texture) {
                    v.push_back(index);
                    texture->active(index);
                    texture->bind();
                    index++;
                }
            }
            setUniform(location, v);
            
        }
    }
    GLuint compileShader(string str,GLenum type) {
        
        GLint success;
        GLchar infoLog[512];
        auto c = str.c_str();
        GLuint shader = GLCHECK(glCreateShader(type));
        GLCHECK(glShaderSource(shader, 1, &c, nullptr));
        GLCHECK(glCompileShader(shader));
        GLCHECK(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));
        if (!success) {
            GLCHECK(glGetShaderInfoLog(shader, 512, nullptr, infoLog));
            OGL_ERROR(ObjectiveGLError_ShaderCompileFailed, glGetError(), infoLog);
        }
        
        return shader;
    }

    void link() {
        GLint linked;
        GLchar infoLog[512];
        GLCHECK(glLinkProgram(programID));
        GLCHECK(glGetProgramiv(programID, GL_LINK_STATUS, &linked));
        if (!linked) {
            GLCHECK(glGetProgramInfoLog(programID, 512, nullptr, infoLog));
            OGL_ERROR(ObjectiveGLError_ProgramLinkFailed, glGetError(), infoLog);
        }
    }

public:
    GLuint programID;
    GLuint vertexShaderID;
    GLuint fragmentShaderID;
    
    static shared_ptr<GLProgram> create(function<void(GLProgram *program)> deleter = nullptr) {
        if (deleter) {
            return shared_ptr<GLProgram>(new GLProgram(),deleter);
        }
        else {
            return shared_ptr<GLProgram>(new GLProgram());
        }
    }
    
    void clearTextures() {
        textureList.clear();
    }

    ~GLProgram() {
        check();
        GLuint vs = vertexShaderID;
        GLuint fs = fragmentShaderID;
        GLuint p = programID;
        if (vs) {
            GLCHECK(glDeleteShader(vs));
        }
        if (fs) {
            GLCHECK(glDeleteShader(fs));
        }
        if (p) {
            GLCHECK(glDeleteProgram(p));
        }
    }
    
    GLProgramType getType() {
        return type;
    }

    void setRenderShader(string vs, string fs) {
        vertexShaderID = compileShader(vs, GL_VERTEX_SHADER);
        GLCHECK(glAttachShader(programID, vertexShaderID));
        
        fragmentShaderID = compileShader(fs, GL_FRAGMENT_SHADER);
        GLCHECK(glAttachShader(programID, fragmentShaderID));
        
        link();
        type = GLProgramTypeRender;
    }
#ifdef ES3
    void setTransformFeedbackShader(string vs,vector<const GLchar*> varyings,GLenum bufferMode = GL_INTERLEAVED_ATTRIBS) {
        vertexShaderID = compileShader(vs, GL_VERTEX_SHADER);
        GLCHECK(glAttachShader(programID, vertexShaderID));
        
        fragmentShaderID = compileShader("#version 300 es\nvoid main(){}", GL_FRAGMENT_SHADER);
        GLCHECK(glAttachShader(programID, fragmentShaderID));
        
        GLCHECK(glTransformFeedbackVaryings(programID, (GLsizei)varyings.size(), varyings.data(), bufferMode));
        
        link();
        type = GLProgramTypeTransformFeedback;
    }
#endif


    void use() {
        check();
        GLCHECK(glUseProgram(programID));
        
        setUniformToGL();
    }

    void setUniform(GLuint location, GLfloat x) {
        setUniform(location, [=] {
            GLCHECK(glUniform1f(location, x));
            
        });
    }

    void setUniform(GLuint location, GLfloat x, GLfloat y) {
        setUniform(location, [=] {
            GLCHECK(glUniform2f(location, x, y));
            
        });

    }

    void setUniform(GLuint location, GLfloat x, GLfloat y, GLfloat z) {
        setUniform(location, [=] {
            GLCHECK(glUniform3f(location, x, y, z));
            
        });
    }

    void setUniform(GLuint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
        setUniform(location, [=] {
            GLCHECK(glUniform4f(location, x, y, z, w));
            
        });
    }

    void setUniform(GLuint location, GLint x) {
        setUniform(location, [=] {
            GLCHECK(glUniform1i(location, x));
            
        });

    }

    void setUniform(GLuint location, GLint x, GLint y) {
        setUniform(location, [=] {
            GLCHECK(glUniform2i(location, x, y));
            
        });

    }

    void setUniform(GLuint location, GLint x, GLint y, GLint z) {
        setUniform(location, [=] {
            GLCHECK(glUniform3i(location, x, y, z));
            
        });
    }

    void setUniform(GLuint location, GLint x, GLint y, GLint z, GLint w) {
        setUniform(location, [=] {
            GLCHECK(glUniform4i(location, x, y, z, w));
            
        });
        check();

    }
#ifdef ES3
    void setUniform(GLuint location, GLuint x) {
        setUniform(location, [=] {
            GLCHECK(glUniform1ui(location, x));
            
        });
    }


    void setUniform(GLuint location, GLuint x, GLuint y) {
        setUniform(location, [=] {
            GLCHECK(glUniform2ui(location, x, y));
            
        });
    }

    void setUniform(GLuint location, GLuint x, GLuint y, GLuint z) {
        setUniform(location, [=] {
            GLCHECK(glUniform3ui(location, x, y, z));
            
        });
    }

    void setUniform(GLuint location, GLuint x, GLuint y, GLuint z, GLuint w) {
        setUniform(location, [=] {
            GLCHECK(glUniform4ui(location, x, y, z, w));
            
        });
    }
#endif
    
    void setUniform(GLuint location, vector<GLfloat> v,int size = 1) {
        if (size == 1) {
            setUniform(location, [=] {
                GLCHECK(glUniform1fv(location, (GLsizei)v.size(), v.data()));
                
            });
        }
        else if (size == 2) {
            setUniform(location, [=] {
                GLCHECK(glUniform2fv(location, (GLsizei)v.size(), v.data()));
                
            });
        }
        else if (size == 3) {
            setUniform(location, [=] {
                GLCHECK(glUniform3fv(location, (GLsizei)v.size(), v.data()));
                
            });
        }
        else if (size == 4) {
            setUniform(location, [=] {
                GLCHECK(glUniform4fv(location, (GLsizei)v.size(), v.data()));
                
            });
        }
    }
    
    void setUniform(GLuint location, vector<GLint> v,int size = 1) {
        if (size == 1) {
            setUniform(location, [=] {
                GLCHECK(glUniform1iv(location, (GLsizei)v.size(), v.data()));
                
            });
        }
        else if (size == 2) {
            setUniform(location, [=] {
                GLCHECK(glUniform2iv(location, (GLsizei)v.size(), v.data()));
                
            });
        }
        else if (size == 3) {
            setUniform(location, [=] {
                GLCHECK(glUniform3iv(location, (GLsizei)v.size(), v.data()));
                
            });
        }
        else if (size == 4) {
            setUniform(location, [=] {
                GLCHECK(glUniform4iv(location, (GLsizei)v.size(), v.data()));
                
            });
        }
    }

    void setUniformMatrix(GLuint location, vector<GLfloat> matrix) {
        if (matrix.size() == 4) {
            setUniform(location, [=] {
                GLCHECK(glUniformMatrix2fv(location, 1, false, matrix.data()));
                
            });
        } else if (matrix.size() == 9) {
            setUniform(location, [=] {
                GLCHECK(glUniformMatrix3fv(location, 1, false, matrix.data()));
                
            });
        } else if (matrix.size() == 16) {
            setUniform(location, [=] {
                GLCHECK(glUniformMatrix4fv(location, 1, false, matrix.data()));
                
            });
        } else {
            OGL_ERROR(ObjectiveGLError_InvalidData);
        }
    }

    GLuint getUniformLocation(string name) {
        check();
        auto location = GLCHECK(glGetUniformLocation(programID, name.c_str()));
        
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

#ifdef ES3
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
#endif

    void setUniformMatrix(string name, vector<GLfloat> matrix) {
        auto location = getUniformLocation(name);
        setUniformMatrix(location, matrix);
    }
    
    void setUniform(string name, vector<GLfloat> v, int size = 1) {
        auto location = getUniformLocation(name);
        setUniform(location, v, size);
    }
    
    void setUniform(string name, vector<GLint> v, int size = 1) {
        auto location = getUniformLocation(name);
        setUniform(location, v, size);
    }
    
    void setTexture(string name, shared_ptr<GLTexture> texture) {
        setTextures(name, {texture});
    }

    void setTextures(string name, vector<shared_ptr<GLTexture>> textures) {
        auto location = getUniformLocation(name);
        bool found = false;
        for (int i = 0; i < (int)textureList.size(); i++) {
            if (textureList[i].first == location) {
                found = true;
                textureList[i].second = textures;
            }
        }
        if (!found) {
            textureList.push_back(make_pair(location, textures));
        }
    }
};
OGL_NAMESPACE_END(ObjectiveGL)
