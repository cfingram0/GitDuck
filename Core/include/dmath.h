#pragma once

namespace tag {
  struct NoInit {};
  struct Identity {};
  struct Zero {};
}

class Vec3 {
public:
  float x;
  float y;
  float z;

  Vec3(tag::NoInit) {}
  Vec3(tag::Zero) : x(0), y(0), z(0) {}
  Vec3(const Vec3 & rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {}
  Vec3(float xArg, float yArg, float zArg) : x(xArg), y(yArg), z(zArg) {}
  Vec3 & operator=(const Vec3 & rhs) {
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
  }
};


