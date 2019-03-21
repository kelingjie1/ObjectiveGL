//
//  GLPlatform.h
//  Starfall
//
//  Created by lingtonke on 2019/3/20.
//  Copyright © 2019 Starfall. All rights reserved.
//

#pragma once

#if IOS

#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>

#else

#include <GLES3/gl3.h>

#endif

#include <memory>

namespace ObjectiveGL {
using namespace std;

class GLShareGroup;

class GLContext;

class GLPlatform {
public:
    static shared_ptr<GLShareGroup> createShareGroup();

    static void *createContext(GLContext *context, GLShareGroup *shareGroup);

    static void setContext(void *context);

    static void releaseContext(void *context);

};
}
