#ifndef TREE_HPP
#define TREE_HPP

#include "scene_node.hpp"
#include "primitive.hpp"

/*
  Note: We will allocate a lot of memory on the heap, but since none of it can
  be freed until the ray tracer completes, we won't free it.
*/

class Tree : public SceneNode {
 public:
  Tree(const std::string& name, double initialLength = 7.0, double initialThickness = 1.0,
       int initialBranches = 6, int leavesPerBranch = 3, int leafStartLevel = 2, 
       double thicknessReduction = 2.0, double lengthReduction = 1.25, int recursiveDepth = 4,
       int seed = time(NULL));
  ~Tree();
};

class Branch : public SceneNode {
 public:
  Branch(const std::string& name, int level, const double thickness, const double length);
  Branch(const std::string& name, int level, const double thickness, const double length, 
               double upDist, double upAngle, double zAngle);
  ~Branch();

  Mesh* getBoundingBox();
  
  void intersect(const Point3D& eye, const Vector3D& ray, SegmentList& tVals) const;

 private:
  void createGeometryNode(const double length, const double thickness, const double upDist,
                          const double upAngle, const double zAngle);
  void spawnSubBranches(const int level, const double length, const double thickness);
  void createLeaves(const int level, const double thickness, const double branchLength);

  void createBoundingBox();
  
  Mesh m_boundingBox;
};

class Leaf : public SceneNode {
 public:
  Leaf(const std::string& name, const double branchThickness, const double branchLength);
  ~Leaf();

  Mesh* getBoundingBox();
};

#endif
