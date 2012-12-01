#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <string>
#include "image.hpp"
#include "scene.hpp"
#include "algebra.hpp"
#include "shapes.hpp"

class Renderer {
 public:
  Renderer(const Scene* scene);
  virtual ~Renderer();
  void render(const std::string& filename, const int numThreads);

  void renderRows(const int startRow, const int numRows);

 protected:
  virtual Colour getPixelColour(const int x, const int y) = 0;

  const Scene* m_scene;
  Image m_img;
};

class BasicRenderer : public Renderer {
 public:
  BasicRenderer(const Scene* scene);
  virtual ~BasicRenderer();

 protected:
  virtual Colour getPixelColour(const int x, const int y);
};

class StochasticRenderer : public Renderer {
 public:
  StochasticRenderer(const Scene* scene);
  virtual ~StochasticRenderer();

 protected:
  virtual Colour getPixelColour(const int x, const int y);

 private:
  const int RAYS_PER_PIXEL;
};

class DepthOfFieldRenderer : public Renderer {
 public:
  DepthOfFieldRenderer(const Scene* scene, const Point3D& focalPlanePoint);
  virtual ~DepthOfFieldRenderer();

 protected:
  virtual Colour getPixelColour(const int x, const int y);

 private:
  Plane m_focalPlane;
};

#endif
