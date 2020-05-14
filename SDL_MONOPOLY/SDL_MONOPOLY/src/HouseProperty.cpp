#include "../headers/HouseProperty.h"
#include "../headers/Player.h"
#include "../headers/UserAnimator.h"
#define MAX_HOUSES 4
#define RENT_STAGES 6
HouseProperty::HouseProperty(std::string name, int buyPrice,int updateCost,int mortgageVal,std::vector<int>& rents, Groups groupId,int rectId) :AbstractProperty(name,buyPrice,updateCost,groupId){
	this->houseNumber = 0;
	this->hasHotel = false;
	this->rentStage = 0;
	this->texturePath = "assets/house_properties/" + std::to_string(rectId)+ ".bmp";
	this->renderable = false;
	this->houseOrientation = -1;
	this->mortgageVal = mortgageVal;
	this->mortgaged = false;
	//[ NO HOUSE, ONE HOUSE, TWO_HOUSES, THREE_HOUSES, FOUR_HOUSES, HOTEL]
	for (int i = 0; i < RENT_STAGES; i++) {
		rentPrices.push_back(rents[i]);
	}
}
HouseProperty::~HouseProperty() {

}
void HouseProperty::update() {
	//TO DO -- int answer = UserDialog::tileUpdateDialog()

	if (rentStage < rentPrices.size() - 1) {
		
		if (true) {
			if(owner->getMoney() >= updateCost){
				owner->payMoney(updateCost);
				if (houseNumber < MAX_HOUSES) {
					Sprite* newHouse;
					houseOrientation = (houseOrientation < 0) ? owner->getCurrentPosition() / 10 : houseOrientation;
					if (houseNumber == 0) {
						std::string filePath = "assets/house_properties/house_" + std::to_string(houseOrientation) + ".bmp";
						int unitX = owner->getSprite()->unitX();
						int unitY = owner->getSprite()->unitY();
						switch (houseOrientation) {
						case 0:
							unitY -= 4; unitX -= 3;
							break;
						case 1:
							unitX += 5; unitY -= 4;
							break;
						case 2:
							unitY += 6; unitX += 3;
							break;
						case 3:
							unitX -= 5; unitY += 3;
							break;
						}
						newHouse = new Sprite(filePath.c_str(), 3, 3,0,0,-1,-1,true);
						newHouse->copyScale(owner->getSprite());
						newHouse->setUnitXY(unitX,unitY);
						
					}
					else {
						newHouse = new Sprite(houses[houseNumber - 1]);
						int unitX = houses[houseNumber - 1]->unitX();
						int unitY = houses[houseNumber - 1]->unitY();
						switch (houseOrientation) {
						case 0:
							unitX += 2;
							break;
						case 1:
							unitY += 2;
							break;
						case 2:
							unitX -= 2;
							break;
						case 3:
							unitY -= 2;
							break;
						}
						newHouse->setUnitXY(unitX, unitY);
					}
					rentStage++;
					houses[houseNumber++] = newHouse;
					std::cout << "Property Updated. " << name << " now has" << houseNumber << " houses and rets for " << getRentPrice() << std::endl;
				}
				else {
					std::cout << "It appears" << name << " has already maximum number of houses. Update to a Hotel instead ? 1/0";
					int answer2;
					std::cin >> answer2;
					if (Game::isBuyPressed() == 1 && owner->getMoney() >= updateCost) {
						if (owner->ownsAllOfColor(groupId)) {
							owner->payMoney(updateCost);
							Sprite* hotel = new Sprite(("assets/house_properties/hotel_" + std::to_string(houseOrientation) + ".bmp").c_str(), 3, 4, 0, 0, -1, -1, true);
							hotel->copyScale(houses[1]);
							hotel->setUnitXY(houses[1]->unitX(), houses[1]->unitY());

							houseNumber = 0;
							houses[houseNumber++] = hotel;
							hasHotel = true;
							std::cout << "Property Updated. " << name << " now has one hotel and rents for " << getRentPrice() << std::endl;
						}
						else {
							std::cout << "You do not own all the " << groupId << " properties \n";
						}
					}
					else {
						std::cout << "Update failed due to bakruptcy" << std::endl;
					}
				}
			}
			else {
				std::cout << "Update failed due to bakruptcy" << std::endl;
			}
			//Game::setBuyPressed(false);
		}
	}
	
}
void HouseProperty::mortgage(Player* currentPlayer) {
	if (currentPlayer == owner) {
		if (!mortgaged) {

			if (Game::isMortgagePressed()) {
				/*If it is an improved property, The player must destroy all the houses and hotels form the same colored tiles
				*/
				if (houseNumber > 0) {
					owner->destroyHousesFromColor(groupId);
				}
				mortgaged = true;
				owner->receiveMoney(mortgageVal);
				Game::setMortgagePressed(false);
			}
		}
		else {
			std::cout << "It appears you have mortgaged this property. Lifting the mortgage would mean paying " <<
				"The mortgage Value of the property + 10 % of its value for a total of  " << mortgageVal * 1.1 << ". Proceed? (1/0).";
			int answer;
			std::cin >> answer;
			if (answer == 1) {
				if (owner->getMoney() < buyPrice * 1.1) {
					std::cout << " Insufficient Founds to cpmlete action.\n";
					return;
				}
				owner->payMoney(mortgageVal * 1.1);
				std::cout << "Mortgage lifted for " << name << "\n";
				mortgaged = false;

			}
		}
	}
	else {
		std::cout << "You do not own this property\n";
	}
	
}
int HouseProperty::getHousesPrice() {
	if (hasHotel)
		return 100;
	return houseNumber * 25;
}
void HouseProperty::destroyHouses() {
	std::cout << " Removing " << houseNumber << " houses from " << name << std::endl;
	for (int i = 0; i < houseNumber; i++) {
		delete houses[i];
	}
	houseNumber = 0;
}
//Function that will be called as a listener if the button "Buy" is pressed in game
void HouseProperty::getMeAnOwner(Player* currentPlayer) {
	if (owner == NULL) {
		//Checks if the player pressed the buy button 
		if (Game::isBuyPressed()) {
			if (currentPlayer->getMoney() < buyPrice) {
				std::cout << " Aquisition failed. Lack of funds `\( `-`)/` ";
			}
			else {
				owner = currentPlayer;
				currentPlayer->buyProperty(this);
			}
			Game::setBuyPressed(false);
			UserAnimator::fadePropertyCard(this);
		}
	}
	else {
		if (owner != currentPlayer && mortgaged) {
			std::cout << "Please Wait while " << currentPlayer->getName() << " and" << owner->getName() << "are negotiating for " << name << std::endl;
			currentPlayer->startTrade(owner);
			currentPlayer->setBuyerTradeFlag();
			owner->startTrade(currentPlayer);
			owner->setOwnerTradeFlag();

		}
		
	}
}

void HouseProperty::doEffect(Player* currentPlayer) {
	std::cout << currentPlayer->getName() <<" stepped on " << name << std::endl;
	if (owner == currentPlayer) {
		if(!mortgaged && rentStage < rentPrices.size() - 1)
			update();
		mortgage(currentPlayer);
	}
	else if (owner == NULL) {
		UserAnimator::popPropertyCard(this);
		getMeAnOwner(currentPlayer);		
	}
	else if(currentPlayer != owner){
		if (!mortgaged) {
			std::cout << currentPlayer->getName() << " needs to pay " << getRentPrice() << " to " << owner->getName() << std::endl;
			UserAnimator::playerPaysPlayer(currentPlayer,owner);
			owner->receiveMoney(getRentPrice());
			currentPlayer->payMoney(getRentPrice());
		}
		else {
			std::cout << owner->getName() << " chose to mortgage this property, so no rent is payed. Press Start to start trading with the owner.\n";
			//getMeAnOwner(currentPlayer);
		}
	}
}
void HouseProperty::print() {
	if (owner) {
		std::cout << name << " Color: " << groupId << " Rents for: " << getRentPrice() << " Has houses: " << houseNumber;
		std::cout << "Current Owner : " << owner->getName();
		std::cout << mortgaged ? " Currentyl mortgaged" : " Currently not mortgaged";
	}
	else {
		std::cout << name << " Color: " << groupId << " Buys for: " << buyPrice << " Rents for: " << getRentPrice();
		std::cout << "Not Owned by Anoyone \n";
	}
}
bool HouseProperty::isRenderable() {
	return houseNumber > 0;
}

void HouseProperty::render() {
	if (isRenderable()) {
		for (int i = 0; i < houseNumber; i++) {
			houses[i]->render();
		}
	}
}

