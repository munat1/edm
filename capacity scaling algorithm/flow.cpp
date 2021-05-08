//flow.cpp
#include "flow.h"
#include <vector>
#include <limits>
#include <stdexcept>
#include <algorithm>

Network::Network(NodeId num_nodes) : Graph(num_nodes, Graph::DirType::directed){
}
Network::Network(const char* filename) : Graph(filename, Graph::DirType::directed) {
}
// Setzt die Kapazität einer Kante
void Network::set_capacity(NodeId a, NodeId b, double c){
    //Falls eine Kante bereits existiert, so wird diese entfernt
    if(get_capacity(a,b) != 0){
        remove_edge(a,b);
    }
    //Erstellt eine neue Kante und fügt diese hinzu
    Graph::Edge e(a, b, c);
    add_edge(e);
}

void Flow::relabel(NodeId v, std::vector<int> &psi, Graph * residual){
    int psi_val = num_nodes()*2-1;
    for (auto n : residual->get_node(v).adjacent_nodes()){
        if (psi_val > psi[n.id()])
        {
            psi_val = psi[n.id()];
        }
    }
    psi[v] = psi_val+1;
}

void Flow::push(NodeId s, NodeId t, Graph * residual, std::vector<double> & excess_values){
    //double res_capacity = residual->get_edge(s, t).weight();
    double res_capacity=0; //= residual->get_edge(s, t).weight();
    //da es parallele Kanten existieren, Residualkapazität ist die Summe aller Kapazitäten der Kanten.
    for (auto n : residual->get_node(s).adjacent_nodes())
    {
        if (n.id()==t)
        {
            res_capacity+=n.edge_weight();
        }
    }
    double gamma = std::min(excess_values[s], res_capacity);
    //aktualisiere den Residualgraphen
    residual->add_edge(t,s,gamma);
    residual->remove_edge(s,t);
    if (res_capacity>gamma)
    {
        residual->add_edge(s,t,res_capacity-gamma);
    }
    excess_values[s] -= gamma;
    excess_values[t] +=gamma;
    set_usage(s, t, gamma);
}

Flow* Network::push_relabel(NodeId s, NodeId t){
    Flow* f = new Flow(this, s, t);
    //1&2
    std::vector<int> psi(num_nodes(),0); //init psi
    std::vector<double> excess_values(num_nodes(),0);
    psi[0] = num_nodes(); //init psi(s)
    Graph * residual = f->residual_graph();
    for (auto n : get_node(s).adjacent_nodes())
    {
        double capacity = get_capacity(s,n.id());
        f->set_usage(s,n.id(), capacity); //all neighbors of s get the flow as much as capacity
        excess_values[n.id()] = capacity;
        excess_values[s] -= capacity;
    }
    residual = f->residual_graph();
    std::vector<NodeId> aktive_knoten;  //vector of active nodes

    for (int i = 0; i < num_nodes(); ++i) 
    {
        if (excess_values[i]>0 && i != t) //activity of all nodes are significant except t.
        {
            aktive_knoten.push_back(i);
        }
    }
    //3
    while(!aktive_knoten.empty()){ //while still active nodes
        
        NodeId cur = aktive_knoten.back(); //choose an active node (ideally with the biggest psi value.) 
        for (int i = 0; i < aktive_knoten.size(); ++i)
        {
            if (psi[cur]<psi[aktive_knoten[i]])
            {
                cur = aktive_knoten[i];
            }
        }
        NodeId zulaessig = cur; //set zulaessig to itself. right now its not allowed. because psi(cur) != psi(cur)+1
        for (auto n : residual->get_node(cur).adjacent_nodes()) //check für zulässige Knoten im Residualgraphen.
        {   
            if (psi[cur] == psi[n.id()]+1)  //if there is one
            {
                zulaessig = n.id(); //add it to zulassig
            }
        }
        if (zulaessig != cur) //&& f->get_usage(cur,zulaessig)<f->residual_graph()->get_edge(cur,zulaessig).weight())   //if zulassig is not equal cur then we found a zulassige node
        {
            f->push(cur, zulaessig, residual, excess_values); //push that zulassige node
            //if (f->get_excess(cur)==0)
            if (excess_values[cur]==0)
            {
                auto iter = std::find(begin(aktive_knoten), end(aktive_knoten), cur);
                aktive_knoten.erase(iter); //refresh activeness of cur
            }

            //if ((f->get_excess(zulaessig)>0)  && (zulaessig!=t)) //&& !(std::find(aktive_knoten.begin(),aktive_knoten.end(), zulaessig)==aktive_knoten.end()))
            if ((excess_values[zulaessig]>0)  && (zulaessig!=t))
            {
                aktive_knoten.push_back(zulaessig); //after push, the pushed end can be activated, check for that
            }
            sort( aktive_knoten.begin(), aktive_knoten.end());//kill duplicates in vector
            aktive_knoten.erase( unique( aktive_knoten.begin(), aktive_knoten.end()), aktive_knoten.end() );
        }
        else if (zulaessig == cur && cur != t) {
            f->relabel(cur, psi, residual);   //if we couldnt find any relabel cur, dont relabel t.
        }
    }
    delete residual;
    return f;
}

double Network::get_capacity(NodeId a, NodeId b){
    try{
        double capacity = 0;
        for (auto i : get_node(a).adjacent_nodes())
        {
            if (i.id()==b)
            {
                capacity+=i.edge_weight();
            }
        }
        return capacity;
    }catch(...){}
    return 0;
}
Flow::Flow(Network* n, NodeId s, NodeId t): Graph(n->num_nodes(), Graph::DirType::directed), _network(n), _s(s), _t(t) {}

void Flow::set_usage(NodeId a, NodeId b, double u){
    Graph::Edge e(a,b,u); //add its flow.
    add_edge(e);
}

double Flow::value(){
    return -get_excess(_s);
}

double Flow::get_excess(Graph::NodeId a){
    double eingehender_fluss = 0;
    double ausgehender_fluss = 0;
    for (Edge e : get_edges())
    {
        if (e.end()==a)
        {
            eingehender_fluss+=get_usage(e.start(), e.end());
        }
    }
    for (auto n : get_node(a).adjacent_nodes())
    {

        ausgehender_fluss += n.edge_weight();
    }
    return eingehender_fluss-ausgehender_fluss;
}
double Flow::get_usage(NodeId a, NodeId b){
        try{
        double usage = 0;
        for (auto i : get_node(a).adjacent_nodes())
        {
            if (i.id()==b)
            {
                usage=i.edge_weight();
            }
        }
        return usage;
    }catch(...){}
    return 0;
}


Graph* Flow::residual_graph(){
    Graph* g = new Graph(num_nodes(), Graph::DirType::directed);
    for(Graph::Edge e : _network->get_edges()){
        double f_max = e.weight();
        double f_use = get_usage(e.start(), e.end());
        
        if(f_use != 0){
            Graph::Edge back(e.end(), e.start(), f_use);
            g->add_edge(back);
        }
        if(f_max > f_use){
            Edge remaining(e.start(), e.end(), f_max - f_use);
            g->add_edge(remaining);
        }
    }
    return g;
}

void Flow::print(){
    std::cout << value() << std::endl;
    for (auto e : _network->get_edges())
    {
        if (get_usage(e.start(), e.end())!=0)
        {
            std::cout << e.start() << " " << e.end() << std::endl;
        }
    }
}

std::vector<Graph::Edge> Flow::get_edges(){
    // Ermöglicht Zugriff auf die Kanten des zugrundeliegenden Graphen. Auf die anderen Graphen-Funktionen kann nicht zugegriffen werden.
    return Graph::get_edges();
}
