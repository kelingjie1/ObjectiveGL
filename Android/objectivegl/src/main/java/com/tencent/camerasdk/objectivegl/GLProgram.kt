package com.tencent.camerasdk.objectivegl

import android.opengl.GLES30
import android.util.Log
import android.util.SparseArray
import java.nio.FloatBuffer
import java.nio.IntBuffer


class GLProgram: GLObject() {
    enum class ProgramType {
        Normal,
        TransformFeedback
    }

    private val uniformFunc: SparseArray<() -> Unit> = SparseArray()
    private val TAG = "GLProgram-${Integer.toHexString(hashCode())}"

    private var fs: Int = GLES30.GL_NONE
    private var vs: Int = GLES30.GL_NONE

    private var type = ProgramType.Normal
    private var programId = GLES30.GL_NONE

    private val textureList = HashMap<Int, Array<GLTexture>>()

    fun setShader(vertexShader: String, fragShader: String) {
        if (programId != GLES30.GL_NONE) {
            Log.d(TAG, "setShader: already link to program #$programId")
        }

        vs = compileShader(vertexShader, GLES30.GL_VERTEX_SHADER)
        fs = compileShader(fragShader, GLES30.GL_FRAGMENT_SHADER)

        if (vs != GLES30.GL_NONE && fs != GLES30.GL_NONE) {
            programId = GLES30.glCreateProgram()
            if (!checkError()) {
                destroy()
                return
            }

            GLES30.glAttachShader(programId, vs)
            if (!checkError()) {
                destroy()
                return
            }

            GLES30.glAttachShader(programId, fs)
            if (!checkError()) {
                destroy()
                return
            }

        } else {
            destroy()
            return
        }

        type = ProgramType.Normal
    }

    fun setTransformShader() {
        type = ProgramType.TransformFeedback
    }

    fun use() {
        if (programId == GLES30.GL_NONE) {
            throw RuntimeException("invalid program")
        }

        GLES30.glUseProgram(programId)
        if (!checkError()) {
//            throw RuntimeException("use program #$programId error")
        }

        applyUniforms()
    }

    private fun applyUniforms() {
        for (k in 0 until uniformFunc.size()) {
            uniformFunc[uniformFunc.keyAt(k)]()
        }

        var index = 0
        for (e in textureList.entries) {
            val indexs = IntArray(e.value.size)
            for (t in e.value.indices) {
                e.value[t].use(index)
                indexs[t] = index++
            }

            setUniform(e.key, indexs)
            checkError()
        }
    }

    fun setUniform(loc: Int, x: Float) {
        uniformFunc.put(loc) {
            GLES30.glUniform1f(loc, x)
            if (!checkError()) {
//                throw java.lang.RuntimeException("setUniform1f error")
            }
        }
    }

    fun setUniform(loc: Int, x: Float, y: Float) {
        uniformFunc.put(loc) {
            GLES30.glUniform2f(loc, x, y)
            if (!checkError()) {
//                throw RuntimeException("setUniform2f error")
            }
        }
    }

    fun setUniform(loc: Int, x: Float, y: Float, z: Float) {
        uniformFunc.put(loc) {
            GLES30.glUniform3f(loc, x, y, z)
            if (!checkError()) {
//                throw java.lang.RuntimeException("setUniform3f error")
            }
        }
    }

    fun setUniform(loc: Int, x: Float, y: Float, z: Float, w: Float) {
        uniformFunc.put(loc) {
            GLES30.glUniform4f(loc, x, y, z, w)
            if (!checkError()) {
//                throw java.lang.RuntimeException("setUniform4f error")
            }
        }
    }

    fun setUniform(loc: Int, x: Int) {
        uniformFunc.put(loc) {
            GLES30.glUniform1i(loc, x)
            if (!checkError()) {
//                throw java.lang.RuntimeException("setUniform1i error")
            }
        }
    }

    fun setUniform(loc: Int, x: Int, y: Int) {
        uniformFunc.put(loc) {
            GLES30.glUniform2i(loc, x, y)
            if (!checkError()) {
//                throw java.lang.RuntimeException("setUniform2i error")
            }
        }
    }

    fun setUniform(loc: Int, x: Int, y: Int, z: Int) {
        uniformFunc.put(loc) {
            GLES30.glUniform3i(loc, x, y, z)
            if (!checkError()) {
//                throw java.lang.RuntimeException("setUniform1i error")
            }
        }
    }

    fun setUniform(loc: Int, x: Int, y: Int, z: Int, w: Int) {
        uniformFunc.put(loc) {
            GLES30.glUniform4i(loc, x, y, z, w)
            if (!checkError()) {
//                throw java.lang.RuntimeException("setUniform1i error")
            }
        }
    }

    fun setUniformU(loc: Int, x: Int) {
        uniformFunc.put(loc) {
            GLES30.glUniform1ui(loc, x)
            if (!checkError()) {
//                throw java.lang.RuntimeException("setUniform1i error")
            }
        }
    }

    fun setUniformU(loc: Int, x: Int, y: Int) {
        uniformFunc.put(loc) {
            GLES30.glUniform2ui(loc, x, y)
            if (!checkError()) {
//                throw java.lang.RuntimeException("setUniform1i error")
            }
        }
    }

    fun setUniformU(loc: Int, x: Int, y: Int, z: Int) {
        uniformFunc.put(loc) {
            GLES30.glUniform3ui(loc, x, y, z)
            if (!checkError()) {
//                throw java.lang.RuntimeException("setUniform1i error")
            }
        }
    }

    fun setUniformU(loc: Int, x: Int, y: Int, z: Int, w: Int) {
        uniformFunc.put(loc) {
            GLES30.glUniform4ui(loc, x, y, z, w)
            if (!checkError()) {
//                throw java.lang.RuntimeException("setUniform1i error")
            }
        }
    }

    fun setUniform(loc: Int, v: FloatArray, vectorSize: Int = 1) {
        if (v.size < vectorSize || v.size % vectorSize != 0) {
            throw java.lang.RuntimeException("invalid size")
        }

        when (vectorSize) {
            1 -> uniformFunc.put(loc) { GLES30.glUniform1fv(loc, v.size / vectorSize, v, 0) }

            2 -> uniformFunc.put(loc) { GLES30.glUniform2fv(loc, v.size / vectorSize, v, 0) }

            3 -> uniformFunc.put(loc) { GLES30.glUniform3fv(loc, v.size / vectorSize, v, 0) }

            4 -> uniformFunc.put(loc) { GLES30.glUniform4fv(loc, v.size / vectorSize, v, 0) }
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
            1 -> uniformFunc.put(loc) { GLES30.glUniform1fv(loc, v.remaining() / vectorSize, v) }

            2 -> uniformFunc.put(loc) { GLES30.glUniform2fv(loc, v.remaining() / vectorSize, v) }

            3 -> uniformFunc.put(loc) { GLES30.glUniform3fv(loc, v.remaining() / vectorSize, v) }

            4 -> uniformFunc.put(loc) { GLES30.glUniform4fv(loc, v.remaining() / vectorSize, v) }
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
            1 -> uniformFunc.put(loc) { GLES30.glUniform1iv(loc, v.size / vectorSize, v, 0) }

            2 -> uniformFunc.put(loc) { GLES30.glUniform2iv(loc, v.size / vectorSize, v, 0) }

            3 -> uniformFunc.put(loc) { GLES30.glUniform3iv(loc, v.size / vectorSize, v, 0) }

            4 -> uniformFunc.put(loc) { GLES30.glUniform4iv(loc, v.size / vectorSize, v, 0) }
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
            1 -> uniformFunc.put(loc) { GLES30.glUniform1iv(loc, v.remaining() / vectorSize, v) }

            2 -> uniformFunc.put(loc) { GLES30.glUniform2iv(loc, v.remaining() / vectorSize, v) }

            3 -> uniformFunc.put(loc) { GLES30.glUniform3iv(loc, v.remaining() / vectorSize, v) }

            4 -> uniformFunc.put(loc) { GLES30.glUniform4iv(loc, v.remaining() / vectorSize, v) }
        }

        if (!checkError()) {
//            throw java.lang.RuntimeException("setUniform${vectorSize}iv error")
        }
    }

    fun setUniformMatrix(loc: Int, v: FloatArray) {
        when (v.size) {
            4 -> uniformFunc.put(loc) { GLES30.glUniformMatrix2fv(loc, 1, false, v, 0) }

            9 -> uniformFunc.put(loc) { GLES30.glUniformMatrix3fv(loc, 1, false, v, 0) }

            16 -> uniformFunc.put(loc) { GLES30.glUniformMatrix4fv(loc, 1, false, v, 0) }
        }

        if (!checkError()) {
//            throw java.lang.RuntimeException("setUniformMatrix${Math.sqrt(v.size.toDouble())}fv error")
        }
    }

    fun setUniformMatrix(loc: Int, v: FloatBuffer) {
        when (v.remaining()) {
            4 -> uniformFunc.put(loc) { GLES30.glUniformMatrix2fv(loc, 1, false, v) }

            9 -> uniformFunc.put(loc) { GLES30.glUniformMatrix3fv(loc, 1, false, v) }

            16 -> uniformFunc.put(loc) { GLES30.glUniformMatrix4fv(loc, 1, false, v) }

            else -> throw java.lang.RuntimeException("invalid size")
        }

        if (!checkError()) {
//            throw java.lang.RuntimeException("setUniformMatrix${Math.sqrt(v.remaining().toDouble())}fv error")
        }
    }

    fun getUniformLocation(name: String): Int {
        val location = GLES30.glGetUniformLocation(programId, name)
        checkError()

        return location
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

    fun setUniformU(name: String, x: Int) {
        setUniformU(getUniformLocation(name), x)
    }

    fun setUniformU(name: String, x: Int, y: Int) {
        setUniformU(getUniformLocation(name), x, y)
    }

    fun setUniformU(name: String, x: Int, y: Int, z: Int) {
        setUniformU(getUniformLocation(name), x, y, z)
    }

    fun setUniformU(name: String, x: Int, y: Int, z: Int, w: Int) {
        setUniformU(getUniformLocation(name), x, y, z, w)
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
        setTexture(name, arrayOf(texture))
    }

    fun setTexture(name: String, textures: Array<GLTexture>) {
        val location = getUniformLocation(name)
        if (textureList[location] != null) {
            for (tex in textureList[location]!!) {
                tex.destroy()
            }
        }

        textureList[location] = textures
    }

    private fun compileShader(str: String, type: Int): Int {
        val status = IntArray(1)
        val shader = GLES30.glCreateShader(type)
        GLES30.glShaderSource(shader, str)
        GLES30.glCompileShader(shader)
        GLES30.glGetShaderiv(shader, GLES30.GL_COMPILE_STATUS, status, 0)
        if (status[0] == GLES30.GL_FALSE) {
            val info = GLES30.glGetShaderInfoLog(shader)
            Log.e(TAG, "compileShader: error $info")
            return GLES30.GL_NONE

        } else if (!checkError()) return GLES30.GL_NONE

        return shader
    }

    override fun destroy() {
        if (vs != GLES30.GL_NONE) {
            GLES30.glDeleteShader(vs)
            vs = GLES30.GL_NONE
        }

        if (fs != GLES30.GL_NONE) {
            GLES30.glDeleteShader(fs)
            fs = GLES30.GL_NONE
        }

        if (programId != GLES30.GL_NONE) {
            GLES30.glDeleteProgram(programId)
            programId = GLES30.GL_NONE
        }
    }
}