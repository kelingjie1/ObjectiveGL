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

namespace ObjectiveGL {
using namespace std;

class GLTexture;

class GLFrameBuffer;

class GLRenderBuffer;

template<class T>
class GLVertexBuffer;

template<class T>
class GLElementBuffer;

class GLProgram;

class GLBaseVertex;

template<class vboType, class eboType>
class GLVertexArray;

template<class T>
class GLBuffer;

class GLShareGroup {
public:
    static shared_ptr<GLShareGroup> create() {
        return GLPlatform::createShareGroup();
    }
};

class GLContext;

class GLContext : public enable_shared_from_this<GLContext> {

    static shared_ptr<GLContext> &currentContext();

    GLContext(shared_ptr<GLShareGroup> sharegroup);

public:
    shared_ptr<GLShareGroup> sharegroup;
    void *context;

    static shared_ptr<GLContext> current();

    static shared_ptr<GLContext> create(shared_ptr<GLShareGroup> sharegroup = nullptr);

    void setCurrent();

    shared_ptr<GLTexture> createTexture();

    shared_ptr<GLFrameBuffer> createFrameBuffer();

    shared_ptr<GLRenderBuffer> createRenderBuffer();

    shared_ptr<GLProgram> createProgram();

    template<class vboType, class eboType>
    shared_ptr<GLVertexArray<vboType, eboType>> createVertexArray();

    template<class T>
    shared_ptr<GLElementBuffer<T>> createElementBuffer();

    template<class T>
    shared_ptr<GLVertexBuffer<T>> createVertexBuffer();

    ~GLContext();

    void check(bool share = false);
};
}
