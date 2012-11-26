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

  // Returns background colour based on screen coordinates.
  Colour getBackground(const int x, const int y) const;

  // Returns backgroun colour based on ray that missed entire scene.
  Colour getBackground(const Point3D& eye, const Vector3D& ray) const;

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
};

#endif
