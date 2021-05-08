// graph.h (Declaration of Class Graph)
#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>

class Graph {
public:
  using NodeId = int;  // vertices are numbered 0,...,num_nodes()-1
  enum DirType {directed, undirected};  // enum defines a type with possible values
  class Neighbor {
  public:
        Neighbor(Graph::NodeId n, double w); 
        double edge_weight() const;
        Graph::NodeId id() const;
  private:
        Graph::NodeId _id;
        double _edge_weight;
    };

  class Edge {
  public:
        Edge();
        Edge(Graph::NodeId start, Graph::NodeId end, 
          double weight = 1, Graph::DirType dirtype = Graph::DirType::directed);
        Graph::NodeId start() const;
        Graph::NodeId end() const;
        Graph::DirType dirtype() const;
        void print() const;
        double weight() const;
        inline bool operator== (const Edge & e){
            if(_dirtype == Graph::DirType::undirected || 
              e.dirtype() == Graph::DirType::undirected){
                return (start() == e.start() && end() == e.end())
                        || (start() == e.end() && end() == e.start());
            }else{
                return (start() == e.start() && end() == e.end());
            }
        }
        inline bool operator< (const Edge & e){
            if((_dirtype == Graph::DirType::undirected) ||
                (e.dirtype() == Graph::DirType::undirected)){
                if(std::min(start(), end()) == std::min(e.start(), e.end())){
                    return std::max(start(), end()) < std::max(e.start(), e.end()); 
                }
                return std::min(start(), end()) < std::min(e.start(), e.end());
            }else{
                if(start() == e.start()){
                    return end() < e.end();
                }
                    return start() < e.start();
            }
        }
  private:
        Graph::NodeId _start;
        Graph::NodeId _end;
        double _weight;
        Graph::DirType _dirtype;
    };

  class Node {
  public:
    Node();
    ~Node();
        void add_neighbor(Graph::NodeId nodeid, double weight);
        void remove_neighbor(Graph::NodeId nodeid);
        const std::vector<Neighbor> & adjacent_nodes() const;
        const std::vector<double> data() const;
        void set_data(int index, double value);
  private:
        std::vector<Neighbor> _neighbors;
        std::vector<double> _data;
    };
    
  Graph(NodeId num_nodes, DirType dirtype);
  Graph(char const* filename, DirType dirtype);
  Graph(char const * filename, DirType dtype, char const * data_x,  char const * data_y);
  Graph(const Graph & g);
  ~Graph(); 

  void add_nodes(NodeId num_new_nodes);
  void add_edge(NodeId tail, NodeId head, double weight = 1.0);
  void add_edge(Edge e);

  void remove_edge(NodeId tail, NodeId head);

  Edge get_edge(NodeId a, NodeId b);
  Neighbor get_neighbor(NodeId a, NodeId b);
  
  std::vector<Edge> get_edges();

  NodeId num_nodes() const;
  const Node & get_node(NodeId) const;
  void print() const;

  Graph transform_edge_weights( double (*transform)(double));

  const DirType dirtype;
  static const NodeId invalid_node;
  static const double infinite_weight;

private:
  std::vector<Node> _nodes;
  void read_from_file(char const * filename, DirType dtype);
};

#endif // GRAPH_H
