//
//  GLPlatform.h
//  Starfall
//
//  Created by lingtonke on 2019/3/20.
//  Copyright © 2019 Starfall. All rights reserved.
//

#include "GLPlatform.h"
#include <OpenGLES/EAGL.h>
#include "GLContext.h"

using namespace ObjectiveGL;

shared_ptr<GLShareGroup> GLPlatform::createShareGroup() {
    auto sharegroup = shared_ptr<GLShareGroup>(new GLShareGroup());
    sharegroup->sharegroup = (__bridge_retained void *)[[EAGLSharegroup alloc] init];
    return sharegroup;
}

void *GLPlatform::createContext(GLShareGroup *shareGroup) {
    void *context;
    if (!shareGroup) {
        context = (__bridge_retained void *)[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    }
    else {
        context = (__bridge_retained void *)[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3 sharegroup:(__bridge EAGLSharegroup*)shareGroup->sharegroup];
    }
    return context;
}

void GLPlatform::setContext(void *context) {
    [EAGLContext setCurrentContext:(__bridge EAGLContext*)context];
}

void GLPlatform::releaseContext(void *context) {
    [EAGLContext setCurrentContext:nil];
    CFRelease(context);
}
