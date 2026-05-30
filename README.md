# Parser-Tree

The goal of this project is to implement two containers: `trie<T>` (the main container) and `bag<T>` (the support container). To construct the `trie<T>` container, a file with a specific format is parsed using a context-free grammar.

Below are the details regarding the `trie<T>` data type, the parsing format, and the supported operations.

## 1. Definitions

The `trie<T>` container is implemented internally as a *tree*.

In computer science, a tree is a set of *nodes* and *edges*. Edges connect nodes together. Recursively, a tree is defined as:

* A node called "parent", `p`;
* It has 0 or more "children";
* Children are themselves trees;
* For every child `c`, there is an edge `(p,c)` connecting parent `p` to child `c`.

There is always a node without a parent called the *root*; nodes without children are called *leaves*.

The `trie<T>` container implements a tree where edges are labeled with a value of type `T` and leaves are labeled with a `double` value.

A `trie<T>` corresponds to a set of sequences of type `T` where each sequence has a *weight*. There is one sequence for every leaf in the tree. The weight is the `double` value stored in the corresponding leaf.

A sequence is logically obtained by *concatenating* the labels on each edge encountered along the path from the root to the leaf. For example, the `trie<char>` in Figure 1 represents the following set:

```
{(a,3.1), (bb,2.9), (bcb,7.0)}

```

## 2. Syntax of the File Format

To be well-defined, the type `T` must satisfy the following requirements:

* It must define `operator<`, `operator==`, `operator<<`, and `operator>>`. The `operator+` is also required for path compression.


* Objects of type `T`, when written to a `std::ostream` (via `operator<<`), must not contain separators (spaces, tabs, newlines, etc.).



The base case of `trie<T>` is a leaf, represented as:

```
x children = {}

```

where `x` is a `double`. An internal node is represented as:

```
children = { x1 trie1, x2 trie2, ... }

```

where `children` lists the nodes; `x1`, `x2`, ... are *distinct* values of type `T` labeling the incoming edges, and `trie1`, `trie2` are the children nodes.

The set corresponding to the trie in Figure 2 is `{(<2.71,1.61,9.8>,6.1), (<3.14>,17.0), (<6.67>,4.3)}`.

The set corresponding to the trie in Figure 3 is `{(<compilers,g++>,2.8), (<compilers,javac>,3.1), (<languages,c++>,1.1), (<languages,java>,0.5)}`.

**Stream Operators**
The external methods `operator<<` and `operator>>` allow writing to and reading from `std::ostream` and `std::istream` respectively, utilizing the context-free grammar parser.

## 3. Container `trie<T>`

The container structure is defined as follows:

```cpp
template <typename T>
struct trie {
    /* methods and iterators */
private:
    trie<T>* m_p;      // parent
    T* m_l;            // label
    bag<trie<T>> m_c;  // children
    double m_w;        // weight
};

```

The `bag` container must store child nodes sorted by their labels in ascending order using `operator<` on type `T`.

### 3.1. Default Constructor and Destructor

Constructs a trie corresponding to the file `0.0 children = {}`.

### 3.2. Conversion Constructor from double

Constructs a leaf with weight `w`.

### 3.3. Copy/Move Semantics

Supports full copy and move constructors and assignment operators. These ensure that the trie remains in a consistent state, specifically ensuring that parent pointers are correctly updated when sub-tries are moved or copied.

### 3.4. Setters/Getters

Methods such as `set_weight`, `get_weight`, `set_label`, `get_label`, `set_parent`, `get_parent`, `get_children`, and `add_child` provide the interface for constructing and traversing the tree during parsing.

### 3.5. Comparison

`operator==` and `operator!=` compare two tries, verifying that all nodes, edges, and child orderings are identical.

### 3.6. Prefix Search (`operator[]`)

Returns a reference to the sub-tree reached by following the longest possible prefix match from the provided `std::vector<T>`.

### 3.7. Iterators

Two forward iterators are provided:

* **`node_iterator`**: Navigates from a node towards the root using `operator++`.


* **`leaf_iterator`**: Navigates through leaves in lexicographical order. It supports conversion to `node_iterator`.



### 3.8. Maximum Weight Leaf

`max()` returns a reference to the leaf containing the maximum weight, allowing search within a specific prefix using `t[s].max()`.

### 3.9. Union

`operator+` and `operator+=` implement set union logic. If sequences overlap, weights are summed; if one sequence is a prefix of another, the weight of the prefix sequence is added to the longer sequence.

### 3.10. Path Compression

`path_compress()` merges unary paths (chains of nodes with a single child) into a single edge. The new edge label is the result of applying `operator+` on the labels along the path.
