#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <array>
#include <random>

// Window size
const int winWidth = 640;
const int winHeight = 480;
// Pi
const float pi = 3.14159f;

int main() {
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_real_distribution<> distribution(0., 360.);

	const float scale_W{ winWidth / 640.f };
	const float scale_H{ winHeight / 480.f };

	sf::RenderWindow window(sf::VideoMode(winWidth, winHeight), "Break Out", sf::Style::Titlebar | sf::Style::Close);
	window.setFramerateLimit(60);

	// Ball Texture + Sound + Sprite
	sf::SoundBuffer ball_sounbuffer;
	if (!ball_sounbuffer.loadFromFile("sounds//ball.wav")) { return EXIT_FAILURE; }
	sf::Sound ball_sound{ ball_sounbuffer };

	// Fon music
	sf::Music music;
	if (!music.openFromFile("sounds//theme.ogg")) { return EXIT_FAILURE; }
	music.play();

	// BackGround Texture + Sprite + Scale
	sf::Texture background_texture;
	if (!background_texture.loadFromFile("images//background.png")) { return EXIT_FAILURE; }
	sf::Sprite background_sprite{ background_texture };
	background_sprite.scale(scale_W, scale_H);

	// Orange block initialisation
	sf::Texture block_orange;
	if (!block_orange.loadFromFile("images//block_orange.png")) { return EXIT_FAILURE; }
	//sf::Sprite block_orange_sprite{ block_orange };

	// Blocks initialisation
	//sf::Sprite blocks[1000];
	std::array<sf::Sprite, 1000> blocks;
	size_t n{ 0 };

	const float block_scale{ 1.33f };
	for (size_t i = 1; i <= 10; ++i) {

		for (size_t j = 1; j <= 5; ++j) {

			blocks[n].setTexture(block_orange);
			blocks[n].scale(scale_W * block_scale, scale_H * block_scale);
			blocks[n].setPosition(i * 40 * scale_W * block_scale, j * 16 * scale_H * block_scale);
			++n;

		}
	}



	// Font
	////sf::Font font;
	////if (!font.loadFromFile("fonts/sansation.ttf")) { return EXIT_FAILURE; }

	// Initialize the pause message
	///*sf::Text pauseMessage;
	//pauseMessage.setFont(font);
	//pauseMessage.setCharacterSize(40);
	//pauseMessage.setPosition(170.f, 150.f);
	//pauseMessage.setFillColor(sf::Color::White);
	//pauseMessage.setString("Welcome to SFML BreakOut!\nPress space to start the game");*/

	sf::Clock clock;
	bool is_playing{ false };

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if ((event.type == sf::Event::Closed)) { //|| 
				////((event.type == sf::Event::KeyPressed) && (event.key.code = sf::Keyboard::Escape))) {
				window.close();
				////break;
			}

			////if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Space)) {
			////	if (!is_playing)
			////	{
			////		// (re)start the game
			////		is_playing = true;
			////		clock.restart();

			////	}
			////}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) ball_sound.play();

		window.clear();
		window.draw(background_sprite);
		for (size_t i = 0; i < n; ++i) {
			window.draw(blocks[i]);
		}


		window.display();
	}

	return EXIT_SUCCESS;
}