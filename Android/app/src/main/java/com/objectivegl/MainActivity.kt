package com.objectivegl

import android.opengl.GLSurfaceView
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import kotlinx.android.synthetic.main.activity_main.*
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class MainActivity : AppCompatActivity(), GLSurfaceView.Renderer {
    override fun onDrawFrame(gl: GL10?) {
        runTest()
    }

    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
    }

    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
        glView.renderMode = GLSurfaceView.RENDERMODE_CONTINUOUSLY

        init()
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        glView.setEGLContextClientVersion(3)
        glView.setRenderer(this)
    }

    private external fun runTest()
    private external fun init()

    companion object {

        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("objective-gl")
        }
    }
}
