#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

#include "algebra.hpp"
#include <list>
#include <iosfwd>
#include "shapes.hpp"

class Mesh;

class Primitive {
 public:
  virtual ~Primitive();
  virtual bool filteredIntersect(const Point3D& eye, const Vector3D& ray, const double offset,
                                 std::list<IntersectionPoint>& tVals) const;
  virtual bool intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                         std::list<IntersectionPoint>& tVals) const = 0;
  virtual bool containsPoint(const Point3D& p) const = 0;
  virtual Point2D textureMapCoords(const Point3D& p) const = 0;
  virtual Vector3D getNormal(const Point3D& p) const = 0;

  virtual Mesh* getBoundingBox() const = 0;

 protected:
  bool checkQuadraticRoots(const Point3D& eye, const Vector3D& ray, const double minValue,
                           const double A, const double B, const double C,
                           std::list<IntersectionPoint>& tVals) const;

  virtual bool checkPoint(const Point3D& poi) const;
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const Point3D& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  virtual ~NonhierSphere();

  bool intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                 std::list<IntersectionPoint>& tVals) const;
  bool containsPoint(const Point3D& p) const;
  Vector3D getNormal(const Point3D& p) const;
  Point2D textureMapCoords(const Point3D& p) const;

  Mesh* getBoundingBox() const;

private:
  Point3D m_pos;
  double m_radius;
};

// A polygonal mesh.
class Mesh : public Primitive {
public:
  Mesh();
  Mesh(const std::vector<Point3D>& verts,
       const std::vector< std::vector<int> >& faces,
       const std::vector<Vector3D>& upVectors = std::vector<Vector3D>() );

  typedef std::vector<int> Face;
  
  bool intersect(const Point3D& eye, const Vector3D& ray, const double offset, Point3D& poi) const;
  bool intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                 std::list<IntersectionPoint>& tVals) const;
  bool containsPoint(const Point3D& p) const;
  Vector3D getNormal(const Point3D& p) const;
  Point2D textureMapCoords(const Point3D& p) const;

  Mesh* getBoundingBox() const;
  void transform(const Matrix4x4& m);
  void getExtremePoints(double points[6]) const;

private:
  const Polygon& determinePolygon(const Point3D& p) const;

  std::vector<Point3D> m_verts;

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
                 std::list<IntersectionPoint>& tVals) const;
  bool containsPoint(const Point3D& p) const;
  Vector3D getNormal(const Point3D& p) const;
  Point2D textureMapCoords(const Point3D& p) const;

  Mesh* getBoundingBox() const;

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
                 std::list<IntersectionPoint>& tVals) const;
  bool containsPoint(const Point3D& p) const;
  Vector3D getNormal(const Point3D& p) const;
  Point2D textureMapCoords(const Point3D& p) const;

  Mesh* getBoundingBox() const;

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
                 std::list<IntersectionPoint>& tVals) const;
  bool containsPoint(const Point3D& p) const;
  Vector3D getNormal(const Point3D& p) const;
  Point2D textureMapCoords(const Point3D& p) const;

  Mesh* getBoundingBox() const;

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
                 std::list<IntersectionPoint>& tVals) const;
  bool containsPoint(const Point3D& p) const;
  Vector3D getNormal(const Point3D& p) const;
  Point2D textureMapCoords(const Point3D& p) const;

  Mesh* getBoundingBox() const;

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
                 std::list<IntersectionPoint>& tVals) const;
  bool containsPoint(const Point3D& p) const;
  Vector3D getNormal(const Point3D& p) const;
  Point2D textureMapCoords(const Point3D& p) const;

  Mesh* getBoundingBox() const;

 protected:
  bool checkPoint(const Point3D& poi) const;

 private:
  int determineRegion(const Point3D& p) const;

  Circle m_top;
  Circle m_bottom;
};
#endif
