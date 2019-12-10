#ifndef EDGE_H
#define EDGE_H

#include <SFML/Graphics.hpp>
#include <string>

class Edge
{
	using vec = sf::Vector2<double>;
public:
	Edge();
	Edge(std::string value, double thickness = 2.0);
	Edge(vec point1, vec point2, std::string value = std::string(), double thickness = 2.0);
	void draw(sf::RenderWindow& window) const;
	void setPosition(vec p1, vec p2);
	void setPosition(std::pair<vec, vec> ps);
	std::pair<vec, vec> getPosition() const;
	void update();
	void updateLine();
	void updateText();
private:
	vec textCenter(const sf::Text& text);
	double vecLength(const vec& v);
	vec m_point1, m_point2;
	double m_thickness;
	std::string m_value;
	static sf::Font m_font;
	static bool m_fontLoaded;
	sf::Text m_text;
	sf::VertexArray m_line;
};

#endif
