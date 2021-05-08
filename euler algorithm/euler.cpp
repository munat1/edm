#include "graph.h"

void euler(Graph &g, Graph::NodeId start, std::vector<Graph::NodeId> &tour)
{
  Graph::NodeId cur = start;
  std::vector<Graph::NodeId> kantenzug;
  while (!g.get_node(cur).adjacent_nodes().empty())
  {
    Graph::NodeId neighbor = g.get_node(cur).adjacent_nodes().back().id();
    g.remove_edge(cur, neighbor);
    kantenzug.push_back(neighbor);
    cur = neighbor;
  }
  if ((not kantenzug.empty()) &&(start!=kantenzug.back()))
  {
    throw "Der Graph ist nicht eulersch, weil es existiert Kanten mit ungeradem Grad.";
  }
  for (size_t i = 0; i < kantenzug.size(); i++)
    {
      tour.push_back(kantenzug[i]);
      euler(g, kantenzug[i], tour);
    }
}

int main(int argc, char* argv[])
{	
	if (argc > 1) 
  {
    try
    {
      Graph g(argv[1], Graph::directed);
      Graph::NodeId start = 0;

      while(start < g.num_nodes() && g.get_node(start).adjacent_nodes().empty())
      {
        start++;
      }
      if(start == g.num_nodes())
      {
        start = 0;
      }

    	std::vector<Graph::NodeId> main_euler={start};
      euler(g, start, main_euler);
      for (int i = 0; i < g.num_nodes(); ++i)
      {
        if (!g.get_node(i).adjacent_nodes().empty())
        {
          throw "Der Graph ist nicht eulersch, da er nicht stark zusammenhängend ist.";
        }
      }
      for (Graph::NodeId i : main_euler)
      {
        std::cout << i << std::endl;
      }
    }
    catch(const char* e)
    {
      std::cout << e << std::endl;
    }
	}
}