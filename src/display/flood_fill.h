#ifndef FLOOD_FILL_H_
#define FLOOD_FILL_H_
#include <unordered_set>
#include <iostream>

namespace algorithm
{
typedef std::pair<int, int> NodeCoordinates;

template<class Node> class FloodFill
{
private:
  class NodeHash 
  {
  public:
    std::size_t operator()(NodeCoordinates const& node) const 
    {
      static const int nice_prime = 257;
      return node.first + nice_prime * node.second;
    }
  };

  typedef std::unordered_set<NodeCoordinates, NodeHash> NodeSet;

  NodeSet set_;
  NodeCoordinates coords_;
  
  void Fill(Node& node)
  {
    if(set_.insert(coords_).second)
    {
      if(node.Fill(coords_))
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
  FloodFill(Node& node) : coords_(0, 0)
  {
    Fill(node);
  }
};
}
#endif