#ifndef SEGMENTLIST_HPP
#define SEGMENTLIST_HPP

#include <list>
#include <cstddef>
#include "algebra.hpp"
using namespace std;

struct Segment {
  Segment(const IntersectionPoint& start, const IntersectionPoint& end) :
    m_start(start), m_end(end) 
  {}

  bool operator<(const Segment& other) {
    if (m_start.m_t == other.m_start.m_t) {
      return m_end < other.m_end;
    }
    return m_start< other.m_start;
  }

  double start() { return m_start.m_t; }
  double end() { return m_end.m_t; }

  void flipNormals() {
    m_start.m_normal = -1 * m_start.m_normal; 
    m_end.m_normal = -1 * m_end.m_normal; 
  }

  IntersectionPoint m_start;
  IntersectionPoint m_end;
};

class SegmentList {
 public:
  SegmentList() :
    m_segments()
  {}

  void insert(IntersectionPoint t1, IntersectionPoint t2, const GeometryNode* owner);
  void insert(SegmentList& segments);
  void intersect(SegmentList& segmens);
  void remove(SegmentList& segments);

  void clear();

  bool getMin(const double offset, IntersectionPoint& poi);
  void getValidSegments(const double offset, list<Segment>& segments);

  void transformNormals(const Matrix4x4& m);

 private:
  list<Segment> m_segments;
};

#endif
