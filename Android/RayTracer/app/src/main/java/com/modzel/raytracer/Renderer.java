package com.modzel.raytracer;

import java.util.ArrayList;
import java.util.List;

class LocalCoordinateSystem
{
    // A unit vector, which indicates coordinate system's horizontal axis.
    protected Vec3f HorizontalAxis;
    // A unit vector, which indicates coordinate system's vertical axis.
    protected Vec3f VerticalAxis;
    // A vector, which indicates the facing of the coordinate system.
    protected Vec3f Direction;
    // The 3 vectors, HorizontalAxis, VerticalAxis and Direction, together make a
    // local (possibly rotated) coordinate system.

    protected LocalCoordinateSystem(Vec3f horizontalAxis, Vec3f verticalAxis, Vec3f direction) {
        HorizontalAxis = new Vec3f(horizontalAxis);
        VerticalAxis = new Vec3f(verticalAxis);
        Direction = new Vec3f(direction);
    }
    protected LocalCoordinateSystem() {
         HorizontalAxis = new Vec3f(1,0,0);
         VerticalAxis = new Vec3f(0,1,0);
         Direction = new Vec3f(0, 0, -1);
    }

    protected void RotateX(float angle)
    {
        Direction.RotateX(angle);
        HorizontalAxis.RotateX(angle);
        VerticalAxis.RotateX(angle);
    }
    protected void RotateY(float angle)
    {
        Direction.RotateY(angle);
        HorizontalAxis.RotateY(angle);
        VerticalAxis.RotateY(angle);
    }
    protected void RotateZ(float angle)
    {
        Direction.RotateZ(angle);
        HorizontalAxis.RotateZ(angle);
        VerticalAxis.RotateZ(angle);
    }
    protected void RotateAxis(Vec3f axis, float angle)
    {
        Direction.RotateAxisQuaternion(axis, angle);
        HorizontalAxis.RotateAxisQuaternion(axis, angle);
        VerticalAxis.RotateAxisQuaternion(axis, angle);
    }
    protected void SetDirection(Vec3f direction)
    {
        Direction = new Vec3f(direction);
        if(Direction.X == 0 && Direction.Y == 0)
        {
            VerticalAxis = new Vec3f(0,1,0);
            if(Direction.Z == -1)
                HorizontalAxis = new Vec3f(1,0,0);
            else // if(Direction.Z == 1)
                HorizontalAxis = new Vec3f(-1,0,0);
        }
        else
        {
            final float x = Direction.X, y = Direction.Y, z = Direction.Z,
                    commonCoefficient = (1.f + z) / (x*x + y*y);

            // HorizontalAxis = (y^2 * (1 + z) / (x^2 + y^2) - z, -x*y * (1 + z) / (x^2 + y^2), x)
            // VerticalAxis = (-x*y * (1 + z) / (x^2 + y^2), x^2 * (1 + z) / (x^2 + y^2) - z, y)*/

            HorizontalAxis.X = y*y * commonCoefficient - z;
            HorizontalAxis.Y = -x*y * commonCoefficient;
            HorizontalAxis.Z = x;

            VerticalAxis.X = -x*y * commonCoefficient;
            VerticalAxis.Y = x*x * commonCoefficient - z;
            VerticalAxis.Z = y;
        }
    }
}

public class Renderer
{
    public class Camera extends LocalCoordinateSystem
    {
        // Position of the camera.
        public Vec3f Position;
        // Distance between camera and screen depending on field of view.
        private float ScreenDistance;
        // A vector from camera's position to the center of the screen.
        private Vec3f DirectionTimesDistance;
        // Used to compute distance between the camera and the screen.
        private final int ScreenHeight;

        public Camera(int frameHeight, float fieldOfView, Vec3f position) {
            ScreenHeight = frameHeight;
            Position = new Vec3f(position);
            SetFieldOfView(fieldOfView);
        }
        public Camera() {
            ScreenHeight = 512;
            Position = new Vec3f(0, 0, 0);
            SetFieldOfView((float)Math.PI / 3.f);
        }

        public void SetFieldOfView(float fieldOfView)
        {
            ScreenDistance = ScreenHeight / (2.f * (float)Math.tan(fieldOfView / 2.f));
            DirectionTimesDistance = Vec3f.Multiply(Direction, ScreenDistance);
        }
        public void RotateX(float angle)
        {
            super.RotateX(angle);
            DirectionTimesDistance = Vec3f.Multiply(Direction, ScreenDistance);
        }
        public void RotateY(float angle)
        {
            super.RotateY(angle);
            DirectionTimesDistance = Vec3f.Multiply(Direction, ScreenDistance);
        }
        public void RotateZ(float angle)
        {
            super.RotateZ(angle);
            DirectionTimesDistance = Vec3f.Multiply(Direction, ScreenDistance);
        }
        public void RotateAxis(Vec3f axis, float angle)
        {
            super.RotateAxis(axis, angle);
            DirectionTimesDistance = Vec3f.Multiply(Direction, ScreenDistance);
        }
        public Vec3f GetScreenPixelPosition(int x, int y)
        {
            // return HorizontalAxis * x + VerticalAxis * y + DirectionTimesDistance;
            return new Vec3f(
                    HorizontalAxis.X * x + VerticalAxis.X * y + DirectionTimesDistance.X,
                    HorizontalAxis.Y * x + VerticalAxis.Y * y + DirectionTimesDistance.Y,
                    HorizontalAxis.Z * x + VerticalAxis.Z * y + DirectionTimesDistance.Z);
        }
        @Override
        public void SetDirection(Vec3f direction)
        {
            super.SetDirection(direction);
            DirectionTimesDistance = Vec3f.Multiply(Direction, ScreenDistance);
        }
    }

    public final int Width, Height;
    // public final byte TotalThreads;
    public ArrayList<Shape> Shapes;
    public ArrayList<Light> Lights;
    public Camera Eye;
    public Grid OutputGrid;

    public Renderer(Grid outputGrid) {
        OutputGrid = outputGrid;
        Width = outputGrid.Columns;
        Height = outputGrid.Rows;
        // TotalThreads = numberOfThreads;
        Eye = new Camera(Height, (float)Math.PI / 3.f, new Vec3f(0,0,0));
        Shapes = new ArrayList<Shape>();
        Lights = new ArrayList<Light>();
    }
    public Renderer() {
        Width = 512;
        Height = 512;
        // TotalThreads = 8;
    }

    /*public void RenderFrame()
    {
        WorkingThreads = TotalThreads;
        int y = Height / 2;
        const int deltaY = Height / TotalThreads;
        for(int i = 0; i < TotalThreads - 1; ++i)
        {
            std::thread(&Renderer::RenderFramePart, this, y, y - deltaY).detach();
            y -= deltaY;
        }
        RenderFramePart(y, y - deltaY);
        while(WorkingThreads > 0);
    }*/

    public void RenderFrame() {
        int x, y, index = 0;
        // byte *p = FrameBuffer + 4 * Width * (Height / 2 - y); // offset
        for(y = Height / 2; y > -Height / 2; --y) // going from top
        {
            for(x = -Width / 2; x < Width / 2; ++x) // going from left
            {
                Vec3f color = CastRay(Eye.Position, Eye.GetScreenPixelPosition(x, y).Normalize(), (byte)0).Truncate();
                OutputGrid.SetCell(index, color.X, color.Y, color.Z, 1.f);
                ++index;
            }
            ++index;
        }
    }

    /*private std::atomic<byte> WorkingThreads;
    private void RenderFramePart(int y, const int endY)
    {
        int x;
        byte *p = FrameBuffer + 4 * Width * (Height / 2 - y); // offset
        for( ; y > endY; --y) // going from top
        {
            for(x = -Width / 2; x < Width / 2; ++x) // going from left
            {
                Vec3b color = static_cast<Vec3b>(CastRay(Eye.Position, Eye.GetScreenPixelPosition(x, y).Normalize()));
                *p = color.R; ++p;
                *p = color.G; ++p;
                *p = color.B; ++p;
                ++p;
                // Adding 4, because every pixel is coded by four bytes. The fourth byte is
                // alpha value, which is ignored by GifWriter.
            }
        }
        --WorkingThreads;
    }*/

    private boolean SceneIntersect(Vec3f orig, Vec3f dir, Vec3f closestShapeHitPoint,
                                   Vec3f closestShapeNormal, Material material) {
        byte i;
        float closestShapeDistance = Float.MAX_VALUE;
        FloatWrapper distance = new FloatWrapper();
        Vec3f normal = new Vec3f(), hitPoint = new Vec3f();
        for(i = 0; i < Shapes.size(); ++i)
        {
            if(Shapes.get(i).RayIntersect(orig, dir, distance, hitPoint, normal) &&
                    distance.Value < closestShapeDistance)
            {
                closestShapeDistance = distance.Value;
                closestShapeHitPoint.CopyFrom(hitPoint);
                closestShapeNormal.CopyFrom(normal);
                material.CopyFrom(Shapes.get(i).Surface);
            }
        }
        return closestShapeDistance < 1000;
    }

    private boolean SceneIntersect(Vec3f orig, Vec3f dir, Vec3f closestShapeHitPoint,
                                   Vec3f closestShapeNormal) {
        byte i;
        float closestShapeDistance = Float.MAX_VALUE;
        FloatWrapper distance = new FloatWrapper();
        Vec3f normal = new Vec3f(), hitPoint = new Vec3f();
        for(i = 0; i < Shapes.size(); ++i)
        {
            if(Shapes.get(i).RayIntersect(orig, dir, distance, hitPoint, normal) &&
                    distance.Value < closestShapeDistance)
            {
                closestShapeDistance = distance.Value;
                closestShapeHitPoint.CopyFrom(hitPoint);
                closestShapeNormal.CopyFrom(normal);
            }
        }
        return closestShapeDistance < 1000;
    }

    private Vec3f CastRay(Vec3f orig, Vec3f dir, byte depth)
    {
        Vec3f point = new Vec3f(), N = new Vec3f();
        Material material = new Material();

        if (depth>=3 || !SceneIntersect(orig, dir, point, N, material))
            return new Vec3f(0.f, 0.f, 0.f); // background color

        Vec3f reflect_dir = Vec3f.Reflect(dir, N).Normalize();
        Vec3f refract_dir = Vec3f.Refract(dir, N, material.RefractiveIndex, 1.f).Normalize();
        // Vec3f reflect_orig = reflect_dir*N < 0 ? point - N*1e-3 : point + N*1e-3; // offset the original point to avoid occlusion by the object itself
        Vec3f reflect_orig = Vec3f.Add(point, Vec3f.Multiply(N, 0.001f));
        // Vec3f refract_orig = refract_dir*N < 0 ? point - N*1e-3 : point + N*1e-3;
        Vec3f refract_orig = Vec3f.Subtract(point, Vec3f.Multiply(N, 0.001f));
        Vec3f reflect_color = CastRay(reflect_orig, reflect_dir, (byte)(depth + 1));
        Vec3f refract_color = CastRay(refract_orig, refract_dir, (byte)(depth + 1));

        float diffuse_light_intensity = 0, specular_light_intensity = 0;
        for (int i = 0; i < Lights.size(); ++i)
        {
            Vec3f light_dir      = Vec3f.Subtract(Lights.get(i).Position, point);
            float light_distance = light_dir.NormalizeReturnNorm();

            Vec3f shadow_orig = Vec3f.Dot(light_dir, N) < 0
                    ? Vec3f.Subtract(point, Vec3f.Multiply(N, 0.001f))
                    : Vec3f.Add(point, Vec3f.Multiply(N, 0.001f)); // checking if the point lies in the shadow of the Lights[i]
            Vec3f shadow_pt = new Vec3f(), shadow_N = new Vec3f();
            if (SceneIntersect(shadow_orig, light_dir, shadow_pt, shadow_N)
                    && (Vec3f.Subtract(shadow_pt, shadow_orig)).Norm() < light_distance)
                continue;

            diffuse_light_intensity  += Lights.get(i).Intensity * Math.max(0.f, Vec3f.Dot(light_dir, N));
            specular_light_intensity += Math.pow(Math.max(0.f, -Vec3f.Dot(Vec3f.Reflect(Vec3f.Inverse(light_dir), N), dir)),
            material.SpecularExponent) * Lights.get(i).Intensity;
        }
        return Vec3f.Add(
            Vec3f.Multiply(material.DiffuseColor, diffuse_light_intensity * material.Albedo.X),
            Vec3f.Add(Vec3f.Multiply(new Vec3f(1.f, 1.f, 1.f), specular_light_intensity * material.Albedo.Y),
                Vec3f.Add(Vec3f.Multiply(reflect_color, material.Albedo.Z), Vec3f.Multiply(refract_color, material.Albedo.W)))
        );
    }
}