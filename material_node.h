#pragma once

#include <vector>
#include <map>
#include <string>
#include <algorithm>

namespace bgfx
{

class NodeBase;

class NodeInput;
class NodeOutput;

class NodeInput
{
	static int _id_counter;
public:
	int id;
	NodeInput() : id(_id_counter++) {}
    std::string name;
    std::string type;
	NodeOutput* connection = nullptr;
	NodeBase* owner;
	std::string id_string() { return std::to_string(id); }
};

class NodeOutput
{
	static int _id_counter;
public:
	int id;
	NodeOutput(): id(_id_counter++) {}
    std::string name;
    std::string type;
    std::vector<NodeInput*> connections;
	//NodeInput* connection = nullptr;
	NodeBase* owner;
	std::string id_string() { return std::to_string(id); }
};

class NodeBase
{
public:
	bool required = false;
	int id;
	int rank = 0;
    std::string name;
	std::map<std::string, NodeInput> inputs;
	std::map<std::string, NodeOutput> outputs;
};

template <class T>
class Node : public NodeBase
{
public:
    T data;
	Node(const std::string& in_name)
	{
        name = in_name;
	}
	Node(const std::string& in_name, bool do_default)
	{
        add_input("i1");
        add_output("o1");
        name = in_name;
	}
	void add_input(const std::string& name, const std::string& type)
	{
		inputs[name] = NodeInput();
		inputs[name].owner = this;
		inputs[name].name = name;
        inputs[name].type = type;
	}
	void add_output(const std::string& name, const std::string& type)
	{
		outputs[name] = NodeOutput();
		outputs[name].owner = this;
		outputs[name].name = name;
        outputs[name].type = type;
	}
	bool has_input_connections()
	{
		for (auto& in : inputs)
		{
			if (in.second.connection)
			{
				return true;
			}
		}
		return false;
	}

    void connect(NodeInput* input)
    {
        input->connection = &(outputs.begin()->second);
        outputs.begin()->second.connections.push_back(input);
    }

    void connect(const std::string& output_name, NodeInput* input)
    {
        input->connection = &outputs[output_name];
        outputs[output_name].connections.push_back(input);
    }
};

template <class T>
class NodeGraph
{
	std::map<std::string, Node<T>*> _nodes;
public:
	void add_node(Node<T>* in_node)
	{
		_nodes[in_node->name] = in_node;
	}

	std::vector<Node<T>*> sort_nodes()
	{
		std::vector<Node<T>*> sortable_nodes;

		for (auto& node : _nodes)
		{
			if (!node.second->has_input_connections())
			{
				rank_up_node_tree(node.second);
			}
            sortable_nodes.push_back(node.second);
		}
        std::sort(sortable_nodes.begin(), sortable_nodes.end(), [](const NodeBase* a, const NodeBase* b)
            {
                return a->rank < b->rank;
            }
            );
        return sortable_nodes;
	}

	void rank_up_node_tree(NodeBase* in_node)
	{
		for (auto& output : in_node->outputs)
		{
			if (output.second.connections.size())
			{
                for (auto& con : output.second.connections)
                {
                    auto child_node = con->owner;
                    child_node->rank = std::max(child_node->rank, in_node->rank + 1);
                    rank_up_node_tree(child_node);
                }
			}
		}
        printf("Ranked up node %s to %d\n", in_node->name.c_str(), in_node->rank);
	}
};

}  // namespace bgfx
