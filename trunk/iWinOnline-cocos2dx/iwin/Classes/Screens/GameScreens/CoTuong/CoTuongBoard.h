#ifndef __CO_TUONG_BOARD_H__
#define __CO_TUONG_BOARD_H__

//#include "cocos2d.h"

//#include "UI/UIImageView.h"
#include "Screens/ScreenManager.h"
//USING_NS_CC;
using namespace ui;

class CoTuongPiece;
class CoTuongScreen;

class CoTuongBoard
{
	Vec2		_size_cell;
	Vec2		_pos_board;
	float		_distance_default;

	std::vector<CoTuongPiece*> _pool_piece;
	std::vector<ImageView*> _pool_move;
	std::vector<Vec2>		_suggest_move;
	ParticleSystemQuad*		_explosion;

	Scale9Sprite*					_last_move_from;
	Scale9Sprite*					_last_move_to;


	int			_board[10][9];

	CC_SYNTHESIZE(ui::Widget*, _board_node, BoardNode)

	CC_SYNTHESIZE(int, _myColor, MyColor)
	CC_SYNTHESIZE(int, _turnColor, TurnColor)
	CC_SYNTHESIZE(int, _lastRow, LastRow)
	CC_SYNTHESIZE(int, _lastCol, LastCol)
	CC_SYNTHESIZE(bool, _replay_mode, ReplayMode)

	CC_SYNTHESIZE(CoTuongScreen*, _game_screen, GameRoot)
	


		void		addPiece(int id, bool mau);
	CoTuongPiece*	getPieceById(int id, bool mau);
	ImageView*		getFreeMove();
	
public:
	CoTuongBoard();
	virtual ~CoTuongBoard();
	void init(Vec2 pos_board, Vec2 size_board);

	void loadBoard(int board[10][9], int myColor, int turnColor);
	CoTuongPiece* getPiece(int row, int col);
	CoTuongPiece*	getInvisiblePiece();
	int				getBoardValue(int row, int col);
	void addSuggestMove(std::vector<Vec2> moves);
	Vec2 getPosFromRowCol(int row, int col);
	void clearMoves();
	void resetSelect();
	void move(CoTuongPiece*, int toRow, int toCol, int new_piece_id, std::function<void()> action_done = nullptr);
	Vec2 convertToRowCol(Vec2 pos_touch);
	void playParticle(Vec2 dest);

	bool isCheckMate(int turnColor);
	void setBoard(int newBoard[10][9], int map[10][9], int myColor, int turnColor);
	void setBoardReplay(int newBoard[10][9], int myColor, int turnColor);
	void setCellSize(Vec2 cell_size);
	void enableTouch(bool enable);
	void enableBoardTouch(bool enable);
	bool isMoveSuggest(Vec2 row_col);
	void clear();

};


#endif //__CO_TUONG_BOARD_H__