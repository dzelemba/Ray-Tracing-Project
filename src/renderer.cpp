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
  int numRows;
};

void* startThread(void* data)
{
  struct Args* args = (struct Args*)data;
  args->renderer->renderRows(args->startRow, args->numRows);

  return NULL;
}

/*
  *************** Renderer **************
*/

Renderer::Renderer(const Scene* scene) :
  scene(scene),
  img(scene->width, scene->height, 3)
{
}

Renderer::~Renderer()
{
}

void Renderer::render(const std::string& filename, const int numThreads)
{
  std::vector<pthread_t*> threads;
  int rowsPerThread = scene->height / numThreads;
  struct Args* data = new Args[numThreads];
  for (int i = 0; i < numThreads; i++) {
    pthread_t* thread = new pthread_t;
    threads.push_back(thread);

    data[i].renderer = this;
    data[i].startRow = i * rowsPerThread;
    data[i].numRows = rowsPerThread + (i == numThreads - 1 ? scene->height % numThreads : 0);
    pthread_create(thread, NULL, &startThread, (void *)&data[i]);
  }

  for (std::vector<pthread_t*>::iterator it = threads.begin(); it != threads.end(); it++) {
    pthread_join(**it, NULL);
    delete *it;
  }
  delete [] data;
  
  img.savePng(filename);
}

void Renderer::renderRows(const int startRow, const int numRows)
{
  for (int y = startRow; y < startRow + numRows; y++) {
    for (int x = 0; x < scene->width; x++) {
      Colour c = getPixelColour(x, y);
      img(x, y, 0) = c.R();
      img(x, y, 1) = c.G();
      img(x, y, 2) = c.B();
    }
  }
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
  Colour c = scene->getBackground(x, y);
  scene->intersect(((double)scene->width / 2.0) - (double)x, ((double)scene->height / 2.0) - (double)y, c);
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
  double pixelY = ((double)scene->height / 2) - (double)y;
  double pixelX = ((double)scene->width / 2) - (double)x;

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
      Colour sample = scene->getBackground(x, y);
      scene->intersect(dx, dy, sample);

      double distSquared = xOff * xOff + yOff * yOff; 
      double weight = (1.0 / sqrt(2 * M_PI)) * exp(-1.0/2.0 * distSquared);
      c = c + weight * sample;
      totalWeight += weight;
    }
  }
  c = (1.0 / totalWeight) * c;

  return c;
}
