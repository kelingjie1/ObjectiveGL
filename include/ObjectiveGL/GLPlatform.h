//
//  GLPlatform.h
//  Starfall
//
//  Created by lingtonke on 2019/3/20.
//  Copyright © 2019 Starfall. All rights reserved.
//

#pragma once
#ifdef QT
#include <QtGui/qopengl.h>
#elif __APPLE__

#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>

#else

#include <GLES3/gl3.h>

#endif

#include <memory>
#include <string>


