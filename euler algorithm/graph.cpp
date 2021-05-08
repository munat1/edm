// graph.cpp (Implementation of Class Graph)

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <limits>
#include "graph.h"
#include <iostream>

const Graph::NodeId Graph::invalid_node = -1;
const double Graph::infinite_weight = std::numeric_limits<double>::max();


void Graph::add_nodes(NodeId num_new_nodes)
{
   _nodes.resize(num_nodes() + num_new_nodes);
}
//neighbor constructor
Graph::Neighbor::Neighbor(Graph::NodeId n, double w): _id(n), _edge_weight(w) {}

//edge constructors
Graph::Edge::Edge(Graph::NodeId a, Graph::NodeId b, double c, Graph::DirType d):
    _start(a), _end(b), _weight(c), _dirtype(d) {}

Graph::Edge::Edge() :
  _start(Graph::invalid_node), _end(Graph::invalid_node), 
    _weight(Graph::infinite_weight),  _dirtype(Graph::DirType::undirected) {}

Graph::NodeId Graph::Edge::start() const {
    return _start;
}
Graph::NodeId Graph::Edge::end() const {
    return _end;
}
double Graph::Edge::weight() const {
    return _weight;
}
Graph::DirType Graph::Edge::dirtype() const {
    return _dirtype;
}
void Graph::Edge::print() const {
    std::cout << start() << (_dirtype == Graph::DirType::undirected ? "<->" : "->")
     << end() << std::endl;
} 
  
Graph::Node::Node(){
    _data.resize(2);
}
Graph::Node::~Node(){}
Graph::Graph(NodeId num, DirType dtype): dirtype(dtype) {
    _nodes.resize(num);
}

void Graph::add_edge(Graph::Edge e){
    add_edge(e.start(), e.end(), e.weight());
}

void Graph::add_edge(NodeId tail, NodeId head, double weight)
{
   if (tail >= num_nodes() or tail < 0 or head >= num_nodes() or head < 0) {
       throw std::runtime_error("Edge cannot be added due to undefined endpoint.");
   }
   _nodes[tail].add_neighbor(head, weight);
   if (dirtype == Graph::undirected) {
        _nodes[head].add_neighbor(tail, weight);
   }
}


void Graph::Node::add_neighbor(Graph::NodeId nodeid, double weight)
{
   _neighbors.push_back(Graph::Neighbor(nodeid, weight));
}

void Graph::remove_edge(NodeId tail, NodeId head){
 if (tail >= num_nodes() or tail < 0 or head >= num_nodes() or head < 0) {
       throw std::runtime_error("Edge cannot be removed due to undefined endpoint.");
   }
   _nodes[tail].remove_neighbor(head);
   if (dirtype == Graph::undirected) {
        _nodes[head].remove_neighbor(tail);
   }
    
}

Graph::Neighbor Graph::get_neighbor(Graph::NodeId a, Graph::NodeId b){
    for(Graph::Neighbor n : get_node(a).adjacent_nodes()){
        if(n.id() == b){
            return n;
        }
    }
    throw std::runtime_error("No such edge!");
}
Graph::Edge Graph::get_edge(Graph::NodeId a, Graph::NodeId b){
    Graph::Neighbor n = get_neighbor(a,b);
    Edge e(a, b, n.edge_weight(), dirtype);
    return e;
}

std::vector<Graph::Edge> Graph::get_edges(){
    std::vector<Graph::Edge> edges;
    for(int i = 0; i < num_nodes(); i++){
        for(Graph::Neighbor n : get_node(i).adjacent_nodes()){
      if(dirtype == Graph::DirType::undirected && i > n.id()){
                continue;
            }
            Edge e(i, n.id(), n.edge_weight(), dirtype);
            edges.push_back(e);
        }
    }
    return edges;
}

void Graph::Node::remove_neighbor(Graph::NodeId nodeid){

    _neighbors.erase(
        std::remove_if(_neighbors.begin(), _neighbors.end(),
                       [nodeid](Graph::Neighbor n)
                       {
                           return n.id() == nodeid;
                       }));
}
const std::vector<double> Graph::Node::data() const
{
    return _data;
}
void Graph::Node::set_data(int index, double value)
{
    _data[index] = value;
}


const std::vector<Graph::Neighbor> & Graph::Node::adjacent_nodes() const
{
   return _neighbors;
}

Graph::NodeId Graph::num_nodes() const
{
   return _nodes.size();
}

const Graph::Node & Graph::get_node(NodeId node) const
{
   if (node < 0 or node >= static_cast<int>(_nodes.size())) {
        throw std::runtime_error("Invalid nodeid in Graph::get_node.");
   }
   return _nodes[node];
}

Graph::NodeId Graph::Neighbor::id() const
{
   return _id;
}

double Graph::Neighbor::edge_weight() const
{
   return _edge_weight;
}

void Graph::print() const
{
   if (dirtype == Graph::directed) {
        std::cout << "Digraph ";
   } else {
        std::cout << "Undirected graph ";
   }
   std::cout << "with " << num_nodes() << " vertices, numbered 0,...,"
              << num_nodes() - 1 << ".\n";

   for (auto nodeid = 0; nodeid < num_nodes(); ++nodeid) {
        std::cout << "The following edges are ";
        if (dirtype == Graph::directed) {
            std::cout << "leaving";
        } else {
            std::cout << "incident to";
        }
        std::cout << " vertex " << nodeid << ":\n";
        for (auto neighbor: _nodes[nodeid].adjacent_nodes()) {
            std::cout << nodeid << " - " << neighbor.id()
                      << " weight = " << neighbor.edge_weight() << "\n";
        }
   }
}

Graph Graph::transform_edge_weights( double (*transform)(double)){
    Graph g(num_nodes(), dirtype);    
    for(int i = 0; i < num_nodes(); i++){
        for(Neighbor m : get_node(i).adjacent_nodes()){
            if(dirtype == DirType::undirected && m.id() > i){
                continue;            
            }else{
                double new_weight =         
                  transform(m.edge_weight());
    g.add_edge(i, m.id(), new_weight);            
            }
        }
    }
    return g;
}


void Graph::read_from_file(char const * filename, DirType dtype){
   std::ifstream file(filename);                             // open file
   if (not file) {
        throw std::runtime_error("Cannot open file.");
   }

   Graph::NodeId num = 0;
   std::string line;
   std::getline(file, line);                 // get first line of file
   std::stringstream ss(line);               // convert line to a stringstream
   ss >> num;                                // for which we can use >>
   if (not ss) {
        throw std::runtime_error("Invalid file format.");
   }
   add_nodes(num);

   int linenum = 0;
   while (std::getline(file, line)) {
        std::stringstream ss(line);
        Graph::NodeId head, tail;
        ss >> tail >> head;
        if (not ss) {
            throw std::runtime_error("Invalid file format at line " + std::to_string(linenum));
        }
        double weight = 1.0;
        ss >> weight;
        if (tail != head) {
            add_edge(tail, head, weight);
        }
        else {
            throw std::runtime_error("Invalid file format: loops not allowed.");
        }
    linenum++;
   }
}

Graph::Graph(char const * filename, DirType dtype): dirtype(dtype)
{
   std::ifstream file(filename);                             // open file
   if (not file) {
        throw std::runtime_error("Cannot open file.");
   }

   Graph::NodeId num = 0;
   std::string line;
   std::getline(file, line);                 // get first line of file
   std::stringstream ss(line);               // convert line to a stringstream
   ss >> num;                                // for which we can use >>
   if (not ss) {
        throw std::runtime_error("Invalid file format.");
   }
   add_nodes(num);

   while (std::getline(file, line)) {
        std::stringstream ss(line);
        Graph::NodeId head, tail;
        ss >> tail >> head;
        if (not ss) {
            throw std::runtime_error("Invalid file format.");
        }
        double weight = 1.0;
        ss >> weight;
        if (tail != head) {
            add_edge(tail, head, weight);
        }
        else {
            throw std::runtime_error("Invalid file format: loops not allowed.");
        }
   }
}
Graph::~Graph(){}