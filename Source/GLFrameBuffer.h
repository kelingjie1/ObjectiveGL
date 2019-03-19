//
//  GLFrameBuffer.h
//  ObjectiveGL
//
//  Created by lingtonke on 2018/10/8.
//  Copyright © 2018 tencent. All rights reserved.
//

#pragma once

#include "GLTexture.h"
#include "GLRenderBuffer.h"
#include "GLObject.h"
#include <memory>
#include <vector>
namespace ObjectiveGL
{
    using namespace std;
    class GLFrameBuffer:public GLObject
    {
        GLFrameBuffer(shared_ptr<GLContext> context):GLObject(context){};
    public:
        GLuint frameBufferID;
        static shared_ptr<GLFrameBuffer> create(shared_ptr<GLContext> context)
        {
            return shared_ptr<GLFrameBuffer>(new GLFrameBuffer(context));
        }
        virtual void init()
        {
            glGenBuffers(1, &frameBufferID);
        }
        virtual void cleanup()
        {
            glDeleteBuffers(1, &frameBufferID);
        }
        
        
        void setColorTextures(vector<shared_ptr<GLTexture> > textures)
        {
            glBindBuffer(GL_FRAMEBUFFER, frameBufferID);
            vector<GLenum> bufs;
            for (int i=0; i<textures.size(); i++)
            {
                bufs.push_back(GL_COLOR_ATTACHMENT0+i);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, textures[i]->textureID, 0);
            }
            glDrawBuffers((GLsizei)textures.size(), bufs.data());
        }
        
        void setColorTexture(shared_ptr<GLTexture> texture)
        {
            vector<shared_ptr<GLTexture>> vec;
            vec.push_back(texture);
            setColorTextures(vec);
        }
        
        void setRenderBuffer(shared_ptr<GLRenderBuffer> renderBuffer)
        {
            glBindBuffer(GL_FRAMEBUFFER, frameBufferID);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderBuffer->renderBufferID);
            GLenum buf = GL_COLOR_ATTACHMENT0;
            glDrawBuffers(1, &buf);
        }
        
    };
}
