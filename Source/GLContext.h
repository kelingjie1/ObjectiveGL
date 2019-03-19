//
//  GLContext.h
//  GPUPower
//
//  Created by lingtonke on 2018/10/9.
//  Copyright © 2018 tencent. All rights reserved.
//
#pragma once

#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#include "../NodeChain/TaskQueue.h"
#include "../Error.h"
#include "../Platform/GPUPowerPlatform.h"
#include <iostream>

namespace GPUPower
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
            return GPUPowerIOSBridge::createShareGroup();
        }
    };
    
    class GLContext;
    
    class GLContext:public enable_shared_from_this<GLContext>
    {
        NodeChain::TaskQueue taskQueue;
        
        static weak_ptr<GLContext> &currentContext()
        {
            static thread_local weak_ptr<GLContext> context;
            return context;
        };
        
        GLContext(shared_ptr<GLShareGroup> sharegroup):sharegroup(sharegroup),isMainThreadContext(false)
        {
            context = GPUPowerIOSBridge::createContext(sharegroup.get(), this);
            cout<<"GLContext("<<this<<")"<<endl;
        }
        void init()
        {
            auto s = shared_from_this();
            if (isMainThreadContext)
            {
                GPUPowerIOSBridge::setContext(this);
                currentContext() = s;
            }
            else
            {
                taskQueue.start();
                taskQueue.addTask([=]
                                  {
                                      GPUPowerIOSBridge::setContext(this);
                                      currentContext() = s;
                                  });
            }
            
        }
    public:
        bool isMainThreadContext;
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
        
        static shared_ptr<GLContext> createMainThreadContext(shared_ptr<GLShareGroup> sharegroup = nullptr)
        {
            auto context = shared_ptr<GLContext>(new GLContext(sharegroup));
            context->isMainThreadContext = true;
            context->init();
            return context;
        }
        
        
        
        ~GLContext()
        {
            GPUPowerIOSBridge::releaseContext(this);
            cout<<"~GLContext("<<this<<")"<<endl;
        }

        void check(bool share=false)
        {
            bool failed = false;
            if (isMainThreadContext)
            {
                if (!GPUPowerIOSBridge::isMainThread())
                {
                    failed = true;
                }
            }
            else
            {
                if (this_thread::get_id() != taskQueue.getid())
                {
                    if(share&&sharegroup&&GLContext::current()->sharegroup == sharegroup)
                    {
                        //sharegroup
                    }
                    else
                    {
                        failed = true;
                    }
                }
            }
            if (failed)
            {
                throw Error(GPUPowerError_ContextCheckFailed);
            }
            
        }

        void asyncTask(function<void()> func)
        {
            if (isMainThreadContext)
            {
                GPUPowerIOSBridge::addMainThreadTask(func);
            }
            else
            {
                taskQueue.addTask(func);
            }
        }
        void checkAndAsyncTask(function<void()> func)
        {
            if (isMainThreadContext)
            {
                if (GPUPowerIOSBridge::isMainThread())
                {
                    func();
                }
                else
                {
                    GPUPowerIOSBridge::addMainThreadTask(func);
                }
            }
            else
            {
                if (this_thread::get_id() == taskQueue.getid())
                {
                    func();
                }
                else
                {
                    taskQueue.addTask(func);
                }
            }
        }
    };
}
