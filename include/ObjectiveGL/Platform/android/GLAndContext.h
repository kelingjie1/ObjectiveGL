//
// Created by willisdai on 2019-09-10.
//
#pragma once

#include "GLPlatform.h"
#include "GLError.h"
#include "GLContext.h"
#include <EGL/egl.h>
#include "GLAndUtil.h"

namespace ObjectiveGL {
    using namespace std;

    class GLAndShareGroup:public GLShareGroup {
        GLAndShareGroup(GLShareGroupID shareGroupID):GLShareGroup(shareGroupID) {

        };
    public:
        static shared_ptr<GLShareGroup> create() {
            auto sharegroup = new GLAndShareGroup(GLAndUtil::createShareGroup());
            return shared_ptr<GLShareGroup>(sharegroup);
        }

        virtual ~GLAndShareGroup() {
            GLAndUtil::releaseShareGroup(shareGroupID);
        }
    };

    class GLAndContext;

    class GLAndContext : public GLContext {
    protected:
        GLAndContext(GLContextID contextID, shared_ptr<GLShareGroup> sharegroup):GLContext(contextID,sharegroup) {

        }
    public:

        virtual ~GLAndContext() {
            GLAndUtil::releaseContext(contextID);
        }

        // FIXME always share with current context
        static shared_ptr<GLContext> create(/*shared_ptr<GLShareGroup> sharegroup = nullptr*/) {
//            auto context = shared_ptr<GLContext>(new GLAndContext(GLAndUtil::createContext(sharegroup? sharegroup->shareGroupID:0),sharegroup));

            auto shared = GLAndShareGroup::create();
            auto context = shared_ptr<GLContext>(new GLAndContext(GLAndUtil::createContext(shared->shareGroupID), shared));
            return context;
        }

        virtual void setCurrent() {
            auto s = shared_from_this();
            GLAndUtil::setContext(contextID);
            currentContext() = s;
        }

        virtual void check(bool share = false) {
            bool failed = false;
            if (share && sharegroup && GLContext::current()->sharegroup == sharegroup) {
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
