#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <string>
#include "image.hpp"
#include "scene.hpp"
#include "algebra.hpp"

class Renderer {
 public:
  Renderer(const Scene* scene);
  virtual ~Renderer();
  void render(const std::string& filename, const int numThreads);

  void renderRows(const int startRow, const int numRows);

 protected:
  virtual Colour getPixelColour(const int x, const int y) = 0;

  const Scene* scene;
  Image img;
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
#endif
