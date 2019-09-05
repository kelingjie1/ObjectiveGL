package com.tencent.camerasdk.objectivegl2

import android.opengl.GLES20
import android.util.Log

class GLVertexArray(var primitives: Int = GLES20.GL_TRIANGLES/*, var eboTyp: Int = GLES20.GL_UNSIGNED_BYTE, var count: Int = 0*/): GLObject() {
    private val TAG = "GLVerterxArray-${Integer.toHexString(this.hashCode())}"

    var attributes = listOf<GLVertexAttribute>()
    var arrayBuffer: GLArrayBuffer? = null

    val vertexCount: Int
    get() = arrayBuffer?.elementCount?:0

    override fun destroy() {
    }

    fun apply(program: GLProgram) {
        if (arrayBuffer == null) {
            Log.e(TAG, "apply: array buffer is null")
            return
        }

        arrayBuffer!!.bind()

        var offset = 0
        attributes.forEach { attr ->
            if (attr.location == -1) {
                attr.location = GLES20.glGetAttribLocation(program.programId, attr.name)
                checkError()
            }

            GLES20.glVertexAttribPointer(attr.location, attr.dimension.size, attr.type, attr.normalized, arrayBuffer!!.elementSize, offset)
            GLES20.glEnableVertexAttribArray(attr.location)
            checkError()

            offset += sizeOf(attr.type) * attr.dimension.size
        }

        val max = IntArray(1)
        GLES20.glGetIntegerv(GLES20.GL_MAX_VERTEX_ATTRIBS, max, 0)
        for (i in attributes.size until max[0]) {
            GLES20.glDisableVertexAttribArray(i)
        }
    }

    private fun sizeOf(type: Int): Int {
        return when (type) {
            GLES20.GL_BYTE -> 1
            GLES20.GL_UNSIGNED_BYTE -> 1
            GLES20.GL_SHORT -> 2
            GLES20.GL_UNSIGNED_SHORT -> 2
            GLES20.GL_FIXED -> 4
            GLES20.GL_FLOAT -> 4
            else -> throw IllegalArgumentException("invalid type=$type")
        }
    }
}

data class GLVertexAttribute(val name: String, val dimension: Dimension = Dimension.SCALAR, val type: Int = GLES20.GL_FLOAT, val normalized: Boolean = false) {
    var location: Int = -1

    enum class Dimension(val size: Int) {
        SCALAR(1),
        VECTOR2(2),
        VECTOR3(3),
        VECTOR4(4)
    }
}