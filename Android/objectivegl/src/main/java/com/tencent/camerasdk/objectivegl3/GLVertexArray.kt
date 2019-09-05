package com.tencent.camerasdk.objectivegl3

import android.opengl.GLES30

class GLVertexArray(var primitives: Int = GLES30.GL_TRIANGLES, var eboTyp: Int = GLES30.GL_UNSIGNED_BYTE, var count: Int = 0): GLObject() {
    private var vao: Int = GLES30.GL_NONE

    init {
        val a = IntArray(1)
        GLES30.glGenVertexArrays(1, a, 0)
        checkError()

        vao = a[0]
    }

    override fun destroy() {
        if (vao != GLES30.GL_NONE) {
            GLES30.glDeleteVertexArrays(1, IntArray(vao), 0)
            checkError()
        }
    }

    fun draw() {
        TODO("not implemented") //To change body of created functions use File | Settings | File Templates.
    }
}

data class VertexAttribute(val location: Int, val dimension: Dimension = Dimension.SCALAR, val type: Int = GLES30.GL_FLOAT, val normalized: Boolean = false) {
    enum class Dimension(val size: Int) {
        SCALAR(1),
        VECTOR2(2),
        VECTOR3(3),
        VECTOR4(4)
    }
}