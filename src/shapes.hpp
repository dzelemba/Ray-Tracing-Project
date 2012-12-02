#ifndef SHAPES_HPP
#define SHAPES_HPP

#include "algebra.hpp"
#include <list>
#include <vector>

struct Plane {
  Plane() {}
  Plane(const Vector3D& normal, const Point3D& p, const Vector3D& up = Vector3D(0.0, 1.0, 0.0))
    : m_normal(normal), m_p(p), m_up(up), m_right(m_up.cross(m_normal))
  {
    m_normal.normalize();
    m_up.normalize();
    m_right.normalize(); 
  }

  double intersect(const Point3D& eye, const Vector3D& ray) const {
    return m_normal.dot(m_p - eye) / ray.dot(m_normal);
  }

  void set_upVector(const Vector3D& up) {
    m_up = up;
    m_right = m_up.cross(m_normal);
    m_up.normalize();
    m_right.normalize();
  }

  Vector3D m_normal;
  Point3D m_p;
  Vector3D m_up;
  Vector3D m_right;
};

class Polygon {
 public:
  Polygon() {}
  Polygon(std::vector<Point3D>& pts, const Vector3D& normal, const Vector3D& up = Vector3D(0.0, 1.0, 0.0));
  virtual ~Polygon();

  bool intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                 std::list<IntersectionPoint>& tVals) const;
  bool intersect(const Point3D& p) const;
  bool checkConstraint(const Point3D& p) const;
  Vector3D getNormal(const Point3D& p) const;
  Point2D textureMapCoords(const Point3D& p) const;

  void set_upVector(const Vector3D& up) { m_plane.set_upVector(up); }

  void transform(const Matrix4x4& m);

 private:
  bool checkPointForLine(const Point3D& p, const Point3D& p1, const Point3D& p2,
                         const Vector3D& normal) const;

  std::vector<Point3D> m_verts;
  Plane m_plane;
};

class Circle {
 public:
  Circle(const Vector3D& normal, const Point3D& center, double radius);
  Circle(const Vector3D& normal, const Point3D& center, double radius, const Vector3D& up);
  virtual ~Circle();

  bool intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                 std::list<IntersectionPoint>& tVals) const;
  Vector3D getNormal(const Point3D& p) const;
  Point2D textureMapCoords(const Point3D& p) const;

 private:
  Plane m_plane;
  Point3D m_center;
  double m_radius;
};

#endif
