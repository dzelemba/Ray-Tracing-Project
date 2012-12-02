#include "tree.hpp"
#include "image_primitive.hpp"

static TextureMap wood("wood_texture.png", Colour(0.0, 0.0, 0.0), 5, 0.0, 0.0);

static int totalBranches = 0;
static int totalLeaves = 0;

// Parameters determining tree look.
static double initialLength = 7.0;
static double initialThickness = 1.0;
static int initialBranches = 5;

static int leavesPerBranch = 3;
static int leafStartLevel = 2;

static double thicknessReduction = 2.0;
static double lengthReduction = 1.25;

static int recursiveDepth = 3;

// Helper functions for random number generators

static double rand(double lowerBound, double upperBound, double size)
{
  return lowerBound * size + (upperBound - lowerBound) * size * ((double)rand() / (double)RAND_MAX);
}

// Helper function for recursive paramaters

static double nextThickness(const double thickness, const int level)
{
  (void)level;

  return thickness / thicknessReduction;
}

static double prevThickness(const double thickness, const int level)
{
  (void)level;

  return thickness * thicknessReduction;
}

static double nextLength(const double length, const int level)
{
  return length / pow(lengthReduction, 2 * (level + 1) - 1);
}

static double prevLength(const double length, const int level)
{
  return length * pow(lengthReduction, 2 * level - 1); 
}

/*
  ********** Tree **********
*/


Tree::Tree(const std::string& name)
  : SceneNode(name)
{
  srand( time(NULL) );

  Branch* trunk = new Branch("Trunk", 0, initialThickness, initialLength);
  trunk->rotate('x', -90);

  add_child(trunk);

  std::cerr << "Total Branches: " <<  totalBranches 
            << " Total Leaves: " << totalLeaves << std::endl;
}

Tree::~Tree()
{
}

/*
  ********** Branch **********
*/

Branch::Branch(const std::string& name, int level, const double thickness, const double length)
  : SceneNode(name)
{
  createLeaves(level, thickness, length);

  double upDist = 0.0;
  double upAngle = 0.0;
  double zAngle = 0.0;
  if (level != 0) {
    upDist = rand(1.0/3.0, 2.0/3.0, prevLength(length, level));

    upAngle = rand(1.0/3.0, 8.0/9.0, 90.0);
    zAngle = rand(0.0, 1.0, 360.0);
  }

  createGeometryNode(length, thickness, upDist, upAngle, zAngle);
  spawnSubBranches(level, length, thickness);
}

Branch::Branch(const std::string& name, int level, const double thickness, const double length, 
               double upDist, double upAngle, double zAngle)
  : SceneNode(name)
{
  double branchLength = length;
  if (level == 1) {
    branchLength /= 1.5;
  }

  createLeaves(level, thickness, length);

  createGeometryNode(branchLength, thickness, upDist, upAngle, zAngle);
  spawnSubBranches(level, branchLength, thickness);
}

Branch::~Branch()
{
}

void Branch::createGeometryNode(const double length, const double thickness, const double upDist,
                                const double upAngle, const double zAngle)
{  
  Cylinder* c_branch= new Cylinder;
  GeometryNode* branch = new GeometryNode("TreeTrunk", c_branch);
  branch->set_material(&wood);

  this->rotate('z', zAngle);
  this->translate(Vector3D(0.0, 0.0, upDist));
  this->rotate('y', upAngle);
  branch->scale(Vector3D(thickness, thickness, length));

  add_child(branch);
}

void Branch::spawnSubBranches(const int level, const double length, const double thickness)
{
  if (level == recursiveDepth) return;

  // Spawn subbranches (make more as we get smaller)
  int numBranches = rand(0.2, 0.8, (level == 0 ? 2 : level) * initialBranches);
  for (int i = 0; i < numBranches; i++) {
    add_child(new Branch("SubBranch", level + 1,
                         nextThickness(thickness, level),
                         nextLength(length, level)));
    totalBranches++;
  }

  // Tree branches seem to split in two at the ends so simulate that.
  add_child(new Branch("SplitBranch", level + 1, nextThickness(thickness, level), 
                       nextLength(length, level), length, -30.0, 0.0));
  add_child(new Branch("SplitBranch", level + 1, nextThickness(thickness, level), 
                       nextLength(length, level), length, 30.0, 0.0));

  // Two branches coming out of the top looks a little weird so add a sphere.
  Sphere* p_sphere = new Sphere();
  GeometryNode* sphere = new GeometryNode("BranchTop", p_sphere);
  sphere->set_material(&wood);

  sphere->translate(Vector3D(0.0, 0.0, length));
  sphere->scale(Vector3D(thickness, thickness, thickness));

  add_child(sphere);

  totalBranches += 3;
}

void Branch::createLeaves(const int level, const double thickness, const double length)
{
  if (level >= leafStartLevel) {
    for (int i = 0; i < leavesPerBranch; i++) {
      add_child(new Leaf("Leaf", thickness, length));
      totalLeaves++;
    }
  }
}

/*
  ********** Leaf **********
*/

static TextureMap leafTexture("leaf.png", Colour(0.0, 0.0, 0.0), 5, 0.0, 0.0);

Leaf::Leaf(const std::string& name, const double branchThickness, const double branchLength)
  : SceneNode(name)
{
  ImagePrimitive* i_leaf = new ImagePrimitive();
  GeometryNode* leaf = new GeometryNode("Leaf", i_leaf);
  leaf->set_material(&leafTexture);

  double upDist = rand(1.0/3.0, 1.0, branchLength);
  double zAngle = rand(0.0, 1.0, 360.0);

  double diagonalDist = branchThickness + sqrt(2.0) - 0.25;
  double xyDist = -diagonalDist * sqrt(2.0) / 2.0;

  leaf->rotate('z', zAngle); 
  leaf->translate(Vector3D(xyDist, xyDist, upDist));

  add_child(leaf);
}

Leaf::~Leaf()
{
}
