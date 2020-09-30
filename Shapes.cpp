#ifndef SHAPES_CPP
#define SHAPES_CPP

#include "Vector.hpp"
#include <cmath>

struct Light
{
    Vec3f position;
    float intensity;

    Light(const Vec3f &p, const float i) : position(p), intensity(i) {}
};

struct Material
{
    float refractive_index;
    Vec4f albedo;
    Vec3f diffuse_color;
    float specular_exponent;

    Material(const float r, const Vec4f &a, const Vec3f &color, const float spec) : refractive_index(r), albedo(a), diffuse_color(color), specular_exponent(spec) {}
    Material() : refractive_index(1), albedo(1,0,0,0), diffuse_color(), specular_exponent() {}
};

struct Shape
{
    Vec3f Center;
    Material _material;
    Shape(const Vec3f &center, const Material &material) : Center(center), _material(material) {}
    virtual bool RayIntersect(const Vec3f &origin, const Vec3f &direction, float &distance) const = 0;
};

struct Sphere : public Shape
{
    float Radius;

    Sphere(const Vec3f &center, const float radius, const Material &material)
        : Shape(center, material), Radius(radius) {}

    virtual bool RayIntersect(const Vec3f &origin, const Vec3f &direction, float &distance) const override
    {
        Vec3f L = Center - origin;
        float tca = L*direction;
        float d2 = L*L - tca*tca;
        if (d2 > Radius*Radius) return false;
        float thc = sqrtf(Radius*Radius - d2);
        distance = tca - thc;// thc jest zawsze nieujemne, więc tca - thc jest zawsze mniejsze od tca + thc
        if(distance > 0) return true;
        distance = tca + thc;
        return distance > 0;
    }
};

struct Cube : Shape
{
    float Edge;

    Cube(const Vec3f &center, const float edge, const Material &material)
        : Shape(center, material), Edge(edge) {}
    
    virtual bool RayIntersect(const Vec3f &origin, const Vec3f &direction, float &distance) const override
    {
        return false;
    }
};

struct PlainShape : Shape
{
protected:
    Vec3f Direction;
public:
    PlainShape(const Vec3f &center, const Vec3f &normal, const Material &material)
        : Shape(center, material), Direction(normal) {}
    virtual void SetDirection(const Vec3f &direction) { Direction = direction; }
};

struct Circle : PlainShape
{
    float Radius;

    Circle(const Vec3f &center, const float radius, const Vec3f &direction,
    const Material &material)
        : PlainShape(center, direction, material), Radius(radius) {}

    virtual bool RayIntersect(const Vec3f &origin, const Vec3f &direction, float &distance) const override
    {
        distance = ( Direction*(Center - origin) ) / (Direction*direction);
        if(distance <= 0) return false;
        return ((origin + (distance*direction)) - Center).Norm() <= Radius;
    }
};

struct Plane : public PlainShape
{
    Plane(const Vec3f &center, const Vec3f &direction, const Material &material)
        : PlainShape(center, direction, material) {}

    virtual bool RayIntersect(const Vec3f &origin, const Vec3f &direction, float &distance) const override
    {
        distance = ( Direction*(Center - origin) ) / (Direction*direction);
        return distance > 0;
    }
};

struct Rectangle : public PlainShape
{
    float Width, Height;

    Rectangle(const Vec3f &center, const float width, const float height,
        const Vec3f &direction, const Material &material)
        : PlainShape(center, direction, material), Width(width), Height(height)
    { RotateAxes(); }

    virtual bool RayIntersect(const Vec3f &origin, const Vec3f &direction, float &distance) const override
    {
        distance = ( Direction*(Center - origin) ) / (Direction*direction);
        if(distance <= 0) return false;
        Vec3f p = origin + distance*direction;

        /* return true if:
        1) projection of fromCenterToP = p - Center onto 'HorizontalAxis' has norm less than 'Width' / 2 and
        2) projection of fromCenterToP onto VerticalAxis has norm less than Height / 2

        1) cos(HorizontalAxis, fromCenterToP) = (fromCenterToP * HorizontalAxis) / (fromCenterToP.Norm() * 1)
        cos(HorizontalAxis, fromCenterToP) = horizontalDistance / fromCenterToP.Norm()
        horizontalDistance = cos(HorizontalAxis, fromCenterToP) * fromCenterToP.Norm() =
        (fromCenterToP * HorizontalAxis) / (fromCenterToP.Norm() * 1) * fromCenterToP.Norm() =
        fromCenterToP * HorizontalAxis
        horizontalDistance is the norm of the projection of fromCenterToP onto HorizontalAxis

        2) verticalDistance = odleglosc * VerticalAxis
        verticalDistance is the norm of the projection of fromCenterToP onto VerticalAxis */
        
        Vec3f fromCenterToP = p - Center;
        /* the norm of the projection (horizontal or vertical distance) is:
        - negative if the p point is to the left from the Center point on the rectangle
        - positive if the p point is to the right from the Center point on the rectangle
        - equal to 0 if the p point is equal to the Center point */
        const float horizontalLength = fromCenterToP * HorizontalAxis,
                    verticalLength = fromCenterToP * VerticalAxis;
        return(horizontalLength <= Width / 2.f && horizontalLength >= -Width / 2.f &&
            verticalLength <= Height / 2.f && verticalLength >= -Height / 2.f);
    }

    void SetDirection(const Vec3f &direction)
    {
        Direction = direction;
        RotateAxes();
    }

private:
    Vec3f HorizontalAxis, VerticalAxis;
    void RotateAxes()
    {
        const float cosA_and_cosB = Direction.Z,
                    sinB = Direction.X,
                    sinA = Direction.Y;

        HorizontalAxis = Vec3f(1,0,0);
        VerticalAxis = Vec3f(0,1,0);

        HorizontalAxis.RotateX(sinA, cosA_and_cosB);
        HorizontalAxis.RotateY(sinB, cosA_and_cosB);

        VerticalAxis.RotateX(sinA, cosA_and_cosB);
        VerticalAxis.RotateY(sinB, cosA_and_cosB);
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

    virtual bool RayIntersect(const Vec3f &origin, const Vec3f &direction, float &distance) const override
    {
        distance = ( Direction*(Center - origin) ) / (Direction*direction);
        if(distance <= 0) return false;
        Vec3f p = origin + distance*direction;
        return ((p - Center).Norm() + (p - Focus2).Norm() <= FocusDistanceSum);
    }
};
#endif // SHAPES_CPP