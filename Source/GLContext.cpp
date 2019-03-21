
#include "GLContext.h"
#include "GLTexture.h"
#include "GLBuffer.h"
#include "GLProgram.h"
#include "GLFrameBuffer.h"
#include "GLRenderBuffer.h"

using namespace ObjectiveGL;
shared_ptr<GLContext> &GLContext::currentContext()
{
    static thread_local shared_ptr<GLContext> context;
    return context;
};

GLContext::GLContext(shared_ptr<GLShareGroup> sharegroup):sharegroup(sharegroup)
{
    context = GLPlatform::createContext(this,sharegroup.get());
    cout<<"GLContext("<<this<<")"<<endl;
}
void GLContext::setCurrent()
{
    auto s = shared_from_this();
    GLPlatform::setContext(this);
    currentContext() = s;
}
shared_ptr<GLContext> GLContext::current()
{
    return currentContext();
}

shared_ptr<GLContext> GLContext::create(shared_ptr<GLShareGroup> sharegroup)
{
    auto context = shared_ptr<GLContext>(new GLContext(sharegroup));
    return context;
}

shared_ptr<GLTexture> GLContext::createTexture()
{
    check();
    return shared_ptr<GLTexture>(new GLTexture());
}

shared_ptr<GLFrameBuffer> GLContext::createFrameBuffer()
{
    check();
    return shared_ptr<GLFrameBuffer>(new GLFrameBuffer());
}

shared_ptr<GLRenderBuffer> GLContext::createRenderBuffer()
{
    check();
    return shared_ptr<GLRenderBuffer>(new GLRenderBuffer());
}

shared_ptr<GLProgram> GLContext::createProgram()
{
    check();
    return shared_ptr<GLProgram>(new GLProgram());
}

template<class vboType,class eboType>
shared_ptr<GLVertexArray<vboType,eboType>> GLContext::createVertexArray()
{
    check();
    return shared_ptr<GLVertexArray<vboType,eboType>>(new GLVertexArray<vboType,eboType>());
}

template<class T>
shared_ptr<GLElementBuffer<T>> GLContext::createElementBuffer()
{
    check();
    return shared_ptr<GLElementBuffer<T>>(new GLElementBuffer<T>());
}

template<class T>
shared_ptr<GLVertexBuffer<T>> GLContext::createVertexBuffer()
{
    check();
    return shared_ptr<GLVertexBuffer<T>>(new GLVertexBuffer<T>());
}

GLContext::~GLContext()
{
    GLPlatform::releaseContext(this->context);
    cout<<"~GLContext("<<this<<")"<<endl;
}

void GLContext::check(bool share)
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

