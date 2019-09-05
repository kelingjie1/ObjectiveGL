package com.tencent.camerasdk.objectivegl2

import android.opengl.GLES20
import android.util.Log
import android.util.LogPrinter
import java.nio.Buffer

class GLArrayBuffer(val elementSize: Int, val elementCount: Int, val usage: Int = GLES20.GL_STREAM_DRAW): GLObject() {
    private val TAG = "GLArrayBuffer"

    override fun destroy() {
        if (id != GLES20.GL_NONE) {
            GLES20.glDeleteBuffers(1, intArrayOf(id), 0)
            id = GLES20.GL_NONE
        }
    }

    var id = GLES20.GL_NONE
    private set

    var size = 0
    private set

    private var alloc: Boolean = false

//    var elementCount = 0
//    private set

    init {
        val a = IntArray(1)
        GLES20.glGenBuffers(1, a, 0)
        checkError()
        id = a[0]
    }

    fun bind() {
        if (id == GLES20.GL_NONE) {
            Log.e(TAG, "offer: id is none")
            return
        }

        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, id)
        checkError()
    }

    fun offer(data: Buffer) {
        if (id == GLES20.GL_NONE) {
            Log.e(TAG, "offer: id is none")
            return
        }

        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, id)
        checkError()

        if (!alloc) {
            GLES20.glBufferData(GLES20.GL_ARRAY_BUFFER, elementSize * this.elementCount, data, usage)
            checkError()

            alloc = true

        } else {
            GLES20.glBufferSubData(GLES20.GL_ARRAY_BUFFER, 0, elementSize * this.elementCount, data)
            checkError()
        }

        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, GLES20.GL_NONE)
        checkError()
    }
}