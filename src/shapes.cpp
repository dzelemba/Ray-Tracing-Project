#include "shapes.hpp"
#include <cfloat>

/* 
  ********** Polygon **********
*/

Polygon::Polygon(std::vector<Point3D>& pts, const Vector3D& normal, const Vector3D& up) 
  :  m_verts(pts), m_plane(normal, m_verts.front(), up)
{
}

Polygon::~Polygon()
{
}

bool Polygon::checkPointForLine(const Point3D& p, const Point3D& p1, const Point3D& p2,
                                const Vector3D& normal) const
{
  Vector3D v = p1 - p2;
  Vector3D planeNormal = v.cross(normal);

  return planeNormal.dot(p - p1) < tightEpsilon;
}

bool Polygon::intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                        std::list<IntersectionPoint>& tVals) const
{
  double t = m_plane.intersect(eye, ray); 

  Point3D poi = eye + t * ray;

  if (!intersect(poi)) {
    return false;
  }
  tVals.push_back(IntersectionPoint(t, getNormal(poi)));

  return true;
}

Vector3D Polygon::getNormal(const Point3D& p) const
{
  (void)p;

  return m_plane.m_normal;
}

bool Polygon::checkConstraint(const Point3D& p) const
{
  return m_plane.m_normal.dot(p - m_plane.m_p) < tightEpsilon;
}

bool Polygon::intersect(const Point3D& p) const
{
  double dot = m_plane.m_normal.dot(p - m_plane.m_p);
  if (dot < -epsilon || dot > epsilon) {
    return false;
  }

  std::vector<Point3D>::const_iterator prev = m_verts.begin();
  for (std::vector<Point3D>::const_iterator it = m_verts.begin() + 1; it != m_verts.end(); it++, prev++) {
    if (!checkPointForLine(p, *it, *prev, m_plane.m_normal)) {
      return false;
    }
  }

  // Also check line formed by first and last.
  if (!checkPointForLine(p, m_verts.front(), m_verts.back(), m_plane.m_normal)) {
    return false;
  }

  return true;
}

Point2D Polygon::textureMapCoords(const Point3D& p) const
{
  // Pick any point to be our "center"
  Point3D center = m_verts.front();  

  // Find max x and y distance based on a coordinate system defined by center
  // and m_up and m_right of the plane.
  Point2D coords;
  double minX = DBL_MAX, maxX = -DBL_MAX;
  double minY = DBL_MAX, maxY = -DBL_MAX;
  for (std::vector<Point3D>::const_iterator it = m_verts.begin(); it != m_verts.end(); it++) {
    Vector3D v = *it - center;
    if (solve3x2System(m_plane.m_up, m_plane.m_right, v, coords)) {
      if (coords[0] < minX) minX = coords[0];
      if (coords[0] > maxX) maxX = coords[0];
      if (coords[1] < minY) minY = coords[1];
      if (coords[1] > maxY) maxY = coords[1];
    } else {
      std::cerr << "Failed to solve system in polygon: " << m_plane.m_up << " "
                << m_plane.m_right << " " << v << std::endl;
    }
  }
  
  // Now get coordinates for our point.
  if (solve3x2System(m_plane.m_up, m_plane.m_right, p - center, coords)) {
    double x = (coords[0] - minX) / (maxX - minX);
    double y = (coords[1] - minY) / (maxY - minY);

    return Point2D(x, y);
  } else {
    std::cerr << "Failed to solve system in polygon: " << m_plane.m_up << " "
              << m_plane.m_right << " " << p - center << std::endl;
  }
  return Point2D(-1, -1);
}
/* 
  ********** Circle **********
*/

Circle::Circle(const Vector3D& normal, const Point3D& center, double radius)
  : m_plane(normal, center), m_center(center), m_radius(radius)
{
}

Circle::Circle(const Vector3D& normal, const Point3D& center, double radius, const Vector3D& up)
  : m_plane(normal, center, up), m_center(center), m_radius(radius)
{
}

Circle::~Circle()
{
}

bool Circle::intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                       std::list<IntersectionPoint>& tVals) const
{
  double t = m_plane.intersect(eye, ray); 

  Point3D poi = eye + t * ray;

  // Check if point is within circle
  if ((poi - m_center).length() <= m_radius) {
    tVals.push_back(IntersectionPoint(t, getNormal(poi)));

    return true;
  }

  return false;
}

Vector3D Circle::getNormal(const Point3D& p) const
{
  (void)p;

  return m_plane.m_normal;
}

Point2D Circle::textureMapCoords(const Point3D& p) const
{
  Vector3D v = p - m_center;
  Point2D coords;
  
  if (solve3x2System(m_plane.m_up, m_plane.m_right, v, coords)) {
    return Point2D((coords[0] + 1.0) / 2.0, (coords[1] + 1.0) / 2.0);
  }

  std::cerr << "Circle Texture Map failed to find solution: " << m_plane.m_up << " "
            << m_plane.m_right << " " << v << std::endl;
  return Point2D(-1.0, 1.0);
}
