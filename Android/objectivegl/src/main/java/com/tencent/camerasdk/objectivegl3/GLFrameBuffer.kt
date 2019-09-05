package com.tencent.camerasdk.objectivegl3

import android.opengl.GLES30


class GLFrameBuffer: GLObject {
    override fun destroy() {
        if (fbo != GLES30.GL_NONE) {
            GLES30.glDeleteFramebuffers(1, intArrayOf(fbo), 0)
            fbo = -1
        }
    }

    var isBackend = false
    private set

    var fbo = GLES30.GL_NONE
    private set

    constructor(backendFbo: Int): super() {
        if (backendFbo < 0) {
            val a = IntArray(1)
            GLES30.glGenFramebuffers(1, a, 0)
            checkError()

            fbo = a[0]

        } else {
            fbo = backendFbo
            isBackend = true
        }
    }

    fun draw(program: GLProgram, vao: GLVertexArray, option: GLOption) {
        GLES30.glBindFramebuffer(GLES30.GL_FRAMEBUFFER, fbo)
        checkError()

        program.use()
        option.apply()
        vao.draw()
    }

    fun clear(red: Float, green: Float, blue: Float, alpha: Float) {
        GLES30.glBindFramebuffer(GLES30.GL_FRAMEBUFFER, fbo)
        checkError()

        GLES30.glClearColor(red, green, blue, alpha)
        GLES30.glClear(GLES30.GL_COLOR_BUFFER_BIT)
    }

    fun bindTexture(textures: Array<GLTexture>) {
        GLES30.glBindFramebuffer(GLES30.GL_FRAMEBUFFER, fbo)

        val buf = IntArray(textures.size)
        for (i in textures.indices) {
            buf[i] = GLES30.GL_COLOR_ATTACHMENT0 + i
            GLES30.glFramebufferTexture2D(GLES30.GL_FRAMEBUFFER, GLES30.GL_COLOR_ATTACHMENT0 + i, GLES30.GL_TEXTURE_2D, textures[i].texId, 0)
        }

        GLES30.glDrawBuffers(textures.size, buf, 0)
    }

    fun bindTexture(texture: GLTexture) {
        bindTexture(arrayOf(texture))
    }

    /*fun setRenderBuffer(shared_ptr<GLRenderBuffer> renderBuffer) {
        glBindBuffer(GL_FRAMEBUFFER, frameBufferID);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER,
            renderBuffer->renderBufferID);
        GLenum buf = GL_COLOR_ATTACHMENT0;
        glDrawBuffers(1, &buf);
    }*/
}

class GLOption: GLObject() {
    override fun destroy() {
    }

    var enableBlend = false
    var srcBlendFactor = GLES30.GL_SRC_ALPHA
    var dstBlendFactor = GLES30.GL_ONE_MINUS_SRC_ALPHA

    var enableDepthTest = false

    fun apply() {
        if (enableBlend) {
            GLES30.glEnable(GLES30.GL_BLEND)
            GLES30.glBlendFunc(srcBlendFactor, dstBlendFactor)

        } else GLES30.glDisable(GLES30.GL_BLEND)

        if (enableDepthTest) GLES30.glEnable(GLES30.GL_DEPTH_TEST) else GLES30.glDisable(GLES30.GL_DEPTH_TEST)

        checkError()
    }
}