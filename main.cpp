#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <array>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <cmath>
#include <ctime>
#include <cstdlib>

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
	std::uniform_int_distribution<> distr_int(0, 1);
	std::normal_distribution<float> distribution20_70(20.f, 135.f);

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
	ball_sprite.setScale(
		scale_W < 1.f ? 1 / scale_W * additional_scale : 1 * scale_W * additional_scale,
		scale_H < 1.f ? 1 / scale_H * additional_scale : 1 * scale_H * additional_scale
	);

	// Sounds
	sf::SoundBuffer ball_sounbuffer;
	if (!ball_sounbuffer.loadFromFile("sounds//ball.wav")) { return EXIT_FAILURE; }
	sf::Sound ball_sound{ ball_sounbuffer };

	sf::SoundBuffer win_sounbuffer;
	if (!win_sounbuffer.loadFromFile("sounds//win.ogg")) { return EXIT_FAILURE; }
	sf::Sound win_sound{ win_sounbuffer };

	sf::SoundBuffer lose_sounbuffer;
	if (!lose_sounbuffer.loadFromFile("sounds//lose.ogg")) { return EXIT_FAILURE; }
	sf::Sound lose_sound{ lose_sounbuffer };
	//////////////////////////////////////////////////////////////////////////

	// Paddle texture
	sf::Texture paddle_texture;
	if (!paddle_texture.loadFromFile("images//paddle.png")) { return EXIT_FAILURE; }
	sf::Sprite paddle_sprite(paddle_texture);
	paddle_sprite.setScale(
		scale_W < 1.f ? 1 / scale_W * additional_scale : 1 * scale_W * additional_scale,
		scale_H < 1.f ? 1 / scale_H * additional_scale : 1 * scale_H * additional_scale
	);
	//////////////////////////////////////////////////////////////////////////

	// Intro music
	sf::Music intro;
	if (!intro.openFromFile("sounds//intro.ogg")) { return EXIT_FAILURE; }
	intro.play();
	// Fon music
	sf::Music music;
	if (!music.openFromFile("sounds//theme.ogg")) { return EXIT_FAILURE; }
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
	//////////////////////////////////////////////////////////////////////////

	// Game bound
	sf::Texture bound;
	if (!bound.loadFromFile("images//bound.png")) { return EXIT_FAILURE; }
	sf::Sprite bound_sprite(bound);
	bound_sprite.setPosition(25.f, 20.f);
	bound_sprite.setScale(
		scale_W < 1.f ? 1 / scale_W * additional_scale * 0.7f : 1 * scale_W * additional_scale * 0.8f,
		scale_H < 1.f ? 1 / scale_H * additional_scale * 0.7f : 1 * scale_H * additional_scale * 0.8f
	);
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
				blocks[size].setPosition(i * 90 * scale_W * additional_scale, j * 44 * scale_H * additional_scale);
			}
		}
	};
	//////////////////////////////////////////////////////////////////////////

	// Pause menu
	sf::Font font;
	if (!font.loadFromFile("fonts/KLMNFP2004.ttf")) { return EXIT_FAILURE; }
	// Initialize the pause message
	sf::Text pauseMessage, score;
	pauseMessage.setFont(font);
	pauseMessage.setCharacterSize(25);
	pauseMessage.setPosition(winHeight / 2.f * additional_scale * scale_H, 150.f);
	pauseMessage.setFillColor(sf::Color::Cyan);
	pauseMessage.setString("Welcome to SFML BreakOut!\nPress space to start the game");

	// Text score
	long long score_count = 0;
	score.setFont(font);
	score.setCharacterSize(12);
	score.setPosition(winWidth * scale_W * 1.06, 50);
	score.setFillColor(sf::Color::White);
	score.setString("SCORE : " + std::to_string(score_count));
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	sf::Clock clock;
	bool is_playing{ false };

	float ball_dx;
	float ball_dy;
	float paddle_dx;

	float ballSpeed;
	float paddle_speed;
	//////////////////////////////////////////////////////////////////////////	

	while (window.isOpen()) {
		sf::Event event;

		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed ||
				(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
				window.close();
				break;
			}

			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Space)) {
				if (!is_playing) {
					intro.setLoop(true);
					if (distr_int(generator)) {
						ball_dx = 6;
					}
					else {
						ball_dx = -6;
					}
					ball_dy = 5;
					paddle_dx = 6;
					ballSpeed = 40.0f;
					paddle_speed = 3.0f;
					// Fill block texture
					blocks.clear();
					lambda_block_initialization(block_orange, 3);
					lambda_block_initialization(block_violet, 4);
					lambda_block_initialization(block_yellow, 5);
					lambda_block_initialization(block_pink, 6);
					lambda_block_initialization(block_green, 7);

					music.play();
					is_playing = true;
					clock.restart();
					score_count = 0;
					score.setString("SCORE: " + std::to_string(score_count));

					ball_sprite.setPosition(winWidth / 2.f, winHeight / 2.f);
					paddle_sprite.setPosition(winWidth / 2.f * scale_W, winHeight * 0.9f);
				}
			}
		} // while (window.pollEvent(event))

		if (is_playing) {
			intro.stop();
			float time = clock.restart().asSeconds();

			//////////////////////////////////////////////////////////////////////////
			// Repeat music
			music.setLoop(true);
			//////////////////////////////////////////////////////////////////////////
			float factor = ballSpeed * time;

			// Ball moves
			ball_sprite.move(ball_dx * factor, ball_dy * factor);
			//////////////////////////////////////////////////////////////////////////

			// Lose
			if (ball_sprite.getGlobalBounds().top + ball_sprite.getGlobalBounds().height >= paddle_sprite.getGlobalBounds().top + paddle_sprite.getGlobalBounds().height) {
				music.stop();
				lose_sound.play();
				is_playing = false;
				pauseMessage.setString("You lost!\nPress space to restart or\nescape to exit");
			}

			// Win
			if (blocks.empty()) {
				music.stop();
				win_sound.play();
				is_playing = false;
				pauseMessage.setString("You won!\nPress space to restart or\nescape to exit");
			}
			//////////////////////////////////////////////////////////////////////////

			// Collision bounds
			sf::Vector2f b = ball_sprite.getPosition();
			if (b.x <= 55 || b.x >= winWidth * 0.79) { ball_dx = -ball_dx; }
			if (b.y <= 40) { ball_dy = -ball_dy; }
			//////////////////////////////////////////////////////////////////////////

			// 
			sf::Vector2f paddle_position = paddle_sprite.getPosition();
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
				paddle_position.x > winWidth * 0.65 ? paddle_dx = 0 : paddle_dx = 6;
				paddle_sprite.move(paddle_dx * factor * paddle_speed, 0);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
				paddle_position.x < 53 ? paddle_dx = 0 : paddle_dx = -6;
				paddle_sprite.move(paddle_dx * factor * paddle_speed, 0);
			}
			//////////////////////////////////////////////////////////////////////////

			// Colision ball -> blocks
			for (size_t i = 0; i < blocks.size(); ++i) {
				if (isCollide(ball_sprite, blocks[i])) {
					score_count += 100;
					score.setString("SCORE: " + std::to_string(score_count));
					blocks.erase(blocks.begin() + i);
					ball_sound.play();
					ball_dx = -ball_dx;
				}
			}
			for (size_t i = 0; i < blocks.size(); ++i) {
				if (isCollide(ball_sprite, blocks[i])) {
					score_count += 100;
					score.setString("SCORE: " + std::to_string(score_count));
					blocks.erase(blocks.begin() + i);
					ball_sound.play();
					ball_dy = -ball_dy;
				}
			}
			//////////////////////////////////////////////////////////////////////////

			// Collision ball -> paddle
			if (isCollide(paddle_sprite, ball_sprite)) {
				ball_sound.play();

				if (distr_int(generator)) {
					ball_dy = -ball_dy + std::cos(distribution20_70(generator)) * factor;
				}
				else {
					ball_dy = -ball_dy - std::cos(distribution20_70(generator)) * factor;
				}
				if (distr_int(generator)) {
					ball_dx = -ball_dx;
				}
				if (ballSpeed < 20.f) {
					ballSpeed += 0.5;
				}
			}
			//////////////////////////////////////////////////////////////////////////


		} // game

		window.clear();
		if (is_playing) {
			window.draw(background_sprite);
			window.draw(bound_sprite);
			window.draw(ball_sprite);
			window.draw(paddle_sprite);
			for (const auto& elem : blocks) { window.draw(elem); }
		}
		else {
			window.draw(pauseMessage);
		}
		score_count >= 10000 ? score.setCharacterSize(11) : score.setCharacterSize(12);
		score.setString("SCORE: " + std::to_string(score_count));
		window.draw(score);
		window.display();
	} // while (window.isOpen())

	return EXIT_SUCCESS;
} // main