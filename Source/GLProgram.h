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

namespace ObjectiveGL
{
    using namespace std;
    
    class GLProgram:public GLObject
    {
        friend class GLContext;
    protected:
        string vertexShaderStr;
        string fragmentShaderStr;
        
        map<GLuint,function<void()>> uniformFunc;
        vector<pair<GLuint, shared_ptr<GLTexture>>> textures;
        
        GLProgram(): programID(0),vertexShaderID(0),fragmentShaderID(0){}
        
        void setUniform(string name,function<void()> func)
        {
            auto location = getUniformLocation(name);
            setUniform(location, func);
        }
        
        void setUniform(GLuint location,function<void()> func)
        {
            check();
            uniformFunc[location] = func;
            func();
            checkError();
        }
        
        
        void setUniformToGL()
        {
            for(auto &uniform:uniformFunc)
            {
                uniform.second();
            }
            for (int i=0; i<textures.size(); i++)
            {
                auto texture = textures[i].second;
                auto location = textures[i].first;
                texture->active(i);
                texture->bind();
                glUniform1i(location, i);
            }
        }
    public:
        GLuint programID;
        GLuint vertexShaderID;
        GLuint fragmentShaderID;
        
        ~GLProgram()
        {
            check();
            GLuint vs = vertexShaderID;
            GLuint fs = fragmentShaderID;
            GLuint p = programID;
            if (vs)
            {
                glDeleteShader(vs);
            }
            if (fs)
            {
                glDeleteShader(fs);
            }
            if (p)
            {
                glDeleteProgram(p);
            }
        }
        
        void loadFromFile(string vertexShaderFile, string fragmentShaderFile)
        {
            auto vs = Util::readFile(vertexShaderFile);
            auto fs = Util::readFile(fragmentShaderFile);
            setShaderString(vs,fs);
        }
        void setShaderString(string vs, string fs)
        {
            vertexShaderStr = vs;
            fragmentShaderStr = fs;
        }
        virtual void init()
        {
            GLint success;
            GLchar infoLog[512];

            const GLchar *const vsStr = vertexShaderStr.c_str();
            const GLchar *const fsStr = fragmentShaderStr.c_str();
            try
            {
                vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
                glShaderSource(vertexShaderID, 1, &vsStr, nullptr);
                glCompileShader(vertexShaderID);
                glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
                if (!success)
                {
                    glGetShaderInfoLog(vertexShaderID, 512, nullptr, infoLog);
                    throw GLError(ObjectiveGLError_VertexShaderCompileFailed,infoLog);
                }

                fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
                glShaderSource(fragmentShaderID, 1, &fsStr, nullptr);
                glCompileShader(fragmentShaderID);
                glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
                if (!success)
                {
                    glGetShaderInfoLog(fragmentShaderID, 512, nullptr, infoLog);
                    throw GLError(ObjectiveGLError_FragmentShaderCompileFailed,infoLog);
                }

                programID = glCreateProgram();
                glAttachShader(programID, vertexShaderID);
                glAttachShader(programID, fragmentShaderID);

                GLint linked;

                glLinkProgram(programID);
                glGetProgramiv(programID, GL_LINK_STATUS, &linked);
                if (!linked)
                {
                    glGetProgramInfoLog(programID, 512, nullptr, infoLog);
                    throw GLError(ObjectiveGLError_ProgramLinkFailed,infoLog);
                }
            }
            catch (GLError error)
            {
                if (vertexShaderID)
                {
                    glDeleteShader(vertexShaderID);
                }
                if (fragmentShaderID)
                {
                    glDeleteShader(fragmentShaderID);
                }
                if (programID)
                {
                    glDeleteProgram(programID);
                }
                throw error;
            }
        }
        
        void use()
        {
            check();
            glUseProgram(programID);
        }
        
        void setUniform(GLuint location,GLfloat x)
        {
            setUniform(location, [=]{
                glUniform1f(location, x);
            });
        }
        
        void setUniform(GLuint location,GLfloat x,GLfloat y)
        {
            setUniform(location, [=]{
                glUniform2f(location, x, y);
            });
            
        }
        
        void setUniform(GLuint location,GLfloat x,GLfloat y,GLfloat z)
        {
            setUniform(location, [=]{
                glUniform3f(location, x, y, z);
            });
        }
        
        void setUniform(GLuint location,GLfloat x,GLfloat y,GLfloat z,GLfloat w)
        {
            setUniform(location, [=]{
                glUniform4f(location, x, y, z, w);
            });
        }
        
        void setUniform(GLuint location,GLint x)
        {
            setUniform(location, [=]{
                glUniform1i(location, x);
            });
            
        }
        
        void setUniform(GLuint location,GLint x,GLint y)
        {
            setUniform(location, [=]{
                glUniform2i(location, x, y);
            });
            
        }
        
        void setUniform(GLuint location,GLint x,GLint y,GLint z)
        {
            setUniform(location, [=]{
                glUniform3i(location, x, y, z);
            });
        }
        
        void setUniform(GLuint location,GLint x,GLint y,GLint z,GLint w)
        {
            setUniform(location, [=]{
                glUniform4i(location, x, y, z, w);
            });
            check();
            
        }
        
        void setUniform(GLuint location,GLuint x)
        {
            setUniform(location, [=]{
                glUniform1ui(location, x);
            });
        }
        
        void setUniform(GLuint location,GLuint x,GLuint y)
        {
            setUniform(location, [=]{
                glUniform2ui(location, x, y);
            });
        }
        
        void setUniform(GLuint location,GLuint x,GLuint y,GLuint z)
        {
            setUniform(location, [=]{
                glUniform3ui(location, x, y, z);
            });
        }
        
        void setUniform(GLuint location,GLuint x,GLuint y,GLuint z,GLuint w)
        {
            setUniform(location, [=]{
                glUniform4ui(location, x, y, z, w);
            });
        }
        
        void setUniform(GLuint location,vector<GLfloat> matrix)
        {
            if (matrix.size()==4)
            {
                setUniform(location, [=]{
                    glUniformMatrix2fv(location, (GLsizei)matrix.size(), false, matrix.data());
                });
            }
            else if (matrix.size()==9)
            {
                setUniform(location, [=]{
                    glUniformMatrix3fv(location, (GLsizei)matrix.size(), false, matrix.data());
                });
            }
            else if (matrix.size()==16)
            {
                setUniform(location, [=]{
                    glUniformMatrix4fv(location, (GLsizei)matrix.size(), false, matrix.data());
                });
            }
            else
            {
                throw GLError(ObjectiveGLError_InvalidData);
            }
        }
        
        GLuint getUniformLocation(string name)
        {
            check();
            auto location = glGetUniformLocation(programID, name.c_str());
            checkError();
            return location;
        }
        
        void setUniform(string name,GLfloat x)
        {
            auto location = getUniformLocation(name);
            setUniform(location, x);
        }
        
        void setUniform(string name,GLfloat x,GLfloat y)
        {
            auto location = getUniformLocation(name);
            setUniform(location, x, y);
        }
        
        void setUniform(string name,GLfloat x,GLfloat y,GLfloat z)
        {
            auto location = getUniformLocation(name);
            setUniform(location, x, y, z);
        }
        
        void setUniform(string name,GLfloat x,GLfloat y,GLfloat z,GLfloat w)
        {
            auto location = getUniformLocation(name);
            setUniform(location, x, y, z, w);
        }
        
        void setUniform(string name,GLint x)
        {
            auto location = getUniformLocation(name);
            setUniform(location, x);
        }
        
        void setUniform(string name,GLint x,GLint y)
        {
            auto location = getUniformLocation(name);
            setUniform(location, x, y);
        }
        
        void setUniform(string name,GLint x,GLint y,GLint z)
        {
            auto location = getUniformLocation(name);
            setUniform(location, x, y, z);
        }
        
        void setUniform(string name,GLint x,GLint y,GLint z,GLint w)
        {
            auto location = getUniformLocation(name);
            setUniform(location, x, y, z, w);
        }
        
        void setUniform(string name,GLuint x)
        {
            auto location = getUniformLocation(name);
            setUniform(location, x);
        }
        
        void setUniform(string name,GLuint x,GLuint y)
        {
            auto location = getUniformLocation(name);
            setUniform(location, x, y);
        }
        
        void setUniform(string name,GLuint x,GLuint y,GLuint z)
        {
            auto location = getUniformLocation(name);
            setUniform(location, x, y, z);
        }
        
        void setUniform(string name,GLuint x,GLuint y,GLuint z,GLuint w)
        {
            auto location = getUniformLocation(name);
            setUniform(location, x, y, z, w);
        }
        
        void setUniform(string name,vector<GLfloat> matrix)
        {
            auto location = getUniformLocation(name);
            setUniform(location, matrix);
        }
        
        void setTexture(string name,shared_ptr<GLTexture> texture)
        {
            auto location = getUniformLocation(name);
            bool found = false;
            for (int i=0; i<textures.size(); i++)
            {
                if (textures[i].first == location)
                {
                    found = true;
                    textures[i].second = texture;
                    texture->active(i);
                    texture->bind();
                    glUniform1i(location, i);
                }
            }
            if (!found)
            {
                textures.push_back(pair<GLuint,shared_ptr<GLTexture>>(location,texture));
            }
        }
    };
}
