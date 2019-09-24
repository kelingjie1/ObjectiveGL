//
//  GLPlatform.h
//  Starfall
//
//  Created by lingtonke on 2019/3/20.
//  Copyright © 2019 Starfall. All rights reserved.
//

#include "GLContext.h"
#include "GLTexture.h"
#include "EGL/egl.h"
#include <map>

using namespace ObjectiveGL;

static map<GLShareGroupID, EGLContext> shareGroups;
static map<GLContextID, EGLContext> contexts;

static inline EGLContext initEGLContext(EGLContext sharedContext = NULL) {
    auto display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, 0, 0);

    /*
     * Here specify the attributes of the desired configuration.
     * Below, we select an EGLConfig with at least 8 bits per color
     * component compatible with on-screen windows
     */
    const EGLint attribs[] = {EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,  // Request opengl ES2.0
                              EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                              EGL_BLUE_SIZE, 8,
                              EGL_GREEN_SIZE, 8,
                              EGL_RED_SIZE, 8,
                              EGL_DEPTH_SIZE, 0,
                              EGL_ALPHA_SIZE, 8,
                              EGL_STENCIL_SIZE, 8,
                              EGL_NONE};

    EGLint num_configs;
    EGLConfig config;
    eglChooseConfig(display, attribs, &config, 1, &num_configs);

    const EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3,  // Request opengl ES2.0
                                      EGL_NONE};

    auto context = eglCreateContext(display, config, NULL, context_attribs);

    return context;
}

class GLAndUtil {
public:
    // FIXME just share with current egl context...
    static GLShareGroupID createShareGroup() {
        if (!shareGroups.empty()) {
            return shareGroups.begin()->first;
        }

        GLShareGroupID glid = rand();
        shareGroups[glid] = eglGetCurrentContext();
        return glid;
    }

    static GLContextID createContext(GLShareGroupID shareGroupID) {
        GLContextID glid = rand();
        EGLContext context;
        if (!shareGroupID) {
            context = initEGLContext();
        }
        else {
            context = initEGLContext(shareGroups[shareGroupID]);
        }
        contexts[glid] = context;
        return glid;
    }

    static void releaseShareGroup(GLShareGroupID shareGroupID) {
        shareGroups.erase(shareGroupID);
    }
    
    static void setContext(GLContextID contextID) {
        auto eglcontext = contexts[contextID];
        eglMakeCurrent(eglGetDisplay(EGL_DEFAULT_DISPLAY), eglGetCurrentSurface(EGL_DRAW), eglGetCurrentSurface(EGL_READ), eglcontext);
    }
    
    static void releaseContext(GLContextID contextID) {
        auto eglcontext = contexts[contextID];
        eglDestroyContext(eglGetDisplay(EGL_DEFAULT_DISPLAY), eglcontext);

        contexts.erase(contextID);
    }
};


