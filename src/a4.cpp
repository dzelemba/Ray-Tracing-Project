#include "a4.hpp"
#include "image.hpp"

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
  // Copy view and up so we can normalize them.
  Vector3D nUp = up;
  nUp.normalize();
  Vector3D nView = view;
  nView.normalize();

  // Get our image
  Image img(width, height, 3);

  //  --------- Draw Background ----------------

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < height; x++) {
      // Blue: increasing from top to bottom
      img(x, y, 2) = (double)y / height;
    }
  }

  // ------ Casting primary rays --------

  // Calculate how far screen is from eye based on fov and width/height
  int largerDimension = std::max(width, height);
  double distance = ((double)largerDimension / 2.0) / tan(M_PI * fov / 360); // Divide fov by 2 to get angle wrt to mid

  // Get the left vector.
  Vector3D nLeft = up.cross(view); 
  nLeft.normalize();

  // For each screen point calculate its view coordinates.
  Vector3D ray;
  Colour c(1.0);
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      double yDisplacement = ((double)height / 2) - (double)y;
      double xDisplacement = ((double)width / 2) - (double)x;
      ray = distance * nView + yDisplacement * nUp + xDisplacement * nLeft;

      // Now we intersect this ray with our scene.
      // and get a colour back.
      if (root->intersect(eye, ray, ambient, lights, 0.0, c)) {
        img(x,y,0) = c.R();
        img(x,y,1) = c.G();
        img(x,y,2) = c.B();
      } 
    }
    if (y % (height / 10) == 0) {
      std::cerr << 10 * (y / (height / 10)) << "% .. ";
    } 
  }
  std::cerr << std::endl;

  img.savePng(filename);
}
