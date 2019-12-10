#include <Edge.h>
#include <cmath>
#include <iostream>

using vec = sf::Vector2<double>;

bool Edge::m_fontLoaded = 0;
sf::Font Edge::m_font;

Edge::Edge() : Edge("")
{
	
}

Edge::Edge(std::string value, double thickness)
	: m_thickness(thickness),
		m_line(sf::VertexArray(sf::Quads, 4))
{
	for (int i = 0; i < 4; i++) m_line[i].color = sf::Color::Black;
	if (!m_fontLoaded)
	{
		if (!m_font.loadFromFile("fonts/arial.ttf"))
		{
			std::cerr << "Unable to open font file. Termianting\n";
			std::exit(1);
		}
		m_fontLoaded = 1;
	}
	m_text = sf::Text(sf::String(value), m_font);
	m_text.setCharacterSize(100);
	m_text.setStyle(sf::Text::Bold);
	m_text.setFillColor(sf::Color::Black);
	m_text.setOrigin(sf::Vector2f(textCenter(m_text)));

}

Edge::Edge(vec point1, vec point2, std::string value, double thickness)
	: Edge(value, thickness)
{
	setPosition(point1, point2);
}

void Edge::draw(sf::RenderWindow& window) const
{
	window.draw(m_line);
	window.draw(m_text);
}

void Edge::setPosition(vec p1, vec p2)
{
	m_point1 = p1;
	m_point2 = p2;
}

void Edge::setPosition(std::pair<vec, vec> ps)
{
	auto [p1, p2] = ps;
	setPosition(p1, p2);
}

std::pair<vec, vec> Edge::getPosition() const
{
	return {m_point1, m_point2};
}

void Edge::update()
{
	updateLine();
	updateText();
}

void Edge::updateLine()
{
	vec v = m_point2 - m_point1;
	vec normv = v / sqrt(v.x * v.x + v.y * v.y);
	vec normper = {-normv.y, normv.x};
	m_line[0].position = sf::Vector2f(m_point1 + normper * m_thickness / 2.);
	m_line[1].position = sf::Vector2f(m_point1 - normper * m_thickness / 2.);
	m_line[2].position = sf::Vector2f(m_point2 - normper * m_thickness / 2.);
	m_line[3].position = sf::Vector2f(m_point2 + normper * m_thickness / 2.);
}

void Edge::updateText()
{
	const double textScale = 1 / 10.0;
	m_text.setScale({m_thickness * textScale, m_thickness * textScale});

	auto v = m_point2 - m_point1;
	auto [x, y] = v;
	auto mag = vecLength(v);
	auto norm = v / mag;
	auto per = vec{-norm.y, norm.x};
	auto textHeight = m_text.getLocalBounds().height * m_thickness * textScale;
	auto edgeMiddle = vec{(m_point1 + m_point2) / 2.0};
	auto padding = per * (m_thickness + textHeight * 0.7);
	auto rotation = std::atan2(y, x) / atan(1) * 45 + 180;
	if (rotation > 90 && rotation < 270) rotation += 180, padding *= -1.0;
	m_text.setPosition(sf::Vector2f(edgeMiddle + padding));
	m_text.setRotation(rotation);
}

vec Edge::textCenter(const sf::Text& text)
{
	auto x = text.getGlobalBounds();
	return {x.left + x.width / 2.f, x.top + x.height / 2.f};
}

double Edge::vecLength(const vec& v)
{
	return sqrt(v.x * v.x + v.y * v.y);
}
