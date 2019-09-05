package com.tencent.camerasdk.objectivegl2

import android.opengl.GLES20
import android.util.Log


private val Pair<Int, Int>.width: Int
    get() {
        return this.first
    }

private val Pair<Int, Int>.height : Int
    get() {
        return this.second
    }

class GLFrameBuffer: GLObject {
    private val TAG = "GLFrameBuffer-${Integer.toHexString(this.hashCode())}"

    override fun destroy() {
        if (!isBackend && fbo != GLES20.GL_NONE) {
            GLES20.glDeleteFramebuffers(1, intArrayOf(fbo), 0)
            fbo = -1
        }
    }

    var isBackend = false
    private set

    var fbo = -1
    private set

    constructor(backendFbo: Int): super() {
        if (backendFbo < 0) {
            val a = IntArray(1)
            GLES20.glGenFramebuffers(1, a, 0)
            checkError()

            fbo = a[0]

        } else {
            fbo = backendFbo
            isBackend = true
        }
    }

    fun draw(program: GLProgram, vao: GLVertexArray, option: GLOption, dimension: Pair<Int, Int>) {
        if (fbo != 0) Log.w(TAG, "draw: fbo=$fbo, but doesn't specify texture")

        if (!bindSelf()) return

        GLES20.glViewport(0, 0, dimension.width, dimension.height)

        program.use()
        vao.apply(program)

        option.apply()

        GLES20.glDrawArrays(vao.primitives, 0, vao.vertexCount)
        GLES20.glFlush()
    }

    fun draw(program: GLProgram, vao: GLVertexArray, option: GLOption, dst: GLTexture) {
        if (!bindSelf()) return

        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0, GLES20.GL_TEXTURE_2D, dst.texId, 0)
        GLES20.glViewport(0, 0, dst.width, dst.height)

        program.use()
        vao.apply(program)

        option.apply()

        GLES20.glDrawArrays(vao.primitives, 0, vao.vertexCount)
        GLES20.glFlush()
    }

    fun clear(red: Float, green: Float, blue: Float, alpha: Float) {
//        if (!bindSelf()) return

        GLES20.glClearColor(red, green, blue, alpha)
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT)
    }

    /*fun bindTexture(texture: GLTexture) {
        if (!bindSelf()) return

        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0, GLES20.GL_TEXTURE_2D, texture.texId, 0)
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0)
    }*/

    private fun bindSelf(): Boolean {
        if (fbo < 0) {
            Log.e(TAG, "bindSelf: invalid fbo=$fbo")
            return false
        }

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, fbo)
        checkError()

        return true
    }
}

class GLOption: GLObject() {
    override fun destroy() {
    }

    var enableBlend = false
    var srcBlendFactor = GLES20.GL_SRC_ALPHA
    var dstBlendFactor = GLES20.GL_ONE_MINUS_SRC_ALPHA

    var enableDepthTest = false

    fun apply() {
        if (enableBlend) {
            GLES20.glEnable(GLES20.GL_BLEND)

            GLES20.glBlendFunc(srcBlendFactor, dstBlendFactor)
            checkError()

        } else GLES20.glDisable(GLES20.GL_BLEND)

//        if (enableDepthTest) GLES20.glEnable(GLES20.GL_DEPTH_TEST) else GLES20.glDisable(GLES20.GL_DEPTH_TEST)
    }
}