#pragma once
#include <string>
#include <Engine/OGLGame.h>

/**
*  Pairs. An OpenGL Game based on ASGE.
*/
class NemoGame :
	public ASGE::OGLGame
{
public:
	NemoGame();
	~NemoGame();
	virtual bool init() override;

private:
	void keyHandler(const ASGE::SharedEventData data);
	void clickHandler(const ASGE::SharedEventData data);
	void spawn();

	virtual void update(const ASGE::GameTime &) override;
	virtual void render(const ASGE::GameTime &) override;

	bool isInside(const ASGE::Sprite* sprite, float x, float y) const;

	int  key_callback_id = -1;	        /**< Key Input Callback ID. */
	int  mouse_callback_id = -1;        /**< Mouse Input Callback ID. */
	ASGE::Sprite* background = nullptr; /**< Sprite Object. The background sprite. */
	ASGE::Sprite* clownfish = nullptr;  /**< Sprite Object. The clown fish sprite. */

	bool in_menu = true;
	int menu_option = 0;
	int score = 0;
};