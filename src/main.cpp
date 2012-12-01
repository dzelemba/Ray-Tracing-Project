#include <iostream>
#include "scene.hpp"
#include "scene_lua.hpp"
#include "renderer.hpp"

int main(int argc, char** argv)
{ 
  std::string filename = argv[argc - 1];
  std::string outfile = filename.substr(0, filename.find_first_of('.')).append(".png");

  Scene* scene = import_lua(filename);
  if (!scene) {
    std::cerr << "Could not open " << filename << std::endl;
    return 1;
  }

  Renderer* renderer = NULL;
  if (scene->hasFocalPlane()) {
    renderer = new DepthOfFieldRenderer(scene, scene->getFocalPlanePoint());  
  }
  int numCores = 1;
  if (argc >= 3) {
    for (int i = 1; i < argc - 1; i++) {
      if (std::string(argv[i]) == "-s" && !renderer) {
        renderer = new StochasticRenderer(scene);
      } else if (std::string(argv[i]) == "-c") {
        numCores = atoi(argv[i+1]);
      }
    }
  }

  if (renderer == NULL) {
    renderer = new BasicRenderer(scene);
  }

  renderer->render(outfile, numCores);
  delete renderer;
}

