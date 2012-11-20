#include "mesh.hpp"
#include <iostream>
#include <cfloat>

Mesh::Mesh(const std::vector<Point3D>& verts,
           const std::vector< std::vector<int> >& faces)
  : m_verts(verts),
    m_faces(faces),
    m_boundingSphere(Point3D(0.0, 0.0, 0.0), 0.0)
{
  // Precompute the plane for each face.
  Plane p;
  for (std::vector<Face>::iterator it = m_faces.begin(); it != m_faces.end(); it++) {
    Vector3D v1 = m_verts[it->at(0)] - m_verts[it->at(1)];
    Vector3D v2 = m_verts[it->at(2)] - m_verts[it->at(1)];
    p.normal = v2.cross(v1); 

    p.Q = m_verts[it->at(0)];
    m_planes.push_back(p);
  }

  // Now create our bounding sphere
  // Idea is to find max difference in each axis
  double maxX = -DBL_MAX, maxY = -DBL_MAX, maxZ = -DBL_MAX;
  double minX = DBL_MAX, minY = DBL_MAX, minZ = DBL_MAX;
  for (std::vector<Point3D>::iterator it = m_verts.begin(); it != m_verts.end(); it++) {
    maxX = std::max(maxX, (*it)[0]);
    maxY = std::max(maxY, (*it)[1]);
    maxZ = std::max(maxZ, (*it)[2]);

    minX = std::min(minX, (*it)[0]);
    minY = std::min(minY, (*it)[1]);
    minZ = std::min(minZ, (*it)[2]);
  }

  double radius = std::max(maxX - minX, std::max(maxY - minY, maxZ - minZ)) / 2;
  Point3D center = Point3D( (maxX + minX) / 2, (maxY + minY) / 2, (maxZ + minZ) / 2);
  m_boundingSphere = NonhierSphere(center, radius);
}

bool Mesh::checkPointForLine(const Point3D& p, const int p1, const int p2, const Vector3D& normal) const
{
  Vector3D v = m_verts[p1] - m_verts[p2];
  Vector3D planeNormal = v.cross(normal);

  return planeNormal.dot(p - m_verts[p1]) < 0;
}

bool Mesh::checkPoint(const Face& f, const Vector3D& normal, const Point3D& p) const
{
  Face::const_iterator prev = f.begin();
  for (Face::const_iterator it = f.begin() + 1; it != f.end(); it++, prev++) {
    if (!checkPointForLine(p, *it, *prev, normal)) {
      return false;
    }
  }

  // Also check line formed by first and last.
  if (!checkPointForLine(p, f.front(), f.back(), normal)) {
    return false;
  }

  return true;
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

  std::vector<Face>::const_iterator fIt = m_faces.begin();
  std::vector<Plane>::const_iterator pIt = m_planes.begin();
  for ( ; fIt != m_faces.end() && pIt != m_planes.end(); fIt++, pIt++) {
    double t = pIt->normal.dot(pIt->Q - eye) / ray.dot(pIt->normal);
    
    if (t > offset && t < minT && checkPoint(*fIt, pIt->normal, eye + t * ray)) {
      found = true;
      minT = t;
      normal = pIt->normal;
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
  std::cerr << "mesh({";
  for (std::vector<Point3D>::const_iterator I = mesh.m_verts.begin(); I != mesh.m_verts.end(); ++I) {
    if (I != mesh.m_verts.begin()) std::cerr << ",\n      ";
    std::cerr << *I;
  }
  std::cerr << "},\n\n     {";
  
  for (std::vector<Mesh::Face>::const_iterator I = mesh.m_faces.begin(); I != mesh.m_faces.end(); ++I) {
    if (I != mesh.m_faces.begin()) std::cerr << ",\n      ";
    std::cerr << "[";
    for (Mesh::Face::const_iterator J = I->begin(); J != I->end(); ++J) {
      if (J != I->begin()) std::cerr << ", ";
      std::cerr << *J;
    }
    std::cerr << "]";
  }
  std::cerr << "});" << std::endl;
  return out;
}
