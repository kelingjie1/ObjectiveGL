//
//  GLObject.h
//  GPUPower
//
//  Created by lingtonke on 2018/10/11.
//  Copyright © 2018 tencent. All rights reserved.
//
#pragma once
#include "GLContext.h"
#include <memory>
namespace GPUPower
{
    using namespace std;
    class GLObject
    {
    public:
        bool ready;
        weak_ptr<GLContext> context;
    protected:
        GLObject(shared_ptr<GLContext> context):context(weak_ptr<GLContext>(context)),ready(false)
        {

        }
        virtual ~GLObject()
        {
            
        }

    public:
        virtual void init()
        {
            auto c = context.lock();
            c->check();
            ready = true;
        }
        void check(bool share=false)
        {
            auto c = context.lock();
            c->check(share);
            checkInit();
        }
        
        void checkError()
        {
            auto error = glGetError();
            if (error)
            {
                //GL_NO_ERROR;
                throw Error(GPUPowerError_GLError,error);
            }
        }
        
        void checkInit()
        {
            if (!ready)
            {
                init();
            }
        }
        virtual void cleanup()
        {
            
        }

    };
}
