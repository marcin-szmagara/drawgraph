#include <iostream>
#include <chrono>
#include <cmath>
#include <SFML/System.hpp>

#include "Node.h"
#include "Edge.h"
#include "Graph.h"

inline long long timeNano()
{
	return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono
			::high_resolution_clock::now().
		time_since_epoch()).count();
}

int main()
{
	const int framerateTarget = 100;
	const bool lowPowerMode = 0;
	sf::ContextSettings settings;
	settings.antialiasingLevel = 16;
	sf::RenderWindow window(sf::VideoMode(800, 600), "node", sf::Style::None, settings);
	window.setFramerateLimit(framerateTarget + 2);

	Graph g(10);
	g.loadFromStdin();

	long long lastFrameTime = timeNano(), frameDelta = 1,
	     		lastAfterUpdates = timeNano(), lastUpdateStart = timeNano();
	bool firstRun = 1;
	while (window.isOpen())
	{
		auto frameTime = timeNano();
		frameDelta = frameTime - lastFrameTime;
		lastFrameTime = frameTime;
		// handle events
		
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
				case sf::Event::Closed:
					window.close();
					break;
				case  sf::Event::KeyPressed:
					switch (event.key.code)
					{
						case sf::Keyboard::Escape:
							window.close();
							break;
						case sf::Keyboard::F:
							std::cout << "fps: " << (int)
								(1e9 / frameDelta + 1e-6) << '\n';
							break;
						default: break;
					}
					break;
				default: break;
			}
			g.handleEvent(event);
		}

		{
			int updateNumber = 0;
			long long updateDelta = 0;

			if (firstRun) lastUpdateStart = timeNano();
			// update
			do
			{
				auto updateStartTime = timeNano();
				auto deltaTime = updateStartTime - lastUpdateStart;
				lastUpdateStart = updateStartTime;
				
				g.update(deltaTime);

				updateNumber++;
				auto updateEndTime = timeNano();
				updateDelta = updateEndTime - updateStartTime;
			}
			while (!lowPowerMode && timeNano() + 2 * updateDelta + (long long)1e4 <
					lastAfterUpdates + (long long)(1e9 / framerateTarget));
			lastAfterUpdates = timeNano();
		}
		// draw
		window.clear(sf::Color::White);

		g.draw(window);

		window.display();

		firstRun = 0;
	}
}
