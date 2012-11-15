#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

#include "algebra.hpp"

class Primitive {
public:
  virtual ~Primitive();
  virtual bool intersect(const Point3D& eye, const Vector3D& ray, double offset, double& minT, Vector3D& normal) = 0;
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const Point3D& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  virtual ~NonhierSphere();
  bool intersect(const Point3D& eye, const Vector3D& ray, double offset, double& minT, Vector3D& normal);

private:
  Point3D m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const Point3D& pos, double size)
    : m_pos(pos), m_size(size)
  {
  }
  
  virtual ~NonhierBox();

  bool intersect(const Point3D& eye, const Vector3D& ray, double offset, double& minT, Vector3D& normal);
private:
  Point3D m_pos;
  double m_size;

  // Face: 1 - x, 2 - y, 3 - z
  // NormalDir: 1 - Left, 2 - Right
  bool checkPoint(Point3D p, int face, double t, double& minT, Vector3D& normal, int normalDir);
};

class Sphere : public Primitive {
public:
  Sphere() :
    m_unitSphere(Point3D(0.0, 0.0, 0.0), 1.0)
  {}
  virtual ~Sphere();

  bool intersect(const Point3D& eye, const Vector3D& ray, double offset, double& minT, Vector3D& normal);
private:
  NonhierSphere m_unitSphere;
};

class Cube : public Primitive {
 public:
  Cube() :
    m_unitCube(Point3D(0.0, 0.0, 0.0), 1.0)
  {}
  virtual ~Cube();

  bool intersect(const Point3D& eye, const Vector3D& ray, double offset, double& minT, Vector3D& normal);
 private:
  NonhierBox m_unitCube;
};
#endif
