#ifndef NODE_H
#define NODE_H

#include <SFML/Graphics.hpp>

class Node
{
	using vec = sf::Vector2<double>;
public:
	Node() = delete;
	Node(long long index, double radius = 25.f);
	void draw(sf::RenderWindow& window) const;
	void setPosition(vec pos);
	vec getPosition() const;
	double getRadius() const;
	void applyContinuousForce(vec force);
	void update(long long nanoseconds);
	bool handleEvent(const sf::Event& event);
private:
	vec shapeCenter(const sf::Shape& shape);
	vec textCenter(const sf::Text& text);
	double vecLength(const vec& v);
	void fitText();

	vec m_force, m_velocity, m_position;
	long long m_index;
	double m_radius;

	sf::CircleShape m_shape;
	static sf::Font m_font;
	static bool m_fontLoaded;
	sf::Text m_text;

	bool isDragged = 0;
	vec draggedTo;
};

#endif
