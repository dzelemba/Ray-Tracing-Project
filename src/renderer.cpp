#include "renderer.hpp"
#include <iostream>
#include <pthread.h>
#include <vector>

/*
  Wrapper types and functions for the creation of new threads
*/

struct Args {
  Renderer* renderer;
  int startRow;
  int stepSize;
};

void* startThread(void* data)
{
  struct Args* args = (struct Args*)data;
  args->renderer->renderRows(args->startRow, args->stepSize);

  return NULL;
}

/*
  *************** Renderer **************
*/

Renderer::Renderer(const Scene* scene) :
  m_scene(scene),
  m_img(m_scene->width, m_scene->height, 3)
{
}

Renderer::~Renderer()
{
}

void Renderer::render(const std::string& filename, const int numThreads)
{
  std::vector<pthread_t*> threads;
  struct Args* data = new Args[numThreads];
  for (int i = 0; i < numThreads; i++) {
    pthread_t* thread = new pthread_t;
    threads.push_back(thread);

    data[i].renderer = this;
    data[i].startRow = i;
    data[i].stepSize = numThreads;
    pthread_create(thread, NULL, &startThread, (void *)&data[i]);
  }

  for (std::vector<pthread_t*>::iterator it = threads.begin(); it != threads.end(); it++) {
    pthread_join(**it, NULL);
    delete *it;
  }
  delete [] data;
  
  m_img.savePng(filename);
}

void Renderer::renderRows(const int startRow, const int stepSize)
{
  int threadNo = startRow +1; 
  int numRows = m_scene->height / stepSize;

  for (int y = startRow; y < m_scene->height; y+= stepSize) {
    for (int x = 0; x < m_scene->width; x++) {
      Colour c = getPixelColour(x, y);
      m_img(x, y, 0) = c.R();
      m_img(x, y, 1) = c.G();
      m_img(x, y, 2) = c.B();
    }
    if ((y / stepSize) % (numRows / 4) == 0) {
      std::cerr << "Thread " << threadNo << ": " << 25 * ((y / stepSize) / (numRows / 4)) << "% ";
    }
  }
  std::cerr << "Thread " << threadNo << " done" << std::endl;
}
/*
  *************** BasicRenderer **************
*/

BasicRenderer::BasicRenderer(const Scene* scene) :
  Renderer(scene)
{
}

BasicRenderer::~BasicRenderer()
{
}

Colour BasicRenderer::getPixelColour(const int x, const int y)
{
  Colour c = m_scene->getBackground(x, y);
  m_scene->intersect(((double)m_scene->width / 2.0) - (double)x, ((double)m_scene->height / 2.0) - (double)y, c);
  return c;
}

/*
  *************** StochasticRenderer **************
*/

StochasticRenderer::StochasticRenderer(const Scene* scene) :
  Renderer(scene),
  RAYS_PER_PIXEL(16)
{
}

StochasticRenderer::~StochasticRenderer()
{
}

Colour StochasticRenderer::getPixelColour(const int x, const int y)
{
  // Stocastic Sampling.
  // Break the pixel into subpixels and cast a random ray in each subpixel.
  // Weight the samples using a gaussian distribution based on distance from pixel.
  double pixelY = ((double)m_scene->height / 2) - (double)y;
  double pixelX = ((double)m_scene->width / 2) - (double)x;

  int gridSize = sqrt(RAYS_PER_PIXEL);
  double stepSize = 1.0 / (double)gridSize;

  Colour c(0.0);
  double totalWeight = 0;
  for (int subY = 0; subY < gridSize; subY++) {
    for (int subX = 0; subX < gridSize; subX++) {
      double xOff = (((double)rand() / (double)RAND_MAX - 0.5) / gridSize) + 
                    stepSize * (double)subX - 0.5 + stepSize / 2.0; 
      double yOff = (((double)rand() / (double)RAND_MAX - 0.5) / gridSize) +
                    stepSize * (double)subX - 0.5 + stepSize / 2.0;

      double dx = pixelX + xOff;
      double dy = pixelY + xOff;

      // Now we intersect this ray with our scene and get a colour back.
      Colour sample = m_scene->getBackground(x, y);
      m_scene->intersect(dx, dy, sample);

      double distSquared = xOff * xOff + yOff * yOff; 
      double weight = (1.0 / sqrt(2 * M_PI)) * exp(-1.0/2.0 * distSquared);
      c = c + weight * sample;
      totalWeight += weight;
    }
  }
  c = (1.0 / totalWeight) * c;

  return c;
}

/*
  *************** DepthOfFieldRenderer **************
*/

DepthOfFieldRenderer::DepthOfFieldRenderer(const Scene* scene, const Point3D& focalPlanePoint) :
  Renderer(scene),
  m_focalPlane(m_scene->getView(), focalPlanePoint)
{
}

DepthOfFieldRenderer::~DepthOfFieldRenderer()
{
}
  
Colour DepthOfFieldRenderer::getPixelColour(const int x, const int y)
{
  double pixelY = ((double)m_scene->height / 2) - (double)y;
  double pixelX = ((double)m_scene->width / 2) - (double)x;

  // First we need to get our point on the focal plane;
  Vector3D ray = m_scene->getRay(pixelX, pixelY);
  Point3D eye = m_scene->getEye();
  Point3D focalPoint = eye + (m_focalPlane.intersect(eye, ray)) * ray;

  static const int sampleRays = 10;
  Colour c(0.0);
  for (int i = 0; i < sampleRays; i++) {
    Point3D offsetEye = m_scene->getJitteredEye(); 
    Vector3D offsetRay = focalPoint - offsetEye; 
    
    Colour sample = m_scene->getBackground(x, y);
    m_scene->intersect(offsetEye, offsetRay, sample);
    c = c + sample;
  }

  return (1.0 / (double)sampleRays) * c;
}
