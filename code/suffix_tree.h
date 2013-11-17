#ifndef SUFFIX_TREE_H_
#define SUFFIX_TREE_H_

#include <string>
#include <vector>

using std::string;
using std::vector;
using std::cin;
using std::cout;
using std::endl;

namespace suffixtree {

/******************************************************************************
 * The Visitor class (not sure whether we need this)
 * Calculates the max length of common prefixes for each first k suffixes
 * during Ukkonen's suffix tree building.
 ******************************************************************************/
class Visitor {
 public:
  virtual void VisitOnInsert(size_t, size_t, char) = 0;
  virtual void VisitOnSplit(size_t, size_t) = 0;
  virtual ~Visitor()
  {}
};

class MaxLengthOfCommonPrefixesVisitor: public Visitor {
 public:
  explicit MaxLengthOfCommonPrefixesVisitor(const string& str)
      : common_prefixes_max_length(str.length(), 0)
  {}

  void VisitOnInsert(size_t depth, size_t pos, char inserted_char) {
    if (pos < common_prefixes_max_length.size()) {
      common_prefixes_max_length[pos] = depth;
    }
  }

  void VisitOnSplit(size_t depth, size_t pos) {
    common_prefixes_max_length[pos] = depth;
  }

  const vector<size_t>& get_common_prefixes_max_length() const {
    return common_prefixes_max_length;
  }

 private:
  vector<size_t> common_prefixes_max_length;
};


/******************************************************************************
 * Suffix Tree implementation
 ******************************************************************************/
class SuffixTree {
 public:
  explicit SuffixTree(const string& str)
      : the_string(str)
  {
    CanonizeCharacters();
  }

  void Build(Visitor* visitor);

 private:
  static const size_t ALPHABET_SIZE = 26;   // we use latin lowercase alphabet
  static const char FIRST_ALPHABET_CHARACTER = 'a';
  static const char SENTINEL_SIGN = static_cast<char>(ALPHABET_SIZE);

  struct Node;
  struct Edge;
  struct ActivePoint;

  struct Node {
    size_t depth;
    Node* suffix_link;
    vector<Edge> edges;

    explicit Node(size_t node_depth)
        : depth(node_depth)
        , suffix_link(NULL)
        , edges(ALPHABET_SIZE + 1)  // +1 for SENTINEL_SIGN
    {}
  };

  struct Edge {
    size_t from;
    size_t to;
    Node* tail;
    bool exists;    // flag of edge existance

    Edge()
        : exists(false)
    {}

    void Assign(size_t from_index, size_t to_index, Node* node_to_point) {
      from = from_index;
      to = to_index;
      tail = node_to_point;
      exists = true;
    }

    // NOTE: the length is not the real Length as we usually understand,
    //       but it's the Length - 1, because the segment of the_string
    //       on the edge is presented by [from, to], NOT [from, to)!
    size_t length()  const {
      return to - from;   // to >= from according to the logic of the algorithm
    }
  };

  struct ActivePoint {
    Node* node;
    Edge* edge;
    size_t length;

    ActivePoint()
        : node(NULL)
        , edge(NULL)
        , length(0)
    {}
    
    bool isExplicit() const {
      return length == 0;
    }
  };

  string the_string;
  vector<Node> nodes;
  Node* ROOT;
  Node* last_created_node_in_current_iteration;

  ActivePoint active;
  size_t  unresolved_suffixes;
  size_t  current_suffix_start_index;
  size_t  current_suffix_end_index;
  char    current_suffix_last_char;

  void CanonizeCharacters();
  void InsertEdge();
  void SplitEdge();
  bool AddSuffixImplicitly();
  bool NormalizeActivePoint();
  void UpdateActivePointAfterEdgeSplitting();
  void CreateSuffixLink(Node* node);
};

}  // namespace suffixtree

#endif  // SUFFIX_TREE_H_
