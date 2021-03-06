#include <string>

#include <Engine/Keys.h>
#include <Engine/Input.h>
#include <Engine/InputEvents.h>
#include <Engine/Sprite.h>

#include "NemoGame.h"

/**
*   @brief   Default Constructor.
*   @details Consider setting the game's width and height
             and even seeding the random number generator.
*/
NemoGame::NemoGame()
{
	game_width = 1024;
	game_height = 768;
	toggleFPS();
	srand(time(NULL));
}

/**
*   @brief   Destructor.
*   @details Remove any non-managed memory and callbacks.
*/
NemoGame::~NemoGame()
{
	this->inputs->unregisterCallback(key_callback_id);
	this->inputs->unregisterCallback(mouse_callback_id);

	if (background)
	{
		delete background;
		background = nullptr;
	}

	if (clownfish)
	{
		delete clownfish;
		clownfish = nullptr;
	}
}

/**
*   @brief   Initialises the game.
*   @details The game window is created and all assets required to
			 run the game are loaded. The keyHandler and clickHandler
			 callback should also be set in the initialise function.
*   @return  True if the game initialised correctly.
*/
bool NemoGame::init()
{
	if (!initAPI())
	{
		return false;
	}

	renderer->setSpriteMode(ASGE::SpriteSortMode::IMMEDIATE);
	renderer->setWindowTitle("Nemo Game");

	background = renderer->createRawSprite();
	background->loadTexture(".\\Resources\\Textures\\background.jpg");
	background->width(1024);
	background->height(768);

	clownfish = renderer->createRawSprite();
	clownfish->loadTexture(".\\Resources\\Textures\\clown-fish-icon.png");
	clownfish->width(64);
	clownfish->height(64);
	clownfish->setFlipFlags(ASGE::Sprite::FlipFlags::FLIP_X);

	// input handling functions
	key_callback_id = inputs->addCallbackFnc(
		ASGE::E_KEY, &NemoGame::keyHandler, this);
	
	mouse_callback_id =inputs->addCallbackFnc(
		ASGE::E_MOUSE_CLICK, &NemoGame::clickHandler, this);

	spawn();
	return true;
}

/**
*   @brief   Processes any key inputs
*   @details This function is added as a callback to handle the game's
			 keyboard input. For this game, calls to this function
			 are thread safe, so you may alter the game's state as you
			 see fit.
*   @param   data The event data relating to key input.
*   @see     KeyEvent
*   @return  void
*/
void NemoGame::keyHandler(const ASGE::SharedEventData data)
{
	auto key = static_cast<const ASGE::KeyEvent*>(data.get());
	
	if (key->key == ASGE::KEYS::KEY_RIGHT &&
		key->action == ASGE::KEYS::KEY_RELEASED)
	{
		menu_option = 1 - menu_option;
	}
	else if (key->key == ASGE::KEYS::KEY_LEFT &&
		key->action == ASGE::KEYS::KEY_RELEASED)
	{
		menu_option = 1 - menu_option;
	}
	else if (key->key == ASGE::KEYS::KEY_ENTER &&
		key->action == ASGE::KEYS::KEY_RELEASED)
	{
		if (menu_option == 1)
		{
			signalExit();
		}
		else
		{
			in_menu = false;
		}
	}
	else if (key->key == ASGE::KEYS::KEY_ESCAPE)
	{
		signalExit();
	}
}

/**
*   @brief   Processes any key inputs
*   @details This function is added as a callback to handle the game's
		     mouse button input. For this game, calls to this function
             are thread safe, so you may alter the game's state as you
             see fit.
*   @param   data The event data relating to key input.
*   @see     ClickEvent
*   @return  void
*/
void NemoGame::clickHandler(const ASGE::SharedEventData data)
{
	auto click = static_cast<const ASGE::ClickEvent*>(data.get());

	double x_pos, y_pos;
	inputs->getCursorPos(x_pos, y_pos);

	if (isInside(clownfish, x_pos, y_pos))
	{
		spawn();
		score++;
	}
}

/**
*   @brief   Spawns a Clownfish
*   @details This function is used to randomly set the position
             of the Clownfish i.e. spawn it. 
*   @see     KeyEvent
*   @return  void
*/
void NemoGame::spawn()
{
	auto x_pos = (rand() % game_width) + 1;
	auto y_pos = (rand() % game_height) + 1;

	clownfish->xPos(x_pos);
	clownfish->yPos(y_pos);
}

/**
*   @brief   Updates the scene
*   @details Prepares the renderer subsystem before drawing the
		     current frame. Once the current frame is has finished
		     the buffers are swapped accordingly and the image shown.
*   @return  void
*/
void NemoGame::update(const ASGE::GameTime& us)
{
	if (!in_menu)
	{
		auto x_pos = clownfish->xPos();

		x_pos += 500 * (us.delta_time.count() / 1000.f);

		clownfish->xPos(x_pos);

		if (x_pos >= game_width)
		{
			clownfish->xPos(0);
		}
	}
}

/**
*   @brief   Renders the scene
*   @details Renders all the game objects to the current frame.
	         Once the current frame is has finished the buffers are
			 swapped accordingly and the image shown.
*   @return  void
*/
void NemoGame::render(const ASGE::GameTime &)
{
	
	renderer->setFont(0);

	renderer->renderSprite(*background);

	if (in_menu)
	{
		renderer->renderText(
			"FISH ARE FRIENDS NOT FOOD \n SELECT FOOD TO START", 
			150, 150, 1.0, ASGE::COLOURS::DARKORANGE);

		renderer->renderText(menu_option == 0 ? ">FOOD" : "FOOD",
			250, 650, 1.0, ASGE::COLOURS::DARKORANGE);

		renderer->renderText(menu_option == 1 ? ">FRIENDS" : "FRIENDS",
			450, 650, 1.0, ASGE::COLOURS::DARKORANGE);
	}
	else
	{
		renderer->renderSprite(*clownfish);

		std::string score_str = "SCORE: " + std::to_string(score);
		renderer->renderText(score_str.c_str(), 850, 25, 1.0, ASGE::COLOURS::DARKORANGE);
	}
	
}

/**
*   @brief   Checks to see if a click is on a sprite. 
*   @details Create a bounding rectangle around the sprite
             and using a point represented by x and y check
			 to see if it resides inside the bounding box.
*   @param   sprite The sprite to generate the box from.
*   @param   x The x coordinate being checked.
*   @param   y The y coordinate being checked.
*   @return  void
*/
bool NemoGame::isInside(const ASGE::Sprite* sprite, float x, float y) const
{
	auto sprite_min_x = sprite->xPos();
	auto sprite_max_x = sprite->xPos() + sprite->width();
	auto sprite_min_y = sprite->yPos();
	auto sprite_max_y = sprite->yPos() + sprite->height();

	if((sprite_min_x <= x && x <= sprite_max_x) && 
	   (sprite_min_y <= y && y <= sprite_max_y))
	{
		return true;	
	}
	else
	{
		return false;
	}
}