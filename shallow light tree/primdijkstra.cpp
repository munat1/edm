// primdijkstra.cpp (Prim's Algorithm and Dijkstra's Algorithm)

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


Graph shortest_paths_tree(const Graph & g, Graph::NodeId start_nodeid)
{   // Dijkstra's Algorithm. The graph g can be directed or undirected.
    Graph tree(g.num_nodes(), g.dirtype);
    NodeHeap heap(g.num_nodes());
    std::vector<PrevData> prev(g.num_nodes(), {Graph::invalid_node, 0.0});

    heap.decrease_key(start_nodeid, 0);

    while (not heap.is_empty()) {
        double key = heap.find_min()._key;
        if (key == Graph::infinite_weight) {
            break;                    // break exits the while loop immediately
        }
        Graph::NodeId nodeid = heap.extract_min();
        if (nodeid != start_nodeid) {
            tree.add_edge(prev[nodeid].id, nodeid, prev[nodeid].weight);
        }
        for (auto neighbor: g.get_node(nodeid).adjacent_nodes()) {
            if (heap.is_member(neighbor.id()) and
                (key + neighbor.edge_weight() < heap.get_key(neighbor.id())))
            {
                prev[neighbor.id()] = {nodeid, neighbor.edge_weight()};
                heap.decrease_key(neighbor.id(), key + neighbor.edge_weight());
            }
        }
    }
    return tree;
}


int main(int argc, char * argv[])
{
    if (argc > 1) {
        Graph g(argv[1], Graph::undirected);
        std::cout << "The following is the undirected input graph:\n";
        g.print();

        std::cout << "\nThe following is a minimum weight spanning tree:\n";
        Graph t = mst(g);
        t.print();

        Graph h(argv[1], Graph::directed);
        std::cout << "\nThe following is the directed input graph:\n";
        h.print();

        std::cout << "\nThe following is a shortest paths tree:\n";
        Graph u = shortest_paths_tree(h, 0);
        u.print();
    }
}
