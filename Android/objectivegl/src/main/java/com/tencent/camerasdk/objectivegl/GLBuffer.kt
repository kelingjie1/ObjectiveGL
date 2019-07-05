package com.tencent.camerasdk.objectivegl

import android.opengl.GLES30
import java.nio.Buffer

class GLBuffer: GLObject() {
    override fun destroy() {
        if (id != GLES30.GL_NONE) {
            GLES30.glDeleteBuffers(1, intArrayOf(id), 0)
            id = GLES30.GL_NONE
        }
    }

    var id = GLES30.GL_NONE
    private set

    var size = 0
    private set

    var elementCount = 0
    private set

    var elementSize = 0
    private set

    init {
        val a = IntArray(1)
        GLES30.glGenBuffers(1, a, 0)
        checkError()
        id = a[0]
    }

    fun offer(data: Buffer, elementSize: Int, elementCount: Int, usage: Int = GLES30.GL_STREAM_DRAW) {
        val sz = elementCount * elementSize

        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, id)
        checkError()

        GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER, sz, data, usage)
        checkError()

        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, 0)
        checkError()

        size = sz
        this.elementCount = elementCount
        this.elementSize =  elementSize
    }

    fun lock(offset: Int = 0, length: Int = 0, access: Int = (GLES30.GL_MAP_READ_BIT or GLES30.GL_MAP_WRITE_BIT)): Buffer {

        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, id)
        checkError()

        val len = if (length == 0) size - offset else length
        val buf = GLES30.glMapBufferRange(GLES30.GL_ARRAY_BUFFER, offset, len, access)
        checkError()

        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, 0)
        checkError()

        return buf
    }

    fun unlock() {
        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, id)
        checkError()

        GLES30.glUnmapBuffer(GLES30.GL_ARRAY_BUFFER)
        checkError()

        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, 0)
        checkError()
    }

    fun walk(func: (Buffer) -> Unit, offset: Int = 0, length: Int = 0, access: Int = (GLES30.GL_MAP_READ_BIT or GLES30.GL_MAP_WRITE_BIT)) {
        func(lock(offset, length, access))
        unlock()
    }

    fun offer(buffer: GLBuffer, readOffset: Int = 0, writeOffset: Int = 0, size: Int = 0) {
        GLES30.glBindBuffer(GLES30.GL_COPY_READ_BUFFER, buffer.id)
        checkError()

        GLES30.glBindBuffer(GLES30.GL_COPY_WRITE_BUFFER, id)
        checkError()

        val sz = if (size == 0) Math.min(buffer.size, size) else size
        GLES30.glCopyBufferSubData(GLES30.GL_COPY_READ_BUFFER, GLES30.GL_COPY_WRITE_BUFFER, readOffset, writeOffset, sz)
        checkError()

        GLES30.glBindBuffer(GLES30.GL_COPY_READ_BUFFER, 0)
        checkError()

        GLES30.glBindBuffer(GLES30.GL_COPY_WRITE_BUFFER, 0)
        checkError()
    }
}