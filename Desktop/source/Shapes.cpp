#ifndef SHAPES_CPP
#define SHAPES_CPP

#include "../include/Vector.hpp"
#include <cmath>

struct Light
{
    Vec3f Position;
    float Intensity;

    Light(const Vec3f &position, const float intensity) : Position(position), Intensity(intensity) {}
};

struct Material
{
    float RefractiveIndex;
    Vec4f Albedo;
    Vec3f DiffuseColor;
    float SpecularExponent;

    Material(const float refractiveIndex, const Vec4f &albedo, const Vec3f &color, const float specularExponent)
        : RefractiveIndex(refractiveIndex), Albedo(albedo), DiffuseColor(color), SpecularExponent(specularExponent) {}
    Material() : RefractiveIndex(1), Albedo(1,0,0,0), DiffuseColor(), SpecularExponent() {}
};

struct Shape
{
    Vec3f Center;
    Material Surface;
    Shape(const Vec3f &center, const Material &material) : Center(center), Surface(material) {}
    virtual bool RayIntersect(const Vec3f &origin, const Vec3f &direction, float &distance, Vec3f &hitPoint, Vec3f &normal)
        const = 0;
};

struct Sphere : public Shape
{
    float Radius;

    Sphere(const Vec3f &center, const float radius, const Material &material)
        : Shape(center, material), Radius(radius) {}

    virtual bool RayIntersect(const Vec3f &origin, const Vec3f &direction, float &distance, 
        Vec3f &hitPoint, Vec3f &normal) const override
    {
        Vec3f L = Center - origin;
        float tca = L*direction;
        float d2 = L*L - tca*tca;
        if (d2 > Radius*Radius) return false;
        float thc = sqrtf(Radius*Radius - d2);
        distance = tca - thc;// thc jest zawsze nieujemne, więc tca - thc jest zawsze mniejsze od tca + thc
        if(distance > 0)
        {
            hitPoint = origin + distance * direction;
            // if(L.Norm() >= Radius)
            if(L*L >= Radius*Radius)
                normal = (hitPoint - Center).Normalize();
            else
                normal = (Center - hitPoint).Normalize();
            return true;
        }
        distance = tca + thc;
        if(distance > 0)
        {
            hitPoint = origin + distance * direction;
            // if(L.Norm() >= Radius)
            if(L*L >= Radius*Radius)
                normal = (hitPoint - Center).Normalize();
            else
                normal = (Center - hitPoint).Normalize();
            return true;
        }
        return false;
    }
};

struct Cube : public Shape
{
    float Edge;

    Cube(const Vec3f &center, const float edge, const Material &material)
        : Shape(center, material), Edge(edge) {}
    
    virtual bool RayIntersect(const Vec3f &origin, const Vec3f &direction, float &distance, 
        Vec3f &hitPoint, Vec3f &normal) const override
    {
        return false;
    }
};

struct PlainShape : public Shape
{
protected:
    Vec3f Direction;
public:
    PlainShape(const Vec3f &center, const Vec3f &normal, const Material &material)
        : Shape(center, material), Direction(normal) {}
    virtual void SetDirection(const Vec3f &direction) { Direction = direction; }
};

struct Circle : public PlainShape
{
    float Radius;

    Circle(const Vec3f &center, const float radius, const Vec3f &direction,
    const Material &material)
        : PlainShape(center, direction, material), Radius(radius) {}

    virtual bool RayIntersect(const Vec3f &origin, const Vec3f &direction, float &distance, 
        Vec3f &hitPoint, Vec3f &normal) const override
    {
        const float cosDd = Direction*direction;
        if(cosDd == 0) return false; // the ray does not hit the circle, because they are parallel
        distance = ( Direction*(Center - origin) ) / cosDd;
        if(distance <= 0) return false;
        hitPoint = origin + distance * direction;
        // (hitPoint - Center).Norm() <= Radius
        const Vec3f fromCenterToP = hitPoint - Center;
        if(fromCenterToP*fromCenterToP <= Radius*Radius)
        {
            if(cosDd < 0) // the ray comes from the side of the circle that is pointed by its Direction vector
                normal = Direction;
            else // the ray comes from the other side of the circle
                normal = -Direction;
            return true;
        }
        return false;
    }
};

struct Plane : public PlainShape
{
    Plane(const Vec3f &center, const Vec3f &direction, const Material &material)
        : PlainShape(center, direction, material) {}

    virtual bool RayIntersect(const Vec3f &origin, const Vec3f &direction, float &distance, 
        Vec3f &hitPoint, Vec3f &normal) const override
    {
        const float cosDd = Direction*direction;
        if(cosDd == 0) return false; // the ray does not hit the plane, because they are parallel
        distance = ( Direction*(Center - origin) ) / cosDd;
        if(distance > 0)
        {
            hitPoint = origin + distance * direction;
            if(cosDd < 0) // the ray comes from the side of the plane that is pointed by its Direction vector
                normal = Direction;
            else // the ray comes from the other side of the plane
                normal = -Direction;
            return true;
        }
        return false;
    }
};

struct Rectangle : public PlainShape
{
    float Width, Height;

    Rectangle(const Vec3f &center, const float width, const float height,
        const Vec3f &direction, const Material &material)
        : PlainShape(center, direction, material), Width(width), Height(height)
    { RotateAxes(); }

    virtual bool RayIntersect(const Vec3f &origin, const Vec3f &direction, float &distance, 
        Vec3f &hitPoint, Vec3f &normal) const override
    {
        const float cosDd = Direction*direction;
        if(cosDd == 0) return false; // the ray does not hit the rectangle, because they are parallel
        distance = ( Direction*(Center - origin) ) / cosDd;
        if(distance <= 0) return false;
        hitPoint = origin + distance * direction;

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
        
        Vec3f fromCenterToP = hitPoint - Center;
        /* the norm of the projection (horizontal or vertical distance) is:
        - negative if the p point is to the left from the Center point on the rectangle
        - positive if the p point is to the right from the Center point on the rectangle
        - equal to 0 if the p point is equal to the Center point */
        const float horizontalLength = fromCenterToP * HorizontalAxis,
                    verticalLength = fromCenterToP * VerticalAxis;
        if(horizontalLength <= Width / 2.f && horizontalLength >= -Width / 2.f &&
            verticalLength <= Height / 2.f && verticalLength >= -Height / 2.f)
        {
            if(cosDd < 0) // the ray comes from the side of the rectangle that is pointed by its Direction vector
                normal = Direction;
            else // the ray comes from the other side of the rectangle
                normal = -Direction;
            return true;
        }
        return false;
    }

    virtual void SetDirection(const Vec3f &direction)
    {
        Direction = direction;
        RotateAxes();
    }
    void RotateX(float angle)
    {
        Direction.RotateX(angle);
        HorizontalAxis.RotateX(angle);
        VerticalAxis.RotateX(angle);
    }
    void RotateY(float angle)
    {
        Direction.RotateY(angle);
        HorizontalAxis.RotateY(angle);
        VerticalAxis.RotateY(angle);
    }
    void RotateZ(float angle)
    {
        Direction.RotateZ(angle);
        HorizontalAxis.RotateZ(angle);
        VerticalAxis.RotateZ(angle);
    }
    void RotateAxis(const Vec3f &axis, float angle)
    {
        Direction.RotateAxisQuaternion(axis, angle);
        HorizontalAxis.RotateAxisQuaternion(axis, angle);
        VerticalAxis.RotateAxisQuaternion(axis, angle);
    }

private:
    Vec3f HorizontalAxis, VerticalAxis;
    void RotateAxes()
    {
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
            VerticalAxis = Vec3f(0,1,0);
            if(Direction.Z == -1)
                HorizontalAxis = Vec3f(1,0,0);
            else // if(Direction.Z == 1)
                HorizontalAxis = Vec3f(-1,0,0);
        }
        else
        {
            const float x = Direction.X, y = Direction.Y, z = Direction.Z, commonCoefficient = (1.f + z) / (x*x + y*y);

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
};

struct Ellipse : public PlainShape
{
    Vec3f Focus2;//Center == Focus1
    float FocusDistanceSum;

    /* additionalFocusesDistance - a value that is added to the distance between ellipse's 
    focuses to avoid situation, when user given FocusDistanceSum is less than actual distance 
    between focuses, so the ellipse does not exist */

    Ellipse(const Vec3f &center1, const Vec3f &center2, const float additionalFocusesDistance,
    const Vec3f &direction, const Material &material)
        : PlainShape(center1, direction, material), Focus2(center2),
        FocusDistanceSum((center1 - center2).Norm() + additionalFocusesDistance) {}

    virtual bool RayIntersect(const Vec3f &origin, const Vec3f &direction, float &distance, 
        Vec3f &hitPoint, Vec3f &normal) const override
    {
        const float cosDd = Direction*direction;
        if(cosDd == 0) return false; // the ray does not hit the ellipse, because they are parallel
        distance = ( Direction*(Center - origin) ) / cosDd;
        if(distance <= 0) return false;
        hitPoint = origin + distance * direction;
        if((hitPoint - Center).Norm() + (hitPoint - Focus2).Norm() <= FocusDistanceSum)
        {
            if(cosDd < 0) // the ray comes from the side of the ellipse that is pointed by its Direction vector
                normal = Direction;
            else // the ray comes from the other side of the ellipse
                normal = -Direction;
            return true;
        }
        return false;
    }
};
#endif // SHAPES_CPP