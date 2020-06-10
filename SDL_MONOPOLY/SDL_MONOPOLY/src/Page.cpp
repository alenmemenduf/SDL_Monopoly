#include "../headers/Page.h"
#include <string>
#include "../headers/Prompt.h"

Page::Page(Game* game, int i) {
	this->game = game;
	this->pageNumber = i;

	std::string text = "Player" + std::to_string(i);
	nameLabel = new UILabel(100, 30, 22, 10, text, TTF_OpenFont("assets/fonts/lucida_sans.ttf", 60), *(new SDL_Color({0,0,0,0})));
	nameLabel->setLabelTexture(game->getScreenW());
	nameLabel->setScale(game->getScreenW(), game->getScreenH());
	nameLabel->updateXY(5, 5);
	
	std::string money = std::to_string(game->getPlayer(this->pageNumber - 1)->getMoney());
	moneyLabel = new UILabel(100, 41, 22, 10, money, TTF_OpenFont("assets/fonts/lucida_sans.ttf", 60), *(new SDL_Color({ 0,0,0,0 })));
	moneyLabel->setLabelTexture(game->getScreenW());
	moneyLabel->setScale(game->getScreenW(), game->getScreenH());
	moneyLabel->updateXY(5, 5);
}

void Page::listen_event() {
}

void Page::render() {
	nameLabel->render();
	std::string money = std::to_string(game->getPlayer(this->pageNumber - 1)->getMoney());
	moneyLabel->setMessage(money);
	moneyLabel->setLabelTexture(game->getScreenW());
	moneyLabel->render();
}