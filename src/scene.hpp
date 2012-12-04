#ifndef SCENE_HPP
#define SCENE_HPP

#include "algebra.hpp"
#include "scene_node.hpp"
#include "light.hpp"
#include "primitive.hpp"

class Scene {
 public:
  Scene(SceneNode* root,
        int width, int height,
        const Point3D eye, const Vector3D view,
        const Vector3D up, double fov,
        const Colour ambient,
        const std::list<Light*> lights);

  bool intersect(const double dx, const double dy, Colour &c) const;
  bool intersect(const Point3D& start, const Vector3D& ray, Colour &c) const;

  Point3D getJitteredEye() const;
  Vector3D getRay(const double dx, const double dy) const;

  // Returns background colour based on screen coordinates.
  Colour getBackground(const int x, const int y) const;

  // Returns background colour based on ray that missed entire scene.
  Colour getBackground(const Point3D& eye, const Vector3D& ray) const;

  // Needed by DepthOfFieldRenderer.
  const Vector3D& getView() const { return view; }
  const Point3D& getEye() const { return eye; }

  void setFocalPlanePoint(const Point3D& p) {
    m_focalPlanePoint = p;
    m_hasFocalPlane = true;
  }
  bool hasFocalPlane() { return m_hasFocalPlane; }
  const Point3D& getFocalPlanePoint() { return m_focalPlanePoint; }

  // Image Size
  const int height;
  const int width;

  const SceneNode* root;

  // Lighting parameters
  const Colour ambient;
  const std::list<Light*> lights;

 private:
  // Viewing parameters
  const Point3D eye;
  Vector3D view;
  Vector3D up;
  Vector3D left;
  const double fov;
  const double screenDist;

  int backgroundDist;
  Mesh background;

  // Depth of Field
  Point3D m_focalPlanePoint;
  bool m_hasFocalPlane;
};

#endif
