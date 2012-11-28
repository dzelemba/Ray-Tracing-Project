#include "primitive.hpp"
#include <iostream>
#include <cfloat>

Mesh::Mesh(const std::vector<Point3D>& verts,
           const std::vector< std::vector<int> >& faces,
           const std::vector<Vector3D>& upVectors)
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

    // Just put something on the plane for the up vector.
    m_polygons.push_back(Polygon(vertices, normal, v1));
  }

  // If we're given up vectors add them to the polygons.
  if (!upVectors.empty()) {
    std::vector<Polygon>::iterator p = m_polygons.begin();
    std::vector<Vector3D>::const_iterator it = upVectors.begin();
    for ( ; p != m_polygons.end() && it != upVectors.end(); it++, p++) {
      p->set_upVector(*it);
    }
  }

  // Now create our bounding sphere
  // Idea is to find max distance between two points.
  // TODO: I don't think this is right....
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

bool Mesh::intersect(const Point3D& eye, const Vector3D& ray, const double offset, std::list<IntersectionPoint>& tVals) const
{
  // Draw bouding sphere instead?
  // return m_boundingSphere.intersect(eye, ray, offset, minT, normal);

  // First intersect with our bounding sphere.
  std::list<IntersectionPoint> dummyTVals;
  if (!m_boundingSphere.intersect(eye, ray, offset, dummyTVals)) {
    return false;
  }

  for (std::vector<Polygon>::const_iterator it = m_polygons.begin(); it != m_polygons.end(); it++) {
    it->intersect(eye, ray, offset, tVals);
  }

  return tVals.size() != 0;
}

bool Mesh::intersect(const Point3D& eye, const Vector3D& ray, const double offset, Point3D& poi) const
{
  std::list<IntersectionPoint> tVals;
  if (intersect(eye, ray, offset, tVals)) {
    double minT = DBL_MAX;
    for (std::list<IntersectionPoint>::iterator it = tVals.begin(); it != tVals.end(); it++) {
      if (it->m_t < minT) {
        minT = it->m_t;
      }
    }
    poi = eye + minT * ray;
  
    return true;
  }
  return false; 
}

bool Mesh::containsPoint(const Point3D& p) const
{
  // Each polygon normal and point define a constraint of the polyhedra
  for (std::vector<Polygon>::const_iterator it = m_polygons.begin(); it != m_polygons.end(); it++) {
    if (!it->checkConstraint(p)) {
      return false;
    }
  }

  return true;
}

Vector3D Mesh::getNormal(const Point3D& p) const
{
  return determinePolygon(p).getNormal(p);
}

Point2D Mesh::textureMapCoords(const Point3D& p) const
{
  return determinePolygon(p).textureMapCoords(p);
}

const Polygon& Mesh::determinePolygon(const Point3D& p) const
{
  for (std::vector<Polygon>::const_iterator it = m_polygons.begin(); it != m_polygons.end(); it++) {
    if (it->intersect(p)) {
      return *it;
    }
  }

  std::cerr << "No Polygon found for point: " << p << std::endl;
  return m_polygons.front();
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  (void)mesh;

  std::cerr << "mesh({";

  std::cerr << "});" << std::endl;
  return out;
}
