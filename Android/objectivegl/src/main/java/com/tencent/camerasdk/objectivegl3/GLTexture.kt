package com.tencent.camerasdk.objectivegl3

import android.opengl.GLES30
import java.nio.Buffer

class GLTexture: GLObject() {
    var texId: Int = GLES30.GL_NONE
    private set

    init {
        val tex = IntArray(1)
        GLES30.glGenTextures(1, tex, 0)
        checkError()

        texId = tex[0]
    }

    fun setImageData(pixels: Buffer, width: Int, height: Int, internalformat: Int = GLES30.GL_RGBA, format: Int = GLES30.GL_RGBA) {
        GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, texId)
        checkError()

        GLES30.glTexImage2D(GLES30.GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GLES30.GL_UNSIGNED_BYTE, pixels)
        GLES30.glTexParameteri(GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_MIN_FILTER, GLES30.GL_LINEAR)
        GLES30.glTexParameteri(GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_MAG_FILTER, GLES30.GL_LINEAR)
        GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, 0)

        checkError()
    }

    fun active(index: Int) {
        GLES30.glActiveTexture(GLES30.GL_TEXTURE0 + index)
        checkError()
    }

    fun bind() {
        GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, texId)
        checkError();
    }

    fun use(index: Int) {
        active(index)
        bind()
    }

    override fun destroy() {
        if (texId != GLES30.GL_NONE) {
            val tex: IntArray = intArrayOf(texId)
            GLES30.glDeleteTextures(1, tex, 0)
            texId = GLES30.GL_NONE
        }
    }
}