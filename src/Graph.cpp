#include "Graph.h"

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <cmath>

Graph::Graph()
{

}

Graph::Graph(int nNodes)
{
	for (int i = 0; i < nNodes; i++)
	{
		m_nodes.push_back({i});
		m_adjacencyList.push_back({});
	}
}

void Graph::addEdge(int a, int b)
{
	m_adjacencyList[a].push_back({b, ""});
	m_adjacencyList[b].push_back({a, ""});
	m_edges.push_back({});
}

void Graph::addEdge(int a, int b, std::string c)
{
	m_adjacencyList[a].push_back({b, c});
	m_adjacencyList[b].push_back({a, c});
	m_edges.push_back({c});
}

void Graph::loadFromStdin()
{
	reset();
	int n, m; std::cin >> n >> m;
	std::string rest;
	std::getline(std::cin, rest);
	std::vector<std::tuple<int, int, std::string>> V;
	for (int i = 0; i < m; i++)
	{
		std::string line; std::getline(std::cin, line);
		std::vector<std::string> I;
		std::istringstream stream(line);
		std::copy(std::istream_iterator<std::string>(stream),
				std::istream_iterator<std::string>(),
				std::back_inserter(I));
		if (I.size() == 2)
		{
			int v = atoi(I.front().c_str()), u = atoi(I.back().c_str());
			if (v != u)
				V.push_back({v, u, ""});
		}
		else if (I.size() == 3)
		{
			int v = std::atoi(I.front().c_str()), u = std::atoi(I[1].c_str());
			if (v != u)
				V.push_back({v, u, I.back()});
		}
	}
	int low = 1e9, high = -1e9;
	for (auto [x, y, z] : V)
	{
		low = std::min(low, std::min(x, y));
		high = std::max(high, std::max(x, y));
	}
	std::map<int, int> M;
	if (low >= 1 && high <= n)
	{
		for (int i = 1; i <= n; i++)
			M[i] = 0;
	}
	else if (low >= 0 && high <= n - 1)
	{
		for (int i = 0; i < n; i++)
			M[i] = 0;
	}
	for (auto& [v, u, s] : V) M[v] = 0, M[u] = 0;
	int c = 0;
	for (auto& [v, u] : M)
	{
		u = c++;
		m_adjacencyList.push_back({});
		m_nodes.push_back({v});
	}
	for (auto [v, u, s] : V)
	{
		m_edges.push_back({s});
		m_adjacencyList[M[v]].push_back({M[u], s});
		m_adjacencyList[M[u]].push_back({M[v], s});
	}
}

Node& Graph::getNode(int v)
{
	if (v < 0 || v >= (int)m_nodes.size())
		throw std::invalid_argument("getNode: node " + std::to_string(v) +
				" does not exist; number of nodes is " +
					std::to_string(m_nodes.size()));
	return m_nodes[v];
}

int Graph::getNodeCount()
{
	return m_nodes.size();
}

void Graph::update(long long deltaTime)
{
	applyMechanics();
	for (auto& x : m_nodes) x.update(deltaTime);
	int k = 0;
	for (int i = 0; i < m_adjacencyList.size(); i++)
	{
		for (int j = 0; j < m_adjacencyList[i].size(); j++)
		{
			int l = m_adjacencyList[i][j].first;
			if (i > l) continue;
			auto p1 = m_nodes[i].getPosition(), p2 = m_nodes[l].getPosition();
			auto norm = (p2 - p1) / vecLength(p2 - p1);
			p1 += norm * m_nodes[i].getRadius();
			p2 -= norm * m_nodes[l].getRadius();
			m_edges[k] = {p1, p2,
				m_adjacencyList[i][j].second};
			m_edges[k].update();
			k++;
		}
	}
}

void Graph::draw(sf::RenderWindow& window)
{
	for (auto& x : m_nodes) x.draw(window);
	for (auto& x : m_edges) x.draw(window);
}

void Graph::handleEvent(const sf::Event& event)
{
	for (auto& x: m_nodes)
	{
		if (x.handleEvent(event)) break;
	}
}

void Graph::reset()
{
	m_nodes = {};
	m_edges = {};
	m_adjacencyList = {};
}

double Graph::vecLength(const sf::Vector2<double>& v)
{
	return sqrt(v.x * v.x + v.y * v.y);
}

void Graph::applyMechanics()
{
	using vec = sf::Vector2<double>;
	const vec windowSize{800, 600};
	const double targetDistance = 170.0 - std::min(70.0, m_nodes.size() * 5.0);
	const double eps = 1e-7;
	/*
	vec sum{};
	for (auto& x : m_nodes)
		sum += x.getPosition() - windowSize / 2.0;
	vec avg = sum / (double)m_nodes.size();
	for (auto& x : m_nodes)
		x.setPosition(x.getPosition() - avg);
	*/

	for (auto& x : m_nodes)
	{
		auto v = windowSize / 2.0 - x.getPosition();
		x.applyContinuousForce(v * vecLength(v) / 2000.0 * (double)m_nodes.size());
	}
	
	for (int i = 0; i < m_nodes.size(); i++)
		for (int j = 0; j < m_nodes.size(); j++)
		{
			if (i == j) continue;
			auto p1 = m_nodes[i].getPosition();
			auto p2 = m_nodes[j].getPosition();
			auto x = p2 - p1;
			auto dist = vecLength(x);
			if (dist < eps) continue;
			auto f = x / dist * (targetDistance / dist) * 70.0;
			m_nodes[i].applyContinuousForce(-f);
			m_nodes[j].applyContinuousForce(f);
		}
	
	for (int i = 0; i < m_adjacencyList.size(); i++)
		for (int k = 0; k < m_adjacencyList[i].size(); k++)
		{
			int j = m_adjacencyList[i][k].first;
			auto p1 = m_nodes[i].getPosition();
			auto p2 = m_nodes[j].getPosition();
			auto x = p2 - p1;
			auto dist = vecLength(x);
			auto f = x / dist * (dist - targetDistance) * fabs(dist - targetDistance)
				/ 10.0;
			m_nodes[i].applyContinuousForce(f);
			m_nodes[j].applyContinuousForce(-f);
		}
}
