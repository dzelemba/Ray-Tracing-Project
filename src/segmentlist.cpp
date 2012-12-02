#include "segmentlist.hpp"
#include <cfloat>
#include <iostream>

void SegmentList::insert(IntersectionPoint t1, IntersectionPoint t2, const GeometryNode* owner)
{
  SegmentList segment;

  t1.m_owner = owner;
  t2.m_owner = owner;

  if (t1 < t2) {
    m_segments.push_back(Segment(t1, t2));
  } else {
    m_segments.push_back(Segment(t2, t1));
  }
}

void SegmentList::insert(SegmentList& segments)
{
  for (list<Segment>::iterator it = segments.m_segments.begin(); it != segments.m_segments.end(); it++) {
    m_segments.push_back(*it);
  }

  // TODO: Combine overlappings segements.
}

void SegmentList::intersect(SegmentList& segments)
{
  list<Segment> newSegments;
  // TODO: This could be done a lot more efficiently.
  for (list<Segment>::iterator it1 = segments.m_segments.begin(); it1 != segments.m_segments.end(); it1++) {
    for (list<Segment>::iterator it2 = m_segments.begin(); it2 != m_segments.end(); it2++) {
      if (it1->m_end > it2->m_start && it2->m_end > it1->m_start) {
        newSegments.push_back(Segment(max(it1->m_start, it2->m_start),
                                      min(it1->m_end, it2->m_end)));
      }
    }
  }

  m_segments = newSegments;
}
  
void SegmentList::remove(SegmentList& segments)
{
  // Flip normals.
  for (list<Segment>::iterator it = segments.m_segments.begin(); it != segments.m_segments.end(); it++) {
    it->flipNormals(); 
  }

  list<Segment> newSegments;
  // TODO: This could be done a lot more efficiently.
  for (list<Segment>::iterator it2 = m_segments.begin(); it2 != m_segments.end(); it2++) {
    bool keep = true;
    for (list<Segment>::iterator it1 = segments.m_segments.begin(); it1 != segments.m_segments.end(); it1++) {
      if (it1->m_end > it2->m_start && it2->m_end > it1->m_start) {
        if (it2->m_start < it1->m_start) {
          newSegments.push_back(Segment(it2->m_start, it1->m_start));
        }
        if (it2->m_end > it1->m_end) {
          newSegments.push_back(Segment(it1->m_end, it2->m_end));
        }
        keep = false;
      }
    }
    if (keep) {
      newSegments.push_back(*it2);
    }
  }

  m_segments = newSegments;
}

void SegmentList::clear()
{
  m_segments.clear();
}

bool SegmentList::getMin(const double offset, IntersectionPoint& poi)
{
  IntersectionPoint min(DBL_MAX);
  for (list<Segment>::iterator it = m_segments.begin(); it != m_segments.end(); it++) {
    if (it->start() > offset && it->m_start < min) {
      min = it->m_start;
    } else if (it->end() > offset && it->m_end < min) {
      min = it->m_end;
    }
  }
  poi = min;

  return min.m_t != DBL_MAX;
}

void SegmentList::getValidSegments(const double offset, list<Segment>& segments)
{
  for (list<Segment>::iterator it = m_segments.begin(); it != m_segments.end(); it++) {
    if (it->start() > offset) {
      segments.push_back(*it);
    }
  }
  segments.sort();
}

void SegmentList::transformNormals(const Matrix4x4& m)
{
  for (list<Segment>::iterator it = m_segments.begin(); it != m_segments.end(); it++) {
    it->m_start.m_normal = m * it->m_start.m_normal;
    it->m_end.m_normal = m * it->m_end.m_normal;
  }
}
