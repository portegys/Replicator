/*
 * Mathematical objects.
 * Source: "Physics for Game Developers", Copyright 2000-2001 by David Bourg.
 */

#ifndef __MATH_ETC__
#define __MATH_ETC__

#include <math.h>

//------------------------------------------------------------------------//
// Misc. Constants
//------------------------------------------------------------------------//

float   const   pi  = 3.14159265f;
float   const   g   = -32.174f;                   // acceleration due to gravity, ft/s^2
float   const   rho = 0.0023769f;                 // desity of air at sea level, slugs/ft^3
float   const   tol = 0.0000000001f;              // float type tolerance

//------------------------------------------------------------------------//
// Misc. Functions
//------------------------------------------------------------------------//
inline  float   DegreesToRadians(float deg);
inline  float   RadiansToDegrees(float rad);

inline  float   DegreesToRadians(float deg)
{
    return deg * pi / 180.0f;
}


inline  float   RadiansToDegrees(float rad)
{
    return rad * 180.0f / pi;
}


//------------------------------------------------------------------------//
// 3D Vector3D Class and vector functions
//------------------------------------------------------------------------//
class Vector3D
{
    public:
        float x;
        float y;
        float z;

        Vector3D(void);
        Vector3D(float xi, float yi, float zi);

        float Magnitude(void);
        void  Normalize(void);
        void  Normalize(float);
        void  Reverse(void);
        float Distance(Vector3D);
        float SquareDistance(Vector3D);
        void  Zero(void);

        Vector3D& operator+=(Vector3D u);         // vector addition
        Vector3D& operator-=(Vector3D u);         // vector subtraction
        Vector3D& operator*=(float s);            // scalar multiply
        Vector3D& operator/=(float s);            // scalar divide

        Vector3D operator-(void);

};

inline  Vector3D operator+(Vector3D u, Vector3D v);
inline  Vector3D operator-(Vector3D u, Vector3D v);
inline  Vector3D operator^(Vector3D u, Vector3D v);
inline  float operator*(Vector3D u, Vector3D v);
inline  Vector3D operator*(float s, Vector3D u);
inline  Vector3D operator*(Vector3D u, float s);
inline  Vector3D operator/(Vector3D u, float s);
inline  float TripleScalarProduct(Vector3D u, Vector3D v, Vector3D w);

inline Vector3D::Vector3D(void)
{
    x = 0;
    y = 0;
    z = 0;
}


inline Vector3D::Vector3D(float xi, float yi, float zi)
{
    x = xi;
    y = yi;
    z = zi;
}


inline  float Vector3D::Magnitude(void)
{
    return (float) sqrt(x*x + y*y + z*z);
}


inline  void  Vector3D::Normalize(void)
{
    float m = (float) sqrt(x*x + y*y + z*z);
    if(m <= tol) m = 1;
    x /= m;
    y /= m;
    z /= m;

    if (fabs(x) < tol) x = 0.0f;
    if (fabs(y) < tol) y = 0.0f;
    if (fabs(z) < tol) z = 0.0f;
}


inline  void  Vector3D::Normalize(float n)
{
    float m = (float) sqrt(x*x + y*y + z*z);
    if(m <= tol) m = 1;
    m = n / m;
    x *= m;
    y *= m;
    z *= m;

    if (fabs(x) < tol) x = 0.0f;
    if (fabs(y) < tol) y = 0.0f;
    if (fabs(z) < tol) z = 0.0f;
}


inline  void  Vector3D::Reverse(void)
{
    x = -x;
    y = -y;
    z = -z;
}


inline  float Vector3D::Distance(Vector3D v)
{
    float dx,dy,dz;

    dx = x - v.x;
    dy = y - v.y;
    dz = z - v.z;
    return (float) sqrt(dx*dx + dy*dy + dz*dz);
}


inline  float Vector3D::SquareDistance(Vector3D v)
{
    float dx,dy,dz;

    dx = x - v.x;
    dy = y - v.y;
    dz = z - v.z;
    return (float) (dx*dx + dy*dy + dz*dz);
}


inline  void  Vector3D::Zero(void)
{
    x = 0.0;
    y = 0.0;
    z = 0.0;
}


inline Vector3D& Vector3D::operator+=(Vector3D u)
{
    x += u.x;
    y += u.y;
    z += u.z;
    return *this;
}


inline  Vector3D& Vector3D::operator-=(Vector3D u)
{
    x -= u.x;
    y -= u.y;
    z -= u.z;
    return *this;
}


inline  Vector3D& Vector3D::operator*=(float s)
{
    x *= s;
    y *= s;
    z *= s;
    return *this;
}


inline  Vector3D& Vector3D::operator/=(float s)
{
    x /= s;
    y /= s;
    z /= s;
    return *this;
}


inline  Vector3D Vector3D::operator-(void)
{
    return Vector3D(-x, -y, -z);
}


inline  Vector3D operator+(Vector3D u, Vector3D v)
{
    return Vector3D(u.x + v.x, u.y + v.y, u.z + v.z);
}


inline  Vector3D operator-(Vector3D u, Vector3D v)
{
    return Vector3D(u.x - v.x, u.y - v.y, u.z - v.z);
}


// Vector3D cross product (u cross v)
inline  Vector3D operator^(Vector3D u, Vector3D v)
{
    return Vector3D(    u.y*v.z - u.z*v.y,
        -u.x*v.z + u.z*v.x,
        u.x*v.y - u.y*v.x );
}


// Vector3D dot product
inline  float operator*(Vector3D u, Vector3D v)
{
    return (u.x*v.x + u.y*v.y + u.z*v.z);
}


inline  Vector3D operator*(float s, Vector3D u)
{
    return Vector3D(u.x*s, u.y*s, u.z*s);
}


inline  Vector3D operator*(Vector3D u, float s)
{
    return Vector3D(u.x*s, u.y*s, u.z*s);
}


inline  Vector3D operator/(Vector3D u, float s)
{
    return Vector3D(u.x/s, u.y/s, u.z/s);
}


// triple scalar product (u dot (v cross w))
inline  float TripleScalarProduct(Vector3D u, Vector3D v, Vector3D w)
{
    return float(   (u.x * (v.y*w.z - v.z*w.y)) +
        (u.y * (-v.x*w.z + v.z*w.x)) +
        (u.z * (v.x*w.y - v.y*w.x)) );
    //return u*(v^w);

}


//------------------------------------------------------------------------//
// Matrix Class and matrix functions
//------------------------------------------------------------------------//

class Matrix3x3
{
    public:
        // elements eij: i -> row, j -> column
        float   e11, e12, e13, e21, e22, e23, e31, e32, e33;

        Matrix3x3(void);
        Matrix3x3(  float r1c1, float r1c2, float r1c3,
            float r2c1, float r2c2, float r2c3,
            float r3c1, float r3c2, float r3c3 );

        float   det(void);
        Matrix3x3   Transpose(void);
        Matrix3x3   Inverse(void);

        Matrix3x3& operator+=(Matrix3x3 m);
        Matrix3x3& operator-=(Matrix3x3 m);
        Matrix3x3& operator*=(float s);
        Matrix3x3& operator/=(float s);
};

inline  Matrix3x3 operator+(Matrix3x3 m1, Matrix3x3 m2);
inline  Matrix3x3 operator-(Matrix3x3 m1, Matrix3x3 m2);
inline  Matrix3x3 operator/(Matrix3x3 m, float s);
inline  Matrix3x3 operator*(Matrix3x3 m1, Matrix3x3 m2);
inline  Matrix3x3 operator*(Matrix3x3 m, float s);
inline  Matrix3x3 operator*(float s, Matrix3x3 m);
inline  Vector3D operator*(Matrix3x3 m, Vector3D u);
inline  Vector3D operator*(Vector3D u, Matrix3x3 m);

inline  Matrix3x3::Matrix3x3(void)
{
    e11 = 0;
    e12 = 0;
    e13 = 0;
    e21 = 0;
    e22 = 0;
    e23 = 0;
    e31 = 0;
    e32 = 0;
    e33 = 0;
}


inline  Matrix3x3::Matrix3x3(   float r1c1, float r1c2, float r1c3,
float r2c1, float r2c2, float r2c3,
float r3c1, float r3c2, float r3c3 )
{
    e11 = r1c1;
    e12 = r1c2;
    e13 = r1c3;
    e21 = r2c1;
    e22 = r2c2;
    e23 = r2c3;
    e31 = r3c1;
    e32 = r3c2;
    e33 = r3c3;
}


inline  float   Matrix3x3::det(void)
{
    return  e11*e22*e33 -
        e11*e32*e23 +
        e21*e32*e13 -
        e21*e12*e33 +
        e31*e12*e23 -
        e31*e22*e13;
}


inline  Matrix3x3   Matrix3x3::Transpose(void)
{
    return Matrix3x3(e11,e21,e31,e12,e22,e32,e13,e23,e33);
}


inline  Matrix3x3   Matrix3x3::Inverse(void)
{
    float   d = e11*e22*e33 -
        e11*e32*e23 +
        e21*e32*e13 -
        e21*e12*e33 +
        e31*e12*e23 -
        e31*e22*e13;

    if (d == 0) d = 1;

    return  Matrix3x3(  (e22*e33-e23*e32)/d,
        -(e12*e33-e13*e32)/d,
        (e12*e23-e13*e22)/d,
        -(e21*e33-e23*e31)/d,
        (e11*e33-e13*e31)/d,
        -(e11*e23-e13*e21)/d,
        (e21*e32-e22*e31)/d,
        -(e11*e32-e12*e31)/d,
        (e11*e22-e12*e21)/d );
}


inline  Matrix3x3& Matrix3x3::operator+=(Matrix3x3 m)
{
    e11 += m.e11;
    e12 += m.e12;
    e13 += m.e13;
    e21 += m.e21;
    e22 += m.e22;
    e23 += m.e23;
    e31 += m.e31;
    e32 += m.e32;
    e33 += m.e33;
    return *this;
}


inline  Matrix3x3& Matrix3x3::operator-=(Matrix3x3 m)
{
    e11 -= m.e11;
    e12 -= m.e12;
    e13 -= m.e13;
    e21 -= m.e21;
    e22 -= m.e22;
    e23 -= m.e23;
    e31 -= m.e31;
    e32 -= m.e32;
    e33 -= m.e33;
    return *this;
}


inline  Matrix3x3& Matrix3x3::operator*=(float s)
{
    e11 *= s;
    e12 *= s;
    e13 *= s;
    e21 *= s;
    e22 *= s;
    e23 *= s;
    e31 *= s;
    e32 *= s;
    e33 *= s;
    return *this;
}


inline  Matrix3x3& Matrix3x3::operator/=(float s)
{
    e11 /= s;
    e12 /= s;
    e13 /= s;
    e21 /= s;
    e22 /= s;
    e23 /= s;
    e31 /= s;
    e32 /= s;
    e33 /= s;
    return *this;
}


inline  Matrix3x3 operator+(Matrix3x3 m1, Matrix3x3 m2)
{
    return  Matrix3x3(  m1.e11+m2.e11,
        m1.e12+m2.e12,
        m1.e13+m2.e13,
        m1.e21+m2.e21,
        m1.e22+m2.e22,
        m1.e23+m2.e23,
        m1.e31+m2.e31,
        m1.e32+m2.e32,
        m1.e33+m2.e33);
}


inline  Matrix3x3 operator-(Matrix3x3 m1, Matrix3x3 m2)
{
    return  Matrix3x3(  m1.e11-m2.e11,
        m1.e12-m2.e12,
        m1.e13-m2.e13,
        m1.e21-m2.e21,
        m1.e22-m2.e22,
        m1.e23-m2.e23,
        m1.e31-m2.e31,
        m1.e32-m2.e32,
        m1.e33-m2.e33);
}


inline  Matrix3x3 operator/(Matrix3x3 m, float s)
{
    return  Matrix3x3(  m.e11/s,
        m.e12/s,
        m.e13/s,
        m.e21/s,
        m.e22/s,
        m.e23/s,
        m.e31/s,
        m.e32/s,
        m.e33/s);
}


inline  Matrix3x3 operator*(Matrix3x3 m1, Matrix3x3 m2)
{
    return Matrix3x3(   m1.e11*m2.e11 + m1.e12*m2.e21 + m1.e13*m2.e31,
        m1.e11*m2.e12 + m1.e12*m2.e22 + m1.e13*m2.e32,
        m1.e11*m2.e13 + m1.e12*m2.e23 + m1.e13*m2.e33,
        m1.e21*m2.e11 + m1.e22*m2.e21 + m1.e23*m2.e31,
        m1.e21*m2.e12 + m1.e22*m2.e22 + m1.e23*m2.e32,
        m1.e21*m2.e13 + m1.e22*m2.e23 + m1.e23*m2.e33,
        m1.e31*m2.e11 + m1.e32*m2.e21 + m1.e33*m2.e31,
        m1.e31*m2.e12 + m1.e32*m2.e22 + m1.e33*m2.e32,
        m1.e31*m2.e13 + m1.e32*m2.e23 + m1.e33*m2.e33 );
}


inline  Matrix3x3 operator*(Matrix3x3 m, float s)
{
    return  Matrix3x3(  m.e11*s,
        m.e12*s,
        m.e13*s,
        m.e21*s,
        m.e22*s,
        m.e23*s,
        m.e31*s,
        m.e32*s,
        m.e33*s);
}


inline  Matrix3x3 operator*(float s, Matrix3x3 m)
{
    return  Matrix3x3(  m.e11*s,
        m.e12*s,
        m.e13*s,
        m.e21*s,
        m.e22*s,
        m.e23*s,
        m.e31*s,
        m.e32*s,
        m.e33*s);
}


inline  Vector3D operator*(Matrix3x3 m, Vector3D u)
{
    return Vector3D(    m.e11*u.x + m.e12*u.y + m.e13*u.z,
        m.e21*u.x + m.e22*u.y + m.e23*u.z,
        m.e31*u.x + m.e32*u.y + m.e33*u.z);
}


inline  Vector3D operator*(Vector3D u, Matrix3x3 m)
{
    return Vector3D(    u.x*m.e11 + u.y*m.e21 + u.z*m.e31,
        u.x*m.e12 + u.y*m.e22 + u.z*m.e32,
        u.x*m.e13 + u.y*m.e23 + u.z*m.e33);
}


//------------------------------------------------------------------------//
// Quaternion Class and Quaternion functions
//------------------------------------------------------------------------//

class Quaternion
{
    public:
        float   n;                                // number (scalar) part
        Vector3D    v;                            // vector part: v.x, v.y, v.z

        Quaternion(void);
        Quaternion(float e0, float e1, float e2, float e3);

        float   Magnitude(void);
        Vector3D    GetVector3D(void);
        float   GetScalar(void);
        Quaternion  operator+=(Quaternion q);
        Quaternion  operator-=(Quaternion q);
        Quaternion operator*=(float s);
        Quaternion operator/=(float s);
        Quaternion  operator~(void) const { return Quaternion(n, -v.x, -v.y, -v.z);}
};

inline  Quaternion operator+(Quaternion q1, Quaternion q2);
inline  Quaternion operator-(Quaternion q1, Quaternion q2);
inline  Quaternion operator*(Quaternion q1, Quaternion q2);
inline  Quaternion operator*(Quaternion q, float s);
inline  Quaternion operator*(float s, Quaternion q);
inline  Quaternion operator*(Quaternion q, Vector3D v);
inline  Quaternion operator*(Vector3D v, Quaternion q);
inline  Quaternion operator/(Quaternion q, float s);
inline  float QGetAngle(Quaternion q);
inline  Vector3D QGetAxis(Quaternion q);
inline  Quaternion QRotate(Quaternion q1, Quaternion q2);
inline  Vector3D    QVRotate(Quaternion q, Vector3D v);
inline  Quaternion  MakeQFromEulerAngles(float x, float y, float z);
inline  Vector3D    MakeEulerAnglesFromQ(Quaternion q);

inline  Quaternion::Quaternion(void)
{
    n = 0;
    v.x = 0;
    v.y =  0;
    v.z = 0;
}


inline  Quaternion::Quaternion(float e0, float e1, float e2, float e3)
{
    n = e0;
    v.x = e1;
    v.y = e2;
    v.z = e3;
}


inline  float   Quaternion::Magnitude(void)
{
    return (float) sqrt(n*n + v.x*v.x + v.y*v.y + v.z*v.z);
}


inline  Vector3D    Quaternion::GetVector3D(void)
{
    return Vector3D(v.x, v.y, v.z);
}


inline  float   Quaternion::GetScalar(void)
{
    return n;
}


inline  Quaternion  Quaternion::operator+=(Quaternion q)
{
    n += q.n;
    v.x += q.v.x;
    v.y += q.v.y;
    v.z += q.v.z;
    return *this;
}


inline  Quaternion  Quaternion::operator-=(Quaternion q)
{
    n -= q.n;
    v.x -= q.v.x;
    v.y -= q.v.y;
    v.z -= q.v.z;
    return *this;
}


inline  Quaternion Quaternion::operator*=(float s)
{
    n *= s;
    v.x *= s;
    v.y *= s;
    v.z *= s;
    return *this;
}


inline  Quaternion Quaternion::operator/=(float s)
{
    n /= s;
    v.x /= s;
    v.y /= s;
    v.z /= s;
    return *this;
}


/*inline	Quaternion	Quaternion::operator~()
{
    return Quaternion(n, -v.x, -v.y, -v.z);
}*/

inline  Quaternion operator+(Quaternion q1, Quaternion q2)
{
    return  Quaternion( q1.n + q2.n,
        q1.v.x + q2.v.x,
        q1.v.y + q2.v.y,
        q1.v.z + q2.v.z);
}


inline  Quaternion operator-(Quaternion q1, Quaternion q2)
{
    return  Quaternion( q1.n - q2.n,
        q1.v.x - q2.v.x,
        q1.v.y - q2.v.y,
        q1.v.z - q2.v.z);
}


inline  Quaternion operator*(Quaternion q1, Quaternion q2)
{
    return  Quaternion( q1.n*q2.n - q1.v.x*q2.v.x - q1.v.y*q2.v.y - q1.v.z*q2.v.z,
        q1.n*q2.v.x + q1.v.x*q2.n + q1.v.y*q2.v.z - q1.v.z*q2.v.y,
        q1.n*q2.v.y + q1.v.y*q2.n + q1.v.z*q2.v.x - q1.v.x*q2.v.z,
        q1.n*q2.v.z + q1.v.z*q2.n + q1.v.x*q2.v.y - q1.v.y*q2.v.x);
}


inline  Quaternion operator*(Quaternion q, float s)
{
    return  Quaternion(q.n*s, q.v.x*s, q.v.y*s, q.v.z*s);
}


inline  Quaternion operator*(float s, Quaternion q)
{
    return  Quaternion(q.n*s, q.v.x*s, q.v.y*s, q.v.z*s);
}


inline  Quaternion operator*(Quaternion q, Vector3D v)
{
    return  Quaternion( -(q.v.x*v.x + q.v.y*v.y + q.v.z*v.z),
        q.n*v.x + q.v.y*v.z - q.v.z*v.y,
        q.n*v.y + q.v.z*v.x - q.v.x*v.z,
        q.n*v.z + q.v.x*v.y - q.v.y*v.x);
}


inline  Quaternion operator*(Vector3D v, Quaternion q)
{
    return  Quaternion( -(q.v.x*v.x + q.v.y*v.y + q.v.z*v.z),
        q.n*v.x + q.v.z*v.y - q.v.y*v.z,
        q.n*v.y + q.v.x*v.z - q.v.z*v.x,
        q.n*v.z + q.v.y*v.x - q.v.x*v.y);
}


inline  Quaternion operator/(Quaternion q, float s)
{
    return  Quaternion(q.n/s, q.v.x/s, q.v.y/s, q.v.z/s);
}


inline  float QGetAngle(Quaternion q)
{
    return  (float) (2*acos(q.n));
}


inline  Vector3D QGetAxis(Quaternion q)
{
    Vector3D v;
    float m;

    v = q.GetVector3D();
    m = v.Magnitude();

    if (m <= tol)
        return Vector3D();
    else
        return v/m;
}


inline  Quaternion QRotate(Quaternion q1, Quaternion q2)
{
    return  q1*q2*(~q1);
}


inline  Vector3D    QVRotate(Quaternion q, Vector3D v)
{
    Quaternion t;

    t = q*v*(~q);

    return  t.GetVector3D();
}


inline  Quaternion  MakeQFromEulerAngles(float x, float y, float z)
{
    Quaternion  q;
    double  roll = DegreesToRadians(x);
    double  pitch = DegreesToRadians(y);
    double  yaw = DegreesToRadians(z);

    double  cyaw, cpitch, croll, syaw, spitch, sroll;
    double  cyawcpitch, syawspitch, cyawspitch, syawcpitch;

    cyaw = cos(0.5f * yaw);
    cpitch = cos(0.5f * pitch);
    croll = cos(0.5f * roll);
    syaw = sin(0.5f * yaw);
    spitch = sin(0.5f * pitch);
    sroll = sin(0.5f * roll);

    cyawcpitch = cyaw*cpitch;
    syawspitch = syaw*spitch;
    cyawspitch = cyaw*spitch;
    syawcpitch = syaw*cpitch;

    q.n = (float) (cyawcpitch * croll + syawspitch * sroll);
    q.v.x = (float) (cyawcpitch * sroll - syawspitch * croll);
    q.v.y = (float) (cyawspitch * croll + syawcpitch * sroll);
    q.v.z = (float) (syawcpitch * croll - cyawspitch * sroll);

    return q;
}


inline  Vector3D    MakeEulerAnglesFromQ(Quaternion q)
{
    double  r11, r21, r31, r32, r33, r12, r13;
    double  q00, q11, q22, q33;
    double  tmp;
    Vector3D    u;

    q00 = q.n * q.n;
    q11 = q.v.x * q.v.x;
    q22 = q.v.y * q.v.y;
    q33 = q.v.z * q.v.z;

    r11 = q00 + q11 - q22 - q33;
    r21 = 2 * (q.v.x*q.v.y + q.n*q.v.z);
    r31 = 2 * (q.v.x*q.v.z - q.n*q.v.y);
    r32 = 2 * (q.v.y*q.v.z + q.n*q.v.x);
    r33 = q00 - q11 - q22 + q33;

    tmp = fabs(r31);
    if(tmp > 0.999999)
    {
        r12 = 2 * (q.v.x*q.v.y - q.n*q.v.z);
        r13 = 2 * (q.v.x*q.v.z + q.n*q.v.y);

        u.x = RadiansToDegrees(0.0f);             //roll
                                                  // pitch
        u.y = RadiansToDegrees((float) (-(pi/2) * r31/tmp));
                                                  // yaw
        u.z = RadiansToDegrees((float) atan2(-r12, -r31*r13));
        return u;
    }

                                                  // roll
    u.x = RadiansToDegrees((float) atan2(r32, r33));
    u.y = RadiansToDegrees((float) asin(-r31));   // pitch
                                                  // yaw
    u.z = RadiansToDegrees((float) atan2(r21, r11));
    return u;

}
#endif
