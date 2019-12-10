#include "Node.h"
#include <string>
#include <iostream>
#include <cmath>
#include <random>

using vec = sf::Vector2<double>;
sf::Font Node::m_font;
bool Node::m_fontLoaded = 0;

Node::Node(long long index, double radius) : m_index(index), m_radius(radius)
{
	m_shape.setRadius(m_radius);
	m_shape.setOrigin({m_radius, m_radius});
	m_shape.setOutlineColor(sf::Color::Black);
	m_shape.setOutlineThickness(-m_radius / 10.f);
	m_shape.setFillColor(sf::Color::White);

	if (!m_fontLoaded)
	{
		if (!m_font.loadFromFile("fonts/arial.ttf"))
		{
			std::cerr << "Unable to open font file. Termianting\n";
			std::exit(1);
		}
		m_fontLoaded = 1;
	}
	m_text = sf::Text(sf::String(std::to_string(index)), m_font);
	m_text.setCharacterSize(100);
	m_text.setStyle(sf::Text::Bold);
	m_text.setFillColor(sf::Color::Black);
	m_text.setOrigin(sf::Vector2f(textCenter(m_text)));

	static std::random_device device;
	static std::mt19937 rng(device());
	static std::uniform_real_distribution<> dist(100.0, 500.0);
	setPosition({dist(rng), dist(rng)});
}

void Node::draw(sf::RenderWindow& window) const
{
	window.draw(m_shape);
	window.draw(m_text);
}

void Node::setPosition(vec pos)
{
	m_position = pos;
}

vec Node::getPosition() const
{
	return m_position;
}

double Node::getRadius() const
{
	return m_radius;
}

void Node::applyContinuousForce(vec force)
{
	m_force += force;
}

void Node::update(long long nanoseconds)
{
	if (nanoseconds == 0) return;
	if (!isDragged)
	{
		auto acceleration = m_force;
		m_velocity += acceleration * (double)nanoseconds / 1e9;
		const double resistanceFactor = 50;
		const double resistanceFactorMult = 0.9;
		if (vecLength(m_velocity) * resistanceFactorMult * (double)nanoseconds / 1e9
				<= vecLength(m_velocity))
			m_velocity -= m_velocity * resistanceFactorMult * (double)nanoseconds / 1e9;
		else m_velocity = {0, 0};
		auto mag = vecLength(m_velocity);
		if (mag <= resistanceFactor * (double)nanoseconds / 1e9) m_velocity = {0, 0};
		else
		{
			auto norm = m_velocity / mag;
			m_velocity -= resistanceFactor * (double)nanoseconds / 1e9 * norm;
		}
		m_position += m_velocity * (double)nanoseconds / 1e9;
	}
	else
	{
		m_position = draggedTo;
	}
	m_shape.setPosition(sf::Vector2f(m_position));
	m_text.setPosition(sf::Vector2f(m_position));
	fitText();
	m_force = {0.0, 0.0};
}

bool Node::handleEvent(const sf::Event& event)
{
	if (event.type == sf::Event::MouseButtonPressed)
	{
		auto dist = vecLength(vec{event.mouseButton.x, event.mouseButton.y}
				- m_position);
		if (dist <= m_radius)
		{
			isDragged = 1;
			draggedTo = vec{event.mouseButton.x, event.mouseButton.y};
		}
	}
	if (event.type == sf::Event::MouseButtonReleased)
		isDragged = 0;
	if (isDragged && event.type == sf::Event::MouseMoved)
	{
		draggedTo = {event.mouseMove.x, event.mouseMove.y};
	}
	return isDragged;
}

vec Node::shapeCenter(const sf::Shape& shape)
{
	auto x = shape.getGlobalBounds();
	return {x.left + x.width / 2.f, x.top + x.height / 2.f};
}

vec Node::textCenter(const sf::Text& text)
{
	auto x = text.getGlobalBounds();
	return {x.left + x.width / 2.f, x.top + x.height / 2.f};
}

double Node::vecLength(const vec& v)
{
	return sqrt(v.x * v.x + v.y * v.y);
}

void Node::fitText()
{
	auto textBounds = m_text.getLocalBounds();
	vec textRadiusVec = vec{textBounds.left + textBounds.width / 2.0,
					textBounds.top + textBounds.height / 2.0};
	double textRadius = vecLength(textRadiusVec);
	const double fillCoefficient = 0.80;
	double scaleFactor = m_radius / textRadius * fillCoefficient;
	m_text.setScale({scaleFactor, scaleFactor});
}
