
#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <map>
#include <unordered_map>
#include <stack>
#include <algorithm>

#include "file.h"
#include "gate.h"
#include "graph.h"
#include "gateMatrix.h"

using std::cout;
using std::endl;

using std::ifstream;
using std::ofstream;

using std::string;

using std::stack;

using namespace pln;

const string OUT_FILENAME = "graph.txt";

void topologicalSort(graph& g);
void graphToMatrix(graph& g, char* outHeader);

int main(int argc, char* argv[])
{

    // check arguments
    if (argc < 3)
    {
        cout << "Need to pass in an input file and output file." << endl;
        exit(1);
    }

    else if (argc > 3)
    {
        cout << "Too many arguments." << endl;
        exit(1);
    }

    // open the file for parsing
    string const in_filename{ argv[1] };
    string const out_filename{ OUT_FILENAME };
    cout << "Filename is " << in_filename << endl;

    file_parser fp{ in_filename, out_filename };

    graph g{};
    
    fp.parse(g);

    topologicalSort(g);
    graphToMatrix(g, argv[2]);

    g.print();

    std::vector<gtid_t>& outputs=(g.get_module_list()[g.get_module_list().size()-1]).get_output_ports();
    std::vector<gtid_t>& inputs=(g.get_module_list()[g.get_module_list().size()-1]).get_input_ports();
 
    cout << "Ouput Bit Position (Left to Right)\n";
    for(uint32_t i = 0; i < outputs.size(); i++){
      gate cur_gate = g.get_gate_list()[outputs[i]];
      cout << "" << cur_gate.get_name() << " " << cur_gate.get_gate_pos() << "\n";
    }
    
    cout << "Input Bit Position (Left to Right)\n";
    for(uint32_t i = 0; i < inputs.size(); i++){
      gate cur_gate = g.get_gate_list()[inputs[i]];
      cout << "" << cur_gate.get_name() << " " << cur_gate.get_gate_pos() << "\n";
    }
 
}

// A recursive function used by topologicalSort
void topologicalSortUtil(graph& g, int v, bool visited[],
    stack<int> &Stack)
{
    // Mark the current node as visited.
    visited[v] = true;

    // Recur for all the vertices adjacent to this vertex
    if (g.get_graph()[v].size() > 0)
    {
        for (uint32_t i = 0; i < (g.get_graph()[v]).size(); ++i)
        {
            if (!visited[g.get_graph()[v][i]])
            {
                topologicalSortUtil(g, g.get_graph()[v][i], visited, Stack);
            }
        }
    }
    // Push current vertex to stack which stores result
    Stack.push(v);
}


// The function to do Topological Sort. It uses recursive 
// topologicalSortUtil()
void topologicalSort(graph& g)
{
    stack<int> Stack;

    // Mark all the vertices as not visited
    bool *visited = new bool[g.get_gate_list().size()];
    for (uint32_t i = 0; i < g.get_gate_list().size(); i++)
    {
        visited[i] = false;
    }

    // Call the recursive helper function to store Topological
    // Sort starting from all vertices one by one
    for (uint32_t i = 0; i < g.get_gate_list().size(); i++)
    {
        if (visited[i] == false)
        {
            topologicalSortUtil(g, i, visited, Stack);
        }
    }


    // CREATE FUNCTION FOR THIS: sets heights 
    // Print contents and set heights of stack
   vector<uint32_t> width; width.push_back(0);

    // Creating levels and width
    vector<gate>& cur_gate_list = g.get_gate_list();
    vector<vector<gtid_t>>& cur_graph = g.get_graph();

    // Visited 
    // Mark all the vertices as not visited
    delete visited;
    visited = new bool[g.get_gate_list().size()];
    for (uint32_t i = 0; i < g.get_gate_list().size(); i++)
    {
        visited[i] = false;
    }

    while (Stack.empty() == false)
    {
        cout << Stack.top() << " ";

        gtid_t id = Stack.top();
        gate& cur_gate = cur_gate_list[id];

        // set its own height if it is PORT_O in root  
        std::vector<gtid_t>& outputs=(g.get_module_list()[g.get_module_list().size()-1]).get_output_ports();
        if(std::find(outputs.begin(), outputs.end(), id) != outputs.end()) {
            cur_gate.set_gate_level(0);
            cur_gate.set_gate_pos(width[0]);
            width[0]++;
        }    

        // set its children's height based on its own with comparison
        if (cur_graph[id].size() > 0)
        {

            // CREATE FUNCTION FOR THIS

            // set first child
            gate& input0 = cur_gate_list[cur_graph[id][0]];

            // if input level is smaller than a parent gate + 1 (initialize all heights to 0)
            if (input0.get_gate_level() < cur_gate.get_gate_level() + 1)
            {
                cout << "input0 id: " << input0.get_id();

                // create new level on width if the desired level is not on the list
                if (cur_gate.get_gate_level() + 1 == width.size())
                {
                    cout << " new level, ";
                    width.push_back(0);
                }

                // remove from previous position allocation;          
                if (visited[input0.get_id()])
                {
                    cout << "visited previously,";
//                    width[input0.get_gate_level()]--;
                }

                // give it its new height and position
                input0.set_gate_level(cur_gate.get_gate_level() + 1);
                input0.set_gate_pos(width[cur_gate.get_gate_level() + 1]);
                width[input0.get_gate_level()]++;
                cout << " level " << input0.get_gate_level() << " pos " << input0.get_gate_pos() << "\n";
                visited[input0.get_id()] = true;
            }

            // set second child
            if (cur_graph[id].size() == 2)
            {
                gate& input1 = cur_gate_list[cur_graph[id][1]];
                cout << "  input1 id: " << input1.get_id();

                // if input level is smaller than a parent gate + 1 (initialize all heights to 0)
                if (input1.get_gate_level() < cur_gate.get_gate_level() + 1)
                {

                    // create new level on width if the desired level is not on the list
                    if (cur_gate.get_gate_level() + 1 == width.size())
                    {
                        cout << " new level, ";
                        width.push_back(0);
                    }

                    // remove from previous position allocation;
                    if (visited[input1.get_id()])
                    {
//                        width[input1.get_gate_level()]--;
                        cout << "visited previously,";
                    }

                    // give it its new height and position
                    input1.set_gate_level(cur_gate.get_gate_level() + 1);
                    input1.set_gate_pos(width[cur_gate.get_gate_level() + 1]);
                    width[input1.get_gate_level()]++;
                    cout << " level " << input1.get_gate_level() << " pos " << input1.get_gate_pos() << "\n";
                    visited[input1.get_id()] = true;
                }
            }
        }
        Stack.pop();
    }
    // AFTER THIS, GO TO TOP MODULES INPUT AND FORCE IT TO MAX LEVEL
    g.set_max_level(width.size());

    // Get Top Module
    std::vector<gtid_t>& inputs =  (g.get_module_list()[g.get_module_list().size() - 1]).get_input_ports();

cout << "\nTop module size" << inputs.size() << "\n";
    for(uint32_t i = 0; i < inputs.size(); i++){
cout << "Input " << inputs[i] << "\n";    
      // remove from previous position allocation
//      width[cur_gate_list[inputs[i]].get_gate_level()]--;
      
      cur_gate_list[inputs[i]].set_gate_level(g.get_max_level()-1);
      cur_gate_list[inputs[i]].set_gate_pos(i);
cout << "gate level " << cur_gate_list[inputs[i]].get_gate_level() << "gate pos " << cur_gate_list[inputs[i]].get_gate_pos() << "\n";
    }  
    width[g.get_max_level()-1] = inputs.size();
    g.set_max_width(*std::max_element(width.begin(), width.end()) - 1);

cout << "width max" << g.get_max_width() << " level max" << g.get_max_level(); 
cout << "\n";
}

// graph to matrix function 
// takes g as input from graph
void graphToMatrix(graph& g, char* outHeader)
{
    module& top =  (g.get_module_list()[g.get_module_list().size() - 1]);
    gateMatrix matrix = gateMatrix(g.get_max_level(), g.get_max_width(), top.get_input_ports().size(),
                                   top.get_output_ports().size());
    cout << "" << g.get_gate_list().size() << "\n";
    for (uint32_t i = 0; i < g.get_gate_list().size(); i++)
    {
        // i contains the index of the current gate value

        gtid_t input0, input1;

        // grab first gate id input 0 from graph
        if (g.get_graph()[i].size() == 0)
        {
            input0 = i;
            input1 = i;
        }
        else if (g.get_graph()[i].size() > 0)
        {
            input0 = g.get_graph()[i][0];

            // if gate has second input
            if (g.get_graph()[i].size() == 2)
            {
                // grab seccond gate id input 1 from graph
                input1 = g.get_graph()[i][1];
            }
            else
            {
                input1 = i;
            }
        }

        matrix.addGate(g.get_max_level()-1 - (g.get_gate_list()[i].get_gate_level()), g.get_gate_list()[i].get_gate_pos(),
            (GateType)g.get_gate_list()[i].get_gate_type(), g.get_max_level()-1 - (g.get_gate_list()[input0].get_gate_level()),
            g.get_gate_list()[input0].get_gate_pos(), g.get_max_level()-1 - (g.get_gate_list()[input1].get_gate_level()),
            g.get_gate_list()[input1].get_gate_pos());
    }

    matrix.printMatrix();
    matrix.outputMatrixHeader(outHeader);
}

