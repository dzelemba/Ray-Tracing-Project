#include "image_primitive.hpp"
#include <vector>

ImagePrimitive::ImagePrimitive()
{
  std::vector<Point3D> points;
  points.push_back(Point3D(0.0, 0.0, 0.0));
  points.push_back(Point3D(0.0, 1.0, 0.0));
  points.push_back(Point3D(1.0, 1.0, 0.0));
  points.push_back(Point3D(1.0, 0.0, 0.0));

  m_face = Polygon(points, Vector3D(0.0, 0.0, -1.0));
}

bool ImagePrimitive::filteredIntersect(const Point3D& eye, const Vector3D& ray, const double offset,
                                 std::list<IntersectionPoint>& tVals) const
{
  // No need to filter here
  return intersect(eye, ray, offset, tVals);
}

bool ImagePrimitive::intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                         std::list<IntersectionPoint>& tVals) const
{
  if (m_face.intersect(eye, ray, offset, tVals)) {
    // We want the normal to always be pointing towards the eye
    if (m_face.checkConstraint(eye)) {
      IntersectionPoint& p = tVals.front(); // There should be only one.
      p.m_normal = -1 * p.m_normal;
    }

    // Duplicate this point so we get a segment.
    tVals.push_back(tVals.front());

    return true;
  }

  return false;
}

bool ImagePrimitive::containsPoint(const Point3D& p) const
{
  return m_face.intersect(p);
}

Point2D ImagePrimitive::textureMapCoords(const Point3D& p) const
{
  return m_face.textureMapCoords(p);
}

Vector3D ImagePrimitive::getNormal(const Point3D& p) const
{
  return m_face.getNormal(p);
}
