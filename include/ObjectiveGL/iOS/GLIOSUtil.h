//
//  GLPlatform.h
//  Starfall
//
//  Created by lingtonke on 2019/3/20.
//  Copyright © 2019 Starfall. All rights reserved.
//

#include <GLKit/GLKit.h>
#include "../GLPlatform.h"
#include <OpenGLES/EAGL.h>
#include "../GLContext.h"
#include "../GLTexture.h"
#include <UIKit/UIKit.h>
#include <map>

using namespace ObjectiveGL;

static map<GLShareGroupID,EAGLSharegroup*> shareGroups;
static map<GLContextID,EAGLContext*> contexts;

class GLIOSUtil {
public:
    static GLShareGroupID createShareGroup() {
        GLShareGroupID glid = rand();
        shareGroups[glid] = [[EAGLSharegroup alloc] init];
        return glid;
    }
    
    static void releaseShareGroup(GLShareGroupID shareGroupID) {
        shareGroups.erase(shareGroupID);
    }
    
    static GLContextID createContext(GLShareGroupID shareGroupID) {
        GLContextID glid = rand();
        EAGLContext *context;
        if (!shareGroupID) {
            context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
        }
        else {
            context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3 sharegroup:shareGroups[shareGroupID]];
        }
        contexts[glid] = context;
        return glid;
    }
    
    static void setContext(GLContextID contextID) {
        EAGLContext *eaglcontext = contexts[contextID];
        [EAGLContext setCurrentContext:eaglcontext];
    }
    
    static void releaseContext(GLContextID contextID) {
        contexts.erase(contextID);
    }
    
    static shared_ptr<GLTexture> createTextureFromUIImage(UIImage *image) {
        GLsizei width = (GLsizei)CGImageGetWidth(image.CGImage);
        GLsizei height = (GLsizei)CGImageGetHeight(image.CGImage);
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        CGContext *context = CGBitmapContextCreate(nil, width, height, 8, 4*width, colorSpace, kCGBitmapByteOrder32Host | kCGImageAlphaPremultipliedFirst);
        //CoreGraphic的坐标系和OpenGL的是颠倒的，要转回来
        CGContextTranslateCTM(context, 0, height);
        CGContextScaleCTM(context, 1.0, -1.0);
        CGContextDrawImage(context, CGRectMake(0, 0, width, height), image.CGImage);
        const void *bitmapdata = CGBitmapContextGetData(context);
        auto texture = GLTexture::create();
        texture->setImageData(bitmapdata, width, height,GL_RGBA,GL_BGRA);
        CGContextRelease(context);
        CGColorSpaceRelease(colorSpace);
        
        return texture;
    }
    
    static string getResourcesPath() {
        return [NSBundle.mainBundle.resourcePath cStringUsingEncoding:NSUTF8StringEncoding];
    }
};


