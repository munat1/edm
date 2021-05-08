#include "graph.h"
#include "heap.h"

struct HeapItem
{
    HeapItem(Graph::NodeId nodeid, double key): _nodeid(nodeid), _key(key) {}
    Graph::NodeId _nodeid;
    double _key;
};

bool operator<(const HeapItem & a, const HeapItem & b)
{
    return (a._key < b._key);
}


class NodeHeap : public Heap<HeapItem> {
public:
    NodeHeap(int num_nodes): _heap_node(num_nodes, not_in_heap)
    {   // creates a heap with all nodes having key = infinite weight
        for(auto i = 0; i < num_nodes; ++i) {
            insert(i, Graph::infinite_weight);
        }
    }

    bool is_member(Graph::NodeId nodeid) const
    {
        ensure_is_valid_nodeid(nodeid);
        return _heap_node[nodeid] != not_in_heap;
    }

    double get_key(Graph::NodeId nodeid)
    {
        return get_object(_heap_node[nodeid])._key;
    }

    Graph::NodeId extract_min()
    {
        Graph::NodeId result = Heap<HeapItem>::extract_min()._nodeid;
        _heap_node[result] = not_in_heap;
        return result;
    }

    void insert(Graph::NodeId nodeid, double key)
    {
          ensure_is_valid_nodeid(nodeid);
          HeapItem item(nodeid, key);
          _heap_node[nodeid] = Heap<HeapItem>::insert(item);
    }

    void decrease_key(Graph::NodeId nodeid, double new_key)
    {
        ensure_is_valid_nodeid(nodeid);
        get_object(_heap_node[nodeid])._key = new_key;
        Heap<HeapItem>::decrease_key(_heap_node[nodeid]);
    }

    void remove(Graph::NodeId nodeid)
    {
        ensure_is_valid_nodeid(nodeid);
        Heap<HeapItem>::remove(_heap_node[nodeid]);
        _heap_node[nodeid] = not_in_heap;
    }

private:

    void ensure_is_valid_nodeid(Graph::NodeId nodeid) const
    {
        if (nodeid < 0 or nodeid >= static_cast<int>(_heap_node.size()))
            throw std::runtime_error("invalid nodeid in NodeHeap");
    }

    void swap(HeapItem & a, HeapItem & b)
    {
        std::swap(a,b);
        std::swap(_heap_node[a._nodeid],_heap_node[b._nodeid]);
    }

    static const int not_in_heap;
    std::vector<int> _heap_node;
};

int const NodeHeap::not_in_heap = -1;


struct PrevData {
    Graph::NodeId id;
    double weight;
};


Graph mst(const Graph & g)
{   // Prim's Algorithm. Assumes that g is undirected and connected.
    Graph tree(g.num_nodes(), Graph::undirected);
    NodeHeap heap(g.num_nodes());
    std::vector<PrevData> prev(g.num_nodes(), {Graph::invalid_node, 0.0});

    const Graph::NodeId start_nodeid = 0;         // start at vertex 0
    heap.decrease_key(start_nodeid, 0);

    while (not heap.is_empty()) {
        Graph::NodeId nodeid = heap.extract_min();
        if (nodeid != start_nodeid) {
            tree.add_edge(prev[nodeid].id, nodeid, prev[nodeid].weight);
        }
        for (auto neighbor: g.get_node(nodeid).adjacent_nodes()) {
            if (heap.is_member(neighbor.id()) and
                neighbor.edge_weight() < heap.get_key(neighbor.id()))
            {
                prev[neighbor.id()] = {nodeid, neighbor.edge_weight()};
                heap.decrease_key(neighbor.id(), neighbor.edge_weight());
            }
        }
    }
    return tree;
}

Graph shortest_paths_tree(const Graph & g, Graph::NodeId start_nodeid, 
  std::vector<PrevData> &prev, std::vector<double> &paths)
{   // Dijkstra's Algorithm. The graph g can be directed or undirected.

    Graph tree(g.num_nodes(), g.dirtype);
    NodeHeap heap(g.num_nodes());
    //std::vector<PrevData> prev(g.num_nodes(), {Graph::invalid_node, 0.0});

    heap.decrease_key(start_nodeid, 0);

    while (not heap.is_empty()) {
        double key = heap.find_min()._key;
        if (key == Graph::infinite_weight) {
            break;                    // break exits the while loop immediately
        }
        Graph::NodeId nodeid = heap.extract_min();
        if (nodeid != start_nodeid) {
            tree.add_edge(prev[nodeid].id, nodeid, prev[nodeid].weight);
            paths[nodeid] = key;
        }
        for (auto neighbor: g.get_node(nodeid).adjacent_nodes()) {
            if (heap.is_member(neighbor.id()) and
                (key + neighbor.edge_weight() < heap.get_key(neighbor.id())))
            {
                paths[neighbor.id()] = key + neighbor.edge_weight();
                prev[neighbor.id()] = {nodeid, neighbor.edge_weight()};
                heap.decrease_key(neighbor.id(), key + neighbor.edge_weight());
            }
        }
    }
    return tree;
}

std::vector<Graph::NodeId> finde_kantenzug(Graph &g, Graph::NodeId start, 
  Graph::NodeId target, std::vector<PrevData> &prev)
{

  std::vector<Graph::NodeId> v={target};
  Graph::NodeId cur = target;
  while(prev[cur].id != -1){
    v.push_back(prev[cur].id);
    cur = prev[cur].id;
  }
  return v;
}

bool contains_edge(Graph &g, Graph::NodeId head, Graph::NodeId tail){
  bool contains = false; 
  for (auto i : g.get_edges())
  {
    if (i.start() == head && i.end() == tail)
    {
      contains = true;
    }
  }
  return contains;
}

Graph shallow_light_tree(Graph & g, Graph::NodeId start_nodeid, double eps)
{

  std::vector<double> paths(g.num_nodes(),0); //speichert die l Werte für jeden Knoten in G
  std::vector<double> paths_mst(g.num_nodes(),0); //speichert die l Werte für jeden Knoten in S

  std::vector<PrevData> prev_graph(g.num_nodes(), {Graph::invalid_node, 0.0}); //Wurde definiert, um die Wege außerhalb der Dijksta-Funktion zu bestimmen.
  std::vector<PrevData> prev_mst(g.num_nodes(), {Graph::invalid_node, 0.0}); //"

  Graph graph_shortest_path_tree = shortest_paths_tree(g,start_nodeid, prev_graph,paths); 
  Graph mst_shortest_path_tree = shortest_paths_tree(mst(g),start_nodeid, prev_mst, paths_mst);

  std::vector<bool> changed_mst(g.num_nodes(), false); //Bezeichnet, ob für einen Knoten der l-Wert durch kürzeste Wege Baum ersetzt wurde.
  std::vector<bool> changed_graph(g.num_nodes(), false); // "
  for (int i = 0; i < g.num_nodes(); ++i) //Für alle Knoten
  {
    if ((1+eps)*paths[i]<paths_mst[i] && changed_mst[i] == false) //Überprüfe, ob l-Wert in MST viel größer als l-Wert im kürzesten Wege Baum.
    {
      std::vector<Graph::NodeId> v = finde_kantenzug(mst_shortest_path_tree, 
        start_nodeid, i, prev_mst); //Finde den Weg [s,i] in MST.
      std::vector<Graph::NodeId> w = finde_kantenzug(graph_shortest_path_tree, 
        start_nodeid, i, prev_graph); //Finde den Weg [s,i] im kürzesten Wege Baum.
      for (int j = 1; j < v.size(); ++j)
      {
        if /*((changed_mst[v[j]] == false && changed_mst[v[j-1]] == false) || 
          (changed_mst[v[j]] == true && changed_mst[v[j-1]] == false) || 
          (changed_mst[v[j]] == false && changed_mst[v[j-1]] == true))*/
          (contains_edge(mst_shortest_path_tree, v[j], v[j-1]))
        {
            mst_shortest_path_tree.remove_edge(v[j], v[j-1]);
        }
        
      }
      for (int k = 1; k < w.size(); ++k)
      {
        if (!contains_edge(mst_shortest_path_tree, w[k-1], w[k]) &&
          !contains_edge(mst_shortest_path_tree, w[k], w[k-1]))
        {
          mst_shortest_path_tree.add_edge(w[k], w[k-1], 
            graph_shortest_path_tree.get_edge(w[k], w[k-1]).weight());
        }
      }
      for (int l = 0; l < v.size(); ++l)
      {
        changed_mst[v[l]] = true; 
        paths_mst[v[l]]=paths[v[l]];
      }
    }
  }
return mst_shortest_path_tree;
}

int main(int argc, char* argv[])
{	
	if (argc > 1) 
  {
      Graph g(argv[1], Graph::undirected);
      double eps;
      Graph::NodeId start;
      std::vector<PrevData> prev(g.num_nodes(), {Graph::invalid_node, 0.0}); 
      std::vector<double> paths(g.num_nodes(), 0);
      std::cout << "Geben Sie epsilon > 0 ein: "<< std::endl;
      std::cin >> eps;
      std::cout << "Geben Sie den Anfangsknoten ein: "<< std::endl;
      std::cin >> start;
      if (start < 0 || start > g.num_nodes() || eps<0)
      {
        std::cout << "Ungültiger Anfangsknoten oder epsilon < 0" << std::endl;
      }
      else{
        try{

          Graph a = shallow_light_tree(g,start,eps);
          std::cout << "shallow_light_tree" << std::endl;
          for(auto i : a.get_edges()){
            std::cout << i.start() << " " << i.end() << std::endl;
          }
        }
      catch(const char* e)
      {
        std::cout << e << std::endl;
      }
}
	}
}