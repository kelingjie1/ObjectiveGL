package com.tencent.camerasdk.objectivegl3

import android.opengl.GLES30
import android.util.Log
import java.lang.RuntimeException

abstract class GLObject {
    private val TAG = "GLObject-${Integer.toHexString(this.hashCode())}"

    protected fun checkError(): Boolean {
        val err = GLES30.glGetError()
        if (err != 0) {
            Log.e(TAG, "checkError: error=$err")
            throw RuntimeException("GLError $err")
        }

        return err == 0
    }

    abstract fun destroy()
}