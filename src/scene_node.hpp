#ifndef SCENE_NODE_HPP
#define SCENE_NODE_HPP

#include <list>
#include "algebra.hpp"
#include "primitive.hpp"
#include "material.hpp"
#include "light.hpp"

class GeometryNode;

class SceneNode {
public:
  SceneNode(const std::string& name);
  virtual ~SceneNode();

  const Matrix4x4& get_transform() const { return m_trans; }
  const Matrix4x4& get_inverse() const { return m_invtrans; }
  
  void set_transform(const Matrix4x4& m)
  {
    m_trans = m;
    m_invtrans = m.invert();
  }

  void set_transform(const Matrix4x4& m, const Matrix4x4& i)
  {
    m_trans = m;
    m_invtrans = i;
  }

  void add_child(SceneNode* child)
  {
    m_children.push_back(child);
  }

  void remove_child(SceneNode* child)
  {
    m_children.remove(child);
  }

  // Callbacks to be implemented.
  // These will be called from Lua.
  void rotate(char axis, double angle);
  void scale(const Vector3D& amount);
  void translate(const Vector3D& amount);

  // Returns true if and only if this node is a JointNode
  virtual bool is_joint() const;

  // Intersect ray with the scene
  // Returns GeometryNode that is closest to eye. 
  // Also gives point of intersection and normal.
  bool intersect(const Point3D& eye, const Vector3D& ray, double offset) const;
  bool intersect(const Point3D& eye, const Vector3D& ray, const Colour& ambient, const std::list<Light*>& lights, double offset, Colour& c) const;
  const GeometryNode* intersect(const Point3D& eye, const Vector3D& ray, double offset, Point3D& poi, Vector3D& normal) const;
  
  std::string m_name;
protected:
  // Actual function used for the recursion.
  // Keeps track of the minimum t found so far to determine which object to return.
  virtual const GeometryNode* intersect(const Point3D& eye, const Vector3D& ray, double offest, Vector3D& normal, double& minT) const;

  // Useful for picking
  int m_id;

  // Transformations
  Matrix4x4 m_trans;
  Matrix4x4 m_invtrans;

  // Hierarchy
  typedef std::list<SceneNode*> ChildList;
  ChildList m_children;
};

class JointNode : public SceneNode {
public:
  JointNode(const std::string& name);
  virtual ~JointNode();

  virtual bool is_joint() const;

  void set_joint_x(double min, double init, double max);
  void set_joint_y(double min, double init, double max);

  struct JointRange {
    double min, init, max;
  };

  
protected:

  JointRange m_joint_x, m_joint_y;
};

class GeometryNode : public SceneNode {
public:
  GeometryNode(const std::string& name,
               Primitive* primitive);
  virtual ~GeometryNode();

  const Material* get_material() const;
  Material* get_material();

  void set_material(Material* material)
  {
    m_material = material;
  }

  // Overwritten to do actual intersection
  const GeometryNode* intersect(const Point3D& eye, const Vector3D& ray, double offset, Vector3D& normal, double& minT) const;

  Colour getColour(const SceneNode* rootNode, 
                   const Point3D& eye, const Point3D& poi, const Vector3D& normal,
                   const Colour& ambient, const std::list<Light*>& lights, int recursiveDepth = 0) const;
protected:
  Material* m_material;
  Primitive* m_primitive;

private:
  Colour getLightContribution(const SceneNode* rootNode, const Point3D& poi, const Vector3D& viewDirection,
                              const Vector3D& normal, const std::list<Light*>& lights) const;
};

#endif
