/******************************************************************************
 * Suffix tree implementation
 * Copyright 2013, Maruan Al-Shedivat
 ******************************************************************************/
#include <iostream>
#include <string>
#include <vector>

#include "suffix_tree.h"

using std::string;
using std::vector;
using std::cin;
using std::cout;
using std::endl;

namespace suffixtree {

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
 * Also, if we change the active node during 'active node normalization process'
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
 *       question http://stackoverflow.com/questions/9452701/, and then a few
 *       bugs and logic mistakes were fixed up.
 ******************************************************************************/
// Just replace all the characters with their number in the alphabet
// and add '$' to the end of the string
void SuffixTree::CanonizeCharacters() {
  for (size_t pos = 0; pos < the_string.length(); ++pos)
    the_string[pos] -= FIRST_ALPHABET_CHARACTER;  // conver character into ind
  the_string += SENTINEL_SIGN;
}

void SuffixTree::Build() {
  // We know that suffix tree will contain n nodes in total without leafs.
  // And we have to prevent the reallocations! Otherwise it will kill all
  // the pointers to the Nodes. So I chose to reserve 2*n memory spase.
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
          // Insert a new edge, there is no such
          InsertEdge();
        } else {
          // If such edge exists we make it active and add suffix implicitly
          active.edge = &active.node->edges[current_suffix_last_char];
          AddSuffixImplicitly();
          break;
        }
      } else {  // if the active position is implicit
        if (the_string[current_suffix_end_index] ==
            the_string[active.edge->from + active.length]) {
          // if the active point was implicit and next characters coincided
          bool active_point_was_updated = AddSuffixImplicitly();
          if (active_point_was_updated)
            CreateSuffixLink(active.node);
          break;
        } else {
          // if the next characters not coincided -> split the edge
          SplitEdge();

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
      last_created_node_in_current_iteration != active.node)
    last_created_node_in_current_iteration->suffix_link = node;
}

// Insert a new edge from an explicit position
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
  if (active_point_was_updated)
    CreateSuffixLink(active.node);
}

int SuffixTree::Match(string pattern) const {
  // Canonize the pattern
  for (size_t i = 0; i < pattern.size(); ++i)
    pattern[i] -= FIRST_ALPHABET_CHARACTER;

  Node* current_node = ROOT;
  Edge* current_edge = NULL;
  char edge_ind = 0;
  int pos_in_string = 0;
  for (size_t i = 0; i < pattern.size(); ++i) {
    // if we were in an explicit node
    if (!current_edge)
      if (current_node->edges[pattern[i]].exists)
        current_edge = &current_node->edges[pattern[i]];
      else
        return -1;

    // if we are in an implicit node
    if (pattern[i] == the_string[current_edge->from + edge_ind])
      ++edge_ind;
    else
      return -1;

    pos_in_string = current_edge->from + edge_ind;

    // check if we reached the next explicit node
    if (current_edge->from + edge_ind > current_edge->to) {
      current_node = current_edge->tail;
      current_edge = NULL;
      edge_ind = 0;
    }
  }
  
  return pos_in_string - pattern.size();
}

}  // namespace suffixtree

