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

#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#define OGL(x) x

#else

#include <GLES3/gl3.h>
#define OGL(x) x

#endif

#include <memory>
#include <string>


