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

namespace ObjectiveGL
{
    using namespace std;
    class GLTexture;
    class GLFrameBuffer;
    class GLRenderBuffer;
    class GLProgram;
    template<class vboType,class eboType>
    class GLVertexArray;
    
    template<class T>
    class GLBuffer;
    class GLShareGroup
    {
    public:
        static shared_ptr<GLShareGroup> create()
        {
            return GLPlatform::createShareGroup();
        }
    };
    
    class GLContext;
    
    class GLContext:public enable_shared_from_this<GLContext>
    {
        
        static weak_ptr<GLContext> &currentContext()
        {
            static thread_local weak_ptr<GLContext> context;
            return context;
        };
        
        GLContext(shared_ptr<GLShareGroup> sharegroup):sharegroup(sharegroup)
        {
            context = GLPlatform::createContext(this,sharegroup.get());
            cout<<"GLContext("<<this<<")"<<endl;
        }
        void init()
        {
            auto s = shared_from_this();
            GLPlatform::setContext(this);
            currentContext() = s;
        }
    public:
        shared_ptr<GLShareGroup> sharegroup;
        void *context;
        static shared_ptr<GLContext> current()
        {
            return currentContext().lock();
        }
        
        static shared_ptr<GLContext> create(shared_ptr<GLShareGroup> sharegroup = nullptr)
        {
            auto context = shared_ptr<GLContext>(new GLContext(sharegroup));
            context->init();
            return context;
        }
        
        ~GLContext()
        {
            GLPlatform::releaseContext(this->context);
            cout<<"~GLContext("<<this<<")"<<endl;
        }

        void check(bool share=false)
        {
            bool failed = false;
            if(share&&share&&sharegroup&&GLContext::current()->sharegroup == sharegroup)
            {
                //sharegroup
            }
            else if (GLContext::current().get() != this)
            {
                failed = true;
            }
            
            if (failed)
            {
                throw GLError(ObjectiveGLError_ContextCheckFailed);
            }
            
        }
    };
}
