package com.tencent.camerasdk.objectivegl2

import android.graphics.Bitmap
import android.opengl.GLES20
import android.opengl.GLUtils
import java.nio.Buffer

class GLTexture: GLObject() {
    var texId: Int = GLES20.GL_NONE
    private set

    var width: Int = 0
    private set

    var height: Int = 0
    private set

    init {
        val tex = IntArray(1)
        GLES20.glGenTextures(1, tex, 0)
        checkError()

        texId = tex[0]
    }

    /*fun setImageData(pixels: Buffer, width: Int, height: Int, internalformat: Int = GLES20.GL_RGBA, format: Int = GLES20.GL_RGBA) {
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texId)
        checkError()

        GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GLES20.GL_UNSIGNED_BYTE, pixels)
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR)
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR)
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0)

        checkError()
    }*/

    fun setImage(bmp: Bitmap) {
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texId)
        checkError()

        GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGBA, bmp, 0)
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR)
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR)
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE)
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE)
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, GLES20.GL_NONE)

        width = bmp.width
        height = bmp.height

        checkError()
    }

    fun active(index: Int) {
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0 + index)
        checkError()
    }

    fun bind() {
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texId)
        checkError()
    }

    fun use(index: Int) {
        active(index)
        bind()
    }

    override fun destroy() {
        if (texId != GLES20.GL_NONE) {
            val tex: IntArray = intArrayOf(texId)
            GLES20.glDeleteTextures(1, tex, 0)
            texId = GLES20.GL_NONE
        }
    }
}