//
//  GLContext.h
//  ObjectiveGL
//
//  Created by lingtonke on 2018/10/9.
//  Copyright © 2018 tencent. All rights reserved.
//
#pragma once

#include "GLPlatform.h"
#include "GLError.h"
#include <iostream>
#include <cstdlib>

typedef int GLShareGroupID;
typedef int GLContextID;

namespace ObjectiveGL {
using namespace std;

class GLShareGroup {
protected:
    GLShareGroup(GLShareGroupID shareGroupID):shareGroupID(shareGroupID) {
        
    };
public:
    GLShareGroupID shareGroupID;
    static shared_ptr<GLShareGroup> create() {
        auto sharegroup = new GLShareGroup(rand());
        return shared_ptr<GLShareGroup>(sharegroup);
    }
    
    virtual ~GLShareGroup() {
        
    }
};

class GLContext;

class GLContext : public enable_shared_from_this<GLContext> {
protected:
    static shared_ptr<GLContext> &currentContext() {
        static thread_local shared_ptr<GLContext> context;
        return context;
    }

    GLContext(GLContextID contextID, shared_ptr<GLShareGroup> sharegroup):contextID(contextID),sharegroup(sharegroup) {
        
    }

public:
    GLContextID contextID;
    shared_ptr<GLShareGroup> sharegroup;

    
    virtual ~GLContext() {
        
    }

    static shared_ptr<GLContext> current() {
        return currentContext();
    }

    static shared_ptr<GLContext> create(shared_ptr<GLShareGroup> sharegroup = nullptr) {
        auto context = shared_ptr<GLContext>(new GLContext(rand(),sharegroup));
        return context;
    }

    virtual void setCurrent() {
        auto s = shared_from_this();
        currentContext() = s;
    }

    virtual void check(bool share = false) {
        bool failed = false;
        if (share && share && sharegroup && GLContext::current()->sharegroup == sharegroup) {
            //sharegroup
        } else if (GLContext::current().get() != this) {
            failed = true;
        }
        
        if (failed) {
            OGLTHROW(GLError(ObjectiveGLError_ContextCheckFailed));
        }
    }
};
}
