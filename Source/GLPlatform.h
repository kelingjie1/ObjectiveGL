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
#include <string>

namespace ObjectiveGL {
using namespace std;

class GLShareGroup;
class GLContext;
class GLTexture;
    
class GLPlatform {
public:
    static shared_ptr<GLShareGroup> createShareGroup();

    static void *createContext(GLShareGroup *shareGroup);

    static void setContext(void *context);

    static void releaseContext(void *context);
    
    static shared_ptr<GLTexture> createTextureFromFile(string file);
    
    static string getResourcesPath();
};
}
