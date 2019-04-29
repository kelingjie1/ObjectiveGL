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

#ifdef TARGET_OS_IPHONE

#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>

#else

#endif

#else

#include <GLES3/gl3.h>

#endif

#include <memory>
#include <string>

typedef int GLShareGroupID;
typedef int GLContextID;

namespace ObjectiveGL {
using namespace std;

class GLShareGroup;
class GLContext;
class GLTexture;
    
class GLPlatform {
public:
    static GLShareGroupID createShareGroup();
    
    static void releaseShareGroup(GLShareGroupID shareGroupID);

    static GLContextID createContext(GLShareGroupID shareGroupID = 0);

    static void releaseContext(GLContextID contextID);
    
    static void setContext(GLContextID contextID);
    
    static shared_ptr<GLTexture> createTextureFromFile(string file);
    
    static string getResourcesPath();
};
}
