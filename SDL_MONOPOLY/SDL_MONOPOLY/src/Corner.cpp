#include "../headers/Properties.h"
#include "../headers/Corner.h"
#include "../headers/UserAnimator.h"
Corner::Corner(std::string& name, std::string& command,int cornerNo):Tile(name){
	this->command = command;
	this->cornerNo = cornerNo;
	this->groupId = CORNER;
}

Corner::~Corner() {

}

void Corner::getMeAnOwner(Player* currentPlayer) {
	return;
}

void Corner::mortgage(Player* currentPlayer) {
	return;
}

void Corner::doEffect(Player * currentPlayer) {
	print();
	//UserAnimator::cornerAnimation(this);
	switch (cornerNo) {
	case 0:
		std::cout << currentPlayer->getName() << " passed START. It will recieve 200\n";
		currentPlayer->receiveMoney(200);
		break;
	case 3:
		std::cout << "Bad Luck! " << currentPlayer->getName() << " will go to jail";
		currentPlayer->goToJail();
		break;
		
	}

}