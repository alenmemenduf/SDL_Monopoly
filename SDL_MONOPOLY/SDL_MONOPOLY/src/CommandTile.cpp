#include "../headers/Properties.h"
#include "../headers/Player.h"
#include "../headers/UserAnimator.h"
#include <vector>
#include <string>
#define COMMANDS_SIZE 16
std::string allCommands[] =
{
	"Go to Start! Collect 200.",
	"Advance to Illinois Ave�If you pass Go, collect $200",
	"Advance to St. Charles Place � If you pass Go, collect $200",
	"Advance token to nearest Utility. If unowned, you may buy it from the Bank. If owned, throw dice and pay owner a total ten times the amount thrown.",
	"Advance token to the nearest Railroad and pay owner twice the rental to which he/she {he} is otherwise entitled. If Railroad is unowned, you may buy it from the Bank.",
	"Bank pays you dividend of $50",
	"Get Out of Jail Free",
	"Go Back 3 Spaces",
	"Go to Jail�Go directly to Jail�Do not pass Go, do not collect $200",
	"Make general repairs on all your property�For each house pay $25�For each hotel $100",
	"Pay poor tax of $15"
	"Take a trip to Reading Railroad�If you pass Go, collect $200",
	"Take a walk on the Boardwalk�Advance token to Boardwalk",
	"You have been elected Chairman of the Board�Pay each player $50",
	"You have won a crossword competition�Collect $100",
	"Your building and loan matures�Collect $150"
};
CommandTile::CommandTile(const std::string& name) : Tile(name) {
	if (name == "Chance"){
		backTexturePath = "assets/commands/chanceBack.bmp";
		texturePath = "assets/commands/";
		destX = 74;
		destY = 61;
		groupId = CHANCE;
	}
	else 
		if(name == "Community Chest"){
		backTexturePath = "assets/commands/communityChestBack.bmp";
		texturePath = "assets/commands/communityChestBack.bmp";
		destX = 29;
		destY = 17;
		groupId = CHEST;
		}
	else 
		if (name == "Income Tax") {
		backTexturePath = frontTexturePath = "assets/commands/incomeTax.bmp";
		texturePath = "assets/commands/incomeTax.bmp";
		groupId = INC_TAX;
		}
	else {
		backTexturePath = frontTexturePath = "assets/commands/luxTax.bmp";
		texturePath = "assets/commands/luxTax.bmp";
		groupId = LUX_TAX;
	}
	//upon initialization the texture path will be just the folder name;
	
}
void CommandTile::print() {
	std::cout << "Card Type is :" << name << ", thus its animation starts at (" << destX << "," << destY << ")\n";
}
void CommandTile::doEffect(Player *currentPlayer) {
	
	if (groupId == LUX_TAX) {
		UserAnimator::popPropertyCard(this);
		std::cout << "LUXURY TAX. PAY 100$ \n";
		currentPlayer->payMoney(100);
		SDL_Delay(500);
		return;
	}
	if (groupId == INC_TAX) {
		UserAnimator::popPropertyCard(this);
		std::cout << "INCOME TAX. PAY 200$ \n";
		currentPlayer->payMoney(200);
		SDL_Delay(500);
		return;
	}
	int randomExtraction = 0;// rand() % COMMANDS_SIZE;
	frontTexturePath = texturePath + std::to_string(randomExtraction) + ".bmp";
	//TO DO : ANIMATION FOR THE COMMAND TILE WITH ITS BACK
	UserAnimator::popPropertyCard(this);
	std::cout << allCommands[randomExtraction];
	switch (randomExtraction) {
	
	case 0:
		currentPlayer->setRemainingSteps(3);
		currentPlayer->setCommandFlag();//Player must advance to start;
		break;
	
	}
}