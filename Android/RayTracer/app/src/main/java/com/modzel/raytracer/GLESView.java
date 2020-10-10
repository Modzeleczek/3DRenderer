package com.modzel.raytracer;

import android.content.Context;
import android.content.res.Resources;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.MotionEvent;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class GLESView extends GLSurfaceView {

    private Renderer RendererInstance;
    private int TouchX, TouchY;
    public GLESView(Context context) {
        super(context);

        setEGLContextClientVersion(2);//OpenGL ES 2.0
        // setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        RendererInstance = new Renderer();
        setRenderer(RendererInstance);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_MOVE:
                // TouchX = (event.getX() - 540.f) / 540.f; // 1080 / 2; normalized X, in range <-1,1>
                // TouchY = (event.getY() - 1014.f) / 1014.f; // 2029 / 2; normalized Y, in range <-1,1>
                TouchX = (int)event.getRawX();
                TouchY = (int)event.getRawY();
                RendererInstance.PaintCell();
                break;
            case MotionEvent.ACTION_UP:
                break;
        }
        return true;
    }

    /**
     * Utility method for compiling a OpenGL shader.
     *
     * <p><strong>Note:</strong> When developing shaders, use the checkGlError()
     * method to debug shader coding errors.</p>
     *
     * @param type - Vertex or fragment shader type.
     * @param shaderCode - String containing the shader code.
     * @return - Returns an id for the shader.
     */
    public static int LoadShader(int type, String shaderCode) {

        // create a vertex shader type (GLES20.GL_VERTEX_SHADER)
        // or a fragment shader type (GLES20.GL_FRAGMENT_SHADER)
        int shader = GLES20.glCreateShader(type);

        // add the source code to the shader and compile it
        GLES20.glShaderSource(shader, shaderCode);
        GLES20.glCompileShader(shader);

        return shader;
    }

    /**
     * Utility method for debugging OpenGL calls. Provide the name of the call
     * just after making it:
     *
     * <pre>
     * mColorHandle = GLES20.glGetUniformLocation(mProgram, "vColor");
     * GLESView.checkGlError("glGetUniformLocation");</pre>
     *
     * If the operation is not successful, the check throws an error.
     *
     * @param glOperation - Name of the OpenGL call to check.
     */
    public static void CheckGlError(String glOperation) {
        int error;
        while ((error = GLES20.glGetError()) != GLES20.GL_NO_ERROR) {
            Log.e("GLESView", glOperation + ": glError " + error);
            throw new RuntimeException(glOperation + ": glError " + error);
        }
    }

    public class Renderer implements GLSurfaceView.Renderer {

        public Grid GridInstance;
        private int ScreenWidth, ScreenHeight;

        // Method invoked, when the program starts.
        @Override
        public void onSurfaceCreated(GL10 unused, EGLConfig eglConfig) {
            DisplayMetrics metrics = Resources.getSystem().getDisplayMetrics();
            ScreenWidth = metrics.widthPixels; // 1080
            ScreenHeight = metrics.heightPixels; // 2029

            // Set the background frame color.
            GLES20.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            // Create the grid.
            GridInstance = new Grid(16, 16);
        }

        @Override
        public void onSurfaceChanged(GL10 unused, int width, int height) {
            // Adjust the viewport based on geometry changes, such as screen rotation.
            GLES20.glViewport(0, 0, width, height);
        }

        // Method invoked every program's frame.
        @Override
        public void onDrawFrame(GL10 unused) {
            // Draw background color
            GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);
            // Draw the grid.
            GridInstance.Draw();
        }

        public void PaintCell() {
            int index = (int)( (TouchX * GridInstance.Columns) / ScreenWidth)
                    + (GridInstance.Columns + 1) * (int)( (TouchY * GridInstance.Rows) / ScreenHeight);
            // (cols + 1) * (rows + 1) - (cols + 1) = (cols + 1) * rows
            if(index < (GridInstance.Columns + 1) * GridInstance.Rows)
                GridInstance.SetCell(index, 0, 1, 0, 1);
        }
    }
}