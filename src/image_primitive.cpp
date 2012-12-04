#include "image_primitive.hpp"
#include <vector>

ImagePrimitive::ImagePrimitive(const int copies)
  : m_face(), m_copies(copies)
{
  std::vector<Point3D> points;
  points.push_back(Point3D(0.0, 0.0, 0.0));
  points.push_back(Point3D(0.0, copies, 0.0));
  points.push_back(Point3D(copies, copies, 0.0));
  points.push_back(Point3D(copies, 0.0, 0.0));

  m_face = Polygon(points, Vector3D(0.0, 0.0, -1.0));
}

bool ImagePrimitive::filteredIntersect(const Point3D& eye, const Vector3D& ray, 
                                 std::list<IntersectionPoint>& tVals) const
{
  // No need to filter here
  return intersect(eye, ray,  tVals);
}

bool ImagePrimitive::intersect(const Point3D& eye, const Vector3D& ray, 
                         std::list<IntersectionPoint>& tVals) const
{
  if (m_face.intersect(eye, ray,  tVals)) {
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
  double width, height;
  Point2D coords = m_face.textureMapCoords(p, width, height);

  return Point2D(coords[0] - floor(coords[0]),
                 coords[1] - floor(coords[1]));
}

Vector3D ImagePrimitive::getNormal(const Point3D& p) const
{
  return m_face.getNormal(p);
}

Mesh* ImagePrimitive::getBoundingBox() const
{
  Point3D pos(0.0, 0.0, 0.0);
  double size = 1.0;

  std::vector<Point3D> vertices;
  vertices.push_back(Point3D(pos[0], pos[1], pos[2]));
  vertices.push_back(Point3D(pos[0] + size, pos[1], pos[2]));
  vertices.push_back(Point3D(pos[0] + size, pos[1] + size, pos[2]));
  vertices.push_back(Point3D(pos[0], pos[1] + size, pos[2]));
  vertices.push_back(Point3D(pos[0], pos[1], pos[2]));
  vertices.push_back(Point3D(pos[0] + size, pos[1], pos[2]));
  vertices.push_back(Point3D(pos[0] + size, pos[1] + size, pos[2]));
  vertices.push_back(Point3D(pos[0], pos[1] + size, pos[2]));

  std::vector<std::vector<int> > f;
  int faces[6][4] = { {3, 2, 1, 0}, // Front
                      {0, 1, 5, 4}, // Bottom
                      {2, 6, 5, 1}, // Right
                      {7, 3, 0, 4}, // Left
                      {7, 6, 2, 3}, // Top
                      {4, 5, 6, 7}}; // Back
  for (int i = 0; i < 6; i++) f.push_back(std::vector<int> (faces[i], faces[i] + 4));

  return new Mesh(vertices, f);
}
