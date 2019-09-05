package com.tencent.camerasdk.objectivegl2

import android.opengl.GLES20
import android.util.Log
import android.util.SparseArray
import java.nio.FloatBuffer
import java.nio.IntBuffer


class GLProgram: GLObject() {
    private val uniformFunc: SparseArray<() -> Unit> = SparseArray()
    private val TAG = "GLProgram-${Integer.toHexString(hashCode())}"

    private var fs: Int = GLES20.GL_NONE
    private var vs: Int = GLES20.GL_NONE

    var programId = GLES20.GL_NONE
    private set

    private val textureList = SparseArray<GLTexture>()

    private val uniformMap = mutableMapOf<String, Int>()

    fun setShader(vertexShader: String, fragShader: String) {
        if (programId != GLES20.GL_NONE) {
            Log.d(TAG, "setShader: already link to program #$programId")
        }

        vs = compileShader(vertexShader, GLES20.GL_VERTEX_SHADER)
        fs = compileShader(fragShader, GLES20.GL_FRAGMENT_SHADER)

        if (vs != GLES20.GL_NONE && fs != GLES20.GL_NONE) {
            programId = GLES20.glCreateProgram()
            if (!checkError()) {
                destroy()
                return
            }

            GLES20.glAttachShader(programId, vs)
            if (!checkError()) {
                destroy()
                return
            }

            GLES20.glAttachShader(programId, fs)
            if (!checkError()) {
                destroy()
                return
            }

            GLES20.glLinkProgram(programId)
            if (!checkError()) {
                destroy()
                return
            }

        } else {
            destroy()
            return
        }
    }

    fun use() {
        if (programId == GLES20.GL_NONE) {
            throw RuntimeException("invalid program")
        }

        GLES20.glUseProgram(programId)
        checkError()

        applyUniforms()
    }

    private fun applyUniforms() {
        var index = 0
        for (i in 0 until textureList.size()) {
            val loc = textureList.keyAt(i)

            textureList.get(loc).use(index)
//            setUniform(loc, index)
            checkError()

            ++index
        }

        for (k in 0 until uniformFunc.size()) {
            uniformFunc[uniformFunc.keyAt(k)]()
        }
    }

    fun setUniform(loc: Int, x: Float) {
        uniformFunc.put(loc) {
            GLES20.glUniform1f(loc, x)
            checkError()
        }
    }

    fun setUniform(loc: Int, x: Float, y: Float) {
        uniformFunc.put(loc) {
            GLES20.glUniform2f(loc, x, y)
            checkError()
        }
    }

    fun setUniform(loc: Int, x: Float, y: Float, z: Float) {
        uniformFunc.put(loc) {
            GLES20.glUniform3f(loc, x, y, z)
            if (!checkError()) {
//                throw java.lang.RuntimeException("setUniform3f error")
            }
        }
    }

    fun setUniform(loc: Int, x: Float, y: Float, z: Float, w: Float) {
        uniformFunc.put(loc) {
            GLES20.glUniform4f(loc, x, y, z, w)
            checkError()
        }
    }

    fun setUniform(loc: Int, x: Int) {
        uniformFunc.put(loc) {
            GLES20.glUniform1i(loc, x)
            checkError()
        }
    }

    fun setUniform(loc: Int, x: Int, y: Int) {
        uniformFunc.put(loc) {
            GLES20.glUniform2i(loc, x, y)
            checkError()
        }
    }

    fun setUniform(loc: Int, x: Int, y: Int, z: Int) {
        uniformFunc.put(loc) {
            GLES20.glUniform3i(loc, x, y, z)
            checkError()
        }
    }

    fun setUniform(loc: Int, x: Int, y: Int, z: Int, w: Int) {
        uniformFunc.put(loc) {
            GLES20.glUniform4i(loc, x, y, z, w)

            checkError()
        }
    }

    fun setUniform(loc: Int, v: FloatArray, vectorSize: Int = 1) {
        if (v.size < vectorSize || v.size % vectorSize != 0) {
            throw java.lang.RuntimeException("invalid size")
        }

        when (vectorSize) {
            1 -> uniformFunc.put(loc) { GLES20.glUniform1fv(loc, v.size / vectorSize, v, 0) }

            2 -> uniformFunc.put(loc) { GLES20.glUniform2fv(loc, v.size / vectorSize, v, 0) }

            3 -> uniformFunc.put(loc) { GLES20.glUniform3fv(loc, v.size / vectorSize, v, 0) }

            4 -> uniformFunc.put(loc) { GLES20.glUniform4fv(loc, v.size / vectorSize, v, 0) }
        }

        if (!checkError()) {
//            throw java.lang.RuntimeException("setUniform${vectorSize}fv error")
        }
    }

    fun setUniform(loc: Int, v: FloatBuffer, vectorSize: Int = 1) {
        if (v.remaining() < vectorSize || v.remaining() % vectorSize != 0) {
            throw java.lang.RuntimeException("invalid size")
        }

        when (vectorSize) {
            1 -> uniformFunc.put(loc) { GLES20.glUniform1fv(loc, v.remaining() / vectorSize, v) }

            2 -> uniformFunc.put(loc) { GLES20.glUniform2fv(loc, v.remaining() / vectorSize, v) }

            3 -> uniformFunc.put(loc) { GLES20.glUniform3fv(loc, v.remaining() / vectorSize, v) }

            4 -> uniformFunc.put(loc) { GLES20.glUniform4fv(loc, v.remaining() / vectorSize, v) }
        }

        if (!checkError()) {
//            throw java.lang.RuntimeException("setUniform${vectorSize}fv error")
        }
    }

    fun setUniform(loc: Int, v: IntArray, vectorSize: Int = 1) {
        if (v.size < vectorSize || v.size % vectorSize != 0) {
            throw java.lang.RuntimeException("invalid size")
        }

        when (vectorSize) {
            1 -> uniformFunc.put(loc) { GLES20.glUniform1iv(loc, v.size / vectorSize, v, 0) }

            2 -> uniformFunc.put(loc) { GLES20.glUniform2iv(loc, v.size / vectorSize, v, 0) }

            3 -> uniformFunc.put(loc) { GLES20.glUniform3iv(loc, v.size / vectorSize, v, 0) }

            4 -> uniformFunc.put(loc) { GLES20.glUniform4iv(loc, v.size / vectorSize, v, 0) }
        }

        if (!checkError()) {
//            throw java.lang.RuntimeException("setUniform${vectorSize}iv error")
        }
    }

    fun setUniform(loc: Int, v: IntBuffer, vectorSize: Int = 1) {
        if (v.remaining() < vectorSize || v.remaining() % vectorSize != 0) {
            throw java.lang.RuntimeException("invalid size")
        }

        when (vectorSize) {
            1 -> uniformFunc.put(loc) { GLES20.glUniform1iv(loc, v.remaining() / vectorSize, v) }

            2 -> uniformFunc.put(loc) { GLES20.glUniform2iv(loc, v.remaining() / vectorSize, v) }

            3 -> uniformFunc.put(loc) { GLES20.glUniform3iv(loc, v.remaining() / vectorSize, v) }

            4 -> uniformFunc.put(loc) { GLES20.glUniform4iv(loc, v.remaining() / vectorSize, v) }
        }

        if (!checkError()) {
//            throw java.lang.RuntimeException("setUniform${vectorSize}iv error")
        }
    }

    fun setUniformMatrix(loc: Int, v: FloatArray) {
        when (v.size) {
            4 -> uniformFunc.put(loc) { GLES20.glUniformMatrix2fv(loc, 1, false, v, 0) }

            9 -> uniformFunc.put(loc) { GLES20.glUniformMatrix3fv(loc, 1, false, v, 0) }

            16 -> uniformFunc.put(loc) { GLES20.glUniformMatrix4fv(loc, 1, false, v, 0) }
        }

        if (!checkError()) {
//            throw java.lang.RuntimeException("setUniformMatrix${Math.sqrt(v.size.toDouble())}fv error")
        }
    }

    fun setUniformMatrix(loc: Int, v: FloatBuffer) {
        when (v.remaining()) {
            4 -> uniformFunc.put(loc) { GLES20.glUniformMatrix2fv(loc, 1, false, v) }

            9 -> uniformFunc.put(loc) { GLES20.glUniformMatrix3fv(loc, 1, false, v) }

            16 -> uniformFunc.put(loc) { GLES20.glUniformMatrix4fv(loc, 1, false, v) }

            else -> throw java.lang.RuntimeException("invalid size")
        }

        if (!checkError()) {
//            throw java.lang.RuntimeException("setUniformMatrix${Math.sqrt(v.remaining().toDouble())}fv error")
        }
    }

    fun getUniformLocation(name: String): Int {
        return uniformMap[name]?:run {
            val location = GLES20.glGetUniformLocation(programId, name)
            checkError()

            uniformMap[name] = location

            location
        }
    }

    fun setUniform(name: String, x: Float) {
        setUniform(getUniformLocation(name), x)
    }

    fun setUniform(name: String, x: Float, y: Float) {
        setUniform(getUniformLocation(name), x, y)
    }

    fun setUniform(name: String, x: Float, y: Float, z: Float) {
        setUniform(getUniformLocation(name), x, y, z)
    }

    fun setUniform(name: String, x: Float, y: Float, z: Float, w: Float) {
        setUniform(getUniformLocation(name), x, y, z, w)
    }

    fun setUniform(name: String, x: Int) {
        setUniform(getUniformLocation(name), x)
    }

    fun setUniform(name: String, x: Int, y: Int) {
        setUniform(getUniformLocation(name), x, y)
    }

    fun setUniform(name: String, x: Int, y: Int, z: Int) {
        setUniform(getUniformLocation(name), x, y, z)
    }

    fun setUniform(name: String, x: Int, y: Int, z: Int, w: Int) {
        setUniform(getUniformLocation(name), x, y, z, w)
    }

    fun setUniformMatrix(name: String, matrix: FloatArray) {
        setUniformMatrix(getUniformLocation(name), matrix)
    }

    fun setUniformMatrix(name: String, matrix: FloatBuffer) {
        setUniformMatrix(getUniformLocation(name), matrix)
    }

    fun setUniform(name: String, v: FloatArray, vectorSize: Int = 1) {
        setUniform(getUniformLocation(name), v, vectorSize)
    }

    fun setUniform(name: String, v: FloatBuffer, vectorSize: Int = 1) {
        setUniform(getUniformLocation(name), v, vectorSize)
    }

    fun setUniform(name: String, v: IntArray, vectorSize: Int = 1) {
        setUniform(getUniformLocation(name), v, vectorSize)
    }

    fun setUniform(name: String, v: IntBuffer, vectorSize: Int = 1) {
        setUniform(getUniformLocation(name), v, vectorSize)
    }

    fun setTexture(name: String, texture: GLTexture) {
        val location = getUniformLocation(name)
        textureList.put(location, texture)
    }

    private fun compileShader(str: String, type: Int): Int {
        val status = IntArray(1)
        val shader = GLES20.glCreateShader(type)
        GLES20.glShaderSource(shader, str)
        GLES20.glCompileShader(shader)
        GLES20.glGetShaderiv(shader, GLES20.GL_COMPILE_STATUS, status, 0)
        if (status[0] == GLES20.GL_FALSE) {
            val info = GLES20.glGetShaderInfoLog(shader)
            Log.e(TAG, "compileShader: error $info")
            return GLES20.GL_NONE

        } else if (!checkError()) return GLES20.GL_NONE

        return shader
    }

    override fun destroy() {
        if (vs != GLES20.GL_NONE) {
            GLES20.glDeleteShader(vs)
            vs = GLES20.GL_NONE
        }

        if (fs != GLES20.GL_NONE) {
            GLES20.glDeleteShader(fs)
            fs = GLES20.GL_NONE
        }

        if (programId != GLES20.GL_NONE) {
            GLES20.glDeleteProgram(programId)
            programId = GLES20.GL_NONE
        }
    }
}