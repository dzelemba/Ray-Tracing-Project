#include "scene.hpp"

Scene::Scene(const SceneNode* root,
             int width, int height,
             const Point3D eye, const Vector3D view,
             const Vector3D up, double fov,
             const Colour ambient,
             const std::list<Light*> lights) :
  height(height),
  width(width),
  root(root),
  ambient(ambient),
  lights(lights),
  eye(eye),
  view(view),
  up(up),
  left(up.cross(view)),
  fov(fov),
  screenDist(((double)std::max(width, height) / 2.0) / tan(M_PI * fov / 360))
{
  this->view.normalize();
  this->up.normalize();
  this->left.normalize();

  SceneNode::setScene(this);
}

bool Scene::intersect(const double dx, const double dy, Colour& c) const
{
  Vector3D ray = screenDist * view + dy * up + dx * left;
  return root->intersect(eye, ray, 0.0, c);
}

Colour Scene::getBackground(const int x, const int y) const
{
  (void)x;

  return Colour(0.0, 0.0, (double)y / height);
}
