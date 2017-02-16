

#include "UI/AdvanceUI/wSlider.h"
#include "UI/AdvanceUI/wTextField.h"
#include "UI/AdvanceUI/wButton.h"
#include "UI/AdvanceUI/wText.h"

#include "Screens/Object/PlayerGroup.h"
#include "Screens/Object/CardUtils.h"
#include "Screens/Object/Card.h"
#include "Screens/Object/SliderBet.h"

#include "Screens/GamePlayMgr.h"

#include "Network/Game/GameService.h"
#include "Network/IwinProtocol.h"
#include "Network/Global/Globalservice.h"

#include "UI/AdvanceUI/wListViewTable.h"

#include "CoTuongScreen.h"
#include "Screens/NewLobbyScreen.h"
#include "Utils.h"
#include "AnimUtils.h"
#include "CoTuongBoard.h"
#include "CoTuongPiece.h"
#include "Common/Common.h"
#include "Common/CoPhoMgr.h"

int s_mapNewBoard[10][9] = { { PIECE_ROOK, PIECE_KNIGHT, PIECE_BISHOP, PIECE_ADVISOR, PIECE_KING, PIECE_ADVISOR, PIECE_BISHOP, PIECE_KNIGHT, PIECE_ROOK },
						{ PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID },
						{ PIECE_INVALID, PIECE_CANNON, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_CANNON, PIECE_INVALID },
						{ PIECE_PAWN, PIECE_INVALID, PIECE_PAWN, PIECE_INVALID, PIECE_PAWN, PIECE_INVALID, PIECE_PAWN, PIECE_INVALID, PIECE_PAWN },
						{ PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID },
						{ PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID },
						{ PIECE_PAWN, PIECE_INVALID, PIECE_PAWN, PIECE_INVALID, PIECE_PAWN, PIECE_INVALID, PIECE_PAWN, PIECE_INVALID, PIECE_PAWN },
						{ PIECE_INVALID, PIECE_CANNON, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_CANNON, PIECE_INVALID },
						{ PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID },
						{ PIECE_ROOK, PIECE_KNIGHT, PIECE_BISHOP, PIECE_ADVISOR, PIECE_KING, PIECE_ADVISOR, PIECE_BISHOP, PIECE_KNIGHT, PIECE_ROOK }
						};
int s_mapNewBoard_CoUp[10][9] = { { PIECE_FACE, PIECE_FACE, PIECE_FACE, PIECE_FACE, PIECE_KING, PIECE_FACE, PIECE_FACE, PIECE_FACE, PIECE_FACE },
						{ PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID },
						{ PIECE_INVALID, PIECE_FACE, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_FACE, PIECE_INVALID },
						{ PIECE_FACE, PIECE_INVALID, PIECE_FACE, PIECE_INVALID, PIECE_FACE, PIECE_INVALID, PIECE_FACE, PIECE_INVALID, PIECE_FACE },
						{ PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID },
						{ PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID },
						{ PIECE_FACE, PIECE_INVALID, PIECE_FACE, PIECE_INVALID, PIECE_FACE, PIECE_INVALID, PIECE_FACE, PIECE_INVALID, PIECE_FACE },
						{ PIECE_INVALID, PIECE_FACE, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_FACE, PIECE_INVALID },
						{ PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID },
						{ PIECE_FACE, PIECE_FACE, PIECE_FACE, PIECE_FACE, PIECE_KING, PIECE_FACE, PIECE_FACE, PIECE_FACE, PIECE_FACE }
						};


rapidjson::Document CoPhoBoard::toJson()
{
	rapidjson::Document document;
	rapidjson::Document::AllocatorType& locator = document.GetAllocator();
	document.SetObject();

	rapidjson::Value copho_name(userNameRival.c_str(), userNameRival.size());
	document.AddMember("name", copho_name, locator);
	document.AddMember("win", win, locator);
	document.AddMember("winbet", winbet, locator);
	document.AddMember("avatar", avatar, locator);
	document.AddMember("myColor", myColor, locator);
	document.AddMember("result", result, locator);

	rapidjson::Value piece_steps(rapidjson::kArrayType);

	for (size_t i = 0; i < pieceSteps.size(); i++)
	{
		std::string description = pieceSteps[i].getDescription();
		rapidjson::Value one_step;
		one_step.SetString(description.c_str(), description.size(), locator);
		piece_steps.PushBack(one_step, locator);
	}
	document.AddMember("steps", piece_steps, locator);

	return document;
}


void CoPhoBoard::toData(rapidjson::Document& doc)
{
	userNameRival = doc["name"].GetString();
	win = doc["win"].GetInt64();
	winbet = doc["winbet"].GetInt();
	avatar = doc["avatar"].GetInt();
	myColor = doc["myColor"].GetInt();
	result = doc["result"].GetInt();
	const rapidjson::Value& steps = doc["steps"];
	for (rapidjson::SizeType i = 0; i < steps.Size(); i++)
	{
		PieceStep step;
		step.toData(steps[i].GetString());
		pieceSteps.push_back(step);
	}
}

CoTuongScreen* CoTuongScreen::getInstance()
{
	return static_cast<CoTuongScreen*>(GamePlay->GetCurrentGameScreen());
}

CoTuongScreen::CoTuongScreen()
{
	SetGameID(GameType::GAMEID_CHESS);
	SetMaxPlayer(2);
}

CoTuongScreen::~CoTuongScreen()
{
}


bool CoTuongScreen::InitGame(int game_id)
{
	bool res = GameScreen::InitGame(game_id);
	_cur_game_id = game_id;
	p_panel_game_screen->SetVisible(false);
	XMLMgr->OnLoadXMLData<CoTuongScreen>("CoTuong_Screen", std::mem_fn(&CoTuongScreen::LoadXMLGameScreenCustom), this);
	onCreateListPlayer("CoTuong_Screen");
	auto custom_root = p_panel_custom->GetWidgetChildByName(".custom_root");
	_board_widget = custom_root->GetWidgetChildByName(".board_bg");
	_banco_ruler_panel = custom_root->GetWidgetChildByName(".board_bg.banco_ruler");
	_result_panel = custom_root->GetWidgetChildByName(".result_layout");
	_lbl_center_bet = _board_widget->GetWidgetChildByName(".banco_win.center_bet");
	_lbl_chieu_tuong = _board_widget->GetWidgetChildByName(".banco_win.txt_chieu_tuong");
	_btn_draw = custom_root->GetWidgetChildByName(".btn_xinhoa");
	_btn_lose = custom_root->GetWidgetChildByName(".btn_xinthua");
	_btn_eat_pieces = custom_root->GetWidgetChildByName(".btn_ruong");
	_btn_touch_screen = custom_root->GetWidgetChildByName(".btn_touch_screen");
	_btn_clear_board = custom_root->GetWidgetChildByName(".btn_clear_board");
	_eat_pieces_bot = custom_root->GetWidgetChildByName(".panel_eat_pieces_bot");
	_eat_pieces_top = custom_root->GetWidgetChildByName(".panel_eat_pieces_top");
	_btn_play = custom_root->GetWidgetChildByName(".btn_play");
	_btn_pause = custom_root->GetWidgetChildByName(".btn_pause");
	_btn_close = custom_root->GetWidgetChildByName(".btn_close");
	_btn_co_chat = custom_root->GetWidgetChildByName(".btn_chat");
	_last_step_panel = custom_root->GetWidgetChildByName(".last_step");
	p_hud_layout->GetWidgetChildByName(".btn_chat")->SetVisible(false);
	_btn_eat_pieces->SetVisible(false);
	_btn_draw->SetVisible(false);
	_btn_lose->SetVisible(false);

	
	Sprite* spr;
	int row;

	Vec2 board_size = Vec2(GetGameSize().height, GetGameSize().height);
	Vec2 board_pos = Vec2(_btn_co_chat->GetPosition_TopLeft().x - board_size.x, 0);
	Vec2 board_image_pos;
	Vec2 board_image_size;
	cocos2d::Size piece_size = cocos2d::Size(board_size.x / 10, board_size.y / 10);
	_board_widget->SetSize(board_size);
	_board_widget->SetPosition(Vec2(_btn_co_chat->GetPosition_TopLeft().x - Utils::s_padding - board_size.x, 0));

	board_image_size = Vec2(piece_size.width * 8, piece_size.height * 9);
	board_image_pos = Vec2((board_size.x - board_image_size.x) / 2, (board_size.y - board_image_size.y) / 2);

	_board_widget->GetWidgetChildByName(".banco_win")->SetSize(board_image_size);
	_board_widget->GetWidgetChildByName(".banco_win")->SetPosition(board_image_pos);

	_board_ruler = _board_widget->GetWidgetChildByName(".banco_ruler");
	WidgetEntity* ruler_lbl;
	for (int i = 10; i >= 1; i--)
	{
		ruler_lbl = _board_ruler->GetWidgetChildByName(StringUtils::format(".number_%d", i));
		ruler_lbl->SetPosition(Vec2(board_image_pos.x / 3, board_image_pos.y + (10 - i) * piece_size.height));
	}

	for (char i = 'a'; i < 'j'; i++)
	{
		ruler_lbl = _board_ruler->GetWidgetChildByName(StringUtils::format(".number_%c", i));
		ruler_lbl->SetPosition(Vec2(board_size.x - (board_size.x - board_image_size.x) / 4 - piece_size.width * (i - 'a') , board_size.y - (board_size.y - board_image_size.y) / 4));
	}

	btn_quick_play->SetPosition(Vec2(board_pos.x + board_size.x / 2 + btn_quick_play->GetSize().x / 2, board_pos.y + _board_widget->GetWidgetChildByName(".banco_win")->GetPosition().y + btn_quick_play->GetSize().y / 2));
	p_panel_count_down->SetPosition(Vec2(board_pos.x + board_size.x / 2, board_pos.y + board_size.y * 3 / 4));
	_eat_pieces_bot->SetSize(Vec2(piece_size.width * 8, piece_size.height * 2));
	_eat_pieces_top->SetSize(Vec2(piece_size.width * 8, piece_size.height * 2));
	

	cocos2d::Size image_size = SpriteFrameCache::getInstance()->getSpriteFrameByName("gamescreen-cotuong-screen/chot.png")->getOriginalSizeInPixels();
	float pad = (_eat_pieces_bot->GetSize().x - piece_size.width * 8) / 8;
	for (int i = 0; i < 16; i++) {
		spr = Sprite::createWithSpriteFrameName("gamescreen-cotuong-screen/chot.png");
		_eat_pieces_spr.push_back(spr);
		if (i < 8) row = 1;
		else row = 0;
		spr->setPosition(Vec2(pad / 2 + piece_size.width / 2 + (piece_size.width + pad) * (i % 8), row * piece_size.height + piece_size.height / 2));
		spr->setScale(piece_size.width / image_size.width);
		_eat_pieces_bot->GetResource()->addChild(spr);
	}

	for (int i = 0; i < 16; i++) {
		spr = Sprite::createWithSpriteFrameName("gamescreen-cotuong-screen/chot.png");
		_lost_pieces_spr.push_back(spr);
		if (i < 8) row = 0;
		else row = 1;
		spr->setPosition(Vec2(pad / 2 + piece_size.width / 2 + ( piece_size.width + pad ) * (i % 8), row * piece_size.height + piece_size.height / 2));
		spr->setScale(piece_size.width / image_size.width);
		_eat_pieces_top->GetResource()->addChild(spr);
	}

	static_cast<ImageView*>(_board_widget->GetResource())->addTouchEventListener(CC_CALLBACK_2(CoTuongScreen::onTouchBoard, this));
	static_cast<ImageView*>(_board_widget->GetResource())->setTouchEnabled(true);
	
	_board = new CoTuongBoard();
	_board->setGameRoot(this);
	_board->setBoardNode(static_cast<Widget*>(_board_widget->GetResource()));
	_board->setCellSize(piece_size);
	_board->init(_board_widget->GetWidgetChildByName(".banco_win")->GetPosition(), _board_widget->GetWidgetChildByName(".banco_win")->GetSize());


	for (int i = 0; i < 2; i++)
	{
		_player_quotes[i] = static_cast<TextWidget*>(custom_root->GetWidgetChildByName(StringUtils::format(".quote%d.time", i)));
		_player_quotes[i]->GetParentWidget()->SetVisible(false);
	}
	_curSeatUpdateTime = -1;
	_last_piece = nullptr;

	_spr_kill_effect = Sprite::createWithSpriteFrameName("gamescreen-cotuong-screen/chot.png");
	_spr_kill_effect->setVisible(false);
	addChild(_spr_kill_effect);


	ListViewTableWidget* listview_copho = static_cast<ListViewTableWidget*>(_copho_panel);
	listview_copho->addEventListener(
		[this, listview_copho](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
	{
		if (type == EventType_SetCell::ON_SET_CELL)
		{
			int current_idx_to = listview_copho->GetCurrentIdxTo();
			setCoPhoSlot(_widget, current_idx_to);
		}
		else if (type == EventType_SetCell::ON_END_TOUCH_CELL)
		{
			size_t current_idx = listview_copho->GetCurrentIdxSelected();
			if (_replay_mode)
			{
				gotoStep(current_idx);
				showBtnPlay(true);
			}
				


			/*size_t current_idx = list_player_lobby->GetCurrentIdxSelected();
			if (current_idx >= 0 && current_idx < p_list_player_in_lobby.size())
			{
				auto idx_player = p_list_player_in_lobby.at(current_idx);

				int room_id = idx_player.getRoomId();
				int board_id = idx_player.getBoardId();
				bool password = idx_player.getPassword();
				if (!password)
				{
					GlobalService::joinBoard(room_id, board_id, "");
				}
			}
			else
			{
				PASSERT2(false, "have problem here!");
			}*/

		}
	

	});

	GetCoPhoMgr()->load(GameController::myInfo->IDDB);
	_copho_showing = false;

	p_text_wait->SetPosition(Vec2(_board_widget->GetPosition_Middle().x , (20 * GetGameSize().width /  this->GetDesignSize().x)));
	
	return true;
}

void CoTuongScreen::resetToNewBoard(int myColor, int turnColor)
{	
	_eat_pieces.clear();
	_lost_pieces.clear();
	_onReviceonEat = false;
	_numberMyLostPiece = 0;
	_numberMyEatPiece = 0;
	if(_cur_game_id == GameType::GAMEID_CHESS)
		_board->loadBoard(s_mapNewBoard, myColor, turnColor);
	else 
		_board->loadBoard(s_mapNewBoard_CoUp, myColor, turnColor);
}

void CoTuongScreen::OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	GameScreen::OnProcessWidgetChild(name, type_widget, _widget);
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "btn_xinhoa")
		{
			sendCommandChess((ubyte)30, GetRoomID(), GetBoardID());
			//showResult();
		}
		/*else if (name == "btn_in_game_menu")
		{
			notifyLastStep("truongpd");
		}*/
		else if (name == "btn_xinthua")
		{
			ScrMgr->OnShowDialog("notice", "are_you_sure_you_want_to", TYPE_DIALOG::DIALOG_TWO_BUTTON, "", [this](const char* call_back, const char* btn_name) {
				if (strcmp("ok_2_btn", btn_name) == 0) {
					this->sendCommandChess((ubyte)35, this->GetRoomID(), this->GetBoardID());
				}
			});
		}
		else if (name == "btn_ruong" || name =="btn_touch_screen")
		{
			showEatLostPieces();
		}
		else if (name == "btn_play")
		{
			showBtnPlay(false);
			playNext();

		}
		else if (name == "btn_pause")
		{
			showBtnPlay(true);
		}
		else if (name == "btn_close")
		{
			doQuit();
			((NewLobbyScreen*)ScrMgr->GetMenuUI(MENU_LAYER::NEW_LOBBY_SCREEN))->showHistory(true);
		}
		else if (name == "btn_clear_board")
		{
			clearBoard();
			_btn_clear_board->Visible(false);
		}

	}
}

int CoTuongScreen::Update(float dt)
{
	if (GameScreen::Update(dt) == 0)
	{
		return 0;
	}

	if (_curSeatUpdateTime != -1) {
		if (_curSeatUpdateTime == 0){
			_totalTimeToPlayPlayer1 -= dt;
			_player_quotes[_curSeatUpdateTime]->SetText(formatTimer(_totalTimeToPlayPlayer1));
		} else {
			_totalTimeToPlayPlayer2 -= dt;
			_player_quotes[_curSeatUpdateTime]->SetText(formatTimer(_totalTimeToPlayPlayer2));
		}
	}

	return 1;
}

void CoTuongScreen::LoadXMLGameScreenCustom(TiXmlDocument * p_objectXML)
{
	auto p_list_sample_dec = LoadWidgetCustom(p_objectXML);
	PASSERT2(p_list_sample_dec.size() > 0, "no widget loaded!");
	ParseIntoCustom(p_list_sample_dec.at("custom_root"), p_panel_custom, "panel_custom_menu");
	auto custom_root = p_panel_custom->GetWidgetChildByName(".custom_root");
	_copho_layout = custom_root->GetWidgetChildByName(".layout_copho");
	_copho_panel = custom_root->GetWidgetChildByName(".layout_copho.panel_copho.list_step");
	_copho_panel_parent = custom_root->GetWidgetChildByName(".layout_copho.panel_copho");
	_btn_copho = custom_root->GetWidgetChildByName(".layout_copho.btn_copho");
	static_cast<ButtonWidget*>(_btn_copho)->SetCallBack(CC_CALLBACK_2(CoTuongScreen::onTouchBtnCopho, this));

	static_cast<ListViewTableWidget*>(_copho_panel)->InitTable();

	for (auto it = p_list_sample_dec.begin(); it != p_list_sample_dec.end(); it++)
	{
		delete it->second;
	}
	p_list_sample_dec.clear();
}

void CoTuongScreen::HandleAfterInitPlayer()
{
	GameScreen::HandleAfterInitPlayer();
	if (p_list_msg_return.size() > 0)
	{
		for (auto msg : p_list_msg_return)
		{
			OnHandleMsg(msg);
			delete msg;
		}
	}
	p_list_msg_return.clear();


}

/**
* tinh toan duong di cua cac quan co
*/
/**
*
* @param p
* @param turnColor
* @param isLogic
*            : xac dinh trang thai dang tim de danh hay de check lam viec
*            khac (de khong ve co an duoc)
* @return
*/
std::vector<Vec2> CoTuongScreen::findMove(CoTuongPiece* p, int turnColor, bool isLogic) {
	std::vector<Vec2> listCanMove;
	switch (p->getID()) {
	case PIECE_PAWN:
		listCanMove = findPawn(p, turnColor, isLogic);
		break;
	case PIECE_CANNON:
		listCanMove = findCannon(p, turnColor, isLogic);
		break;
	case PIECE_ROOK:
	{
		int tmp_arr[4] = { -1, -1, -1, -1 };
		listCanMove = findRook(p, turnColor, tmp_arr, isLogic);
	}
	break;
	case PIECE_KNIGHT:
		listCanMove = findKnight(p, turnColor, isLogic);
		break;
	case PIECE_BISHOP:
		listCanMove = findBishop(p, turnColor, isLogic);
		break;
	case PIECE_ADVISOR:
		listCanMove = findAdvisor(p, turnColor, isLogic);
		break;
	case PIECE_KING:
		listCanMove = findKing(p, turnColor, isLogic);
		break;
	case PIECE_FACE:
		int id = getID_CoUp(p);
		if (id > 0) {
			CoTuongPiece* pp = CoTuongPiece::create();
			pp->setID(getID_CoUp(p), p->getMau());
			pp->setRow(p->getRow());
			pp->setCol(p->getCol());
			pp->setIsFaceUp(false);
			listCanMove = findMove(pp, turnColor, isLogic);
		}
		break;
	}
	return listCanMove;
}

int CoTuongScreen::getID_CoUp(CoTuongPiece* p) {
	if (p->getCol() % 2 == 0 && (p->getRow() == 3 || p->getRow() == 6)) {
		return PIECE_PAWN;
	}
	if ((p->getCol() == 1 || p->getCol() == 7) && (p->getRow() == 2 || p->getRow() == 7)) {
		return PIECE_CANNON;
	}
	if ((p->getCol() == 0 || p->getCol() == 8) && (p->getRow() == 0 || p->getRow() == 9)) {
		return PIECE_ROOK;
	}
	if ((p->getCol() == 1 || p->getCol() == 7) && (p->getRow() == 0 || p->getRow() == 9)) {
		return PIECE_KNIGHT;
	}
	if ((p->getCol() == 2 || p->getCol() == 6) && (p->getRow() == 0 || p->getRow() == 9)) {
		return PIECE_BISHOP;
	}
	if ((p->getCol() == 3 || p->getCol() == 5) && (p->getRow() == 0 || p->getRow() == 9)) {
		return PIECE_ADVISOR;
	}
	return 0;
}

/**
*
* @param p
* @param turnColor
*            : mau dang di
*/
std::vector<Vec2> CoTuongScreen::findAdvisor(CoTuongPiece* p, int turnColor, bool isLogic) {
	std::vector<Vec2> listCanMove;
	int c[2] = { p->getCol() + 1, p->getCol() - 1 };
	int r[2] = { p->getRow() + 1, p->getRow() - 1 };
	int minRow, maxRow, minCol, maxCol;

	if (GameController::getCurrentGameType() == GameType::GAMEID_CHESS) {
		minCol = 3;
		maxCol = 5;
		if (turnColor == _board->getMyColor()) {
			minRow = 0;
			maxRow = 2;
		}
		else {
			minRow = 7;
			maxRow = 9;
		}
	}
	else {
		if (!p->getIsFaceUp()) {
			minCol = 3;
			maxCol = 5;
		}
		else {
			minCol = 0;
			maxCol = 8;
		}
		minRow = 0;
		maxRow = 9;
	}

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			if (c[j] >= minCol && c[j] <= maxCol && r[i] >= minRow && r[i] <= maxRow) {
				CoTuongPiece* dest = _board->getPiece(r[i], c[j]);
				if (dest != nullptr && dest->getMau() != turnColor) {
					if (!isLogic) {
						dest->setIsFood(true);
					}
					listCanMove.push_back(Vec2( r[i], c[j] ));
				}
				else if (dest == nullptr) {
					listCanMove.push_back( Vec2 ( r[i], c[j] ));
				}
			}
		}
	}
	return listCanMove;
}

std::vector<Vec2> CoTuongScreen::findBishop(CoTuongPiece* p, int turnColor, bool isLogic) {
	std::vector<Vec2> listCanMove;
	int c[2] = { p->getCol() + 2, p->getCol() - 2 };
	int r[2] = { p->getRow() + 2, p->getRow() - 2 };
	int cc[2] = { p->getCol() + 1, p->getCol() - 1 };
	int rc[2] = { p->getRow() + 1, p->getRow() - 1 };
	int minRow, maxRow;

	if (GameController::getCurrentGameType() == GameType::GAMEID_CHESS) {
		if (turnColor == _board->getMyColor()) {
			minRow = 0;
			maxRow = 4;
		}
		else {
			minRow = 5;
			maxRow = 9;
		}
	}
	else {
		minRow = 0;
		maxRow = 9;
	}

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			if (c[j] >= 0 && c[j] <= 8 && r[i] >= minRow && r[i] <= maxRow && _board->getBoardValue(rc[i], cc[j]) == 0) {
				CoTuongPiece* dest = _board->getPiece(r[i], c[j]);
				if (dest != nullptr && dest->getMau() != turnColor) {
					if (!isLogic) {
						dest->setIsFood(true);
					}
					listCanMove.push_back(Vec2( r[i], c[j] ));
				}
				else if (dest == nullptr) {
					listCanMove.push_back(Vec2 ( r[i], c[j] ));
				}
			}
		}
	}
	return listCanMove;
}

std::vector<Vec2> CoTuongScreen::findKing(CoTuongPiece* p, int turnColor, bool isLogic) {
	std::vector<Vec2> listCanMove;
	int c[2] = { p->getCol() + 1, p->getCol() - 1 };
	int r[2] = { p->getRow() + 1, p->getRow() - 1 };
	int minRow, maxRow;
	if (turnColor == _board->getMyColor()) {
		minRow = 0;
		maxRow = 2;
	}
	else {
		minRow = 7;
		maxRow = 9;
	}
	for (int i = 0; i < 2; i++) {
		if (r[i] >= minRow && r[i] <= maxRow) {
			CoTuongPiece* dest = _board->getPiece(r[i], p->getCol());
			if (dest != nullptr && dest->getMau() != turnColor) {
				if (!isLogic) {
					dest->setIsFood(true);
				}
				listCanMove.push_back(Vec2( r[i], p->getCol()));
			}
			else if (dest == nullptr) {
				listCanMove.push_back(Vec2( r[i], p->getCol() ));
			}
		}
	}
	for (int j = 0; j < 2; j++) {
		if (c[j] >= 3 && c[j] <= 5) {
			CoTuongPiece* dest = _board->getPiece(p->getRow(), c[j]);
			if (dest != nullptr && dest->getMau() != turnColor) {
				if (!isLogic) {
					dest->setIsFood( true);
				}
				listCanMove.push_back(Vec2( p->getRow(), c[j] ));
			}
			else if (dest == nullptr) {
				listCanMove.push_back(Vec2 ( p->getRow(), c[j] ));
			}
		}
	}
	return listCanMove;
}

std::vector<Vec2> CoTuongScreen::findPawn(CoTuongPiece* p, int turnColor, bool isLogic) {
	std::vector<Vec2> listCanMove;
	int r;
	bool isUp = false, isLeftRight = false;
	if (turnColor == _board->getMyColor()) {
		r = p->getRow() + 1;
		if (p->getRow() <= 9) {
			if (r <= 9) {
				isUp = true;
			}
			if (p->getRow() >= 5) {
				isLeftRight = true;
			}
		}
	}
	else {
		r = p->getRow() - 1;
		if (p->getRow() >= 0) {
			if (r >= 0) {
				isUp = true;
			}
			if (p->getRow() <= 4) {
				isLeftRight = true;
			}
		}
	}

	if (isUp) {
		CoTuongPiece* dest = _board->getPiece(r, p->getCol());
		if (dest != nullptr && dest->getMau() != turnColor) {
			if (!isLogic) {
				dest->setIsFood(true);
			}
			listCanMove.push_back(Vec2( r, p->getCol() ));
		}
		else if (dest == nullptr) {
			listCanMove.push_back(Vec2( r, p->getCol() ));
		}
	}
	if (isLeftRight) {
		int c[2] = { p->getCol() + 1, p->getCol() - 1 };
		for (int j = 0; j < 2; j++) {
			if (c[j] >= 0 && c[j] <= 8) {
				CoTuongPiece* dest = _board->getPiece(p->getRow(), c[j]);
				if (dest != nullptr && dest->getMau() != turnColor) {
					if (!isLogic) {
						dest->setIsFood(true);
					}
					listCanMove.push_back(Vec2( p->getRow(), c[j] ));
				}
				else if (dest == nullptr) {
					listCanMove.push_back(Vec2( p->getRow(), c[j] ));
				}
			}
		}
	}
	return listCanMove;
}

std::vector<Vec2> CoTuongScreen::findKnight(CoTuongPiece* p, int turnColor, bool isLogic) {
	std::vector<Vec2> listCanMove;
	int c[4] = { p->getCol() - 2, p->getCol() - 1, p->getCol() + 1, p->getCol() + 2 };
	int r[4] = { p->getRow() + 1, p->getRow() + 2, p->getRow() - 2, p->getRow() - 1 };
	int cc[4] = { p->getCol() - 1, p->getCol(), p->getCol(), p->getCol() + 1 };
	int rc[4] = { p->getRow(), p->getRow() + 1, p->getRow() - 1, p->getRow() };
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (c[j] >= 0 && c[j] <= 8 && r[i] >= 0 && r[i] <= 9
				&& ((std::abs(c[j] - p->getCol()) == 2 && std::abs(r[i] - p->getRow()) == 1)
					|| (std::abs(c[j] - p->getCol()) == 1 && std::abs(r[i] - p->getRow()) == 2))) {
				if (_board->getBoardValue(rc[i], cc[j]) == 0) {
					CoTuongPiece* dest = _board->getPiece(r[i], c[j]);
					if (dest != nullptr && dest->getMau() != turnColor) {
						if (!isLogic) {
							dest->setIsFood(true);
						}
						listCanMove.push_back(Vec2( r[i], c[j] ));
					}
					else if (dest == nullptr) {
						listCanMove.push_back(Vec2( r[i], c[j] ));
					}
				}
			}
		}
	}
	return listCanMove;
}

/**
*
* @param p
* @param turnColor
* @param nutBiChan
*            : mang 4 phan tu -1 dua vao de tinh duoc cac diem bi chan cho
*            quan Phao
* @param isLogic
* @return
*/
std::vector<Vec2> CoTuongScreen::findRook(CoTuongPiece* p, int turnColor, int nutBiChan[], bool isLogic) {
	std::vector<Vec2> listCanMove;
	// l,r,u,d
	// left
	for (int i = p->getCol() - 1; i >= 0; i--) {
		if (_board->getBoardValue(p->getRow(), i) == 0) {
			listCanMove.push_back(Vec2( p->getRow(), i ));
		}
		else {
			// co con chan duong
			if (p->getID() == PIECE_ROOK) {
				CoTuongPiece* q = _board->getPiece(p->getRow(), i);
				if (q != nullptr && q->getMau() != turnColor) {
					if (!isLogic) {
						q->setIsFood(true);
					}
					listCanMove.push_back(Vec2(p->getRow(), i ));
				}
			}
			nutBiChan[0] = i;
			break;
		}
	}
	// right
	for (int i = p->getCol() + 1; i <= 8; i++) {
		if (_board->getBoardValue(p->getRow(), i) == 0) {
			listCanMove.push_back(Vec2( p->getRow(), i ));
		}
		else {
			// co con chan duong
			if (p->getID() == PIECE_ROOK) {
				CoTuongPiece* q = _board->getPiece(p->getRow(), i);
				if (q != nullptr && q->getMau() != turnColor) {
					if (!isLogic) {
						q->setIsFood(true);
					}
					listCanMove.push_back(Vec2( p->getRow(), i ));
				}
			}
			nutBiChan[1] = i;
			break;
		}
	}
	// up
	for (int i = p->getRow() + 1; i <= 9; i++) {
		if (_board->getBoardValue(i, p->getCol()) == 0) {
			listCanMove.push_back(Vec2( i, p->getCol() ));
		}
		else {
			// co con chan duong
			if (p->getID() == PIECE_ROOK) {
				CoTuongPiece* q = _board->getPiece(i, p->getCol());
				if (q != nullptr && q->getMau() != turnColor) {
					if (!isLogic) {
						q->setIsFood(true);
					}
					listCanMove.push_back(Vec2( i, p->getCol() ));
				}
			}
			nutBiChan[2] = i;
			break;
		}
	}
	// down
	for (int i = p->getRow() - 1; i >= 0; i--) {
		if (_board->getBoardValue(i, p->getCol()) == 0) {
			listCanMove.push_back(Vec2( i, p->getCol() ));
		}
		else {
			// co con chan duong
			if (p->getID() == PIECE_ROOK) {
				CoTuongPiece* q = _board->getPiece(i, p->getCol());
				if (q != nullptr && q->getMau() != turnColor) {
					if (!isLogic) {
						q->setIsFood(true);
					}
					listCanMove.push_back(Vec2( i, p->getCol() ));
				}
			}
			nutBiChan[3] = i;
			break;
		}
	}
	return listCanMove;
}

std::vector<Vec2> CoTuongScreen::findCannon(CoTuongPiece* p, int turnColor, bool isLogic) {
	int chan[4] ={ -1, -1, -1, -1 };
	std::vector<Vec2> listCanMove = findRook(p, turnColor, chan, isLogic);
	// left
	if (chan[0] > -1) {
		for (int i = chan[0] - 1; i >= 0; i--) {
			if (_board->getBoardValue(p->getRow(), i ) != 0) {
				// gap con an
				CoTuongPiece* q = _board->getPiece(p->getRow(), i);
				if (q != nullptr && q->getMau() != turnColor) {
					if (!isLogic) {
						q->setIsFood(true);
					}
					listCanMove.push_back(Vec2(p->getRow(), i ));
				}
				break;
			}
		}
	}
	// right
	if (chan[1] > -1) {
		for (int i = chan[1] + 1; i <= 8; i++) {
			if (_board->getBoardValue(p->getRow(), i) != 0) {
				// gap con an
				CoTuongPiece* q = _board->getPiece(p->getRow(), i);
				if (q != nullptr && q->getMau() != turnColor) {
					if (!isLogic) {
						q->setIsFood(true);
					}
					listCanMove.push_back(Vec2( p->getRow(), i ));
				}
				break;
			}
		}
	}
	// up
	if (chan[2] > -1) {
		for (int i = chan[2] + 1; i <= 9; i++) {
			if (_board->getBoardValue(i, p->getCol()) != 0) {
				// gap con an
				CoTuongPiece* q = _board->getPiece(i, p->getCol());
				if (q != nullptr && q->getMau() != turnColor) {
					if (!isLogic) {
						q->setIsFood(true);
					}
					listCanMove.push_back(Vec2( i, p->getCol() ));
				}
				break;
			}
		}
	}
	// down
	if (chan[3] > -1) {
		for (int i = chan[3] - 1; i >= 0; i--) {
			if (_board->getBoardValue(i, p->getCol()) != 0) {
				// gap con an
				CoTuongPiece* q = _board->getPiece(i, p->getCol());
				if (q != nullptr && q->getMau() != turnColor) {
					if (!isLogic) {
						q->setIsFood(true);
					}
					listCanMove.push_back(Vec2( i, p->getCol() ));
				}
				break;
			}
		}
	}
	return listCanMove;
}

void CoTuongScreen::onStartGame(int whoFirst, int interval)
{
	OnHideCountDown();
	hideResult();
	doStartGame();
	_remove_lose = false;
	_remove_draw = false;
	btn_quick_play->Visible(false);
	_btn_lose->SetVisible(true);
	_btn_eat_pieces->SetVisible(true);
	_totalTimes = interval;
	_whoFirst = whoFirst;
	_myColor = getMyColor(whoFirst);
	_copho_showing = false;

	_board_ruler = _board_widget->GetWidgetChildByName(".banco_ruler");
	WidgetEntity* ruler_lbl;
	for (int i = 10; i >= 1; i--)
	{
		ruler_lbl = _board_ruler->GetWidgetChildByName(StringUtils::format(".number_%d", i));
		((TextWidget*)ruler_lbl)->SetText(StringUtils::format("%d", covertPositionY(10 - i)));
	}

	for (char i = 'a'; i < 'j'; i++)
	{
		ruler_lbl = _board_ruler->GetWidgetChildByName(StringUtils::format(".number_%c", i));
		((TextWidget*)ruler_lbl)->SetText(toStringPositionX('i' - i));
	}


	for (PlayerGroup* pg : p_list_player_group)
	{
		pg->stopTime();
	}

	PlayerGroup* player_group = getPlayerGroup(whoFirst);
	player_group->startTime(interval, 0);
	resetToNewBoard(_myColor, 0);
	updateLabelTime(whoFirst);
	for (Sprite* spr : _eat_pieces_spr)
	{
		spr->setVisible(false);
	}
	for (Sprite* spr : _lost_pieces_spr)
	{
		spr->setVisible(false);
	}
	_btn_clear_board->Visible(false);
	_board->enableBoardTouch(true);
}


void CoTuongScreen::getCoPhoCoUp(int roomID, int boardID) {

	iwincore::Message* m = new iwincore::Message(GameController::getCurrentGameType() == GameType::GAMEID_CO_UP
		? IwinProtocol::CO_UP_COMMAND : IwinProtocol::CO_TUONG_COMMAND);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putByte(53);
	GlobalService::sendMessage(m);
}


void CoTuongScreen::moveCoTuong(ubyte supCommand, ubyte roomId, ubyte boardId, ubyte xS, ubyte yS, ubyte xD, ubyte yD)
{
	CCLOG("%d %d %d %d", xS, yS, xD, yD);
	iwincore::Message* m = new iwincore::Message((GameController::getCurrentGameType() == GameType::GAMEID_CO_UP
		|| GameController::getCurrentGameType() == GameType::GAMEID_GIAI_DAU_CO_UP) ? IwinProtocol::CO_UP_COMMAND
		: IwinProtocol::CO_TUONG_COMMAND);
	m->putByte(roomId);
	m->putByte(boardId);
	m->putByte(supCommand);
	m->putByte(xS);
	m->putByte(yS);
	m->putByte(xD);
	m->putByte(yD);
	GlobalService::sendMessage(m);
}

/**
*
* @param subCommand
* @param roomID
* @param boardID
* @param request
*            0 = undo_want; 1 = undo_accept; -1 = undo_deny
*/
void CoTuongScreen::sendCommandUndoChess(ubyte subCommand, ubyte roomID, ubyte boardID, ubyte request) {
	iwincore::Message* m = new iwincore::Message((GameController::getCurrentGameType() == GameType::GAMEID_CO_UP
		|| GameController::getCurrentGameType() == GameType::GAMEID_GIAI_DAU_CO_UP) ? IwinProtocol::CO_UP_COMMAND
		: IwinProtocol::CO_TUONG_COMMAND);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putByte(subCommand);
	m->putByte(request);
	GlobalService::sendMessage(m);
}

/**
* command danh cho co tuong moi 77
*
* @param subCommand
*/
void CoTuongScreen::sendCommandChess(ubyte subCommand, ubyte roomID, ubyte boardID) {
	iwincore::Message* m = new iwincore::Message((GameController::getCurrentGameType() == GameType::GAMEID_CO_UP
		|| GameController::getCurrentGameType() == GameType::GAMEID_GIAI_DAU_CO_UP) ? IwinProtocol::CO_UP_COMMAND
		: IwinProtocol::CO_TUONG_COMMAND);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putByte(subCommand);
	GlobalService::sendMessage(m);
}

/**
* xác định màu cờ ở vị trí của mình
*
* @param moveFirst
*            : người đi trước
* @return
*/
int CoTuongScreen::getMyColor(int moveFirst) {
	int seatFirst = GetSeatFromPlayerID(moveFirst);
	if (_isViewer) {
		return seatFirst;
	}
	else {
		return moveFirst == _myInfo->GetIDDB() ? 0 : 1;
	}
}

void CoTuongScreen::setTimeQuota(int userId, int time)
{
	for (int i = 0; i < p_list_player_group.size(); i++)
	{
		if (p_list_player_group[i]->GetIDDB() == userId) {
			_player_quotes[i]->GetParentWidget()->SetVisible(true);
			_player_quotes[i]->SetText(formatTimer(time));
		}
	}

	int seat = GetSeatFromPlayerID(userId);
	if (seat == 0) _totalTimeToPlayPlayer1 = time;
	else _totalTimeToPlayPlayer2 = time;
}


void CoTuongScreen::onTouchBoard(Ref* target, Widget::TouchEventType touch_type)
{
	Widget* board = static_cast<Widget*>(target);
	switch (touch_type)
	{
	case Widget::TouchEventType::ENDED:
	{
		if (_isGameEnd) return;
		Vec2 board_touch = board->getTouchEndPosition();
		board_touch = board->convertToNodeSpace(board_touch);
		Vec2 row_col = _board->convertToRowCol(board_touch);
		CCLOG("%f %f", row_col.x, row_col.y);
		if (_last_piece) 
		{
			if (_board->isMoveSuggest(row_col))
			{
				moveCoTuong(25, GetRoomID(), GetBoardID(), _last_piece->getCol(), _last_piece->getRow(), row_col.x, row_col.y);
			}
			else
			{
				_board->clearMoves();
				_board->resetSelect();
			}
			_last_piece = nullptr;

		}

	}
	break;
	}
}

void CoTuongScreen::onTouchPiece(Ref* target, Widget::TouchEventType touch_type)
{
	CoTuongPiece* piece = static_cast<CoTuongPiece*>(target);
	switch (touch_type)
	{
	case Widget::TouchEventType::ENDED:
	{
		if (_isGameEnd) return;
		CCLOG("Piece Touch %d %d", piece->getRow(), piece->getCol());
		std::vector<Vec2> list_move = findMove(piece, _board->getTurnColor(), false);
		_board->resetSelect();
		_board->addSuggestMove(list_move);
		piece->select();
		_last_piece = piece;
	}
	break;
	}
}

void CoTuongScreen::onTouchBtnCopho(Ref* target, Widget::TouchEventType touch_type)
{
	Widget* btn = static_cast<ui::Button*>(target);
	switch (touch_type)
	{
		case Widget::TouchEventType::BEGAN:
		{
												
		}
		break;
		case Widget::TouchEventType::MOVED:
		{
		
		}
		break;
		case Widget::TouchEventType::ENDED:
		{
			if (_copho_showing)
			{
				hideCoPho();
			}
			else
			{
				showCoPho();
			}
		}
		break;
	}
}

void CoTuongScreen::onMove(int whoMove, ubyte xS, ubyte yS, ubyte xD, ubyte yD, int nextMove, char newPieceType) {
	bool isCoUp = GameController::isHiddenChess();
/*
	if (timer != null) {
		timer.cancel();
	}*/
	_board->resetSelect();
	_board->clearMoves();

	CoTuongPiece* p = _board->getPiece(yS, xS);
	CoTuongPiece* pEnd = _board->getPiece(yD, xD);
	if (isCoUp && CoTuongPiece::isValidId(p->getID()) && CoTuongPiece::isValidId(newPieceType)) {
		newPieceType = -1;
	}

	_board->move(p, yD, xD, isCoUp ? newPieceType : -1);

	if (_last_piece && _last_piece->getRow() == yS && _last_piece->getCol() == xS) 
	{
		_last_piece = nullptr;
	}

	if (!_isViewer) {
		_onReviceonEat = false;
		if (whoMove == _myInfo->GetIDDB())
		{
			Utils::vibrateVeryShort();
		}
		if (pEnd && pEnd->getID() != PIECE_FACE)
		{
			setLastEatID(pEnd->getID());
			setLastEatMau(pEnd->getMau());
		}
		PieceStep step;
		step.setData(0, p->getID(), xS, yS, newPieceType, xD, yD);
		std::string result_left;
		std::string result_mid;
		std::string result_right;
		result_left += StringUtils::format("%d. ", (_list_piece_steps.size() + 1));

		result_mid += " ";
		result_mid += getShortName(step.typePieceBefore);
		result_mid += toStringPositionX(step.x);
		result_mid += StringUtils::format("%d", covertPositionY(step.y));
		result_right += " - " + toStringPositionX(step.xTo);
		result_right += StringUtils::format("%d", covertPositionY(step.yTo));
		step.description_left = result_left;
		step.description_mid = result_mid;
		step.description_right = result_right;
		_list_piece_steps.push_back(step);
		static_cast<ListViewTableWidget*>(_copho_panel)->SetNumberSizeOfCell(_list_piece_steps.size(), true);
		notifyLastStep( step.description_mid + step.description_right);

	}

	PlayerGroup* pg = getPlayerGroup(whoMove);
	pg->stopTime();
	pg = getPlayerGroup(nextMove);
	pg->startTime(_totalTimes, 0);
	updateLabelTime(nextMove);


}

void CoTuongScreen::updateLabelTime(int playerId)
{
	int seat = GetSeatFromPlayerID(playerId);
	_curSeatUpdateTime = seat;
	if (!_isViewer)
	{
		if(!_remove_draw)
			_btn_draw->SetVisible(playerId == _myInfo->GetIDDB());
	}


}

void CoTuongScreen::onOpponentWantUndo()
{
	ScrMgr->OnShowDialog("notice", "can_i_take_a_bad", TYPE_DIALOG::DIALOG_TWO_BUTTON, "", nullptr, nullptr, [this](const char* call_back, const char* btn_name) {
		if (strcmp("cancel_fb_btn", btn_name) == 0) {
			this->sendCommandUndoChess((ubyte)26, this->GetRoomID(), this->GetBoardID(), (ubyte)1);
		}
		else if (strcmp("cancel_2_btn", btn_name) == 0) {
			this->sendCommandUndoChess((ubyte)26, this->GetRoomID(), this->GetBoardID(), (ubyte)-1);
		}
	});
	Utils::vibrateShort();
}

void CoTuongScreen::onOpponentWantDraw()
{
	ScrMgr->OnShowDialog("notice", "opponent_want_draw_are", TYPE_DIALOG::DIALOG_TWO_BUTTON, "",[this](const char* call_back, const char* btn_name) {
		this->sendCommandChess((ubyte)30, this->GetRoomID(), this->GetBoardID());

	}, [this](const char* call_back, const char* btn_name) {

		this->sendCommandChess((ubyte)31, this->GetRoomID(), this->GetBoardID());
	});
	Utils::vibrateShort();
}

void CoTuongScreen::onOpponentWantLose()
{
	ScrMgr->OnShowDialog("notice", "opponent_want_lose_are", TYPE_DIALOG::DIALOG_TWO_BUTTON, "", nullptr, nullptr, [this](const char* call_back, const char* btn_name) {
		if (strcmp("cancel_fb_btn", btn_name) == 0) {
			this->sendCommandChess((ubyte)35, this->GetRoomID(), this->GetBoardID());
		}
		else if (strcmp("cancel_2_btn", btn_name) == 0) {
			this->sendCommandChess((ubyte)36, this->GetRoomID(), this->GetBoardID());
		}
	});
	Utils::vibrateShort();
}

std::string CoTuongScreen::formatTimer(int time)
{
	return StringUtils::format("%02d:%02d", time / 60, time % 60);
}

void CoTuongScreen::finishGame(std::vector<GameResult*> result, ubyte typeFinish)
{
	doStopGame();
	if (GameController::getCurrentGameType() == GameType::GAMEID_CO_UP) {
		if (typeFinish == 0) {
			// play anim chieu bi
			// show match result
			GameScreen::OnMatchResult(result);
		} else {
			// show match result	
			GameScreen::OnMatchResult(result);
		}
	} else {
		GameScreen::OnMatchResult(result);
	}
	for (GameResult* rs : result)
	{
		if (_myInfo && rs->ID == _myInfo->GetIDDB())
		{
			saveCoPho(rs->result);
		}
	}
	for (PlayerGroup* pg : p_list_player_group)
	{
		if(pg) pg->stopTime();
	}
	hideCoPho();
	if (_btn_touch_screen->IsVisible()) showEatLostPieces();
	_btn_draw->Visible(false);
	_btn_eat_pieces->Visible(false);
	_btn_lose->Visible(false);
	_btn_clear_board->Visible(true);
	_list_piece_steps.clear();
	static_cast<ListViewTableWidget*>(_copho_panel)->SetNumberSizeOfCell(_list_piece_steps.size(), true);
	//clearBoard();
	_curSeatUpdateTime = -1;

}

void CoTuongScreen::setBetMoney(s64 money, s64 max_money)
{
	GameScreen::setBetMoney(money, max_money);
	static_cast<TextWidget*>(_lbl_center_bet)->SetText(StringUtils::format(LANG_STR("cotuong_bet").GetString(), Utils::formatNumber_dot(money).c_str(), GameController::getMoneyType().c_str()));
	_lbl_center_bet->Visible(true);
}

/**
* thong tin ban dang choi
*
* @param moveFirst
* @param ownerUserId
* @param currUserID
* @param limitStepTime
* @param intervalView
* @param board
* @param isMove
*/
void CoTuongScreen::onView(int moveFirst, int ownerUserId, int currUserID, int limitStepTime, int intervalView, int board[9][10]) {
	_isViewer = true;
	updateLabelTime(currUserID);
	//boardMenuEatPieces.showButtonShow(!_isViewer);

	resetSeat();
	hideReadyButton();
	hideStartButton();

	//if (GameController::getCurrentGameType() == GameType::GAMEID_GIAI_DAU_CO_UP) {
	//	hideNapWinButton();
	//}

	_totalTimes = limitStepTime;
	if (moveFirst != -1 && currUserID != -1) {
		_isStartGame = true;
		int seatMove = GetSeatFromPlayerID(currUserID);
		int seatFirst = GetSeatFromPlayerID(moveFirst);
		startTimeBar(seatMove, intervalView);
		// doi mang board sang dong,cot
		int map[10][9];
		for (int c = 0; c < 9; c++) {
			for (int r = 0; r < 10; r++) {
				map[r][c] = board[c][r];
			}
		}
		_board->setBoard(
			GameController::getCurrentGameType() == GameType::GAMEID_CHESS ? s_mapNewBoard : s_mapNewBoard_CoUp, map,
			seatFirst, seatMove);
		_board->enableTouch(!_isViewer);
		_btn_draw->SetVisible(false);
		_btn_lose->SetVisible(false);
		_btn_eat_pieces->SetVisible(false);
	}
	//btnChatButton.setVisible(false);
	//commonLayer.setVisible(false);
	_player_quotes[0]->GetParentWidget()->SetVisible(_isStartGame);
	_player_quotes[1]->GetParentWidget()->SetVisible(_isStartGame);

	//imgBgTime[0].setVisible(isStartGame);
	//imgBgTime[1].setVisible(isStartGame);
}

void CoTuongScreen::onReconnect(int moveFirst, int ownerUserId, int currUserID, int limitStepTime, int intervalView,int board[9][10]) {
	//DialogUtil.hideWaitDialog();
	//GameController.vibrateMedium();
	//GameController.playSound(iWinSound.BUZZ);
	GameScreen::onReconnect();
	for (Sprite* spr : _eat_pieces_spr)
	{
		spr->setVisible(false);
	}
	for (Sprite* spr : _lost_pieces_spr)
	{
		spr->setVisible(false);
	}
	resetReady();
	hideReadyButton();
	hideStartButton();

	_numberMyEatPiece = 0;
	_numberMyLostPiece = 0;
	_replay_mode = false;
	// if (GameController.getCurrentGameType() == GameType.GAMEID_CO_UP)
	// imgDownToShowCoPho.setVisible(true);
	_totalTimes = limitStepTime;
	_whoFirst = moveFirst;
	int myColor = getMyColor(_whoFirst);
	if (_isViewer) {
		_btn_draw->SetVisible(false);
		_btn_lose->SetVisible(false);
		_btn_eat_pieces->SetVisible(false);
		// chatLayer.hideNapWinButton();
	}
	else {
		//if (GameController.getCurrentGameType() == GameType.GAMEID_GIAI_DAU_CO_UP && !isStartGame) {
		//	showCountDownLabel();
		//}
		_btn_draw->SetVisible(currUserID == _myInfo->GetIDDB());
		_btn_lose->SetVisible(true);
		_btn_eat_pieces->SetVisible(true);
		// chatLayer.hideNapWinButton();
	}
	//if (GameController.getCurrentGameType() == GameType.GAMEID_GIAI_DAU_CO_UP)
	//	hideNapWinButton();
	//btnWantPlay.remove();
	_player_quotes[0]->GetParentWidget()->SetVisible(true);
	_player_quotes[1]->GetParentWidget()->SetVisible(true);

	//imgBgTime[0].setVisible(true);
	//imgBgTime[1].setVisible(true);

	updateLabelTime(currUserID);

		int seatMove = GetSeatFromPlayerID(currUserID);
		PlayerGroup* pg = GetPlayerGroupAtIdx(seatMove);
		pg->startTime(limitStepTime, intervalView);
		//startTimeBar(seatMove, limitStepTime, intervalView);
		// doi mang board sang dong,cot
		int map[10][9];
		for (int c = 0; c < 9; c++) {
			for (int r = 0; r < 10; r++) {
				map[r][c] = board[c][r];
			}
		}
		_board->setBoard(
			GameController::getCurrentGameType() == GameType::GAMEID_CHESS ? s_mapNewBoard : s_mapNewBoard_CoUp, map,
			myColor, currUserID == _myInfo->GetIDDB() ? myColor : (1 - myColor));
		//this.board.listCanMove.clear();
		_board->enableBoardTouch(true);
		//this.board.setVisible(true);
		_btn_draw->SetVisible(currUserID == _myInfo->GetIDDB());
		//if (GameController.getCurrentGameType() == GameType.GAMEID_CHESS && currUserID != myInfo.IDDB) {
		//	btnWantUndo.setVisible(true);
		//}

		/*if (GameController.getCurrentGameType() == GameType.GAMEID_GIAI_DAU_CO_UP) {
			hideNapWinButton();
		}*/
	//}
	if (GameController::getCurrentGameType() == GameType::GAMEID_CO_UP
		|| GameController::getCurrentGameType() == GameType::GAMEID_CHESS) {
		_myColor = myColor;
		_list_piece_steps.clear();
		getCoPhoCoUp(_roomID, _boardID);

	}
}

void CoTuongScreen::onEatAndLostPieces(int userEat, std::vector<char> eatPieces, std::vector<char> lostPieces)
{
	_onReviceonEat = true;

	if (userEat == _myInfo->GetIDDB()) {
		if (eatPieces.size() != _numberMyEatPiece) {
			setLastEatID(eatPieces[eatPieces.size() - 1]);
			setLastEatMau(1 - getMyColor(_whoFirst));
		}
		_numberMyEatPiece = eatPieces.size();
	}
	if (!lostPieces.empty() && lostPieces.size() != _numberMyLostPiece) {
		if (lostPieces[lostPieces.size() - 1] != -1) {
			setLastEatID(lostPieces[lostPieces.size() - 1]);
			setLastEatMau(getMyColor(_whoFirst));
		}
		else {
			setLastEatID(8);
			setLastEatMau(getMyColor(_whoFirst));
		}
	}
	_numberMyLostPiece = lostPieces.size();

	///*
	//* do chổ ngồi, ở dưới là 0 nên lấy seat sẽ biết trên hay dưới
	//*
	//* vì gọi hàm này sau khi đã gọi move nên màu hiện tại đã bị thay đổi,
	//* khi đó màu hiện tại là màu của lost, cờ ăn đương nhiên màu của lost
	//* rồi :D
	//*/
	//boardMenuEatPieces.clearPieces();
	//int seatEat = getSeatFromPlayerID(userEat);// ng ăn
	//int seatLost = 1 - seatEat;// ng mất
	//int count = 0;u

	//int myColor = getMyColor(this.whoFirst);
	//int colorOpp;
	//if (seatEat == 0) {
	//	// chổ ngồi của người ăn là mình
	//	colorOpp = 1 - myColor;
	//}
	//else {
	//	colorOpp = myColor;
	//}

	int seatEat = GetSeatFromPlayerID(userEat);// ng ăn
	int seatLost = 1 - seatEat;// ng mất
	int count = 0;

	int myColor = getMyColor(_whoFirst);
	int colorOpp;
	if (seatEat == 0) {
		// chổ ngồi của người ăn là mình
		colorOpp = 1 - myColor;
		for (int i = 0; i < eatPieces.size(); i++)
		{
			_eat_pieces_spr[i]->setSpriteFrame(CoTuongPiece::getSpriteFrameFromIdMau(eatPieces[i], colorOpp));
			_eat_pieces_spr[i]->setVisible(true);
		}
		for (int i = 0; i < lostPieces.size(); i++)
		{
			_lost_pieces_spr[i]->setSpriteFrame(CoTuongPiece::getSpriteFrameFromIdMau(lostPieces[i], myColor));
			_lost_pieces_spr[i]->setVisible(true);
		}
	}
	else {
		colorOpp = myColor;
		for (int i = 0; i < eatPieces.size(); i++)
		{
			_lost_pieces_spr[i]->setSpriteFrame(CoTuongPiece::getSpriteFrameFromIdMau(eatPieces[i], 1- myColor));
			_lost_pieces_spr[i]->setVisible(true);
		}
		for (int i = 0; i < lostPieces.size(); i++)
		{
			_eat_pieces_spr[i]->setSpriteFrame(CoTuongPiece::getSpriteFrameFromIdMau(lostPieces[i], colorOpp));
			_eat_pieces_spr[i]->setVisible(true);
		}
	}


	_eat_pieces = eatPieces;
	_lost_pieces = lostPieces;
}

void CoTuongScreen::onRemoveButton(int buttonId)
{
	switch (buttonId) {
	case 0:
		// undo
		//btnWantUndo.remove();

		break;
	case 1:
		// xin hoa
		_btn_draw->Visible(false);
		_remove_draw = true;
		break;
	case 2:
		// xin thua
		_btn_lose->Visible(false);
		_remove_lose = true;
		break;
	}
}

void CoTuongScreen::showEatLostPieces()
{
	if (!_eat_pieces.empty())
	{
		if (!_eat_pieces_bot->IsVisible())
		{
			if (_eat_pieces.size() <= 8)
			{
				showCoAnBot(true);
			}
			else
			{
				showCoAnBot(false);
			}
			_btn_touch_screen->Visible(true);
		}
		else
		{
			if (_eat_pieces.size() <= 8)
			{
				hideCoAnBot(true);
			}
			else
			{
				hideCoAnBot(false);
			}
			_btn_touch_screen->Visible(false);
		}
	}
	
	if (!_lost_pieces.empty())
	{
		if (!_eat_pieces_top->IsVisible())
		{
			if (_lost_pieces.size() <= 8)
			{
				showCoAnTop(true);
			}
			else
			{
				showCoAnTop(false);
			}
			_btn_touch_screen->Visible(true);
		}
		else
		{
			if (_lost_pieces.size() <= 8)
			{
				hideCoAnTop(true);
			}
			else
			{
				hideCoAnTop(false);
			}
			_btn_touch_screen->Visible(false);
		}
	}

	
}

void CoTuongScreen::playKillEffect(int id, int mau, Vec2 start_pos)
{
	_spr_kill_effect->setVisible(true);
	_spr_kill_effect->setSpriteFrame(CoTuongPiece::getSpriteFrameFromIdMau(id, mau));
	_spr_kill_effect->setPosition(start_pos);

	Repeat* zoom_action = Repeat::create(Sequence::create(ScaleTo::create(0.1, 1.2f), ScaleTo::create(0.1f, 1.0f), nullptr), 3);
	_spr_kill_effect->runAction(Sequence::create(MoveTo::create(0.5f, _btn_eat_pieces->GetPosition()), zoom_action, CallFunc::create([this]() {
		this->_spr_kill_effect->setVisible(false);
	}), nullptr));
}

void CoTuongScreen::setCoPhoSlot(WidgetEntity* entity, int idx)
{
	entity->SetVisible(true);
	static_cast<TextWidget*>(entity->GetWidgetChildByName(".content_left"))->SetText(_list_piece_steps[idx].description_left);
	static_cast<TextWidget*>(entity->GetWidgetChildByName(".content_mid"))->SetText(_list_piece_steps[idx].description_mid);
	static_cast<TextWidget*>(entity->GetWidgetChildByName(".content_right"))->SetText(_list_piece_steps[idx].description_right);
	if ((!_replay_mode && idx == _list_piece_steps.size() - 1) || (_replay_mode && idx == _cur_step)) {
		static_cast<TextWidget*>(entity->GetWidgetChildByName(".content_left"))->SetColor(Color3B::YELLOW);
		static_cast<TextWidget*>(entity->GetWidgetChildByName(".content_mid"))->SetColor(Color3B::YELLOW);
		static_cast<TextWidget*>(entity->GetWidgetChildByName(".content_right"))->SetColor(Color3B::YELLOW);
	}
	else
	{
		static_cast<TextWidget*>(entity->GetWidgetChildByName(".content_left"))->SetColor(Color3B::WHITE);
		static_cast<TextWidget*>(entity->GetWidgetChildByName(".content_mid"))->SetColor(Color3B::WHITE);
		static_cast<TextWidget*>(entity->GetWidgetChildByName(".content_right"))->SetColor(Color3B::WHITE);
	}
}

std::string CoTuongScreen::toStringPositionX(int x) {
	if (_myColor != 0) {
		x = 9 - (x + 1);
	}

	x += 1;
	std::string result = "";
	switch (x) {
	case 1:
		result = "A";
		break;
	case 2:
		result = "B";
		break;
	case 3:
		result = "C";
		break;
	case 4:
		result = "D";
		break;
	case 5:
		result = "E";
		break;
	case 6:
		result = "F";
		break;
	case 7:
		result = "G";
		break;
	case 8:
		result = "H";
		break;
	case 9:
		result = "I";
		break;

	default:
		break;
	}
	return result;
}

int CoTuongScreen::covertPositionY(int y) {
	if (_myColor != 0) {
		y = 10 - (y + 1);
	}
	y += 1;
	return 11 - y;
}

std::string CoTuongScreen::getShortName(int pId) {
	
	if (LangMgr->GetCurLanguage() == LANGUAGE_TYPE::LANG_VI) {
		switch (pId) {
		case PIECE_KING:
			return "T";// tuong
		case PIECE_ADVISOR:
			return "S"; // si
		case PIECE_BISHOP:
			return "B"; // tuong
		case PIECE_KNIGHT:
			return "M";// ma
		case PIECE_ROOK:
			return "X"; // xe
		case PIECE_CANNON:
			return "P";// phao
		case PIECE_PAWN:
			return "C";
		case -1:
		case PIECE_FACE:
			return "U";
		}
	}
	else {
		switch (pId) {
		case PIECE_KING:
			return "K";// tuong
		case PIECE_ADVISOR:
			return "A"; // si
		case PIECE_BISHOP:
			return "E"; // tuong
		case PIECE_KNIGHT:
			return "H";// ma
		case PIECE_ROOK:
			return "R"; // xe
		case PIECE_CANNON:
			return "C";// phao
		case PIECE_PAWN:
			return "P";
		case -1:
		case PIECE_FACE:
			return "F";
		}
	}
	return "";
}

void CoTuongScreen::hideCoPho()
{
	if (!_copho_showing) return;

	_copho_layout->SetActionCommandWithName("HIDE");
	_copho_panel_parent->SetActionCommandWithName("HIDE");
	_banco_ruler_panel->SetActionCommandWithName("HIDE");
	_btn_copho->SetActionCommandWithName("HIDE");
	if (GameController::currentMoneyType == RUBY_MONEY)
	{
		p_btn_purchase_ruby->SetVisible(true);
	}
	else
	{
		p_btn_purchase_iwin->SetVisible(true);
	}

	p_text_name->SetVisible(true);
	_copho_showing = false;
}

void CoTuongScreen::showCoPho()
{
	_copho_layout->SetActionCommandWithName("SHOW");
	_copho_panel_parent->SetActionCommandWithName("SHOW");
	_banco_ruler_panel->SetActionCommandWithName("SHOW");
	_btn_copho->SetActionCommandWithName("SHOW");
	p_btn_purchase_iwin->SetVisible(false);
	p_btn_purchase_ruby->SetVisible(false);
	p_text_name->SetVisible(false);
	_copho_showing = true;
}

void CoTuongScreen::saveCoPho(char result)
{
	Player* p = getPlayerNotMeInSeat();
	if (p == nullptr) return;
	CoPhoBoard save_data;
	save_data.userNameRival = p->name;
	save_data.win = p->getMoney();
	save_data.winbet = _money;
	save_data.avatar = p->avatar;
	save_data.myColor = _myColor;
	save_data.result = result;
	save_data.pieceSteps = _list_piece_steps;

	GetCoPhoMgr()->add(save_data);
	GetCoPhoMgr()->save();
}

/**
* lay ra user co ngoi vao ghe ma kh phai minh
*/
Player* CoTuongScreen::getPlayerNotMeInSeat() {
	for (PlayerGroup* pg : p_list_player_group)
	{
		if (pg != _myInfo) {
			return pg->getPlayer();
		}
	}
	return nullptr;
}

void CoTuongScreen::OnMatchResult(std::vector<GameResult*> results)
{
	GameScreen::OnMatchResult(results);

	for (GameResult* rs : results)
	{
		if (rs->ID == _myInfo->GetIDDB())
		{
			saveCoPho(rs->result);
		}
	}
}

void CoTuongScreen::setupCoPho(int idx)
{
	showCoPho();
	_btn_copho->SetVisible(false);
	 p_btn_ingame_menu->SetVisible(false);
	_board_data = GetCoPhoMgr()->getCoPhoIdx(idx);
	_list_piece_steps = _board_data.pieceSteps;
	_btn_co_chat->Visible(false);
	resetToNewBoard(_board_data.myColor, 0);
	_replay_mode = true;
	_cur_step = -1;
	_board->setReplayMode(true);
	_board->enableTouch(false);
	//p_te
	generateStringCoPho();
	static_cast<ListViewTableWidget*>(_copho_panel)->SetNumberSizeOfCell(_list_piece_steps.size(), true);
}

void CoTuongScreen::OnBeginFadeIn()
{
	GameScreen::OnBeginFadeIn();
	if(_replay_mode) OnShowTxtWait(0);
}

void CoTuongScreen::OnFadeInComplete()
{
	_btn_draw->SetVisible(false);
	_btn_lose->SetVisible(false);
	_btn_eat_pieces->SetVisible(false);
	_btn_copho->SetVisible(!_replay_mode);
	_btn_play->SetVisible(_replay_mode);
	_btn_close->SetVisible(_replay_mode);
	_btn_pause->SetVisible(false);
}

void CoTuongScreen::playNext()
{
	if (!_btn_play->IsVisible()) {
		if (_cur_step >= (int)_board_data.pieceSteps.size() - 1)
		{
			showBtnPlay(true);
			return;
		}
		PieceStep step = _board_data.pieceSteps[++_cur_step];
		CoTuongPiece* piece = _board->getPiece(step.y, step.x);
		static_cast<ListViewTableWidget*>(_copho_panel)->UpdateCellAtIdx(_cur_step);
		if(_cur_step > 0) 
			static_cast<ListViewTableWidget*>(_copho_panel)->UpdateCellAtIdx(_cur_step - 1);
		_board->move(piece, step.yTo, step.xTo, step.typePieceAfter, [this]()
		{
			this->runAction(Sequence::create(DelayTime::create(.75f), CallFunc::create(CC_CALLBACK_0(CoTuongScreen::playNext, this)), nullptr));
		});
	}
}

void CoTuongScreen::gotoStep(int idx)
{
	int myColor = _board_data.myColor;
	int myId = myColor * 10;
	int oppId = (1 - myColor) * 10;
	int mau;

	int board_temp[10][9];
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			board_temp[i][j] = s_mapNewBoard[i][j];
			if (board_temp[i][j] == 0) continue;
			board_temp[i][j] = (i < 5 ? myId : oppId) + board_temp[i][j];
		}
	}
	int bk_idx = _cur_step;
	_cur_step = idx;
	static_cast<ListViewTableWidget*>(_copho_panel)->UpdateCellAtIdx(bk_idx);
	static_cast<ListViewTableWidget*>(_copho_panel)->UpdateCellAtIdx(idx);
	for (int i = 0; i <= idx; i++)
	{
		board_temp[_list_piece_steps[i].yTo][_list_piece_steps[i].xTo] = board_temp[_list_piece_steps[i].y][_list_piece_steps[i].x];
		board_temp[_list_piece_steps[i].y][_list_piece_steps[i].x] = 0;
	}

	_board->setBoardReplay(board_temp, _board_data.myColor, 0);
}

void CoTuongScreen::showBtnPlay(bool visible)
{
	_btn_play->SetVisible(visible);
	_btn_pause->SetVisible(!visible);
}

void CoTuongScreen::notifyLastStep(std::string content)
{
	_last_step_panel->SetActionCommandWithName("SHOW");
	static_cast<TextWidget*>(_last_step_panel->GetWidgetChildByName(".content"))->SetText(content);
}

void CoTuongScreen::setCoPhoReconnect(std::vector<PieceStep> steps)
{
	_list_piece_steps = steps;
	generateStringCoPho();
	static_cast<ListViewTableWidget*>(_copho_panel)->SetNumberSizeOfCell(_list_piece_steps.size(), true);
}

void CoTuongScreen::generateStringCoPho()
{
	for (int i = 0; i < _list_piece_steps.size(); i++)
	{
		std::string result_left;
		std::string result_mid;
		std::string result_right;
		result_left += StringUtils::format("%d. ", (i + 1));
		result_mid += getShortName(_list_piece_steps[i].typePieceBefore);
		result_mid += " ";
		result_mid += toStringPositionX(_list_piece_steps[i].x);
		result_mid += StringUtils::format("%d", covertPositionY(_list_piece_steps[i].y));
		result_right += " - " + toStringPositionX(_list_piece_steps[i].xTo);
		result_right += StringUtils::format("%d", covertPositionY(_list_piece_steps[i].yTo));
		_list_piece_steps[i].description_left = result_left;
		_list_piece_steps[i].description_mid = result_mid;
		_list_piece_steps[i].description_right = result_right;

	}
}

void CoTuongScreen::clearBoard()
{
	_board->clear();
}

void CoTuongScreen::showEffectChieu(bool visible)
{
	if (_lbl_chieu_tuong->IsVisible()) return;

	_lbl_chieu_tuong->Visible(visible);

	if (visible)
	{
		_lbl_chieu_tuong->SetActionCommandWithName("ACTION", CallFunc::create([this]() {
			_lbl_chieu_tuong->Visible(false);
		}));
	}
}

void CoTuongScreen::showCoAnTop(bool one_row)
{
	cocos2d::Size game_size = GetGameSize();
	int mid_board = _board_widget->GetPosition().x + _board_widget->GetSize().x / 2;
	_eat_pieces_top->GetResource()->stopAllActions();
	_eat_pieces_top->SetPosition(Vec2(mid_board, game_size.height));
	_eat_pieces_top->Visible(true);
	if (one_row)
	{
		_eat_pieces_top->GetResource()->runAction(MoveTo::create(0.15f, Vec2(mid_board, game_size.height - _eat_pieces_top->GetSize().y / 2)));
	}
	else
	{
		_eat_pieces_top->GetResource()->runAction(MoveTo::create(0.15f, Vec2(mid_board, game_size.height - _eat_pieces_top->GetSize().y)));
	}
}

void CoTuongScreen::hideCoAnTop(bool one_row)
{
	cocos2d::Size game_size = GetGameSize();
	int mid_board = _board_widget->GetPosition().x + _board_widget->GetSize().x / 2;
	_eat_pieces_top->GetResource()->stopAllActions();
	if (one_row)
	{
		_eat_pieces_top->SetPosition(Vec2(mid_board, game_size.height - _eat_pieces_top->GetSize().y / 2));
	}
	else
	{
		_eat_pieces_top->SetPosition(Vec2(mid_board, game_size.height - _eat_pieces_top->GetSize().y));
	}

	_eat_pieces_top->GetResource()->runAction(Sequence::create(MoveTo::create(0.15f, Vec2(mid_board, game_size.height)), CallFunc::create([this]() 
	{
		_eat_pieces_top->Visible(false);
	}), nullptr));
}


void CoTuongScreen::showCoAnBot(bool one_row)
{
	cocos2d::Size game_size = GetGameSize();
	int mid_board = _board_widget->GetPosition().x + _board_widget->GetSize().x / 2;
	_eat_pieces_bot->Visible(true);
	_eat_pieces_bot->GetResource()->stopAllActions();
	_eat_pieces_bot->SetPosition(Vec2(mid_board, -_eat_pieces_bot->GetSize().y));
	if (one_row)
	{
		_eat_pieces_bot->GetResource()->runAction(MoveTo::create(0.15f, Vec2(mid_board, -_eat_pieces_bot->GetSize().y / 2)));
	}
	else
	{
		_eat_pieces_bot->GetResource()->runAction(MoveTo::create(0.15f, Vec2(mid_board, .0f)));
	}
}

void CoTuongScreen::hideCoAnBot(bool one_row)
{
	cocos2d::Size game_size = GetGameSize();
	int mid_board = _board_widget->GetPosition().x + _board_widget->GetSize().x / 2;
	_eat_pieces_bot->SetPosition(Vec2(mid_board, game_size.height));
	if (one_row)
	{
		_eat_pieces_bot->SetPosition(Vec2(mid_board, -_eat_pieces_bot->GetSize().y / 2));
	}
	else
	{
		_eat_pieces_bot->SetPosition(Vec2(mid_board, 0));
	}

	_eat_pieces_bot->GetResource()->runAction(Sequence::create(MoveTo::create(0.15f, Vec2(mid_board, -_eat_pieces_bot->GetSize().y)), CallFunc::create([this]()
	{
		_eat_pieces_bot->Visible(false);
	}), nullptr));
}

void CoTuongScreen::onPlayerToViewer(int leaveID, int newOwnerID)
{
	if (GameController::myInfo->IDDB == leaveID)
	{
		_isViewer = true;
		_btn_copho->SetVisible(_isViewer);
		OnHideCountDown();
	}

}

void CoTuongScreen::onSomeOneLeaveBoard(int leave, int newOwner)
{
	GameScreen::onSomeOneLeaveBoard(leave, newOwner);
	if (_isViewer)
	{
		for(int i = 0; i < p_list_player_group.size(); i++)
		{
			if (p_list_player_group[i]->getPlayer() != nullptr)
			{
				int seat = p_list_player_group[i]->GetSeat();
				if (seat == 0)
				{
					Player* p = new Player(p_list_player_group[i]->getPlayer());
					p_list_player_group[i + 1 % 2]->setPlayer(p , i);
					p_list_player_group[i]->setPlayer(nullptr);
				}
				break;
			}
		}

		for (PlayerGroup* pg : p_list_player_group)
		{
			if (pg->getPlayer() == nullptr)
			{
				pg->ShowIconWaiting(true);

			}

		}

	}

}

void CoTuongScreen::HandleButtonAddUser()
{
	if (_isViewer)
	{
		GlobalService::joinBoard_Normal(GetRoomID(), GetBoardID(), "");
	}
	else
	{
		HUDScreen::HandleButtonAddUser();
	}
}

void CoTuongScreen::addInfo_new(std::string str)
{
	Utils::vibrateVeryShort();
	ShowMessagePopup_New(str);
}