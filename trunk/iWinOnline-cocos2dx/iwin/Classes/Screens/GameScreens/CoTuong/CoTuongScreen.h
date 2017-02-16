#ifndef __GAME_COTUONG_H__
#define __GAME_COTUONG_H__

#include "Screens/GameScreens/GameScreens.h"
#include "UI/AdvanceUI/wLayout.h"
#include "UI/AdvanceUI/wText.h"
#include "json/writer.h"

class CoTuongBoard;
class CoTuongPiece;

struct PieceStep
{
	int timeDelay;
	int typePieceBefore;
	int typePieceAfter;
	int x;
	int y;
	int xTo;
	int yTo;

	std::string description_left;
	std::string description_mid;
	std::string description_right;

	void setData(int timeDelay, int typePieceBefore, int x, int y, int typePieceAfter, int xTo, int yTo)
	{
		this->timeDelay = timeDelay;
		this->typePieceBefore = typePieceBefore;
		this->x = x;
		this->y = y;
		this->typePieceAfter = typePieceAfter;
		this->xTo = xTo;
		this->yTo = yTo;
	}
	std::string getDescription()
	{
		return StringUtils::format("%d_%d_%d_%d_%d_%d", typePieceBefore, x, y, xTo, yTo, typePieceAfter);
	}
		
	void toData(std::string str)
	{
		RKString data(str.c_str());
		auto split_content = data.Split("_");
		if (split_content.Size() == 6)
		{
			typePieceBefore = atoi(split_content[0].GetString());
			x = atoi(split_content[1].GetString());
			y = atoi(split_content[2].GetString());
			xTo = atoi(split_content[3].GetString());
			yTo = atoi(split_content[4].GetString());
			typePieceAfter = atoi(split_content[5].GetString());
		}
		else
		{
			PASSERT2(false, "ERROR: wrong string!");
		}
	}
	
};

struct CoPhoBoard
{
	int avatar;
	std::string userNameRival;
	s64 win;
	int winbet;
	int boardLevel;
	int myColor;
	char result;
	std::vector<PieceStep> pieceSteps;
	
	rapidjson::Document toJson();
	void toData(rapidjson::Document& doc);

};

class CoTuongScreen : public GameScreen
{
private:
	WidgetEntity*		_board_widget;
	WidgetEntity*		_board_ruler;
	WidgetEntity*		_result_panel;
	WidgetEntity*		_lbl_center_bet;
	WidgetEntity*		_lbl_chieu_tuong;
	WidgetEntity*		_eat_pieces_bot;
	WidgetEntity*		_eat_pieces_top;

	WidgetEntity*		_btn_draw;
	WidgetEntity*		_btn_lose;
	WidgetEntity*		_btn_undo;
	WidgetEntity*		_btn_eat_pieces;
	WidgetEntity*		_btn_touch_screen;
	WidgetEntity*		_btn_clear_board;
	WidgetEntity*		_btn_play;
	WidgetEntity*		_btn_pause;
	WidgetEntity*		_btn_close;
	WidgetEntity*		_btn_co_chat;
	
	WidgetEntity*		_banco_ruler_panel;
	WidgetEntity*		_copho_panel;
	WidgetEntity*		_copho_panel_parent;
	WidgetEntity*		_copho_layout;
	WidgetEntity*		_btn_copho;
	WidgetEntity*		_last_step_panel;

	CoTuongBoard*		_board;

	CoTuongPiece*		_last_piece;
	
	TextWidget*			_player_quotes[2];
	std::vector<char>				_eat_pieces;
	std::vector<char>				_lost_pieces;
	std::vector<Sprite*>	_eat_pieces_spr;
	std::vector<Sprite*>	_lost_pieces_spr;


	float				_totalTimeToPlayPlayer1;
	float				_totalTimeToPlayPlayer2;
	int					_totalTimes;
	int					_whoFirst;
	int					_myColor;
	int					_curSeatUpdateTime;
	int					_cur_game_id;
	Sprite*				_spr_kill_effect;

	bool				_copho_showing = false;

	bool				_replay_mode = false;
	int					_cur_step;

	CC_SYNTHESIZE(int, _onReviceonEat, ReceiveOnEat)
	CC_SYNTHESIZE(int, _lastEatID, LastEatID)
	CC_SYNTHESIZE(int, _lastEatMau, LastEatMau)
	CC_SYNTHESIZE(int, _numberMyEatPiece, NumberMyEatPiece)
	CC_SYNTHESIZE(int, _numberMyLostPiece, NumberMyLostPiece)

	std::vector<PieceStep> _list_piece_steps;
	CoPhoBoard				_board_data;

	bool				_remove_lose = false;
	bool				_remove_draw = false;
	
private:
	void				resetToNewBoard(int myColor, int turnColor);
	void				showEatLostPieces();
	void				setCoPhoSlot(WidgetEntity* entity, int idx);
	void				hideCoPho();
	void				showCoPho();

	void				saveCoPho(char result);
	Player*				getPlayerNotMeInSeat();
	
	void				showEffectChieu(bool visible);

protected:
	virtual void		  HandleAfterInitPlayer() override;
public:
	static CoTuongScreen* getInstance();


	CoTuongScreen();
	virtual ~CoTuongScreen();

	virtual bool InitGame(int game_id) override;
	virtual int Update(float dt)	   override;
	virtual void OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, Utility::UI_Widget::WidgetEntity * _widget) override;
	virtual void LoadXMLGameScreenCustom(TiXmlDocument * p_objectXML)	override;

	virtual void OnFadeInComplete() override;
	virtual void OnBeginFadeIn() override;
	virtual void setBetMoney(s64 money, s64 max_money = 0) override;
	virtual void OnMatchResult(std::vector<GameResult*> results) override;

	virtual	void onSomeOneLeaveBoard(int leave, int newOwner) override;
	virtual		void HandleButtonAddUser();

	void onTouchPiece(Ref* target, Widget::TouchEventType touch_type);
	void onTouchBoard(Ref* target, Widget::TouchEventType touch_type);
	void onTouchBtnCopho(Ref* target, Widget::TouchEventType touch_type);

	void setupCoPho(int idx);
	
	void				onStartGame(int whoMoveFirst, int interval);
	void onMove(int whoMove, ubyte xS, ubyte yS, ubyte xD, ubyte yD, int nextMove, char newPieceType);
	void onOpponentWantUndo();
	void onOpponentWantDraw();
	void onOpponentWantLose();
	void onView(int moveFirst, int ownerUserId, int currUserID, int limitStepTime, int intervalView, int board[9][10]);
	void onReconnect(int moveFirst, int ownerUserId, int currUserID, int limitStepTime, int intervalView, int board[9][10]);
	void onEatAndLostPieces(int userEat, std::vector<char> eatPieces, std::vector<char> lostPieces);
	void onRemoveButton(int buttonId);
	void onPlayerToViewer(int leaveID, int newOwnerID);

	// Send to Server
	void moveCoTuong(ubyte supCommand, ubyte roomId, ubyte boardId, ubyte xS, ubyte yS, ubyte xD, ubyte yD);
	void sendCommandUndoChess(ubyte subCommand, ubyte roomID, ubyte boardID, ubyte request);
	void sendCommandChess(ubyte subCommand, ubyte roomID, ubyte boardID);
	void getCoPhoCoUp(int roomID, int boardID);


	int					getID_CoUp(CoTuongPiece* p);
	std::vector<Vec2>	findMove(CoTuongPiece* p, int turnColor, bool isLogic);
	std::vector<Vec2> findAdvisor(CoTuongPiece* p, int turnColor, bool isLogic);
	std::vector<Vec2> findBishop(CoTuongPiece* p, int turnColor, bool isLogic);
	std::vector<Vec2> findKing(CoTuongPiece* p, int turnColor, bool isLogic);
	std::vector<Vec2> findPawn(CoTuongPiece* p, int turnColor, bool isLogic);
	std::vector<Vec2> findKnight(CoTuongPiece* p, int turnColor, bool isLogic);
	std::vector<Vec2> findRook(CoTuongPiece* p, int turnColor, int nutBiChan[], bool isLogic);
	std::vector<Vec2> findCannon(CoTuongPiece* p, int turnColor, bool isLogic);


	void				generateStringCoPho();

	void				setTimeQuota(int userId, int time);
	int					getMyColor(int moveFirst);
	void				updateLabelTime(int playerid);

	void				finishGame(std::vector<GameResult*> result, ubyte typeFinish);

	void				playKillEffect(int id, int mau, Vec2 start_pos);

	std::string			formatTimer(int time);
	std::string			toStringPositionX(int x);
	int					covertPositionY(int y);
	std::string			getShortName(int pId);
	
	void				playNext();
	void				gotoStep(int idx);
	void				showBtnPlay(bool visible);
	void				notifyLastStep(std::string content);
	void				setCoPhoReconnect(std::vector<PieceStep> steps);
	
	void				clearBoard();
	void				showCoAnTop(bool one_row);
	void				hideCoAnTop(bool one_row);
	void				showCoAnBot(bool one_row);
	void				hideCoAnBot(bool one_row);

	void				addInfo_new(std::string str);

	friend class CoTuongBoard;

};

#define getCoTuongScreen() CoTuongScreen::getInstance()

#endif //__GAME_XIZACH_H__

