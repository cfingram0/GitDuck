#pragma once
constexpr float kEpsilon = 0.0001f;

bool FloatIsEqual(float a, float b, float epsilon = kEpsilon);
namespace tag {
  struct NoInit {};
  struct Identity {};
  struct Zero {};
}

class Vec2 {
public:
  union {
    struct {
      float x;
      float y;
    };

    float m[2];
  };

  Vec2(tag::NoInit) {}
  Vec2(tag::Zero) : x(0), y(0) {}
  Vec2(const Vec2 & rhs) : x(rhs.x), y(rhs.y) {}
  Vec2(float xArg, float yArg) : x(xArg), y(yArg) {}
  Vec2 & operator=(const Vec2 & rhs) {
    x = rhs.x;
    y = rhs.y;
  }

  //addition
  Vec2 operator+ (const Vec2 & rhs) const;
  Vec2 & operator+= (const Vec2 & rhs);

  //subtraction
  Vec2 operator- (const Vec2 & rhs) const;
  Vec2 & operator-= (const Vec2 & rhs);

  //negation
  Vec2 operator- () const;

  //scalar multiplication
  Vec2 operator* (float rhs) const;
  Vec2 & operator*= (float rhs);

  //scalar division
  Vec2 operator/ (float rhs) const;
  Vec2 & operator/= (float rhs);
};

class Vec3 {
public:
  union {
    struct {
      float x;
      float y;
      float z;
    };

    float m[3];
  };

  Vec3(tag::NoInit) {}
  Vec3(tag::Zero) : x(0), y(0), z(0) {}
  Vec3(const Vec3 & rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {}
  Vec3(float xArg, float yArg, float zArg) : x(xArg), y(yArg), z(zArg) {}
  Vec3 & operator=(const Vec3 & rhs) {
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    return *this;
  }

  //addition
  Vec3 operator+ (const Vec3 & rhs) const;
  Vec3 & operator+= (const Vec3 & rhs);

  //subtraction
  Vec3 operator- (const Vec3 & rhs) const;
  Vec3 & operator-= (const Vec3 & rhs);

  //negation
  Vec3 operator- () const;

  //scalar multiplication
  Vec3 operator* (float rhs) const;
  Vec3 & operator*= (float rhs);

  //scalar division
  Vec3 operator/ (float rhs) const;
  Vec3 & operator/= (float rhs);
};

class Vec4 {
public:
  union {
    struct {
      float x;
      float y;
      float z;
      float w;
    };

    float m[4];
  };

  Vec4(tag::NoInit) {}
  Vec4(tag::Zero) : x(0), y(0), z(0), w(0) {}
  Vec4(const Vec4 & rhs) : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {}
  Vec4(float xArg, float yArg, float zArg, float wArg) : x(xArg), y(yArg), z(zArg), w(wArg) {}
  Vec4(const Vec3 & rhs, float rhsW) : x(rhs.x), y(rhs.y), z(rhs.z), w(rhsW) {}
  Vec4 & operator=(const Vec4 & rhs) {
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    w = rhs.w;
    return *this;
  }

  //addition
  Vec4 operator+ (const Vec4 & rhs) const;
  Vec4 & operator+= (const Vec4 & rhs);

  //subtraction
  Vec4 operator- (const Vec4 & rhs) const;
  Vec4 & operator-= (const Vec4 & rhs);

  //negation
  Vec4 operator- () const;

  //scalar multiplication
  Vec4 operator* (float rhs) const;
  Vec4 & operator*= (float rhs);

  //scalar division
  Vec4 operator/ (float rhs) const;
  Vec4 & operator/= (float rhs);

  Vec3 xyz() const;
};

class Matrix4;
class Quaternion {
public:
  union {
    struct {
      float s;
      float x;
      float y;
      float z;
    };

    float m[4];
  };

  Quaternion(float RadAngle, const Vec4& Axis);
  Quaternion(float RadAngle, const Vec3& Axis);
  Quaternion(float S, float X, float Y, float Z);
  Quaternion(const Matrix4& RotMatrix);
  Quaternion(float eulerX, float eulerY, float eulerZ);
  
  Quaternion(tag::NoInit) {}
  Quaternion(tag::Identity) :s(0), x(0), y(0), z(0) {}
  Quaternion(tag::Zero) :s(0), x(0), y(0), z(0) {}

  Quaternion operator+ (const Quaternion& rhs) const;
  Quaternion operator- (const Quaternion& rhs) const;
  Quaternion operator* (const Quaternion& rhs) const;
  Quaternion operator/ (const Quaternion& rhs) const;
  Quaternion operator* (float rhs) const;
  Quaternion operator/ (float rhs) const;

  Quaternion& operator+=(const Quaternion& rhs);
  Quaternion& operator*=(const Quaternion& rhs);
  Quaternion& operator/= (const Quaternion& rhs);
  Quaternion& operator*= (float rhs);
  Quaternion& operator/= (float rhs);

  Quaternion Inverse(void) const;

  Quaternion GetConjugate(void) const;

  Vec4 RotateVec(const Vec4& rhs) const;
  
  Quaternion GetNormalized(void) const;

  static Quaternion Lerp(const Quaternion& Source, const Quaternion& Dest, float t);
  static Quaternion SLerp(const Quaternion& Source, const Quaternion& Dest, float t);

  Matrix4 CalcMatrix(void) const;
};

class Matrix4 {
public:
  union {
    float v[16];
    float m[4][4];
    struct {
      float m00; float m01; float m02; float m03;
      float m10; float m11; float m12; float m13;
      float m20; float m21; float m22; float m23;
      float m30; float m31; float m32; float m33;
    };
  };

  Matrix4(float M00, float M01, float M02, float M03,
    float M10, float M11, float M12, float M13,
    float M20, float M21, float M22, float M23,
    float M30, float M31, float M32, float M33)
    :m00(M00), m01(M01), m02(M02), m03(M03),
    m10(M10), m11(M11), m12(M12), m13(M13),
    m20(M20), m21(M21), m22(M22), m23(M23),
    m30(M30), m31(M31), m32(M32), m33(M33)
  {}

  Matrix4(tag::NoInit) {}
  Matrix4(tag::Identity)
    :m00(1), m01(0), m02(0), m03(0),
    m10(0), m11(1), m12(0), m13(0),
    m20(0), m21(0), m22(1), m23(0),
    m30(0), m31(0), m32(0), m33(1)
  {}

  Matrix4(tag::Zero)
    :m00(0), m01(0), m02(0), m03(0),
    m10(0), m11(0), m12(0), m13(0),
    m20(0), m21(0), m22(0), m23(0),
    m30(0), m31(0), m32(0), m33(0)
  {}

  Matrix4(const Matrix4 & rhs)
    :m00(rhs.m00), m01(rhs.m01), m02(rhs.m02), m03(rhs.m03),
    m10(rhs.m10), m11(rhs.m11), m12(rhs.m12), m13(rhs.m13),
    m20(rhs.m20), m21(rhs.m21), m22(rhs.m22), m23(rhs.m23),
    m30(rhs.m30), m31(rhs.m31), m32(rhs.m32), m33(rhs.m33)
  {}

  // Assignment operator, does not need to handle self-assignment
  Matrix4& operator= (const Matrix4& rhs);

  Matrix4 operator+(const Matrix4& rhs) const;
  Matrix4 operator-(const Matrix4& rhs) const;
  Matrix4 operator*(const Matrix4& rhs) const;

  Vec4 operator*(const Vec4& rhs) const;

  Matrix4& operator+=(const Matrix4& rhs);
  Matrix4& operator-=(const Matrix4& rhs);
  Matrix4& operator*=(const Matrix4& rhs);


  Matrix4 operator*(const float rhs) const;
  Matrix4 operator/(const float rhs) const;

  Matrix4& operator*=(const float rhs);
  Matrix4& operator/=(const float rhs);

  static Matrix4 BuildXaxisRotation(float theta);
  static Matrix4 BuildYaxisRotation(float theta);
  static Matrix4 BuildZaxisRotation(float theta);
  static Matrix4 BuildTranslationMatrix(Vec3 trans);
  static Matrix4 BuildScaleMatrix(Vec3 Scale);

  static Matrix4 BuildRotationAboutArbitraryAxis(Vec3 axis, float angle);
  static Matrix4 BuildCrossMatrix(Vec4 Vec);

  static Matrix4 TensorProduct(const Vec4& arg1, const Vec4& arg2);

  Matrix4& BuildBasisMatrix(Vec4 B1, Vec4 B2, Vec4 B3, Vec4 B4);
  Matrix4& Transpose(void);
  Matrix4 GetTranspose();

  Matrix4 Inverse(void) const;

  Matrix4& MakePerspFOV(float fov, float AspectRatio, float near, float far);

  static Matrix4 BuildPerspProj(float wFov, float aspectRatio, float near, float far);
};

class TransformRT {
public:
  Quaternion rotation;
  Vec3 translation;

  TransformRT(tag::NoInit);
  TransformRT(tag::Identity);
  TransformRT(const Quaternion & r, Vec3 t);
  TransformRT(const TransformRT & rhs);
  TransformRT & operator=(const TransformRT & rhs);

  Matrix4 CalcMatrix() const;
  Matrix4 CalcInvMatrix() const;
};

class TransformSRT {
public:
  Vec3 scale;
  Quaternion rotation;
  Vec3 translation;

  TransformSRT(tag::NoInit);
  TransformSRT(tag::Identity);
  TransformSRT(Vec3 s, const Quaternion & r, Vec3 t);
  TransformSRT(Vec3 s, const TransformRT & rt);
  TransformSRT(const TransformSRT & rhs);
  TransformSRT & operator=(const TransformSRT & rhs);

  Matrix4 CalcMatrix() const;
  Matrix4 CalcInvMatrix() const;
};


float Dot(const Vec2 & a, const Vec2 & b);
float Dot(const Vec3 & a, const Vec3 & b);
float Dot(const Vec4 & a, const Vec4 & b);
float Dot(const Quaternion& a, const Quaternion & b);

Vec3 Cross(const Vec2 & a, const Vec2 & b);
Vec3 Cross(const Vec3 & a, const Vec3 & b);
Vec4 Cross(const Vec4 & a, const Vec4 & b);

float Length(const Vec2 & a);
float Length(const Vec3 & a);
float Length(const Vec4 & a);
float Length(const Quaternion & a);

float LengthSq(const Vec2 & a);
float LengthSq(const Vec3 & a);
float LengthSq(const Vec4 & a);
float LengthSq(const Quaternion & a);

Vec2 Normalize(const Vec2 & a);
Vec3 Normalize(const Vec3 & a);
Vec4 Normalize(const Vec4 & a);
Quaternion Normalize(const Quaternion & a);

bool IsEqual(const Vec3 & a, const Vec3 & b, float epsilon = kEpsilon);