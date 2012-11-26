#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

#include "algebra.hpp"
#include <vector>
#include <iosfwd>

class Primitive {
 public:
  virtual ~Primitive();
  virtual bool intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                         double& minT, Vector3D& normal) const = 0;
  virtual Point2D textureMapCoords(const Point3D& p) const = 0;

 protected:
  bool checkQuadraticRoots(const Point3D& eye, const Vector3D& ray, const double minValue,
                           const double A, const double B, const double C,
                           double& minT) const;

  virtual bool checkPoint(const Point3D& poi) const;

  Point3D m_lastPOI;
};

struct Plane {
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

class Polygon : public Primitive {
 public:
  Polygon(std::vector<Point3D>& pts, const Vector3D& normal, const Vector3D& up = Vector3D(0.0, 1.0, 0.0));
  virtual ~Polygon();

  bool intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                 double& minT, Vector3D& normal) const;
  bool intersect(const Point3D& p) const;
  Point2D textureMapCoords(const Point3D& p) const;

  void set_upVector(const Vector3D& up) { m_plane.set_upVector(up); }

 private:
  bool checkPointForLine(const Point3D& p, const Point3D& p1, const Point3D& p2,
                         const Vector3D& normal) const;

  std::vector<Point3D> m_verts;
  Plane m_plane;
};

class Circle : public Primitive {
 public:
  Circle(const Vector3D& normal, const Point3D& center, double radius);
  Circle(const Vector3D& normal, const Point3D& center, double radius, const Vector3D& up);
  virtual ~Circle();

  bool intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                 double& minT, Vector3D& normal) const;
  Point2D textureMapCoords(const Point3D& p) const;

 private:
  Plane m_plane;
  Point3D m_center;
  double m_radius;
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const Point3D& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  virtual ~NonhierSphere();

  bool intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                 double& minT, Vector3D& normal) const;
  Point2D textureMapCoords(const Point3D& p) const;

private:
  Point3D m_pos;
  double m_radius;
};

// A polygonal mesh.
class Mesh : public Primitive {
public:
  Mesh(const std::vector<Point3D>& verts,
       const std::vector< std::vector<int> >& faces,
       const std::vector<Vector3D>& upVectors = std::vector<Vector3D>() );

  typedef std::vector<int> Face;
  
  bool intersect(const Point3D& eye, const Vector3D& ray, const double offset, Point3D& poi) const;
  bool intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                 double& minT, Vector3D& normal) const;
  Point2D textureMapCoords(const Point3D& p) const;

private:
  const Polygon& determinePolygon(const Point3D& p) const;

  // TODO: Provide option to pass pointers to Polygons to save space.
  std::vector<Polygon> m_polygons;

  NonhierSphere m_boundingSphere;

  friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const Point3D& pos, double size);
  virtual ~NonhierBox();

  bool intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                 double& minT, Vector3D& normal) const;
  Point2D textureMapCoords(const Point3D& p) const;

private:
  Mesh m_box;
};

class Sphere : public Primitive {
public:
  Sphere() :
    m_unitSphere(Point3D(0.0, 0.0, 0.0), 1.0)
  {}
  virtual ~Sphere();

  bool intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                 double& minT, Vector3D& normal) const;
  Point2D textureMapCoords(const Point3D& p) const;

private:
  NonhierSphere m_unitSphere;
};

/*
  Returns unit cube with center at (0.5, 0.5, 0.5)
*/
class Cube : public Primitive {
 public:
  Cube() :
    m_unitCube(Point3D(0.0, 0.0, 0.0), 1.0)
  {}
  virtual ~Cube();

  bool intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                 double& minT, Vector3D& normal) const;
  Point2D textureMapCoords(const Point3D& p) const;

 private:
  NonhierBox m_unitCube;
};

/*
  Returns a unit cone with apex at the origin
  and base at z = 1.
*/
class Cone : public Primitive {
 public:
  Cone();
  virtual ~Cone();

  bool intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                 double& minT, Vector3D& normal) const;
  Point2D textureMapCoords(const Point3D& p) const;

 protected:
  bool checkPoint(const Point3D& poi) const;

 private:
  int determineRegion(const Point3D& p) const;
  Circle m_base;
};

/*
  Returns a Unit Cylinder centered at the origin 
  truncated to 0 <= z <= 1
*/

class Cylinder : public Primitive {
 public:
  Cylinder();
  virtual ~Cylinder();

  bool intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                 double& minT, Vector3D& normal) const;
  Point2D textureMapCoords(const Point3D& p) const;

 protected:
  bool checkPoint(const Point3D& poi) const;

 private:
  int determineRegion(const Point3D& p) const;

  Circle m_top;
  Circle m_bottom;
};
#endif
