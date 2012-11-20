#ifndef CS488_MESH_HPP
#define CS488_MESH_HPP

#include <vector>
#include <iosfwd>
#include "primitive.hpp"
#include "algebra.hpp"

struct Plane {
    Vector3D normal;
    Point3D Q;
};

// A polygonal mesh.
class Mesh : public Primitive {
public:
  Mesh(const std::vector<Point3D>& verts,
       const std::vector< std::vector<int> >& faces);

  typedef std::vector<int> Face;
  
  bool intersect(const Point3D& eye, const Vector3D& ray, const double offset, Point3D& poi) const;
  bool intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                 double& minT, Vector3D& normal) const;
private:
  std::vector<Point3D> m_verts;
  std::vector<Face> m_faces;
  std::vector<Plane> m_planes;

  bool checkPointForLine(const Point3D& p, const int p1, const int p2, const Vector3D& normal) const;
  bool checkPoint(const Face& f, const Vector3D& normal, const Point3D& p) const;

  NonhierSphere m_boundingSphere;

  friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};

#endif
