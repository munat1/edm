//flow.h
#ifndef FLOW_H
#define FLOW_H

#include <vector>
#include "graph.h" 

class Flow;

class Network : public Graph {
    public:
            Network(NodeId num_nodes);
            Network(const char* filename);
            void set_capacity(NodeId a, NodeId b, double c);
            double get_capacity(NodeId a, NodeId b);
            Flow* push_relabel(NodeId s, NodeId t);

    private:
};
class Flow : private Graph {
    public:
        Flow(Network* n, NodeId s, NodeId t);
        void set_usage(NodeId a, NodeId b, double u);
        double get_usage(NodeId a, NodeId b);
        Graph* residual_graph();
        double value();
        void print();
        void push(NodeId s, NodeId t, Graph * residual, std::vector<double> & excess_values);
        void relabel(NodeId v, std::vector<int> &psi, Graph * residual);
        double get_excess(Graph::NodeId a);
        double get_excess_with_print(Graph::NodeId a);
        double eingehender_fluss(Graph::NodeId a);
        double ausgehender_fluss(Graph::NodeId a);
        std::vector<Graph::Edge> get_edges();
    private:
        Network* _network;
        NodeId _s;
        NodeId _t;
};
#endif
