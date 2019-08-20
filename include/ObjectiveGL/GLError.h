//
//  GLError.h
//  Starfall
//
//  Created by lingtonke on 2019/3/20.
//  Copyright © 2019 Starfall. All rights reserved.
//

#pragma once

#include "GLPlatform.h"
#include <exception>
#include <string>
#include <assert.h>
#include <stdio.h>



namespace ObjectiveGL {
using namespace std;
enum ObjectiveGLError {
    ObjectiveGLError_NoError,
    ObjectiveGLError_ContextCheckFailed,
    ObjectiveGLError_GLError,
    ObjectiveGLError_InvalidType,
    ObjectiveGLError_ShaderCompileFailed,
    ObjectiveGLError_ProgramLinkFailed,
    ObjectiveGLError_InvalidData,
    ObjectiveGLError_LocationNotFound,
};
    
enum GLLogLevel {
    GLLogLevel_Info,
    GLLogLevel_Warning,
    GLLogLevel_Error,
};

#define OGL_LOG_FORMAT1 "GLError:code %d"
#define OGL_LOG_FORMAT2 "GLError:code %d,glcode %d"
#define OGL_LOG_FORMAT3 "GLError:code %d,glcode %d,%s"
    
    
#define GET_MACRO(_1,_2,_3,NAME,...) NAME
#define OGL_ERROR(...) GET_MACRO(__VA_ARGS__, OGL_ERROR3, OGL_ERROR2, OGL_ERROR1)(__VA_ARGS__)
#if OGL_STOP_WHEN_ERROR

    #ifndef _LIBCPP_NO_EXCEPTIONS

        #define OGL_ERROR1(code) GLLog::logError(OGL_LOG_FORMAT1, code);assert(code)
        #define OGL_ERROR2(code,glcode) GLLog::logError(OGL_LOG_FORMAT2, code, glcode);assert(code)
        #define OGL_ERROR3(code,glcode,log) GLLog::logError(OGL_LOG_FORMAT3, code, glcode, log);assert(code)
    
    #else
    
        #define OGL_ERROR1(code) GLLog::logError(OGL_LOG_FORMAT1, code);throw GLError(code)
        #define OGL_ERROR2(code,glcode) GLLog::logError(OGL_LOG_FORMAT2, code, glcode);throw GLError(code,glcode)
        #define OGL_ERROR3(code,glcode,log) GLLog::logError(OGL_LOG_FORMAT3, code, glcode, log);throw GLError(code,glcode,log)
    
    #endif
#else
    #define OGL_ERROR1(code) GLLog::logError(OGL_LOG_FORMAT1, code)
    #define OGL_ERROR2(code,glcode) GLLog::logError(OGL_LOG_FORMAT2, code, glcode)
    #define OGL_ERROR3(code,glcode,log) GLLog::logError(OGL_LOG_FORMAT3, code, glcode, log)
#endif

    
class OGL_API GLLog {
protected:
    static function<void(GLLogLevel level,const string &log)> &logCallback() {
        static function<void(GLLogLevel level,const string &log)> callback;
        return callback;
    }
    inline static void internallog(GLLogLevel level, const string &log) {
        static string strlever[] = {"Info","Waring","Error"};
        if (logCallback()) {
            logCallback()(level, log);
        }
        else {
            printf("OGL:%s,%s",strlever[level].c_str(),log.c_str());
        }
    }
public:
    static void setCallback(function<void(GLLogLevel level,const string &log)> callback) {
        logCallback() = callback;
    }
    
    inline static void log(GLLogLevel level, const char format[], ...) {
        char buffer[512];
        va_list args;
        va_start(args, format);
        vsprintf(buffer, format, args);
        va_end(args);
        internallog(level, buffer);
    }
    
    inline static void logInfo(const char format[], ...) {
        char buffer[512];
        va_list args;
        va_start(args, format);
        vsprintf(buffer, format, args);
        va_end(args);
        internallog(GLLogLevel_Info, buffer);
    }
    
    inline static void logWarning(const char format[], ...) {
        char buffer[512];
        va_list args;
        va_start(args, format);
        vsprintf(buffer, format, args);
        va_end(args);
        internallog(GLLogLevel_Warning, buffer);
    }
    
    inline static void logError(const char format[], ...) {
        char buffer[512];
        va_list args;
        va_start(args, format);
        vsprintf(buffer, format, args);
        va_end(args);
        internallog(GLLogLevel_Error, buffer);
    }
};

class OGL_API GLError : exception {
public:
    ObjectiveGLError code;
    GLenum glcode;
    string log;

    GLError(ObjectiveGLError code, GLenum glcode = GL_NO_ERROR,string log = "") noexcept {
        this->code = code;
        this->glcode = glcode;
        this->log = log;
    };

    virtual const char *what() const noexcept { return log.c_str(); }
};
}
