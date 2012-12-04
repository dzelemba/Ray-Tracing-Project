#include "scene.hpp"
#include <vector>

Scene::Scene(SceneNode* root,
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
  screenDist(((double)std::max(width, height) / 2.0) / tan(M_PI * fov / 360)),
  backgroundDist(screenDist + 1000.0), // TODO: Do this properly. Just assume nothing is past 1000 for now.
  background(std::vector<Point3D>(), std::vector<std::vector<int> >()),
  m_focalPlanePoint(),
  m_hasFocalPlane(false)
{
  this->view.normalize();
  this->up.normalize();
  this->left.normalize();

  SceneNode::setScene(this);

  // Create a mesh to represent the background 
  // Used in determining background colour of ray that misses entire scene.
  // TODO: Change this to a plane (and texture map maybe)
  Point3D backgroundCenter = this->eye + (backgroundDist) * this->view;

  std::vector<Point3D> vertices;
  int bHeight = height * backgroundDist / screenDist;
  int bWidth= width * backgroundDist / screenDist;
  vertices.push_back(backgroundCenter - (double)bHeight/2.0 * this->up + (double)bWidth/2.0 * this->left);
  vertices.push_back(backgroundCenter - (double)bHeight/2.0 * this->up - (double)bWidth/2.0 * this->left);
  vertices.push_back(backgroundCenter + (double)bHeight/2.0 * this->up - (double)bWidth/2.0 * this->left);
  vertices.push_back(backgroundCenter + (double)bHeight/2.0 * this->up + (double)bWidth/2.0 * this->left);

  std::vector< std::vector<int> > faces;
  std::vector<int> face1;
  face1.push_back(0);
  face1.push_back(1);
  face1.push_back(2);
  face1.push_back(3);
  faces.push_back(face1);

  background = Mesh(vertices, faces);
}

bool Scene::intersect(const double dx, const double dy, Colour& c) const
{
  Vector3D ray = getRay(dx, dy);
  return intersect(eye, ray, c);
}

bool Scene::intersect(const Point3D& start, const Vector3D& ray, Colour &c) const
{
  return root->intersect(start, ray, 0.0, c);
}

Point3D Scene::getJitteredEye() const
{
  double xOffset = ((((double)rand() / (double)RAND_MAX) - 0.5)) / 4.0;
  double yOffset = ((((double)rand() / (double)RAND_MAX) - 0.5)) / 4.0;

  return eye + xOffset * left + yOffset * up;
}

Vector3D Scene::getRay(const double dx, const double dy) const
{
  return screenDist * view + dy * up + dx * left;
}

Colour Scene::getBackground(const int x, const int y) const
{
  (void)x;

  return Colour(0.0, 0.0, (double)y / height);
}

Colour Scene::getBackground(const Point3D& eye, const Vector3D& ray) const
{
  Point3D backgroundCenter = eye + backgroundDist * view;

  Point3D backgroundPt;
  if (background.intersect(eye, ray, backgroundPt)) {
    Vector3D p = backgroundPt - backgroundCenter;

    // Now we have a simple linear system to solve
    // Since: p = x * left + y * up
    // and we want (x, y)
    // TODO: Solve this properly
    // Left and up are usually just (-1, 0, 0) and (0, 1, 0)
    double x = -p[0] * screenDist / backgroundDist;
    double y = p[1] * screenDist / backgroundDist; 

    return getBackground((double)width / 2.0 - x, (double)height / 2.0 - (int)y);
  } else {
    return Colour(0.0);
  }
}
