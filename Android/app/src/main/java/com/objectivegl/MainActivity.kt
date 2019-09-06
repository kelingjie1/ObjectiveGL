package com.objectivegl

import android.Manifest
import android.graphics.BitmapFactory
import android.opengl.GLES20
import android.opengl.GLSurfaceView
import android.os.Bundle
import android.support.v4.app.ActivityCompat
import android.support.v7.app.AppCompatActivity
import android.util.Log
import com.tencent.camerasdk.objectivegl2.*
import kotlinx.android.synthetic.main.activity_main.*
import java.nio.ByteBuffer
import java.nio.ByteOrder
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10
import kotlin.system.measureTimeMillis

class MainActivity : AppCompatActivity(), GLSurfaceView.Renderer {
    private var surfaceHeight: Int = 0
    private var surfaceWidth: Int = 0
    private lateinit var fastGrayProgram: GLProgram
    private lateinit var grayProgram: GLProgram
    private lateinit var program: GLProgram
    private lateinit var texture: GLTexture
    private lateinit var texture1: GLTexture
    private lateinit var texture2: GLTexture
    private lateinit var buffer: GLArrayBuffer
    private lateinit var vertexArray: GLVertexArray
    private lateinit var fbo0: GLFrameBuffer
    private lateinit var fbo1: GLFrameBuffer
    private lateinit var fbo2: GLFrameBuffer
    private var glOpt = GLOption()

    private var cost0 = 0L
    private var cost00 = 0L
    private var cost1 = 0L
    private var cost11 = 0L
    private var count = 0

    private fun normalDraw() {
        GLES20.glFinish()
        cost0 += measureTimeMillis {
            for (i in 0 until 50) {
                grayProgram.setTexture("tex", texture)
                fbo1.draw(grayProgram, vertexArray, glOpt, texture1)

                grayProgram.setTexture("tex", texture1)
                fbo1.draw(grayProgram, vertexArray, glOpt, texture)
            }
            GLES20.glFinish()
        }
    }

    private fun fbFetchDraw() {
        GLES20.glFinish()
        cost1 += measureTimeMillis {
            program.setTexture("tex", texture)
            fbo2.draw(program, vertexArray, glOpt, texture2)
            for (i in 0 until 100) {
                fastGrayProgram.use()
                vertexArray.apply(fastGrayProgram)
                glOpt.apply()
                GLES20.glDrawArrays(vertexArray.primitives, 0, vertexArray.vertexCount)
                GLES20.glFlush()
            }
            GLES20.glFinish()
        }
    }

    private fun drawTest() {
        fbFetchDraw()
        normalDraw()

        GLES20.glFinish()
        cost00 += measureTimeMillis {
            runTest(0)
            GLES20.glFinish()
        }

        GLES20.glFinish()
        cost11 += measureTimeMillis {
            runTest(1)
            GLES20.glFinish()
        }
    }

    private fun feedbackTest() {
        cost0 += measureTimeMillis {
            runTest(2)
        }
    }

    private fun feedbackGrayScaleTest() {
        cost0 += measureTimeMillis {
            runTest(3)
        }
    }

    override fun onDrawFrame(gl: GL10?) {
        drawTest()
//        feedbackTest()
//        feedbackGrayScaleTest()

        ++count
        if (count % 200 == 0) {
            Log.d("MainActivity", "drawTest: normalCost=${(cost0 * 10 / 200).toFloat() / 10}/${(cost00 * 10 / 200).toFloat() / 10}\tfbCost=${(cost1 * 10 / 200).toFloat() / 10}/${(cost11 * 10 / 200).toFloat() / 10}")
            count = 0
            cost0 = 0
            cost00 = 0
            cost1 = 0
            cost11 = 0
        }
    }

    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
        surfaceWidth = width
        surfaceHeight = height
    }

    private fun drawInit() {
        val bmp = BitmapFactory.decodeFile("/sdcard/_/0.jpg")

        val buf = ByteBuffer.allocateDirect(bmp.byteCount).order(ByteOrder.nativeOrder())
        bmp.copyPixelsToBuffer(buf)
        init(buf, 0)

        texture = GLTexture().apply { setImage(bmp) }
        texture1 = GLTexture().apply { setImage(bmp) }
        texture2 = GLTexture().apply { setImage(bmp) }
        bmp.recycle()

        program = GLProgram().apply { setShader(VERTEX_SHADER, FRAGMENT_SHADER) }
        grayProgram = GLProgram().apply { setShader(VERTEX_SHADER, GRAY_FRAGMENT_SHADER) }
        fastGrayProgram = GLProgram().apply { setShader(VERTEX_SHADER, FAST_GRAY_FRAGMENT_SHADER) }

        buffer = GLArrayBuffer(4 * 4, 4)
        buffer.offer(ByteBuffer.allocateDirect(VERTEX_ATTRIBUTES.size * 4).order(ByteOrder.nativeOrder()).asFloatBuffer().put(VERTEX_ATTRIBUTES).position(0))

        vertexArray = GLVertexArray(GLES20.GL_TRIANGLE_FAN).apply {
            arrayBuffer = buffer
            attributes = listOf(GLVertexAttribute("position", GLVertexAttribute.Dimension.VECTOR2), GLVertexAttribute("texCoords", GLVertexAttribute.Dimension.VECTOR2))
        }

        fbo0 = GLFrameBuffer(0)
        fbo1 = GLFrameBuffer(-1)
        fbo2 = GLFrameBuffer(-1)
    }

    private fun feedbackInit() {
        init(null, 1)
    }

    private fun feedbackGrayScaleInit() {
        val bmp = BitmapFactory.decodeFile("/sdcard/_/0.jpg")

        val buf = ByteBuffer.allocateDirect(bmp.byteCount).order(ByteOrder.nativeOrder())
        bmp.copyPixelsToBuffer(buf)
        init(buf, 2)
    }

    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
        glView.renderMode = GLSurfaceView.RENDERMODE_CONTINUOUSLY

        drawInit()
//        feedbackInit()
//        feedbackGrayScaleInit()
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        ActivityCompat.requestPermissions(this, arrayOf(Manifest.permission.READ_EXTERNAL_STORAGE), 101)

        glView.setEGLContextClientVersion(3)
        glView.setRenderer(this)
    }

    private external fun runTest(case: Int)
    private external fun init(buf: ByteBuffer?, case: Int)

    companion object {

        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("objective-gl")
        }

        const val VERTEX_SHADER = """
        attribute vec2 position;
        attribute vec2 texCoords;
        varying vec2 uv;
        void main() {
            gl_Position = vec4(position.x, position.y, 1.0, 1.0);
            uv = texCoords;
        }
        """

//        gl_FragColor = vec4(1.0, 1.0, 0.0, 0.0);
        const val FRAGMENT_SHADER = """
        varying vec2 uv;
        uniform sampler2D tex;
        void main() {
            gl_FragColor = texture2D(tex, uv);
        }
        """

        const val GRAY_FRAGMENT_SHADER = """
        varying highp vec2 uv;
        uniform sampler2D tex;
        const highp float alpha = 1.0;
        const highp vec3 grayWeight = vec3(0.299, 0.587, 0.114);

        void main() {
            highp vec4 textureColor = texture2D(tex, uv);
            highp float luminance = dot(textureColor.rgb, grayWeight);
            highp vec4 grayTextureColor = vec4(vec3(luminance), textureColor.a);
            gl_FragColor = vec4(mix(textureColor.rgb, grayTextureColor.rgb, alpha), textureColor.a);
        }
        """

        const val FAST_GRAY_FRAGMENT_SHADER = """
        #extension GL_EXT_shader_framebuffer_fetch : require
        const highp float alpha = 1.0;
        const highp vec3 grayWeight = vec3(0.299, 0.587, 0.114);

        void main() {
            highp vec4 textureColor = gl_LastFragData[0];
            highp float luminance = dot(textureColor.rgb, grayWeight);
            highp vec4 grayTextureColor = vec4(vec3(luminance), textureColor.a);
            gl_FragColor = vec4(mix(textureColor.rgb, grayTextureColor.rgb, alpha), textureColor.a);
        }
        """

        val VERTEX_ATTRIBUTES = floatArrayOf(
            -1f, -1f, 0f, 0f,
            -1f, 1f, 0f, 1f,
            1f, 1f, 1f, 1f,
            1f, -1f, 1f, 0f
        )
    }
}
