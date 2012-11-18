#include "a4.hpp"
#include "image.hpp"

static const int RAYS_PER_PIXEL = 1;

Colour getBackground(const int x, const int y, const int height)
{
  return Colour(0.0, 0.0, (double)y / height);
}

void a4_render(// What to render
               SceneNode* root,
               // Where to output the image
               const std::string& filename,
               // Image size
               int width, int height,
               // Viewing parameters
               const Point3D& eye, const Vector3D& view,
               const Vector3D& up, double fov,
               // Lighting parameters
               const Colour& ambient,
               const std::list<Light*>& lights
               )
{
  std::cerr << "WARNING: Old Code Being Executed" << std::endl;

  // Copy view and up so we can normalize them.
  Vector3D nUp = up;
  nUp.normalize();
  Vector3D nView = view;
  nView.normalize();

  // Get our image
  Image img(width, height, 3);

  // ------ Casting primary rays --------

  // Calculate how far screen is from eye based on fov and width/height
  int largerDimension = std::max(width, height);
  double distance = ((double)largerDimension / 2.0) / tan(M_PI * fov / 360);

  // Get the left vector.
  Vector3D nLeft = up.cross(view); 
  nLeft.normalize();

  // For each screen point calculate its view coordinates.
  Vector3D ray;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      // Stocastic Sampling.
      // Break the pixel into subpixels and cast a random ray in each subpixel.
      // Weight the samples using a gaussian distribution based on distance from pixel.
      double pixelY = ((double)height / 2) - (double)y;
      double pixelX = ((double)width / 2) - (double)x;

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
          ray = distance * nView + dy * nUp + dx * nLeft;

          // Now we intersect this ray with our scene and get a colour back.
          Colour sample(getBackground(x, y, height));
          root->intersect(eye, ray, ambient, lights, 0.0, sample);

          double distSquared = xOff * xOff + yOff * yOff; 
          double weight = (1.0 / sqrt(2 * M_PI)) * exp(-1.0/2.0 * distSquared);
          c = c + weight * sample;
          totalWeight += weight;
        }
      }
      c = (1.0 / totalWeight) * c;
      img(x,y,0) = c.R();
      img(x,y,1) = c.G();
      img(x,y,2) = c.B();
    }

    if (y % (height / 10) == 0) {
      std::cerr << 10 * (y / (height / 10)) << "% .. ";
    } 
  }
  std::cerr << std::endl;

  img.savePng(filename);
}
