//
//  GLProgram.h
//  GPUPower
//
//  Created by lingtonke on 2018/10/8.
//  Copyright © 2018 tencent. All rights reserved.
//

#pragma once

#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#include "../Error.h"
#include "../Util.h"
#include "GLObject.h"
#include <string>
#include <memory>
#include <fstream>
namespace GPUPower
{
    using namespace std;
    class GLProgram:public GLObject
    {
        
        string vertexShaderStr;
        string fragmentShaderStr;
        GLProgram(shared_ptr<GLContext> context): GLObject(context),programID(0),vertexShaderID(0),fragmentShaderID(0){}
    public:
        GLuint programID;
        GLuint vertexShaderID;
        GLuint fragmentShaderID;
        static shared_ptr<GLProgram> create(shared_ptr<GLContext> context)
        {
            return shared_ptr<GLProgram>(new GLProgram(context));
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
                    throw Error(GPUPowerError_VertexShaderCompileFailed,infoLog);
                }

                fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
                glShaderSource(fragmentShaderID, 1, &fsStr, nullptr);
                glCompileShader(fragmentShaderID);
                glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
                if (!success)
                {
                    glGetShaderInfoLog(fragmentShaderID, 512, nullptr, infoLog);
                    throw Error(GPUPowerError_FragmentShaderCompileFailed,infoLog);
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
                    throw Error(GPUPowerError_ProgramLinkFailed,infoLog);
                }
            }
            catch (Error error)
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
        
        virtual void clearup()
        {
            auto c = context.lock();
            GLuint vs = vertexShaderID;
            GLuint fs = fragmentShaderID;
            GLuint p = programID;
            c->checkAndAsyncTask([=]
            {
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
            });
            
            
        }
        
        void use()
        {
            check();
            glUseProgram(programID);
        }
        
        void setUniform(GLuint location,GLfloat x)
        {
            check();
            glUniform1f(location, x);
        }
        
        void setUniform(GLuint location,GLfloat x,GLfloat y)
        {
            check();
            glUniform2f(location, x, y);
        }
        
        void setUniform(GLuint location,GLfloat x,GLfloat y,GLfloat z)
        {
            check();
            glUniform3f(location, x, y, z);
        }
        
        void setUniform(GLuint location,GLfloat x,GLfloat y,GLfloat z,GLfloat w)
        {
            check();
            glUniform4f(location, x, y, z, w);
        }
        
        void setUniform(GLuint location,GLint x)
        {
            check();
            glUniform1i(location, x);
        }
        
        void setUniform(GLuint location,GLint x,GLint y)
        {
            check();
            glUniform2i(location, x, y);
        }
        
        void setUniform(GLuint location,GLint x,GLint y,GLint z)
        {
            check();
            glUniform3i(location, x, y, z);
        }
        
        void setUniform(GLuint location,GLint x,GLint y,GLint z,GLint w)
        {
            check();
            glUniform4i(location, x, y, z, w);
        }
        
        void setUniform(GLuint location,GLuint x)
        {
            check();
            glUniform1ui(location, x);
        }
        
        void setUniform(GLuint location,GLuint x,GLuint y)
        {
            check();
            glUniform2ui(location, x, y);
        }
        
        void setUniform(GLuint location,GLuint x,GLuint y,GLuint z)
        {
            check();
            glUniform3ui(location, x, y, z);
        }
        
        void setUniform(GLuint location,GLuint x,GLuint y,GLuint z,GLuint w)
        {
            check();
            glUniform4ui(location, x, y, z, w);
        }
        
        void setUniform(GLuint location,vector<GLfloat> matrix)
        {
            check();
            if (matrix.size()==4)
            {
                glUniformMatrix2fv(location, (GLsizei)matrix.size(), false, matrix.data());
            }
            else if (matrix.size()==9)
            {
                glUniformMatrix3fv(location, (GLsizei)matrix.size(), false, matrix.data());
            }
            else if (matrix.size()==16)
            {
                glUniformMatrix4fv(location, (GLsizei)matrix.size(), false, matrix.data());
            }
            else
            {
                throw Error(GPUPowerError_InvalidData);
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
            setUniform(location, x,y);
        }
        
        void setUniform(string name,GLfloat x,GLfloat y,GLfloat z)
        {
            auto location = getUniformLocation(name);
            setUniform(location, x,y,z);
        }
        
        void setUniform(string name,GLfloat x,GLfloat y,GLfloat z,GLfloat w)
        {
            auto location = getUniformLocation(name);
            setUniform(location, x,y,z,w);
        }
        
        void setUniform(string name,GLint x)
        {
            auto location = getUniformLocation(name);
            setUniform(location, x);
        }
        
        void setUniform(string name,GLint x,GLint y)
        {
            auto location = getUniformLocation(name);
            setUniform(location, x,y);
        }
        
        void setUniform(string name,GLint x,GLint y,GLint z)
        {
            auto location = getUniformLocation(name);
            setUniform(location, x,y,z);
        }
        
        void setUniform(string name,GLint x,GLint y,GLint z,GLint w)
        {
            auto location = getUniformLocation(name);
            setUniform(location, x,y,z,w);
        }
        
        void setUniform(string name,GLuint x)
        {
            auto location = getUniformLocation(name);
            setUniform(location, x);
        }
        
        void setUniform(string name,GLuint x,GLuint y)
        {
            auto location = getUniformLocation(name);
            setUniform(location, x,y);
        }
        
        void setUniform(string name,GLuint x,GLuint y,GLuint z)
        {
            auto location = getUniformLocation(name);
            setUniform(location, x,y,z);
        }
        
        void setUniform(string name,GLuint x,GLuint y,GLuint z,GLuint w)
        {
            auto location = getUniformLocation(name);
            setUniform(location, x,y,z,w);
        }
        
        void setUniform(string name,vector<GLfloat> matrix)
        {
            auto location = getUniformLocation(name);
            setUniform(location, matrix);
        }
    };
}
