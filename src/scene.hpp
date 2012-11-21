#ifndef SCENE_HPP
#define SCENE_HPP

#include "algebra.hpp"
#include "scene_node.hpp"
#include "light.hpp"

class Scene {
 public:
  Scene(const SceneNode* root,
        int width, int height,
        const Point3D eye, const Vector3D view,
        const Vector3D up, double fov,
        const Colour ambient,
        const std::list<Light*> lights);

  bool intersect(const double dx, const double dy, Colour &c) const;
  Colour getBackground(const int x, const int y) const;

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
};

#endif
