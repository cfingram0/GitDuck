#include "dmath.h"
#include <cmath>
#include <cassert>
#include <inttypes.h>


Vec2 Vec2::operator+ (const Vec2 & rhs) const {
  return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 & Vec2::operator+= (const Vec2 & rhs) {
  x += rhs.x;
  y += rhs.y;

  return *this;
}

Vec2 Vec2::operator- (const Vec2 & rhs) const {
  return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 & Vec2::operator-= (const Vec2 & rhs) {
  x -= rhs.x;
  y -= rhs.y;
  return *this;
}

Vec2 Vec2::operator- () const {
  return Vec2(-x, -y);
}

Vec2 Vec2::operator* (float rhs) const {
  return Vec2(x * rhs, y * rhs);
}

Vec2 & Vec2::operator*= (float rhs) {
  x *= rhs;
  y *= rhs;
  return *this;
}

Vec2 Vec2::operator/ (float rhs) const {
  return Vec2(x / rhs, y / rhs);
}

Vec2 & Vec2::operator/= (float rhs) {
  x /= rhs;
  y /= rhs;
  return *this;
}


Vec3 Vec3::operator+ (const Vec3 & rhs) const {
  return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
}

Vec3 & Vec3::operator+= (const Vec3 & rhs) {
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  return *this;
}

Vec3 Vec3::operator- (const Vec3 & rhs) const {
  return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
}

Vec3 & Vec3::operator-= (const Vec3 & rhs) {
  x -= rhs.x;
  y -= rhs.y;
  z -= rhs.x;
  return *this;
}

Vec3 Vec3::operator- () const {
  return Vec3(-x, -y, -z);
}

Vec3 Vec3::operator* (float rhs) const {
  return Vec3(x * rhs, y * rhs, z * rhs);
}

Vec3 & Vec3::operator*= (float rhs) {
  x *= rhs;
  y *= rhs;
  z *= rhs;
  return *this;
}

Vec3 Vec3::operator/ (float rhs) const {
  return Vec3(x / rhs, y / rhs, z / rhs);
}

Vec3 & Vec3::operator/= (float rhs) {
  x /= rhs;
  y /= rhs;
  z /= rhs;
  return *this;
}

Vec4 Vec4::operator+ (const Vec4 & rhs) const {
  return Vec4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
}

Vec4 & Vec4::operator+= (const Vec4 & rhs) {
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  w += rhs.w;
  return *this;
}

Vec4 Vec4::operator- (const Vec4 & rhs) const {
  return Vec4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
}

Vec4 & Vec4::operator-= (const Vec4 & rhs) {
  x -= rhs.x;
  y -= rhs.y;
  z -= rhs.z;
  w -= rhs.w;
  return *this;
}

Vec4 Vec4::operator- () const {
  return Vec4(-x, -y, -z, -w);
}

Vec4 Vec4::operator* (float rhs) const {
  return Vec4(x * rhs, y * rhs, z * rhs, w * rhs);
}

Vec4 & Vec4::operator*= (float rhs) {
  x *= rhs;
  y *= rhs;
  z *= rhs;
  w *= rhs;
  return *this;
}

Vec4 Vec4::operator/ (float rhs) const {
  return Vec4(x / rhs, y / rhs, z / rhs, w / rhs);
}

Vec4 & Vec4::operator/= (float rhs) {
  x /= rhs;
  y /= rhs;
  z /= rhs;
  w /= rhs;
  return *this;
}


Quaternion::Quaternion(float RadAngle, const Vec4& Axis) {
  s = std::cos(RadAngle / 2.0f);
  Vec4 axis = Normalize(Axis);

  axis *= std::sin(RadAngle / 2.0f);

  x = axis.x;
  y = axis.y;
  z = axis.z;
}
Quaternion::Quaternion(float S, float X, float Y, float Z)
  :s(S), x(X), y(Y), z(Z)
{}


Quaternion::Quaternion(const Matrix4& RotMatrix) {
  s = 0.5f * std::sqrt(RotMatrix.m00 + RotMatrix.m11 + RotMatrix.m22 + 1);
  x = (RotMatrix.m21 - RotMatrix.m12) / (4 * s);
  y = (RotMatrix.m02 - RotMatrix.m20) / (4 * s);
  z = (RotMatrix.m10 - RotMatrix.m01) / (4 * s);
}

Quaternion::Quaternion(float eulerX, float eulerY, float eulerZ) {
  *this = Quaternion(eulerZ, Vec4(0, 0, 1, 0)) 
          * Quaternion(eulerY, Vec4(0, 1, 0, 0)) 
          * Quaternion(eulerX, Vec4(1, 0, 0, 0));
}

Quaternion Quaternion::operator+ (const Quaternion& rhs) const {
  return Quaternion(s + rhs.s, x + rhs.x, y + rhs.y, z + rhs.z);
}

Quaternion Quaternion::operator- (const Quaternion& rhs) const {
  return Quaternion(s - rhs.s, x - rhs.x, y - rhs.y, z - rhs.z);
}

Quaternion Quaternion::operator* (const Quaternion& rhs) const {
  Vec4 V_1 = Vec4(x, y, z, 0);
  Vec4 V_2 = Vec4(rhs.x, rhs.y, rhs.z, 0);
  Vec4 V_3 = V_1 *rhs.s + V_2 * s + Cross(V_1, V_2);
  return Quaternion(s * rhs.s - Dot(V_1, V_2), V_3.x, V_3.y, V_3.z);
}

Quaternion Quaternion::operator/ (const Quaternion& rhs) const{
  return Quaternion(*this) * rhs.Inverse();
}

Quaternion Quaternion::operator* (float rhs) const {
  return Quaternion(rhs * s, rhs * x, rhs * y, rhs * z);
}

Quaternion Quaternion::operator/ (float rhs) const {
  return Quaternion(s / rhs, x / rhs, y / rhs, z / rhs);
}

Quaternion& Quaternion::operator+=(const Quaternion& rhs) {
  s += rhs.s;
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  return *this;
}

Quaternion& Quaternion::operator*=(const Quaternion& rhs) {
  Vec4 V_1 = Vec4(x, y, z, 0);
  Vec4 V_2 = Vec4(rhs.x, rhs.y, rhs.z, 0);
  Vec4 V_3 = V_2 * s + V_1 * rhs.s + Cross(V_1, V_2);
  s = s * rhs.s - Dot(V_1, V_2);
  x = V_3.x;
  y = V_3.y;
  z = V_3.z;
  return *this;
}

Quaternion& Quaternion::operator/= (const Quaternion& rhs) {
  Quaternion other = rhs.Inverse();
  *this *= other;
  return *this;
}

Quaternion& Quaternion::operator*= (float rhs) {
  s *= rhs;
  x *= rhs;
  y *= rhs;
  z *= rhs;
  return *this;
}

Quaternion& Quaternion::operator/= (float rhs) {
  s /= rhs;
  x /= rhs;
  y /= rhs;
  z /= rhs;
  return *this;
}

Quaternion Quaternion::Inverse(void) const {
  Quaternion Conj = GetConjugate();
  return Conj / LengthSq(Conj);
}

Quaternion Quaternion::GetConjugate(void) const {
  return Quaternion(s, -x, -y, -z);
}

Vec4 Quaternion::RotateVec(const Vec4& rhs) const {
  Quaternion pure = Quaternion(0, rhs.x, rhs.y, rhs.z);

  Quaternion Res = (*this * pure) * Inverse();

  return Vec4(Res.x, Res.y, Res.z, 0);
}

Matrix4 Quaternion::ToMatrix(void)
{
  return Matrix4(1 - 2 * (y * y + z * z),     2 * (x * y - s * z),     2 * (x * z + s * y), 0,
                     2 * (x * y + s * z), 1 - 2 * (x * x + z * z),     2 * (y * z - s * x), 0,
                     2 * (x * z - s * y),     2 * (y * z + s * x), 1 - 2 * (x * x + y * y), 0,
                                       0,                       0,                       0, 1);
}

Quaternion Quaternion::Lerp(const Quaternion& Source, const Quaternion& Dest, float t)
{
  //clamp t to 0 and 1
  t = t > 1.0f ? 1.0f : t;
  t = t < 0 ? 0 : t;

  return Dest * t + Source * (1 - t);

}

Quaternion Quaternion::SLerp(const Quaternion& Source, const Quaternion& Dest, float t)
{
  //clamp t to 0 and 1
  t = t > 1.0f ? 1.0f : t;
  t = t < 0 ? 0 : t;


  float dot = Dot(Source, Dest);
  dot = dot > 1.0f ? 1.0f : dot;
  dot = dot < -1 ? -1 : dot;


  float alpha = std::acos(dot);

  //no need to interp, close enough to same orientation
  if (alpha < 0.001)
    return Source;

  float Left = std::sin(alpha - t * alpha) / sin(alpha);
  float Right = std::sin(alpha * t) / std::sin(alpha);
  return (Source.GetNormalized() * Left + Dest.GetNormalized() * Right).GetNormalized();

}


float Dot(const Vec2 & a, const Vec2 & b) {
  return a.x * b.x + a.y * b.y;
}

float Dot(const Vec3 & a, const Vec3 & b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

float Dot(const Vec4 & a, const Vec4 & b) {
  return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

float Dot(const Quaternion& a, const Quaternion & b) {
  return a.s * b.s + a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 Cross(const Vec2 & a, const Vec2 & b) {
  return Vec3(0, 0, a.x * b.y - a.y * b.x);
}

Vec3 Cross(const Vec3 & a, const Vec3 & b) {
  float Xvalue = a.y * b.z - a.z * b.y;
  float Yvalue = a.z * b.x - a.x * b.z;
  float Zvalue = a.x * b.y - a.y * b.x;

  return Vec3(Xvalue, Yvalue, Zvalue);
}

Vec4 Cross(const Vec4 & a, const Vec4 & b) {
  // cross only makes sense for vectors (w == 0)
  assert(a.w < kEpsilon && -a.w > -kEpsilon);
  assert(b.w < kEpsilon && -b.w > -kEpsilon);
  float Xvalue = a.y * b.z - a.z * b.y;
  float Yvalue = a.z * b.x - a.x * b.z;
  float Zvalue = a.x * b.y - a.y * b.x;

  return Vec4(Xvalue, Yvalue, Zvalue, 0);
}

float Length(const Vec2 & a) {
  return std::sqrtf(a.x * a.x + a.y * a.y);
}

float Length(const Vec3 & a) {
  return std::sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}

float Length(const Vec4 & a) {
  return std::sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}

float Length(const Quaternion & a) {
  return std::sqrtf(a.s * a.s + a.x * a.x + a.y * a.y + a.z * a.z);
}

float LengthSq(const Vec2 & a) {
  return a.x * a.x + a.y * a.y;
}
float LengthSq(const Vec3 & a) {
  return a.x * a.x + a.y * a.y + a.z * a.z;
}
float LengthSq(const Vec4 & a) {
  return a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w;
}

float LengthSq(const Quaternion & a) {
  return a.s * a.s + a.x * a.x + a.y * a.y + a.z * a.z;
}

Vec2 Normalize(const Vec2 & a) {
  return a / Length(a);
}

Vec3 Normalize(const Vec3 & a) {
  return a / Length(a);
}

Vec4 Normalize(const Vec4 & a) {
  return a / Length(a);
}

Quaternion Normalize(const Quaternion & a) {
  return a / Length(a);
}


Matrix4& Matrix4::operator= (const Matrix4& rhs) {
  for (int32_t i = 0; i < 16; ++i) {
    v[i] = rhs.v[i];
  }
  
  return *this;
}

Matrix4 Matrix4::operator+(const Matrix4& rhs) const
{
  Matrix4 toReturn(tag::NoInit{});
  for (int32_t i = 0; i < 16; ++i) {
    toReturn.v[i] = v[i] + rhs.v[i];
  }

  return toReturn;
}
Matrix4 Matrix4::operator-(const Matrix4& rhs) const
{
  Matrix4 toReturn(tag::NoInit{});
  for (int32_t i = 0; i < 16; ++i) {
    toReturn.v[i] = v[i] - rhs.v[i];
  }

  return toReturn;
}

Matrix4 Matrix4::operator*(const Matrix4& rhs) const
{
  Matrix4 lhs(tag::NoInit{});
  for (int i = 0; i < 4; ++i)
  {
    for (int j = 0; j < 4; ++j)
    {
      lhs.m[i][j] = m[i][0] * rhs.m[0][j]
                  + m[i][1] * rhs.m[1][j]
                  + m[i][2] * rhs.m[2][j]
                  + m[i][3] * rhs.m[3][j];
    }
  }

  return lhs;

}

// Similar to the three above except they modify
// the original 
Matrix4& Matrix4::operator+=(const Matrix4& rhs)
{
  for (int i = 0; i < 16; ++i)
    v[i] += rhs.v[i];

  return *this;
}
Matrix4& Matrix4::operator-=(const Matrix4& rhs)
{
  for (int i = 0; i < 16; ++i)
    v[i] -= rhs.v[i];

  return *this;
}
Matrix4& Matrix4::operator*=(const Matrix4& rhs)
{
  //multiply with self, then assign.
  return *this = *this * rhs;
}

// Scale/Divide the entire matrix by a float
Matrix4 Matrix4::operator*(const float rhs) const
{
  return Matrix4(*this) *= rhs;
}
Matrix4 Matrix4::operator/(const float rhs) const
{
  return Matrix4(*this) /= rhs;
}

// Same as previous 
Matrix4& Matrix4::operator*=(const float rhs)
{
  for (int i = 0; i < 16; ++i)
    v[i] *= rhs;

  return *this;
}
Matrix4& Matrix4::operator/=(const float rhs)
{
  for (int i = 0; i < 16; ++i)
    v[i] /= rhs;

  return *this;
}


Matrix4 Matrix4::BuildXaxisRotation(float alpha)
{
  Matrix4 toReturn(tag::Identity{});
  toReturn.m11 = std::cos(alpha);
  toReturn.m12 = -std::sin(alpha);
  toReturn.m21 = std::sin(alpha);
  toReturn.m22 = std::cos(alpha);
  return toReturn;
}

Matrix4 Matrix4::BuildYaxisRotation(float alpha)
{
  Matrix4 toReturn(tag::Identity{});
  toReturn.m00 = std::cos(alpha);
  toReturn.m02 = std::sin(alpha);
  toReturn.m20 = -std::sin(alpha);
  toReturn.m22 = std::cos(alpha);
  return toReturn;
}

Matrix4 Matrix4::BuildZaxisRotation(float alpha)
{
  Matrix4 toReturn(tag::Identity{});
  toReturn.m00 = std::cos(alpha);
  toReturn.m01 = -std::sin(alpha);
  toReturn.m10 = std::sin(alpha);
  toReturn.m11 = std::cos(alpha);
  return toReturn;
}

Matrix4 Matrix4::BuildTranslationMatrix(Vec4 trans)
{
  Matrix4 toReturn(tag::Identity{});
  toReturn.m03 = trans.x;
  toReturn.m13 = trans.y;
  toReturn.m23 = trans.z;

  return toReturn;
}

Matrix4 Matrix4::BuildScaleMatrix(Vec4 Scale)
{
  Matrix4 toReturn(tag::Zero{});
  toReturn.m00 = Scale.x;
  toReturn.m11 = Scale.y;
  toReturn.m22 = Scale.z;
  toReturn.m33 = 1;
  return toReturn;
}

Matrix4 Matrix4::BuildRotationAboutArbitraryAxis(Vec4 axis, float angle)
{
  Matrix4 toReturn(tag::Identity{});

  Vec4 theaxis = axis;
  if (LengthSq(theaxis) == 0)
  {
    return toReturn;
  }
  theaxis = Normalize(theaxis);

  //tensor product
  Matrix4 Tensor = Matrix4::TensorProduct(theaxis, theaxis);

  //cross matrix
  Matrix4 CrossMatrix = BuildCrossMatrix(theaxis);

  Matrix4 Step1 = Matrix4(tag::Identity{}) * cos(angle);
  Matrix4 Step2 = CrossMatrix * sin(angle);
  Matrix4 Step3 = Tensor * (1 - cos(angle));

  //actual equation
  toReturn = Step1 + Step2 + Step3;
  toReturn.m33 = 1;
  return toReturn;
}

Matrix4 Matrix4::BuildCrossMatrix(Vec4 Vec)
{
  Matrix4 toReturn(tag::NoInit{});

  toReturn.m00 = 0;
  toReturn.m01 = -Vec.z;
  toReturn.m02 = Vec.y;
  toReturn.m03 = 0;

  toReturn.m10 = Vec.z;
  toReturn.m11 = 0;
  toReturn.m12 = -Vec.x;
  toReturn.m13 = 0;

  toReturn.m20 = -Vec.y;
  toReturn.m21 = Vec.x;
  toReturn.m22 = 0;
  toReturn.m23 = 0;

  toReturn.m30 = 0;
  toReturn.m31 = 0;
  toReturn.m32 = 0;
  toReturn.m33 = 1;
  return toReturn;
}

Matrix4 Matrix4::TensorProduct(const Vec4& arg1, const Vec4& arg2)
{
  Matrix4 toReturn(tag::NoInit{});

  toReturn.m00 = arg1.x * arg2.x;
  toReturn.m01 = arg1.x * arg2.y;
  toReturn.m02 = arg1.x * arg2.z;
  toReturn.m03 = 0;

  toReturn.m10 = arg1.y * arg2.x;
  toReturn.m11 = arg1.y * arg2.y;
  toReturn.m12 = arg1.y * arg2.z;
  toReturn.m13 = 0;

  toReturn.m20 = arg1.z * arg2.x;
  toReturn.m21 = arg1.z * arg2.y;
  toReturn.m22 = arg1.z * arg2.z;
  toReturn.m23 = 0;

  toReturn.m30 = 0;
  toReturn.m31 = 0;
  toReturn.m32 = 0;
  toReturn.m33 = 1;
  return toReturn;
}

Matrix4& Matrix4::BuildBasisMatrix(Vec4 B1, Vec4 B2, Vec4 B3, Vec4 B4)
{
  m00 = B1.x;
  m10 = B1.y;
  m20 = B1.z;
  m30 = B1.w;

  m01 = B2.x;
  m11 = B2.y;
  m21 = B2.z;
  m31 = B2.w;

  m02 = B3.x;
  m12 = B3.y;
  m22 = B3.z;
  m32 = B3.w;

  m03 = B4.x;
  m13 = B4.y;
  m23 = B4.z;
  m33 = B4.w;

  return *this;
}

Matrix4& Matrix4::Transpose(void)
{
  for (int i = 0; i < 4; ++i)
  {
    for (int j = i + 1; j < 4; ++j)
    {
      float temp = m[i][j];
      m[i][j] = m[j][i];
      m[j][i] = temp;
    }
  }


  return *this;
}

Matrix4 Matrix4::GetTranspose()
{
  Matrix4 newMat(*this);
  newMat.Transpose();
  return newMat;
}

Matrix4 Matrix4::Inverse(void)
{
  Matrix4 Inverse(tag::Identity{});

  float Determinant = m00 * m11 * m22
    + m10 * m21 * m02
    + m20 * m01 * m12
    - m00 * m21 * m12
    - m20 * m11 * m02
    - m10 * m01 * m22;
  if (Determinant < 0.0001 &&
    Determinant > -0.0001)
    return Inverse;

  Inverse.m00 = (m11 * m22 - m12 * m21) / Determinant;
  Inverse.m01 = (m02 * m20 - m01 * m10) / Determinant;
  Inverse.m02 = (m01 * m12 - m02 * m11) / Determinant;

  Inverse.m10 = (m12 * m20 - m01 * m22) / Determinant;
  Inverse.m11 = (m00 * m22 - m02 * m20) / Determinant;
  Inverse.m12 = (m02 * m10 - m00 * m12) / Determinant;

  Inverse.m20 = (m10 * m21 - m11 * m20) / Determinant;
  Inverse.m21 = (m01 * m20 - m00 * m21) / Determinant;
  Inverse.m22 = (m00 * m11 - m01 * m10) / Determinant;

  return Inverse;
}


Matrix4& Matrix4::MakePerspFOV(float fov, float AspectRatio, float nearz, float farz)
{
  *this = Matrix4(tag::Zero{});

  float tangent = std::tan(fov / 2);
  float cot = 1 / tangent;
  //rowxcolumn

  m00 = cot / AspectRatio;
  m11 = cot;
  m22 = (farz + nearz) / (nearz - farz);
  m32 = -1;
  m23 = (2 * nearz * farz) / (nearz - farz);

  return *this;

}