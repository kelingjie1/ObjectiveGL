//
//  GLPlatform.h
//  Starfall
//
//  Created by lingtonke on 2019/3/20.
//  Copyright © 2019 Starfall. All rights reserved.
//

#pragma once


#if defined(WIN32)
    #if OGL_IMPLEMENTATION
        #define OGL_API __declspec(dllexport)
    #else
        #define OGL_API __declspec(dllimport)
    #endif
#else
    #define OGL_API __attribute__((visibility("default")))
#endif

#define OGL_NAMESPACE_BEGIN(x) namespace x {
#define OGL_NAMESPACE_END(x) }

#if OGL_STATE_SAVER_ENABLE

#define OGL_SAVE_FRAMEBUFFER GLFrameBufferSaver ogl_framebuffer_saver
#define OGL_SAVE_DRAWOPTION GLDrawOptionSaver ogl_drawoption_saver

#else

#define OGL_SAVE_FRAMEBUFFER
#define OGL_SAVE_DRAWOPTION

#endif

#if ANDROID

    #define OGL_PLATFORM_ANDROID 1
    #define OGL_PLATFORM_MOBILE 1

#elif __APPLE__
    #if __MACH__
    #define OGL_PLATFORM_MAC 1
    #define OGL_PLATFORM_PC 1
    #else
    #define OGL_PLATFORM_IOS 1
    #define OGL_PLATFORM_MOBILE 1
    #endif

#elif WIN32

    #define OGL_PLATFORM_WINDOWS 1
    #define OGL_PLATFORM_PC 1

#endif


#if QT
#include <QOpenGLExtraFunctions>
#define OGL(x) QOpenGLContext::currentContext()->extraFunctions()->x
#else
#define OGL(x) x
#endif

#if OGL_USE_DEFAULT_OPENGL_VERSION
    #if OGL_PLATFORM_MOBILE
    #define OGL_GLVERSION_300_ES 1
    #else
    #define OGL_GLVERSION_330 1
    #endif
#endif

#if OGL_PLATFORM_IOS

    #if OGL_GLVERSION_300_ES

    #include <OpenGLES/ES3/gl.h>
    #include <OpenGLES/ES3/glext.h>

    #elif OGL_GLVERSION_200_ES

    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>

    #else

    #error OpenGL Version Not Surpport

    #endif
#elif OGL_PLATFORM_ANDROID

#include "android/log.h"

    #if OGL_GLVERSION_300_ES
    #include <GLES3/gl3.h>

    #elif OGL_GLVERSION_200_ES

    #include <GLES2/gl2.h>

    #else

    #error OpenGL Version Not Surpport

    #endif
#elif OGL_PLATFORM_PC

#endif

#if OGL_GLVERSION_300_ES

#define OGL_SHADER_PRIFIX string("#version 300 ES\n")

#elif OGL_GLVERSION_200_ES

#define OGL_SHADER_PRIFIX string("")

#elif OGL_GLVERSION_330

#define OGL_SHADER_PRIFIX string("#version 330\n")

#endif



#include <memory>
#include <string>


