/******************************************************************************
 * Suffix tree based pattern matching algorithm implementation
 *
 * Copyright 2013, Maruan Al-Shedivat
 ******************************************************************************/
#include <iostream>
#include <string>
#include <vector>

using std::string;
using std::vector;
using std::cin;
using std::cout;
using std::endl;

/******************************************************************************
 * i/o functions
 ******************************************************************************/
void Input(string* input_string) {
  cin >> *input_string;
}

void Output(const vector<size_t>& result) {
  for (size_t pos = 0; pos < result.size(); ++pos) {
    cout << result[pos] << endl;
  }
}

/******************************************************************************
 * The Visitor pattern class
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
 * This implementation based on 3 simple rules of behaviour while stepping.
 * 
 * RULE 1. After an edge splitting from root:
 *    - active.node remains root;
 *    - active.edge is set to the first character of the new suffix we need
 *      to insert;
 *    - active.length is reduced by 1.
 *
 * RULE 2. If we split an edge and insert a new node, and if that is not the
 * first node created during the current step, we connect the previously
 * inserted node and the new node through a special pointer, a suffix link.
 * Also, if we change the active node during 'active node normalizatio process'
 * we should create an active link from the last created node to newly updated
 * active node.
 *
 * RULE 3. After splitting an edge from an active node that is not the root
 * node, we follow the suffix link going out of that node, if there is any, and
 * reset the active.node to the node it points to. If there is no suffix link,
 * we set the active.node to the root. active edge and active length remain
 * unchanged.
 *
 * NOTE: This nice notation was generally taken from the best answer to the
 *       question http://stackoverflow.com/questions/9452701/ and then some
 *       mistakes were corrected.
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

  class Node;
  class Edge;
  class ActivePoint;

  class Node {
   public:
    size_t depth;
    Node* suffix_link;
    vector<Edge> edges;

    explicit Node(size_t node_depth)
        : depth(node_depth)
        , suffix_link(NULL)
        , edges(ALPHABET_SIZE + 1)  // +1 for SENTINEL_SIGN!
    {}
  };

  class Edge {
   public:
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

  class ActivePoint {
   public:
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

  ActivePoint active;
  size_t  unresolved_suffixes;
  size_t  current_suffix_start_index;
  size_t  current_suffix_end_index;
  char    current_suffix_last_char;

  Node* last_created_node_in_current_iteration;

  void CanonizeCharacters();
  void InsertEdge();
  void SplitEdge();
  bool AddSuffixImplicitly();
  bool NormalizeActivePoint();
  void UpdateActivePointAfterEdgeSplitting();
  void CreateSuffixLink(Node* node);
};

/******************************************************************************
 * SuffixTree internal methods and classes implementation
 ******************************************************************************/
// Just replace all the characters with their number in the alphabet
// and add '$' to the end of the string
void SuffixTree::CanonizeCharacters() {
  for (size_t pos = 0; pos < the_string.length(); ++pos) {
    the_string[pos] -= FIRST_ALPHABET_CHARACTER;
  }
  the_string += SENTINEL_SIGN;
}

void SuffixTree::Build(Visitor* visitor) {
  // We know that suffix tree will contain n nodes in total without leafs.
  // And we have to prevent the reallocations! Otherwise it will kill all
  // the pointers to the Nodes.So I chose to reserve 2*n memory spase.
  //
  // Another approach is to use list<Node>, but Michael doesn't like lists :)
  nodes.reserve(2 * the_string.length());

  // Init nodes and active point
  nodes.push_back(Node(0));
  ROOT = &*nodes.begin();
  active.node = ROOT;
  unresolved_suffixes = 0;

  for (current_suffix_end_index = 0;
       current_suffix_end_index < the_string.length();
       ++current_suffix_end_index) {
    ++unresolved_suffixes;
    current_suffix_last_char = the_string[current_suffix_end_index];
    last_created_node_in_current_iteration = NULL;

    for (current_suffix_start_index =
            (current_suffix_end_index + 1) - unresolved_suffixes;
         current_suffix_start_index <= current_suffix_end_index;
         ++current_suffix_start_index) {

      if (active.isExplicit()) {
        if (!active.node->edges[current_suffix_last_char].exists) {
          // If visitor exists, let's perform a visit action on edge insertion
          if (visitor) visitor->VisitOnInsert(active.node->depth,
                                              current_suffix_start_index,
                                              current_suffix_last_char);
          
          // Insert a new edge, there is no such
          InsertEdge();
        } else {
          // if such edge exists we make it active and add suffix implicitly
          active.edge = &active.node->edges[current_suffix_last_char];
          AddSuffixImplicitly();
          break;
        }
      } else {  // if active position is implicit
        if (the_string[current_suffix_end_index] ==
            the_string[active.edge->from + active.length]) {
          // if the active point was implicit and next characters coincided
          bool active_point_was_updated = AddSuffixImplicitly();
          if (active_point_was_updated) {
            CreateSuffixLink(active.node);
          }
          break;
        } else {
          // if the next characters not coincided -> split the edge
          SplitEdge();

          // If visitor exists, let's perform a visit on edge split
          if (visitor) visitor->VisitOnSplit(active.edge->tail->depth,
                                             current_suffix_start_index);

          // Update active point after splitting
          UpdateActivePointAfterEdgeSplitting();
        }
      }
    }
  }
}

bool SuffixTree::AddSuffixImplicitly() {
  ++active.length;
  return NormalizeActivePoint();
}

// In some cases the active point length could lead outside
// the active edge. Then we have to correct the situation by
// chaging the active node and reducing the active length.
bool SuffixTree::NormalizeActivePoint() {
  size_t next_suffix_start_index = current_suffix_start_index + 1;
  bool active_node_was_updated = false;
  while (active.length > 0 && active.length > active.edge->length()) {
    active.length -= active.edge->length() + 1;
    active.node = active.edge->tail;
    active.edge = active.length > 0 ?
        &active.node->edges[
            the_string[next_suffix_start_index + active.node->depth]]
        : NULL;
    active_node_was_updated = true;
  }
  return active_node_was_updated;
}

// Create a suffix link from last created node in this iteration
// to the given node.
void SuffixTree::CreateSuffixLink(Node* node) {
  if (last_created_node_in_current_iteration &&
      last_created_node_in_current_iteration != active.node) {
    last_created_node_in_current_iteration->suffix_link = node;
  }
}

// Insert a new edge from explicit position
void SuffixTree::InsertEdge() {
  active.node->edges[current_suffix_last_char].Assign(current_suffix_end_index,
                                                      the_string.length(),
                                                      NULL);
  // Reassign active point according to RULE 3
  // No need to change the edge, it will stay NULL
  active.node = active.node->suffix_link &&
                active.node->depth > active.node->suffix_link->depth ?
      active.node->suffix_link : ROOT;
  --unresolved_suffixes;
}

// Split the edge from implicit position
void SuffixTree::SplitEdge() {
  // Create a split node
  nodes.push_back(Node(active.node->depth + active.length));
  Node* created_node = &*(nodes.end() - 1);

  // RULE 2: create a suffix link from previously added node in this iteration
  CreateSuffixLink(created_node);

  // Add an edge with previous substr from the position of not coincided char
  char not_coincided_char = the_string[active.edge->from + active.length];
  created_node->edges[not_coincided_char].Assign(active.edge->from +
                                                    active.length,
                                                 active.edge->to,
                                                 active.edge->tail);

  // Add an edge with new character on it
  created_node->edges[current_suffix_last_char].Assign(current_suffix_end_index,
                                                       the_string.length(),
                                                       NULL);

  // Repoint active edge and correct its 'to' index
  active.edge->tail = created_node;
  active.edge->to = active.edge->from + active.length - 1;

  // Make just created node the last created node in this iteration
  last_created_node_in_current_iteration = created_node;
}

void SuffixTree::UpdateActivePointAfterEdgeSplitting() {
  // Reassign active point according to RULE 1 and RULE 3
  if (active.node == ROOT) {
    // if active node is root (RULE 1)
    --active.length;
      size_t next_suffix_start_index = current_suffix_start_index + 1;
      active.edge = active.length > 0 ?
          &(active.node->edges[the_string[next_suffix_start_index]]) : NULL;
  } else {
    // if active node is not ROOT (RULES 3)
    active.node = active.node->suffix_link ? active.node->suffix_link : ROOT;
    size_t new_from = active.edge->from;
    active.edge = &active.node->edges[the_string[new_from]];
  }
  --unresolved_suffixes;

  // After all we don't forget to normalize the active point if any...
  // and to create a suffix link if the active node changes.
  bool active_point_was_updated = NormalizeActivePoint();
  if (active_point_was_updated) {
    CreateSuffixLink(active.node);
  }
}

int main() {
  string input_string;
  Input(&input_string);

  MaxLengthOfCommonPrefixesVisitor visitor(input_string);
  SuffixTree suffix_tree(input_string);
  suffix_tree.Build(&visitor);

  Output(visitor.get_common_prefixes_max_length());
  return 0;
}

