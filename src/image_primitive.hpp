#ifndef LEAF_HPP
#define LEAF_HPP

#include "primitive.hpp"

class ImagePrimitive : public Primitive {
 public:
  ImagePrimitive();

  bool filteredIntersect(const Point3D& eye, const Vector3D& ray, const double offset,
                                 std::list<IntersectionPoint>& tVals) const;
  bool intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                         std::list<IntersectionPoint>& tVals) const;
  bool containsPoint(const Point3D& p) const;
  Point2D textureMapCoords(const Point3D& p) const;
  Vector3D getNormal(const Point3D& p) const;

  Mesh* getBoundingBox() const;
 private:
  Polygon m_face;
};

#endif
