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
#include "GLTexture.h"
#include <UIKit/UIKit.h>

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

shared_ptr<GLTexture> GLPlatform::createTextureFromFile(string file) {
    NSString *path = [NSString stringWithUTF8String:file.c_str()];
    UIImage *image = [UIImage imageWithContentsOfFile:path];
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

string GLPlatform::getResourcesPath() {
    return [NSBundle.mainBundle.resourcePath cStringUsingEncoding:NSUTF8StringEncoding];
}
