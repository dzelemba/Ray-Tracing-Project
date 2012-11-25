#include "primitive.hpp"
#include <iostream>
#include <cfloat>

Mesh::Mesh(const std::vector<Point3D>& verts,
           const std::vector< std::vector<int> >& faces)
  : m_polygons(),
    m_boundingSphere(Point3D(0.0, 0.0, 0.0), 0.0)
{
  for (std::vector<Face>::const_iterator it = faces.begin(); it != faces.end(); it++) {
    Vector3D v1 = verts[it->at(0)] - verts[it->at(1)];
    Vector3D v2 = verts[it->at(2)] - verts[it->at(1)];
    Vector3D normal = v2.cross(v1); 
    
    std::vector<Point3D> vertices;
    for (Face::const_iterator it2 = it->begin(); it2 != it->end(); it2++) {
      vertices.push_back(verts[*it2]);
    }

    m_polygons.push_back(Polygon(vertices, normal));
  }

  // Now create our bounding sphere
  // Idea is to find max distance between two points.
  double maxDist = 0.0;
  Point3D point1, point2;
  for (std::vector<Point3D>::const_iterator p1 = verts.begin(); p1 != verts.end(); p1++) {
    for (std::vector<Point3D>::const_iterator p2 = p1 + 1; p2 != verts.end(); p2++) {
      double dist = (*p1 - *p2).length();
      if (dist > maxDist) {
        maxDist = dist;
        point1 = *p1;
        point2 = *p2;
      }
    }
  }

  double radius = maxDist / 2.0;
  Point3D center = (1.0/2.0) * (point1 + point2);
  m_boundingSphere = NonhierSphere(center, radius);
}

bool Mesh::intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                     double& minT, Vector3D& normal) const
{
  // Draw bouding sphere instead?
  // return m_boundingSphere.intersect(eye, ray, offset, minT, normal);

  // First intersect with our bounding sphere.
  Vector3D dummyNorm;
  double dummyMinT = minT;
  if (!m_boundingSphere.intersect(eye, ray, offset, dummyMinT, dummyNorm)) {
    return false;
  }

  bool found = false;
  for (std::vector<Polygon>::const_iterator it = m_polygons.begin(); it != m_polygons.end(); it++) {
    if (it->intersect(eye, ray, offset, minT, normal)) {
      found = true;
    }
  }

  return found;
}

bool Mesh::intersect(const Point3D& eye, const Vector3D& ray, const double offset, Point3D& poi) const
{
  double t = DBL_MAX; 
  Vector3D normal;

  if (intersect(eye, ray, offset, t, normal)) {
    poi = eye + t * ray;
  
    return true;
  }
  return false; 
}


std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  (void)mesh;

  std::cerr << "mesh({";

  std::cerr << "});" << std::endl;
  return out;
}
