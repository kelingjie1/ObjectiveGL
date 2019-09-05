//
//  GLContext.h
//  ObjectiveGL
//
//  Created by lingtonke on 2018/10/9.
//  Copyright © 2018 tencent. All rights reserved.
//
#pragma once

#include "../GLPlatform.h"
#include "../GLError.h"
#include "../GLContext.h"
#include "GLIOSUtil.h"
#include <iostream>

namespace ObjectiveGL {
    using namespace std;
    
    class GLIOSShareGroup:public GLShareGroup {
        GLIOSShareGroup(GLShareGroupID shareGroupID):GLShareGroup(shareGroupID) {
            
        };
    public:
        static shared_ptr<GLShareGroup> create() {
            auto sharegroup = new GLIOSShareGroup(GLIOSUtil::createShareGroup());
            return shared_ptr<GLShareGroup>(sharegroup);
        }
        
        virtual ~GLIOSShareGroup() {
            GLIOSUtil::releaseShareGroup(shareGroupID);
        }
    };
    
    class GLIOSContext;
    
    class GLIOSContext : public GLContext {
    protected:
        GLIOSContext(GLContextID contextID, shared_ptr<GLShareGroup> sharegroup):GLContext(contextID,sharegroup) {
            
        }
    public:
        
        virtual ~GLIOSContext() {
            GLIOSUtil::releaseContext(contextID);
        }
        
        static shared_ptr<GLContext> create(shared_ptr<GLShareGroup> sharegroup = nullptr) {
            auto context = shared_ptr<GLContext>(new GLIOSContext(GLIOSUtil::createContext(sharegroup? sharegroup->shareGroupID:0),sharegroup));
            return context;
        }
        
        virtual void setCurrent() {
            auto s = shared_from_this();
            GLIOSUtil::setContext(contextID);
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
                OGL_ERROR(ObjectiveGLError_ContextCheckFailed);
            }
        }
    };
}
