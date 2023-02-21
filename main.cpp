#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <array>
#include <vector>
#include <random>
#include <chrono>

// Window size
const int winWidth = 800;
const int winHeight = 600;
// Pi
const float pi = 3.14159f;

bool isCollide(sf::Sprite sprite1, sf::Sprite sprite2) {
	return sprite1.getGlobalBounds().intersects(sprite2.getGlobalBounds());
}

int main() {
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_real_distribution<> distribution(0., 360.);

	//const float scale_W{ winWidth / 640.f };
	//const float scale_H{ winHeight / 480.f };

	const float scale_W{ 640.f / winWidth };
	const float scale_H{ 480.f / winHeight };

	const float additional_scale{ 0.7f };

	sf::RenderWindow window(sf::VideoMode(winWidth, winHeight), "Break Out", sf::Style::Titlebar | sf::Style::Close);
	window.setFramerateLimit(60);
	//////////////////////////////////////////////////////////////////////////
	// Ball Texture + Sound + Sprite
	sf::Texture ball_texture;
	if (!ball_texture.loadFromFile("images//ball.png")) { return EXIT_FAILURE; }
	sf::Sprite ball_sprite(ball_texture);
	ball_sprite.setPosition(static_cast<float>(winWidth) / 2, static_cast<float>(winHeight) / 2);
	ball_sprite.setScale(
		scale_W < 1.f ? 1 / scale_W * additional_scale : 1 * scale_W * additional_scale,
		scale_H < 1.f ? 1 / scale_H * additional_scale : 1 * scale_H * additional_scale
	);
	sf::SoundBuffer ball_sounbuffer;
	if (!ball_sounbuffer.loadFromFile("sounds//ball.wav")) { return EXIT_FAILURE; }
	sf::Sound ball_sound{ ball_sounbuffer };
	//////////////////////////////////////////////////////////////////////////
	// Paddle texture
	sf::Texture paddle_texture;
	if (!paddle_texture.loadFromFile("images//paddle.png")) { return EXIT_FAILURE; }
	sf::Sprite paddle_sprite(paddle_texture);
	paddle_sprite.setPosition(static_cast<float>(winWidth) / 2, winHeight * 0.9);
	paddle_sprite.setScale(
		scale_W < 1.f ? 1 / scale_W * additional_scale : 1 * scale_W * additional_scale,
		scale_H < 1.f ? 1 / scale_H * additional_scale : 1 * scale_H * additional_scale
	);
	//////////////////////////////////////////////////////////////////////////
	// Fon music
	sf::Music music;
	if (!music.openFromFile("sounds//theme.ogg")) { return EXIT_FAILURE; }
	music.play();
	//////////////////////////////////////////////////////////////////////////
	// BackGround Texture + Sprite + Scale
	sf::Texture background_texture;
	if (!background_texture.loadFromFile("images//background.png")) { return EXIT_FAILURE; }
	sf::Sprite background_sprite{ background_texture };
	background_sprite.setScale(scale_W < 1.f ? 1 / scale_W : 1 * scale_W, scale_H < 1.f ? 1 / scale_H : 1 * scale_H);
	//////////////////////////////////////////////////////////////////////////
	// block texture initialisation
	sf::Texture block_orange;
	if (!block_orange.loadFromFile("images//block_orange.png")) { return EXIT_FAILURE; }
	sf::Texture block_violet;
	if (!block_violet.loadFromFile("images//block_violet.png")) { return EXIT_FAILURE; }
	sf::Texture block_yellow;
	if (!block_yellow.loadFromFile("images//block_yellow.png")) { return EXIT_FAILURE; }
	sf::Texture block_pink;
	if (!block_pink.loadFromFile("images//block_pink.png")) { return EXIT_FAILURE; }
	sf::Texture block_green;
	if (!block_green.loadFromFile("images//block_green.png")) { return EXIT_FAILURE; }
	//sf::Sprite block_orange_sprite{ block_orange };
	//////////////////////////////////////////////////////////////////////////
	// Blocks initialisation
	std::vector<sf::Sprite> blocks;
	//////////////////////////////////////////////////////////////////////////
	// Lambda for fill blocks
	auto lambda_block_initialization = [&blocks, additional_scale, scale_W, scale_H](sf::Texture& texture, size_t val) {
		for (size_t i = 2; i <= 11; ++i) {
			for (size_t j = val; j <= val; ++j) {
				blocks.emplace_back(texture);
				auto size = blocks.size() - 1;
				blocks[size].scale(scale_W * additional_scale, scale_H * additional_scale);
				blocks[size].setPosition(i * 84 * scale_W * additional_scale, j * 36 * scale_H * additional_scale);
			}
		}
	};
	//////////////////////////////////////////////////////////////////////////
	// Fill block texture
	lambda_block_initialization(block_orange, 3);
	lambda_block_initialization(block_violet, 4);
	lambda_block_initialization(block_yellow, 5);
	lambda_block_initialization(block_pink, 6);
	lambda_block_initialization(block_green, 7);
	//////////////////////////////////////////////////////////////////////////
	// Font
	sf::Font font;
	if (!font.loadFromFile("fonts/KLMNFP2004.ttf")) { return EXIT_FAILURE; }

	// Initialize the pause message
	///*sf::Text pauseMessage;
	//pauseMessage.setFont(font);
	//pauseMessage.setCharacterSize(40);
	//pauseMessage.setPosition(170.f, 150.f);
	//pauseMessage.setFillColor(sf::Color::White);
	//pauseMessage.setString("Welcome to SFML BreakOut!\nPress space to start the game");*/

	sf::Clock clock;
	bool is_playing{ false };
	//////////////////////////////////////////////////////////////////////////

	float ball_dx = 6;
	float ball_dy = 5;
	float paddle_dx = 6;


	//////////////////////////////////////////////////////////////////////////	
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed ||
				(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
				window.close();
				break;
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
		//////////////////////////////////////////////////////////////////////////
		// Repeat music
		music.setLoop(true);
		//////////////////////////////////////////////////////////////////////////
		// Collision ball -> blocks
		ball_sprite.move(ball_dx, 0);
		for (size_t i = 0; i < blocks.size(); ++i) {
			if (isCollide(ball_sprite, blocks[i])) {
				blocks.erase(blocks.begin() + i);
				ball_sound.play();
				ball_dx = -ball_dx;
			}
		}
		ball_sprite.move(0, ball_dy);
		for (size_t i = 0; i < blocks.size(); ++i) {
			if (isCollide(ball_sprite, blocks[i])) {
				blocks.erase(blocks.begin() + i);
				ball_sound.play();
				ball_dy = -ball_dy;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		// Collision paddle
		if (isCollide(ball_sprite, paddle_sprite)) {
			ball_sound.play();
			ball_dx = -ball_dx;
			ball_dy = -ball_dy;
		}
		//////////////////////////////////////////////////////////////////////////
		sf::Vector2f b = ball_sprite.getPosition();
		if (b.x < 50 || b.x > winWidth * 0.75) { ball_dx = -ball_dx; }
		if (b.y < 30 || b.y > winHeight * 0.9) { ball_dy = -ball_dy; }
		//////////////////////////////////////////////////////////////////////////
		sf::Vector2f paddle_position = paddle_sprite.getPosition();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			paddle_position.x > winWidth * 0.6 ? paddle_dx = 0 : paddle_dx = 6;
			paddle_sprite.move(paddle_dx, 0);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			paddle_position.x < 50 ? paddle_dx = 0 : paddle_dx = -6;
			paddle_sprite.move(paddle_dx, 0);
		}
		//////////////////////////////////////////////////////////////////////////

		window.clear();
		window.draw(background_sprite);
		window.draw(ball_sprite);
		window.draw(paddle_sprite);
		for (const auto& elem : blocks) { window.draw(elem); }



		window.display();
	}

	return EXIT_SUCCESS;
}