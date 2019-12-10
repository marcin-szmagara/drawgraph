#ifndef GRAPH_H
#define GRAPH_H

#include "Node.h"
#include "Edge.h"

#include <functional>
#include <vector>
#include <SFML/Graphics.hpp>
#include <array>
#include <string>

class Graph
{
public:
	Graph();
	Graph(int nNodes);
	void addEdge(int a, int b);
	void addEdge(int a, int b, std::string c);
	void loadFromStdin();
	Node& getNode(int v);
	int getNodeCount();
	void update(long long deltaTime);
	void draw(sf::RenderWindow& window);
	void handleEvent(const sf::Event& event);
private:
	void reset();
	double vecLength(const sf::Vector2<double>& v);
	void applyMechanics();
	std::vector<Node> m_nodes;
	std::vector<Edge> m_edges;
	std::vector<std::vector<std::pair<int, std::string>>> m_adjacencyList;
};

#endif
