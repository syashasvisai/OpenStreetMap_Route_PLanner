#include "route_planner.h"
#include <algorithm>

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    // Convert inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

    // TODO 2: Use the m_Model.FindClosestNode method to find the closest nodes to the starting and ending coordinates.
    // Store the nodes you find in the RoutePlanner's start_node and end_node attributes.
  
  
// QUESTION: Since start_node and end_node are already defined in route_model, you can use them using the 'this' operator or simply by just calling them. What then is the use of 'this' operator? 
  
  start_node = &m_Model.FindClosestNode(start_x, start_y);
  end_node = &m_Model.FindClosestNode(end_x,end_y);
  
}


// TODO 3: Implement the CalculateHValue method.
// Tips:
// - You can use the distance to the end_node for the h value.
// - Node objects have a distance method to determine the distance to another node.

float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
  return node->distance(*end_node); // dereferencing the pointer end_node
 
}


// TODO 4: Complete the AddNeighbors method to expand the current node by adding all unvisited neighbors to the open list.
// Tips:
// - Use the FindNeighbors() method of the current_node to populate current_node.neighbors vector with all the neighbors.
// - For each node in current_node.neighbors, set the parent, the h_value, the g_value. 
// - Use CalculateHValue below to implement the h-Value calculation.
// - For each node in current_node.neighbors, add the neighbor to open_list and set the node's visited attribute to true.

void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
  current_node -> FindNeighbors();
  for(auto neighbor_node : current_node->neighbors){
    neighbor_node->parent = current_node; 
   
// Is this even right? I am just setting the current_node as parent. It could be, since we are searching for the neighbors of the current_node, the current_node becomes the parent. And then we calculate the h_value and g_value from this node. . And CalculateHValue will give the heuristic between the current node and neighbor_node. (I think)
    
 
    
 // QUESTION:g_value will be the number of steps taken by the current node plus the distance between the neighbor node and the current node. But distance between the current_node and neighbor_node is always 1, right? Then why does the code not work with 'neighbor_node->g_value = current ->g_value + 1' 
    
    
    neighbor_node->h_value = CalculateHValue(neighbor_node);
    neighbor_node->g_value = current_node->g_value + current_node->distance(*neighbor_node);
    open_list.emplace_back(neighbor_node);
    neighbor_node->visited = true;
    }

}


// TODO 5: Complete the NextNode method to sort the open list and return the next node.
// Tips:
// - Sort the open_list according to the sum of the h value and g value.
// - Create a pointer to the node in the list with the lowest sum.
// - Remove that node from the open_list.
// - Return the pointer.


// Is there another way to do this? I think there is, else they would have included a function definition. Maybe I should use maps and dictionaries? 
/*bool Compare(const RouteModel::Node *node_1, const RouteModel::Node *node_2) {
  float f1 = node_1->g_value + node_1->h_value; // f1 = g1 + h1
  float f2 = node_2->g_value + node_2->h_value; // f2 = g2 + h2
  return f1 > f2; 
}*/

RouteModel::Node *RoutePlanner::NextNode() {
  
  // sort the list using Compare
  //std::sort(open_list.begin(),open_list.end(),Compare);
  

  // Any other way without using Compare()? Use lambda expression
  sort(open_list.begin(), open_list.end(), [](const RouteModel::Node* node_1, 
                                const RouteModel::Node* node_2)
    {
       return (node_1->g_value + node_1->h_value) > (node_2->g_value + node_2->h_value);
    });


  
  RouteModel::Node *lowest_sum = open_list.back();
  open_list.pop_back();
  return lowest_sum;
}


// TODO 6: Complete the ConstructFinalPath method to return the final path found from your A* search.
// Tips:
// - This method should take the current (final) node as an argument and iteratively follow the 
//   chain of parents of nodes until the starting node is found.
// - For each node in the chain, add the distance from the node to its parent to the distance variable.
// - The returned vector should be in the correct order: the start node should be the first element
//   of the vector, the end node should be the last element.

std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    // Create path_found vector
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;

    // TODO: Implement your solution here.
  // while the parent is not a nullptr perform all the steps and then also add the final parent to the path_found
  while(current_node->parent){
    path_found.emplace_back(*current_node);
    distance += current_node->distance(*current_node->parent); // distance between current node and the parent of next node
    current_node = current_node->parent; // setting the parent_node to the current node
  }
  path_found.emplace_back(*current_node); // to save the last(first) node
  reverse(path_found.begin(), path_found.end());
  
    distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
    return path_found;

}


// TODO 7: Write the A* Search algorithm here.
// Tips:
// - Use the AddNeighbors method to add all of the neighbors of the current node to the open_list.
// - Use the NextNode() method to sort the open_list and return the next node.
// - When the search has reached the end_node, use the ConstructFinalPath method to return the final path that was found.
// - Store the final path in the m_Model.path attribute before the method exits. This path will then be displayed on the map tile.

void RoutePlanner::AStarSearch() {
  RouteModel::Node *current_node = nullptr;
  //initialize the open_list
  current_node = start_node; 
  current_node->visited = true;
  open_list.emplace_back(current_node);
  
  while(open_list.size()>0){
    current_node = NextNode();
    if(current_node == end_node){
      m_Model.path = ConstructFinalPath(end_node);
      break;
    }
   else
     AddNeighbors(current_node);
  }

}