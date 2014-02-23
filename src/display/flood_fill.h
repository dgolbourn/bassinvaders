#ifndef FLOOD_FILL_H_
#define FLOOD_FILL_H_
#include <unordered_set>
#include "boost/functional/hash.hpp"
namespace algorithm
{
typedef std::pair<int, int> NodeCoordinates;

template<class Node> class FloodFill
{
private:
  typedef std::unordered_set<NodeCoordinates, boost::hash<NodeCoordinates>> NodeSet;

  NodeSet set_;
  NodeCoordinates coords_;
  
  void Fill(Node& node)
  {
    if(set_.insert(coords_).second)
    {
      if(node(coords_))
      {
        coords_.first -= 1; Fill(node);
        coords_.first += 2; Fill(node);
        coords_.first -= 1;
        coords_.second += 1; Fill(node);
        coords_.second -= 2; Fill(node);
        coords_.second += 1;
      }
    }
  }

public:
  void operator()(Node& node)
  {
    set_.clear();
    coords_ = NodeCoordinates(0, 0);
    Fill(node);
  }
};
}
#endif