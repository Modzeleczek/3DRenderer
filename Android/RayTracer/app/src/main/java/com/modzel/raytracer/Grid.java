package com.modzel.raytracer;

import android.opengl.GLES20;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;

public class Grid {
    private final String VertexShaderCode =
            "attribute vec4 aColor;" +
            "varying vec4 vColor;" +
            "attribute vec4 aPosition;" +
            "void main() {" +
            "  gl_Position = aPosition;" +
            "  vColor = aColor;" +
            "}";
    private final String FragmentShaderCode =
            //"precision mediump float;" +
            "varying vec4 vColor;" +
            "void main() {" +
            "  gl_FragColor = vColor;" +
            "}";

    private int Columns, Rows;
    private FloatBuffer VerticesBuffer, ColorsBuffer;
    private ShortBuffer IndicesBuffer;
    private int IndicesBufferLength;
    private int ProgramHandle;
    private int PositionHandle;
    private int ColorHandle;

    private static final int COORDS_PER_VERTEX = 3, // number of coordinates per vertex in vertices array (XYZ)
            COLOR_COMPONENTS_PER_VERTEX = 4, // RGBA
            VertexStride = COORDS_PER_VERTEX * 4, // 4 bytes, because each coordinate is a float
            ColorStride = COLOR_COMPONENTS_PER_VERTEX * 4; // 4 bytes, because each color component is a float

    /**
     * Sets up the drawing object data for use in an OpenGL ES context.
     */
    public Grid(int columns, int rows) {
        this.Columns = columns;
        this.Rows = rows;

        int verticesCount = (columns + 1) * (rows + 1);
        float vertices[] = new float[verticesCount * COORDS_PER_VERTEX];

        float colors[] = new float[verticesCount * COLOR_COMPONENTS_PER_VERTEX];

        // order of vertices drawing
        // We are drawing columns*rows cells, each consisting of 2 triangles. A triangle has 3 vertices.
        IndicesBufferLength = columns * rows * 2 * 3;
        short indices[] = new short[IndicesBufferLength];

        // A vertice's coords can be from range <-1, 1>, so the screen's width and height are 2.
        final float cellWidth = 2.f / columns,
                    cellHeight = 2.f / rows;
        int vertexCoordIndex = 0, colorComponentIndex = 0;
        // y >= -1.f and x <= 1.f, because we have to set 1 additional column and row of vertices.
        for(float y = 1.f; y >= -1.f; y -= cellHeight) {
            for(float x = -1.f; x <= 1.f; x += cellWidth) {
                vertices[vertexCoordIndex++] = x;
                vertices[vertexCoordIndex++] = y;
                vertices[vertexCoordIndex++] = 0.f;

                colors[colorComponentIndex++] = 1.f; // red
                colors[colorComponentIndex++] = 0.f; // green
                colors[colorComponentIndex++] = 0.f; // blue
                colors[colorComponentIndex++] = 1.f; // alpha
            }
        }

        int vertexIndex = 0, indiceIndex = 0;
        for(int y = 0; y <= rows - 1; ++y) {
            for(int x = 0; x <= columns - 1; ++x) {
                /*
               squareIndex|\--|squareIndex+1
                          | \ |
   squareIndex+(columns+1)|--\|squareIndex+(columns+1)+1
                 */
                indices[indiceIndex++] = (short) (vertexIndex); // 0
                indices[indiceIndex++] = (short) (vertexIndex + 1); // 1
                indices[indiceIndex++] = (short) (vertexIndex + columns + 2); // 6
                indices[indiceIndex++] = (short) (vertexIndex); // 0
                indices[indiceIndex++] = (short) (vertexIndex + columns + 1); // 5
                indices[indiceIndex++] = (short) (vertexIndex + columns + 2); // 6
                ++vertexIndex;
            }
            ++vertexIndex;
        }

        // initialize vertex byte buffer for grid vertices' coordinates
        ByteBuffer buffer = ByteBuffer.allocateDirect(vertices.length * 4); // (number of coordinates * 4 bytes per float)
        buffer.order(ByteOrder.nativeOrder());
        VerticesBuffer = buffer.asFloatBuffer();
        VerticesBuffer.put(vertices);
        VerticesBuffer.position(0);

        // initialize color byte buffer for grid vertices' color components
        buffer = ByteBuffer.allocateDirect(colors.length * 4); // (number of color components * 4 bytes per float)
        buffer.order(ByteOrder.nativeOrder());
        ColorsBuffer = buffer.asFloatBuffer();
        ColorsBuffer.put(colors);
        ColorsBuffer.position(0);

        // initialize byte buffer for vertices' drawing order indicators (indices)
        buffer = ByteBuffer.allocateDirect(indices.length * 2); // (number of indices * 2 bytes per short)
        buffer.order(ByteOrder.nativeOrder());
        IndicesBuffer = buffer.asShortBuffer();
        IndicesBuffer.put(indices);
        IndicesBuffer.position(0);

        // prepare shaders
        int vertexShader = GLESView.loadShader(GLES20.GL_VERTEX_SHADER, VertexShaderCode);
        int fragmentShader = GLESView.loadShader(GLES20.GL_FRAGMENT_SHADER, FragmentShaderCode);

        // prepare OpenGL program
        ProgramHandle = GLES20.glCreateProgram();             // create empty OpenGL Program
        GLES20.glAttachShader(ProgramHandle, vertexShader);   // add the vertex shader to program
        GLES20.glAttachShader(ProgramHandle, fragmentShader);// add the fragment shader to program
        GLES20.glLinkProgram(ProgramHandle);                  // create OpenGL program executables
    }

    /**
     * Encapsulates the OpenGL ES 2.0 instructions for drawing this grid.
     */
    public void Draw() {
        // Add program to OpenGL environment.
        GLES20.glUseProgram(ProgramHandle);

        // Get handle to vertex shader's aPosition member.
        PositionHandle = GLES20.glGetAttribLocation(ProgramHandle, "aPosition");
        GLESView.checkGlError("glGetAttribLocation");

        // Enable edition of 'aPosition' attribute in the vertex shader.
        GLES20.glEnableVertexAttribArray(PositionHandle);

        // Put the grid cells' vertex coordinate data in GPU memory.
        GLES20.glVertexAttribPointer(
                PositionHandle, COORDS_PER_VERTEX,
                GLES20.GL_FLOAT, false, // GLES20.GL_FALSE
                VertexStride, VerticesBuffer);
        GLESView.checkGlError("glVertexAttribPointer");

        // Get handle to fragment shader's aColor member.
        ColorHandle = GLES20.glGetAttribLocation(ProgramHandle, "aColor");
        GLESView.checkGlError("glGetAttribLocation");

        // Enable edition of 'aColor' attribute in the fragment shader.
        GLES20.glEnableVertexAttribArray(ColorHandle);

        // Put the grid cells' vertex color data in GPU memory.
        GLES20.glVertexAttribPointer(
                ColorHandle, COLOR_COMPONENTS_PER_VERTEX,
                GLES20.GL_FLOAT, false,
                ColorStride, ColorsBuffer);
        GLESView.checkGlError("glVertexAttribPointer");

        // Draw the grid.
        GLES20.glDrawElements(
                GLES20.GL_TRIANGLES, IndicesBufferLength,
                GLES20.GL_UNSIGNED_SHORT, IndicesBuffer);

        // Disable edition of 'aPosition' attribute in the vertex shader.
        GLES20.glDisableVertexAttribArray(PositionHandle);
        // Disable edition of 'aColor' attribute in the fragment shader.
        GLES20.glDisableVertexAttribArray(ColorHandle);
    }
}