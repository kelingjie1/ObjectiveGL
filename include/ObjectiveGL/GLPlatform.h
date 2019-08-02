//
//  GLPlatform.h
//  Starfall
//
//  Created by lingtonke on 2019/3/20.
//  Copyright © 2019 Starfall. All rights reserved.
//

#pragma once
#ifdef QT

#include <QOpenGLExtraFunctions>

#define OGL(x) QOpenGLContext::currentContext()->extraFunctions()->x

#elif __APPLE__

#ifdef ES3

#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>

#else

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#endif

#define OGL(x) x

#else

#ifdef ES3

#include <GLES3/gl3.h>

#else

#include <GLES2/gl2.h>

#endif

#define OGL(x) x

#endif

#include <memory>
#include <string>


