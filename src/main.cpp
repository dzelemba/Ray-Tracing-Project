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
  if (argc >= 3) {
    for (int i = 1; i < argc - 1; i++) {
      if (std::string(argv[i]) == "-s") {
        renderer = new StochasticRenderer(scene);
      }
    }
  }
  if (renderer == NULL) {
    renderer = new BasicRenderer(scene);
  }

  renderer->render(outfile);
  delete renderer;
}

