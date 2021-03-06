#pragma warning( disable : 4244 ) 
#pragma warning( disable : 4018 ) 
#include "../headers/Game.h"
#include "../headers/TextureMaker.h"
#include "../headers/UserAnimator.h"
#include "../headers/GameStateManager.h"

#include <string>
#include <vector>
#include <utility>
#include <fstream>
#define TILE_NUM 39
#define PROP_NUM 22
#define CARD_NUM 6
#define UTIL_NUM 2
#define STATION_NUM 4
#define CORNER_NUM 4
#define UPPER_RENTS_BOUND 10
#define PAWN_SIZE 2			 //changing to GAME UNITS
#define NUMBER_OF_PLAYERS 4
#define START_X 88
#define START_Y 89

#define DICE_MOVE 0
#define MUST_BE_JAILED 1
#define EXEC_COMMAND 2
#define BUYER_TRADE 4
#define OWNER_TRADE 3


//Array of indexes for the HouseProperty properties
// tiles[propIdx[i]] = new HouseProperty();
int propIdx[] = { 1,3,			// BROWN
					6,8,9,		//LTBLUE				
					11,13,14,	//PURPLER
					16,18,19,	//ORANGE
					21,23,24,	//RED
					26,27,29,	//YELLOW
					31,32,34,	//GREEN
					37,39 };		//BLUE

//Array of indexes for the Card type

int cardIdx[] = { 2,7,17,22,33,36 };
int stationIdx[] = { 5,15,25,35 };
int utilIdx[] = { 12,28 };
int Game::nrDoublesThrown = 0;
Dice* Game::dice = nullptr;
SDL_Renderer* Game::renderer = nullptr;
int last = 0;
int globalSum = 0;
int Game::count = 0;
int Game::mouseX = 0;
int Game::mouseY = 0;
int Game::clickX = 0;
int Game::clickY = 0;

bool Game::buyPressed = false;
bool Game::mortgagePressed = false;
bool Game::mousePressed = false;
bool Game::keyStroke = false;
bool Game::backPressed = false;
bool Game::enterPressed = false;

char Game::inChar;


int debounceDelay = 300;
int lastDebounce = 0;
std::string messageString;
int lastTurnToPressBuy = -1;

Game::Game(int width, int height,std::vector<std::string>&playerNames):tiles(40){
	mousePressed = false;
	turn = 0;
	if (TTF_Init() == -1) {
		std::cerr << "TTF init error \n";
		isRunning = false;
		return;
	}
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
		setRenderer(GameStateManager::getInstance()->getRenderer());
		UserAnimator::attach(this);
		background = new Sprite("assets/board.bmp", width, height, 0, 0);
		if (!background)
			isRunning = false;
		background->setScale(100, 100);	
		this->screenWidth = width;
		this->screenHeight = height;
		dice = new Dice();
		dice->getFirstDieSprite()->setScale(width, height);
		dice->getSecondDieSprite()->setScale(width, height);

		/*TO DO 
			The players are now loaded via the playerName vector
			Aka As many Players as names in the vector. 
			Work around to set the propper pawn to everyone. U could rename the pawns like 'i_car.bmp' for i = 0 -> 4;
		*/
		
		std::vector<const char *> playerIcon = {"assets/car.bmp", "assets/ship.bmp", "assets/plane.bmp", "assets/train.bmp"};

		for (int i = 0; i < playerNames.size(); i++) {
			players.push_back(new Player(playerNames[i], playerIcon[i], START_X + i, START_Y - 2, PAWN_SIZE + 3, PAWN_SIZE + 3));
			players[i]->setSpriteScale(width, height);
		}
		
		/*players.push_back(new Player(playerNames[i], "assets/car.bmp", START_X, START_Y, PAWN_SIZE + 3, PAWN_SIZE + 3));
		players.push_back(new Player(playerNames[i], "assets/ship.bmp", START_X + 2, START_Y, PAWN_SIZE + 3, PAWN_SIZE + 3));
		players.push_back(new Player(playerNames[i], "assets/plane.bmp", START_X + 4, START_Y, PAWN_SIZE + 3, PAWN_SIZE + 3));
		players.push_back(new Player(playerNames[i], "assets/train.bmp", START_X + 6, START_Y, PAWN_SIZE + 3, PAWN_SIZE + 3));
		for (Player* p : players)
			p->setSpriteScale(width, height);*/

		std::string txt = "Current player: ";
		currentPlayerLabel = new UILabel(98, 9, 19, 8, txt, TTF_OpenFont("assets/fonts/lucida_sans.ttf", 60), *(new SDL_Color({ 0,0,0,0 })));
		currentPlayerLabel->setLabelTexture(width);
		currentPlayerLabel->setScale(width, height);
		currentPlayerLabel->updateXY(5, 5);
		// This outputs "Current player: "

		currentPlayerSprite = new Sprite("assets/car.bmp", 6, 6, 122, 15);
		currentPlayerSprite->setScale(width, height);


		/* These constants represent the values for the next buttons
		   at the top of menu screen (the ones that change the currentPage).
		   This is a dirty method to display the BLACK/WHITE button sprite if a player doesn't exist,
		   hence it won't let the player "click" it. It's plain garbage.
		*/

		int PAGESPRITEWIDTH = 5; 
		int PAGESPRITEHEIGHT = 5;
		int PAGESPRITECOORDX = 103;
		int PAGESPRITECOORDY = 7;
		int PAGESPRITEOFFSET = 6;

		playerPageButtons.push_back(new Button("assets/menu/car_button.bmp", "assets/menu/car_button1.bmp", "assets/menu/car_button2.bmp", PAGESPRITECOORDX, PAGESPRITECOORDY, PAGESPRITEWIDTH, PAGESPRITEHEIGHT, doesPlayerExist("car") ? NORMAL : NOPLAYER));
		playerPageButtons.push_back(new Button("assets/menu/ship_button.bmp", "assets/menu/ship_button1.bmp", "assets/menu/ship_button2.bmp", PAGESPRITECOORDX + PAGESPRITEOFFSET, PAGESPRITECOORDY, PAGESPRITEWIDTH, PAGESPRITEHEIGHT, doesPlayerExist("ship") ? NORMAL : NOPLAYER));
		playerPageButtons.push_back(new Button("assets/menu/plane_button.bmp", "assets/menu/plane_button1.bmp", "assets/menu/plane_button2.bmp", PAGESPRITECOORDX + PAGESPRITEOFFSET * 2, PAGESPRITECOORDY, PAGESPRITEWIDTH, PAGESPRITEHEIGHT, doesPlayerExist("plane") ? NORMAL : NOPLAYER));
		playerPageButtons.push_back(new Button("assets/menu/train_button.bmp", "assets/menu/train_button1.bmp", "assets/menu/train_button2.bmp", PAGESPRITECOORDX + PAGESPRITEOFFSET * 3, PAGESPRITECOORDY, PAGESPRITEWIDTH, PAGESPRITEHEIGHT, doesPlayerExist("train") ? NORMAL : NOPLAYER));
		for (int i = 0; i < playerPageButtons.size(); i++)
			playerPageButtons[i]->getSprite()->setScale(width, height);

		buttons.push_back(new Button("assets/menu/buy_button.bmp", "assets/menu/buy_button1.bmp", "assets/menu/buy_button2.bmp", 105, 64, 20, 7));
		buttons.push_back(new Button("assets/menu/sell_button.bmp", "assets/menu/sell_button1.bmp", "assets/menu/sell_button2.bmp", 105, 72, 20, 7));
		buttons.push_back(new Button("assets/menu/button_end_turn.bmp", "assets/menu/button_end_turn1.bmp", "assets/menu/button_end_turn2.bmp", 105, 80, 20, 7));

 		for (int i = 0; i < buttons.size(); i++)
			buttons[i]->getSprite()->setScale(width, height);
		isRunning = true;
		fillTiles("assets/houseProperties.txt");
		menu = new Menu(this);
		
	}
	else {
		isRunning = false;
	}

}
Game::~Game() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}
SDL_Renderer* Game::getRenderer() {
	return renderer;
}

bool Game::doesPlayerExist(std::string playerString)
{
	char filepath[30] = "";
	if (playerString.compare("car") == 0) {
		strcpy_s(filepath,"assets/car.bmp");
	}else if (playerString.compare("ship") == 0) {
		strcpy_s(filepath, "assets/ship.bmp");
	} else if (playerString.compare("plane") == 0) {
		strcpy_s(filepath, "assets/plane.bmp");
	} else if (playerString.compare("train") == 0) {
		strcpy_s(filepath, "assets/train.bmp");
	}

	for (Player* x : players) {
		if (strcmp(x->getSprite()->getPath(), filepath) == 0)
			return true;
	}

	return false;
} // function that takes as an argument a string { "car", "ship", "plane", "train" } and returns true if the player exists

int Game::findPlayer(std::string playerString)
{
	char filepath[30] = "";
	if (playerString.compare("car") == 0) {
		strcpy_s(filepath, "assets/car.bmp");
	}
	else if (playerString.compare("ship") == 0) {
		strcpy_s(filepath, "assets/ship.bmp");
	}
	else if (playerString.compare("plane") == 0) {
		strcpy_s(filepath, "assets/plane.bmp");
	}
	else if (playerString.compare("train") == 0) {
		strcpy_s(filepath, "assets/train.bmp");
	}

	for (int i = 0; i < players.size(); i++) {
		if (strcmp(players[i]->getSprite()->getPath(), filepath) == 0)
			return i;
	}
	return -1;
} // function that takes as an argument a string { "car", "ship", "plane", "train" } and returns the index in player if the player exists


int Game::getScreenW() {
	return screenWidth;
}
int Game::getScreenH() {
	return screenHeight;
}

SDL_Window* Game::getWindow() {
	return window;
}

Dice* Game::getDice() {
	return dice;
}

 void Game::listen_event() {
	 menu->listen_event();
	 SDL_Event e;
	 SDL_StartTextInput();
	 SDL_PollEvent(&e);
	 SDL_PumpEvents();
	 switch (e.type) {
	 case SDL_QUIT:
		 isRunning = false;
		 break;
	 case SDL_KEYDOWN:
		 if (e.key.keysym.sym == SDLK_BACKSPACE) {
			 GameStateManager::getInstance()->setBackFlag(true);
			backPressed = true;
		 }
		 else if (e.key.keysym.sym == SDLK_RETURN) {
			 GameStateManager::getInstance()->setEnterFlag(true);
			 enterPressed = true;
		 }
		 break;
	 case SDL_TEXTINPUT:
		 GameStateManager::getInstance()->setKeyStroke(true);
		 GameStateManager::getInstance()->setInChar(e.text.text[0]);
		 keyStroke = true;
		 inChar = e.text.text[0];
		 break;
	 case SDL_MOUSEBUTTONUP:
		 mousePressed = false;
		 GameStateManager::getInstance()->setMousePressed(false);
		 break;
	 case SDL_MOUSEBUTTONDOWN: {
		 if (e.button.state == SDL_PRESSED && e.button.button == SDL_BUTTON_LEFT) {
			 if (!mousePressed) {
				 mousePressed = true;
				 clickX = e.button.x;
				 clickY = e.button.y;
				 GameStateManager::getInstance()->setClickX(clickX);
				 GameStateManager::getInstance()->setClickY(clickY);
				 GameStateManager::getInstance()->setMousePressed(true);

				 std::cout << "Mouse X " << clickX << " Mouse Y :" << clickY << " \n";
				 lastDebounce = SDL_GetTicks();
				 if (dice->getFirstDieSprite()->isClicked() || dice->getSecondDieSprite()->isClicked()) {
					 if (!dice->isBlocked()) {
						 dice->roll(renderer);
						 std::cout << "Ai dat " << dice->getFirstDieValue() + dice->getSecondDieValue() << std::endl;
						 if (players[turn]->isJailed()) {
							 if (dice->thrownDouble()) {
								 players[turn]->freeFromJail();
							 }
							 else {
								 int turns = players[turn]->getJailTurnsLeft();
								 players[turn]->setJailTurnsLeft(turns - 1); // decrement jail turn left for players at current turn
								 if (!players[turn]->isJailed())
									 players[turn]->freeFromJail();
							 }
							 dice->setBlocked(true);
							 Game::nrDoublesThrown = 0;
						 }
						 else {
							 if (dice->thrownDouble()) {
								 Game::nrDoublesThrown++;
							 }

							 if (Game::nrDoublesThrown == 3) {
								 players[turn]->setJailFlag();
								 players[turn]->goToJail();
								 dice->setBlocked(true); //Set the dice block so while the current player is moving nobody can run the dice;
							 }
							 else {
								 players[turn]->setRemainingSteps(dice->getFirstDieValue() + dice->getSecondDieValue());

								 /*			DEBUG
								 /**/
								 //players[turn]->setRemainingSteps(1);
								
								 if (!dice->thrownDouble()) {
									 dice->setBlocked(true);
									 Game::nrDoublesThrown = 0;
								 }
							 }
						 }
					 }
				 }
				
				 else if (buttons[0]->getSprite()->isClicked()) {

					 if (!dice->isBlocked()) {
						 messageString = "You must roll the dice first!";
						 UserAnimator::popUpMessage(messageString);
					 }
					 else {
						 std::cout << "button0" << std::endl;
						 this->setBuyPressed(true);
						 if (players.size() == 1 || lastTurnToPressBuy != turn) {
							 tiles[players[turn]->getCurrentPosition()]->getMeAnOwner(players[turn]); 
							//tiles[12]->getMeAnOwner(players[turn]);
							 lastTurnToPressBuy = turn;
						 }
						 else
							 std::cout << "BUY WAS ALREADY PRESSED";
						 this->setBuyPressed(false);
					 }
				 }
				 else if (buttons[1]->getSprite()->isClicked()) {
						 this->setMortgagePressed(true);
						 std::cout << "button1" << std::endl;
						 //dynamic_cast<HouseProperty*>(tiles[players[turn]->getCurrentPosition()])->mortgage(players[turn]);
						 tiles[players[turn]->getCurrentPosition()]->mortgage(players[turn]);
						 //dynamic_cast<HouseProperty*>(tiles[1])->mortgage(players[turn]);
				 }
						 
					 //}
					 else if (buttons[2]->getSprite()->isClicked()) {

						if (players[turn]->getRemainingSteps() != 0) {
							std::string msg = "You must wait to finish moving to be able to do that!";
							UserAnimator::popUpMessage(msg);
							}

						else if (!(players[turn]->getFlag() == BUYER_TRADE || players[turn]->getFlag() == OWNER_TRADE)) {
							 lastTurnToPressBuy = turn;
							 turn++;
							 turn %= players.size();
							 dice->setBlocked(false);
							 this->setBuyPressed(false);
							 this->setMortgagePressed(false);
							 UserAnimator::fadePropertyCard(tiles[players[turn]->getCurrentPosition()]);
							 // DEBUG PURPOSES UserAnimator::fadePropertyCard(tiles[1]);
							 menu->setCurrentPage(turn);
							 std::cout << "button2" << std::endl;
						 }
						 else {
							 messageString = "You must finish the trade before ending your turn !";
							 UserAnimator::popUpMessage(messageString);
						 }
					 }
					 else if (playerPageButtons[0]->getSprite()->isClicked() && 
						      playerPageButtons[0]->getButtonState() != NOPLAYER)
								    menu->setCurrentPage(findPlayer("car"));
					 else if (playerPageButtons[1]->getSprite()->isClicked() && 
							  playerPageButtons[1]->getButtonState() != NOPLAYER)
								    menu->setCurrentPage(findPlayer("ship"));
					 else if (playerPageButtons[2]->getSprite()->isClicked() && 
							  playerPageButtons[2]->getButtonState() != NOPLAYER)
									menu->setCurrentPage(findPlayer("plane"));
					 else if (playerPageButtons[3]->getSprite()->isClicked() && 
							  playerPageButtons[3]->getButtonState() != NOPLAYER)
									menu->setCurrentPage(findPlayer("train"));
				 mousePressed = true;
			 }
			 else {
				 mousePressed = false;
				 GameStateManager::getInstance()->setMousePressed(false);
			 }
		 }
		 break;
		}
	 }
 }
 void Game::render() {
	 SDL_RenderClear(renderer);
	 background->render();
	 menu->render();
	 currentPlayerLabel->render();
	 currentPlayerSprite->render();
	 for (int i = 0; i < PROP_NUM; i++) {
		 tiles[propIdx[i]]->render();
	 }
	 for (int i = 0; i < players.size(); i++)
		 players[i]->render();
	
	 for (int i = 0; i < buttons.size(); i++)
		 buttons[i]->render();

	 for (int i = 0; i < playerPageButtons.size(); i++)
		 playerPageButtons[i]->render();

	 dice->render();
	 //test->render();
	 UserAnimator::render();
	 SDL_RenderPresent(renderer);
}

 void Game::update() {
	 SDL_GetMouseState(&mouseX, &mouseY);
	 for (int i = 0; i < players.size(); i++) {
		 players[i]->update();

		 /*
		 Daca la iteratia curenta a Game::update() playerul a carui de la tura curent terminat sa de mutat ,i.e. remainingSteps == 0,
		 Vedem in functie de flagType ul pe care il avea setat ce se intampla cu el
		 */

		 if (i == turn && players[turn]->finishedMoving()) {
			 switch (players[turn]->getFlag()) {
			 case DICE_MOVE:
				 tiles[players[turn]->getCurrentPosition()]->doEffect(players[turn]);
				 //DEBUG :
				 //tiles[30]->doEffect(players[turn]);
				 break;
			 case MUST_BE_JAILED:
				 players[turn]->goToJail();
				 break;
			 case EXEC_COMMAND:
				 std::cout << players[turn]->getName() << " finished the command." << std::endl;
				 /*Dupa ce termina command, reintra in starea de "Sunt gata sa mut dupa cum zice zarul"*/
				 UserAnimator::fadePropertyCard(nullptr);

				 if (players[turn]->getCurrentPosition() == 28 || players[turn]->getCurrentPosition() == 12) {
					 if (dynamic_cast<UtilityProperty*>(tiles[players[turn]->getCurrentPosition()])->getOwner() != NULL
						 && !players[turn]->isBankrupt()) {
						 std::cout << players[turn]->getName() << " has to pay something\n";
						 dynamic_cast<UtilityProperty*>(tiles[players[turn]->getCurrentPosition()])->getOwner()->receiveMoney(dice->getFirstDieValue() + dice->getSecondDieValue() * 10);
						 players[turn]->payMoney(dice->getFirstDieValue() + dice->getSecondDieValue() * 10);
					 }
					 else
						 tiles[players[turn]->getCurrentPosition()]->doEffect(players[turn]);
				 }
				 else
					tiles[players[turn]->getCurrentPosition()]->doEffect(players[turn]);
				 players[turn]->setDiceFlag();
				 break;
			 case BUYER_TRADE:
				 if (!players[turn]->onGoingTrade()){
					 players[turn]->buyProperty(tiles[players[turn]->getCurrentPosition()], true);
					 players[turn]->setDiceFlag();

				 }
				 break;
			
			 }
		  }

	 }
	 
	 for (int i = 0; i < buttons.size(); i++) {
		 buttons[i]->update(mouseX, mouseY);
	 }
	 for (int i = 0; i < playerPageButtons.size(); i++) {
		 playerPageButtons[i]->update(mouseX, mouseY);
	 }
	 menu->update();
	 dice->update();

	 
	 if (currentPlayerSprite != NULL) {
		 if (strcmp(currentPlayerSprite->getPath(), players[turn]->getSprite()->getPath()))
		 {
			 delete currentPlayerSprite;
			 currentPlayerSprite = new Sprite(players[turn]->getSprite()->getPath(), 6, 6, 122, 15);
			 currentPlayerSprite->setScale(this->getScreenW(), this->getScreenH());
		 }
			
	 }
	 // This is where we'll have current player's icon displayed

	 UserAnimator::update();
	 //test->update();
 }

 Groups getGroupId(int i) {
	 Groups color = RED;
	 switch (i) {
	 case 0: color = BROWN;
		 break;
	 case 1: color = LTBLUE;
		 break;
	 case 2: color = PURPLE;
		 break;
	 case 3: color = ORANGE;
		 break;
	 case 4: color = RED;
		 break;
	 case 5: color = YELLOW;
		 break;
	 case 6: color = GREEN;
		 break;
	 case 7: color = BLUE;
		 break;
	 case 10: color = STATION;
		 break;
	 }
	 return color;
 }

 std::string parse(const std::string & line, const char token) {
	 std::string result = "";
	 int pos = line.find(token);
	 int prev = 0;
	 while (pos != std::string::npos) {
		 result += line.substr(prev,pos - prev);
		 result += " ";
		 prev = pos + 1;
		 pos = line.find(token, pos + 1);
	 }
	 result += line.substr(prev, pos - prev);
	 return result;
 }
 void Game::fillTiles(const char *filePath) {
	 std::ifstream fin(filePath);
	 if (!fin) {
		 std::cout << "FILE NOT FOUND";
		 return;
	 }
	 int pid = 0, cid = 0, sid = 0, uid = 0;
	 int groupID;
	 std::string name;
	 std::string command;
	 int buyPrice, updateCost, rentStages,mortgage;
	 std::vector<int>rentPrices(UPPER_RENTS_BOUND);	//dirty technique, but what can you do  `\ (' - ') /`
	 Groups color;
	 tiles[4] = new CommandTile("Income Tax");
	 tiles[38] = new CommandTile("Luxury Tax");
	 for (int i = 0; i < tiles.size(); i++) {
		 //For HouseProperty
		 if (pid < PROP_NUM  && i == propIdx[pid]) {
			 fin >> name >> buyPrice >> updateCost >> mortgage >> groupID;
			 name = parse(name, '_');
			 color = getGroupId(groupID);
			 rentStages = 6;
			 for (int i = 0; i < rentStages; i++)
				 fin >> rentPrices[i];
			 tiles[i] = new HouseProperty(name, buyPrice,updateCost,mortgage,rentPrices,color,i);
		
			 pid++;
		 }
		 //For StationProperty
		 else if (sid < STATION_NUM && i == stationIdx[sid]) {
			 fin >> name;
			 name = parse(name, '_');
			 tiles[i] = new StationProperty(name,200,0,STATION,i);
		
			 sid++;
		 }
		 //For UtilityProperty
		 else if (uid < UTIL_NUM && i == utilIdx[uid]) {
			 fin >> name;
			 name = parse(name, '_');
			 tiles[i] = new UtilityProperty(name, 150, 0, UTIL, i);
		
			 uid++;
		 }
		 //For Corners
		 else if (i % 10 == 0) {
			 fin >> name >> command;
			 name = parse(name, '_');
			 command = parse(command, '_');
			 tiles[i / 10 * 10 ] = new Corner(name,command,i / 10);

		 }
		 //For Chance, Community Chest, Income Tax and Luxury Tax
		 else if (cid < CARD_NUM && i == cardIdx[cid]) {
			 if (cid % 2 == 0)
				 tiles[i] = new CommandTile("Community Chest");
			 else
				 tiles[i] = new CommandTile("Chance");
			 cid++;
		 }
	 }

 }

 Menu* Game::getMenu() {
	 return menu;
 }