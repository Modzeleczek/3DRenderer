package com.modzel.raytracer;

public class Rectangle extends PlainShape {

    public float Width, Height;

    public Rectangle(Vec3f center, float width, float height, Vec3f direction, Material material) {
        super(center, direction, material);
        Width = width;
        Height = height;
        RotateAxes();
    }

    @Override
    public boolean RayIntersect(Vec3f origin, Vec3f direction, FloatWrapper distance, Vec3f hitPoint, Vec3f normal) {
        final float cosDd = Vec3f.Dot(Direction, direction);
        if(cosDd == 0) return false; // the ray does not hit the rectangle, because they are parallel
        distance.Value = Vec3f.Dot( Direction, Vec3f.Subtract(Center, origin) ) / cosDd;
        if(distance.Value <= 0) return false;
        hitPoint.CopyFrom(Vec3f.Add(origin, Vec3f.Multiply(distance.Value, direction)));

        /* return true if:
        1) projection of fromCenterToP = hitPoint - Center onto 'HorizontalAxis' has norm less than 'Width' / 2 and
        2) projection of fromCenterToP onto VerticalAxis has norm less than Height / 2

        1) cos(HorizontalAxis, fromCenterToP) = (fromCenterToP * HorizontalAxis) / (fromCenterToP.Norm() * 1)
        cos(HorizontalAxis, fromCenterToP) = horizontalDistance / fromCenterToP.Norm()
        horizontalDistance = cos(HorizontalAxis, fromCenterToP) * fromCenterToP.Norm() =
        (fromCenterToP * HorizontalAxis) / (fromCenterToP.Norm() * 1) * fromCenterToP.Norm() =
        fromCenterToP * HorizontalAxis
        horizontalDistance is the norm of the projection of fromCenterToP onto HorizontalAxis

        2) verticalDistance = fromCenterToP * VerticalAxis
        verticalDistance is the norm of the projection of fromCenterToP onto VerticalAxis */

        Vec3f fromCenterToP = Vec3f.Subtract(hitPoint, Center);
        /* the norm of the projection (horizontal or vertical distance) is:
        - negative if the p point is to the left from the Center point on the rectangle
        - positive if the p point is to the right from the Center point on the rectangle
        - equal to 0 if the p point is equal to the Center point */
        final float horizontalLength = Vec3f.Dot(fromCenterToP, HorizontalAxis),
                    verticalLength = Vec3f.Dot(fromCenterToP, VerticalAxis);
        if(horizontalLength <= Width / 2.f && horizontalLength >= -Width / 2.f &&
            verticalLength <= Height / 2.f && verticalLength >= -Height / 2.f)
        {
            if(cosDd < 0) // the ray comes from the side of the rectangle that is pointed by its Direction vector
                normal.CopyFrom(Direction);
            else // the ray comes from the other side of the rectangle
                normal.CopyFrom(Vec3f.Inverse(Direction));
            return true;
        }
        return false;
    }

    @Override
    public void SetDirection(Vec3f direction) {
        Direction = direction;
        RotateAxes();
    }
    public void RotateX(float angle) {
        Direction.RotateX(angle);
        HorizontalAxis.RotateX(angle);
        VerticalAxis.RotateX(angle);
    }
    public void RotateY(float angle) {
        Direction.RotateY(angle);
        HorizontalAxis.RotateY(angle);
        VerticalAxis.RotateY(angle);
    }
    public void RotateZ(float angle) {
        Direction.RotateZ(angle);
        HorizontalAxis.RotateZ(angle);
        VerticalAxis.RotateZ(angle);
    }
    public void RotateAxis(Vec3f axis, float angle) {
        Direction.RotateAxisQuaternion(axis, angle);
        HorizontalAxis.RotateAxisQuaternion(axis, angle);
        VerticalAxis.RotateAxisQuaternion(axis, angle);
    }

    private Vec3f HorizontalAxis, VerticalAxis;
    private void RotateAxes() {
        /*
        Algorithm:
        a) If Direction = (0,0,-1), the rotation axis = (0,0,-1) x Direction = (0,0,0), so the following algorithm will not work and we have to manually set HorizontalAxis to (1,0,0) and VerticalAxis to (0,1,0).
        b) If Direction = (0,0,1), the rotation axis = (0,0,-1) x Direction = (0,0,0), so the following algorithm will not work and we have to manually set HorizontalAxis to (-1,0,0) and VerticalAxis to (0,1,0).
        c) Otherwise:
            newDir means the new value of Direction
            1. Set Direction to newDir, HorizontalAxis to (1,0,0) and VerticalAxis to (0,1,0).
            2. Compute the axis, around which we would rotate (0,0,-1) vector to get newDir.
                axis = ( (0,0,-1) x newDir ).Normalize();
            3. Compute the angle, by which we would rotate (0,0,-1) vector to get newDir.
                a = acosf( (0,0,-1) * newDir )
            4. Rotate HorizontalAxis and VerticalAxis around the axis computed in 2. by the angle computed in 3.
        */
        // method 1
        /*HorizontalAxis = Vec3f(1,0,0);
        VerticalAxis = Vec3f(0,1,0);

        if(Direction.X == 0 && Direction.Y == 0)
            return;

        const Vec3f axis = Vec3f::Cross(Vec3f(0,0,-1), Direction).Normalize();
        const float angle = acosf(Vec3f(0,0,-1) * Direction);

        HorizontalAxis.RotateAxisQuaternion(axis, angle);
        VerticalAxis.RotateAxisQuaternion(axis, angle);*/

        // method 2 - the same as method 1, but with mathematics simplified for efficiency
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