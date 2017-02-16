
#include "UI/AdvanceUI/wLayout.h"

#include "CoTuongBoard.h"
#include "CoTuongPiece.h"
#include "CoTuongScreen.h"

#define COTUONG_NUM_ROW 10 
#define COTUONG_NUM_COL 9 

CoTuongBoard::CoTuongBoard()
{

}

CoTuongBoard::~CoTuongBoard()
{
	for (auto p : _pool_piece)
	{
		p->release();
	}
	_pool_piece.clear();
}

void CoTuongBoard::init(Vec2 pos_board, Vec2 size_board)
{
	_pos_board = pos_board;
	_distance_default = _size_cell.x * 2;
	for (int i = 0; i < 2; i++)
	{
		addPiece(PIECE_KING, i == 0);
		addPiece(PIECE_ADVISOR, i == 0);
		addPiece(PIECE_ADVISOR, i == 0);
		addPiece(PIECE_BISHOP, i == 0);
		addPiece(PIECE_BISHOP, i == 0);
		addPiece(PIECE_KNIGHT, i == 0);
		addPiece(PIECE_KNIGHT, i == 0);
		addPiece(PIECE_ROOK, i == 0);
		addPiece(PIECE_ROOK, i == 0);
		addPiece(PIECE_CANNON, i == 0);
		addPiece(PIECE_CANNON, i == 0);
		addPiece(PIECE_PAWN, i == 0);
		addPiece(PIECE_PAWN, i == 0);
		addPiece(PIECE_PAWN, i == 0);
		addPiece(PIECE_PAWN, i == 0);
		addPiece(PIECE_PAWN, i == 0);
	}

	_explosion = ParticleSystemQuad::create(File::ResolveNamePath("particle/explosion.plist").GetString());
	_board_node->addChild(_explosion, 4);

	_last_move_from = Scale9Sprite::createWithSpriteFrameName("loginscreen/nine/red5.9.png");
	_last_move_from->setContentSize(cocos2d::Size(_size_cell.x, _size_cell.y));
	_last_move_from->setVisible(false);
	_board_node->addChild(_last_move_from, 4);
	_last_move_to = Scale9Sprite::createWithSpriteFrameName("loginscreen/nine/green5.9.png");
	_board_node->addChild(_last_move_to, 4);
	_last_move_to->setContentSize(cocos2d::Size(_size_cell.x, _size_cell.y));
	_last_move_to->setVisible(false);
	_replay_mode = false;
}

void CoTuongBoard::addPiece(int id, bool mau)
{
	CoTuongPiece* piece = CoTuongPiece::create();
	piece->retain();
	piece->setID(id, mau);
	_pool_piece.push_back(piece);
	piece->addTouchEventListener(CC_CALLBACK_2(CoTuongScreen::onTouchPiece, _game_screen));
}

CoTuongPiece* CoTuongBoard::getInvisiblePiece()
{
	for (CoTuongPiece* p : _pool_piece)
	{
		if (!p->isVisible())
		{
			return p;
		}
	}
	return nullptr;
}

CoTuongPiece* CoTuongBoard::getPieceById(int id, bool mau)
{
	for (CoTuongPiece* p : _pool_piece)
	{
		if (!p->isVisible() && p->getID() == id && p->getMau() == mau)
		{
			return p;
		}
	}
	return nullptr;
}

void CoTuongBoard::loadBoard(int board[COTUONG_NUM_ROW][COTUONG_NUM_COL], int myColor, int turnColor)
{
	_myColor = myColor;
	_turnColor = turnColor;

	for (CoTuongPiece* p : _pool_piece) 
	{
		p->setVisible(false);
	}

	int myId = myColor * 10;
	int oppId = (1 - myColor) * 10;
	int mau;
	for (int i = 0; i < COTUONG_NUM_ROW; i++)
	{
		for (int j = 0; j < COTUONG_NUM_COL; j++)
		{
			_board[i][j] = board[i][j];
			if (_board[i][j] == 0) continue;
			_board[i][j] = (i < 5 ? myId : oppId) + board[i][j];
			mau = (i < 5 ? myColor : (1 - myColor));

			CoTuongPiece* piece = getInvisiblePiece();
			piece->reset();
			piece->setID(_board[i][j] % 10, mau);
			if (piece && !piece->getParent()) {
				_board_node->addChild(piece, 2);
				piece->setSize(cocos2d::Size(_size_cell.x , _size_cell.y ));
			}
			piece->setRow(i);
			piece->setCol(j);
			piece->setPosition(Vec2(_pos_board.x + j * _size_cell.x, _pos_board.y + i * _size_cell.y));
			if (!_replay_mode)
				piece->setTouchEnabled(mau == myColor);
			else
				piece->setTouchEnabled(false);
			piece->setVisible(true);
		}
	}
}

CoTuongPiece* CoTuongBoard::getPiece(int row, int col)
{
	for (CoTuongPiece* p : _pool_piece)
	{
		if (p->isVisible() && p->getRow() == row && p->getCol() == col)
		{
			return p;
		}
	}
	return nullptr;
}

int CoTuongBoard::getBoardValue(int row, int col)
{
	return _board[row][col];
}

Vec2 CoTuongBoard::getPosFromRowCol(int row, int col)
{
	return Vec2(_pos_board.x + col * _size_cell.x, _pos_board.y + row * _size_cell.y);
}

ImageView* CoTuongBoard::getFreeMove()
{
	for (ImageView* img : _pool_move) {
		if ( !img->isVisible()) {
			return img;
		}
	}

	ImageView* new_img = ImageView::create();
	new_img->loadTexture("gamescreen-cotuong-screen/di.png", TextureResType::PLIST);
	_pool_move.push_back(new_img);
	_board_node->addChild(new_img, 5);
	new_img->setContentSize(cocos2d::Size(100, 100));
	return new_img;
}

void CoTuongBoard::addSuggestMove(std::vector<Vec2> moves)
{
	clearMoves();
	_suggest_move = moves;
	ImageView* move_node = getFreeMove();
	for (Vec2 pos : moves) {
		CoTuongPiece* eat_piece = getPiece((int)pos.x, (int)pos.y);
		if (!eat_piece)
		{
			move_node = getFreeMove();
			move_node->setVisible(true);
			move_node->setPosition(getPosFromRowCol(pos.x, pos.y));
		}
		else
		{
			eat_piece->playEffectBounce(Color3B::RED, 1.6f);
		}
	}

}

void CoTuongBoard::clearMoves()
{
	for (ImageView* img : _pool_move) {
		img->setVisible(false);
	}
	_suggest_move.clear();
}

void CoTuongBoard::resetSelect()
{
	for (CoTuongPiece* p : _pool_piece) {
		p->unselect();
	}
}

void CoTuongBoard::move(CoTuongPiece* piece, int toRow, int toCol, int new_piece_id, std::function<void()> action_done)
{
	_lastRow = piece->getRow();
	_lastCol = piece->getCol();
	_last_move_from->setVisible(!_replay_mode);
	_last_move_to->setVisible(!_replay_mode);
	_last_move_from->setPosition(getPosFromRowCol(_lastRow, _lastCol));
	_last_move_to->setPosition(getPosFromRowCol(toRow, toCol));
	_board[_lastRow][_lastCol] = 0;
	if (piece->getID() == PIECE_FACE)
	{
		_board[toRow][toCol] = piece->getMau() * 10 + piece->getID();
	}
	else
	{
		_board[toRow][toCol] = piece->getMau() * 10 + new_piece_id;
	}

		
	CoTuongPiece* eat_piece = getPiece(toRow, toCol);

	piece->setRow(toRow);
	piece->setCol(toCol);
	const float s_time_piece_move = 0.2f;
	Vec2 last_point = getPosFromRowCol(_lastCol, _lastRow);
	Vec2 to_point = getPosFromRowCol(toCol, toRow);
	float time_move = (last_point - to_point).getLength() / _distance_default * s_time_piece_move;
	if (time_move <= 0.3) time_move = .3f;
	if (eat_piece) {
		Vec2 dest = getPosFromRowCol(toRow, toCol);

		piece->runAction(Sequence::create(MoveTo::create(time_move, getPosFromRowCol(toRow, toCol)), CallFunc::create([this, dest, eat_piece, piece, new_piece_id, action_done]() {
			this->isCheckMate(this->getTurnColor());
			this->playParticle(dest);
			eat_piece->setVisible(false);
			Vec2 world_pos = eat_piece->convertToWorldSpaceAR(Vec2::ZERO);
			if (!this->getReplayMode() && GameController::getCurrentGameType() == GAMEID_CO_UP)
			{
				if (this->getGameRoot()->getReceiveOnEat()) 
				{
					this->getGameRoot()->playKillEffect(this->getGameRoot()->getLastEatID(), this->getGameRoot()->getLastEatMau(), world_pos);
				}
			}

			if( new_piece_id != -1 )
				piece->setID(new_piece_id, piece->getMau());
			if (action_done) action_done();
		}), nullptr));
	}
	else
	{
		piece->runAction(Sequence::create(MoveTo::create(time_move, getPosFromRowCol(toRow, toCol)), CallFunc::create([this, piece, new_piece_id, action_done]() {
			this->isCheckMate(this->getTurnColor());
			if(new_piece_id != -1)
				piece->setID(new_piece_id, piece->getMau());
			if (action_done) action_done();
		}), nullptr));
	}

	_turnColor = 1 - _turnColor;
	if (_turnColor == _myColor) {
		for (CoTuongPiece* p : _pool_piece)
		{
			if (p->getParent())
			{
				p->setTouchEnabled(p->getMau() == _myColor);
			}
		}
	}
	else
	{
		for (CoTuongPiece* p : _pool_piece)
		{
			if (p->getParent())
			{
				p->setTouchEnabled(false);
			}
		}
	}

}


Vec2 CoTuongBoard::convertToRowCol(Vec2 pos_touch)
{
    cocos2d::Rect cell;
	int touch_w = _size_cell.x;
	int touch_h = _size_cell.y;
	for (int i = 0; i < COTUONG_NUM_ROW; i++)
	{
		for (int j = 0; j < COTUONG_NUM_COL; j++)
		{
			Vec2 pos = getPosFromRowCol(i, j);
			cell = cocos2d::Rect( pos.x - touch_w / 2, pos.y - touch_h / 2, touch_w, touch_h);
			if (cell.containsPoint(pos_touch)) {
				return Vec2(j, i);
			}
		}
	}
	return Vec2(-1, -1);
}


void CoTuongBoard::playParticle(Vec2 dest)
{
	_explosion->setPosition(dest);
	_explosion->resetSystem();
}

bool CoTuongBoard::isCheckMate(int turnColor)
{
	CoTuongPiece* temp_p;
	for (CoTuongPiece* p : _pool_piece)
	{
		if (p->getParent() && p->isVisible())
		{
			if (p->getMau() == turnColor)
			{
				std::vector<Vec2> listMove = _game_screen->findMove(p, turnColor, true);
				for (int i = 0; i < listMove.size(); i++)
				{
					temp_p = getPiece((int)listMove[i].x, (int)listMove[i].y);
					if (temp_p && temp_p->getID() == PIECE_KING)
					{
						return true;
					}
				}
			}
			else
			{
				std::vector<Vec2> listMove = _game_screen->findMove(p, p->getMau(), true);
				for (int i = 0; i < listMove.size(); i++)
				{
					temp_p = getPiece((int)listMove[i].x, (int)listMove[i].y);
					if (temp_p && temp_p->getID() == PIECE_KING)
					{
						temp_p->setTuongBiChieu(true);
						p->setConChieu(true);
						temp_p->playEffectBounce(Color3B::RED, 1.6f);
						p->playEffectBounce(Color3B::WHITE);
						_game_screen->showEffectChieu(true);
					}
				}
			}
		}
	}

	return false;
}

void CoTuongBoard::setBoardReplay(int newBoard[10][9], int myColor, int turnColor)
{
	for (CoTuongPiece* p : _pool_piece)
	{
		p->setVisible(false);
	}
	int myId = myColor * 10;
	int oppId = (1 - myColor) * 10;
	for (int r = 0; r < 10; r++) {
		for (int c = 0; c < 9; c++) {
			_board[r][c] = newBoard[r][c];
			if (_board[r][c] != 0) {
				CoTuongPiece* p = getInvisiblePiece();
				p->setID(_board[r][c] % 10, _board[r][c] / 10);
				p->setRow(r);
				p->setCol(c);
				p->setPosition(getPosFromRowCol(r, c));
				p->setVisible(true);
			}
		}
	}
}

void CoTuongBoard::setBoard(int newBoard[COTUONG_NUM_ROW][COTUONG_NUM_COL], int map[COTUONG_NUM_ROW][COTUONG_NUM_COL], int myColor, int turnColor)
{
	loadBoard(newBoard, myColor, turnColor);
	for (CoTuongPiece* p : _pool_piece)
	{
		p->setVisible(false);
	}
	for (int r = 0; r < 10; r++) {
		for (int c = 0; c < 9; c++) {
			_board[r][c] = map[r][c];
			if (_board[r][c] != 0) {
				CoTuongPiece* p = getInvisiblePiece();
				p->setID(_board[r][c] % 10, _board[r][c] / 10);
				p->setRow(r);
				p->setCol(c);
				p->setPosition(getPosFromRowCol(r, c));
				p->setVisible(true);
			}
		}
	}
	if (_replay_mode && turnColor == myColor) {
		for (int i = 0; i < _pool_piece.size(); i++) {
			_pool_piece[i]->setTouchEnabled(_pool_piece[i]->getMau() == myColor);
		}
	}
	else if (_replay_mode)
	{
		for (int i = 0; i < _pool_piece.size(); i++) {
			_pool_piece[i]->setTouchEnabled(false);
		}
	}

}

void CoTuongBoard::enableTouch(bool enable)
{
	enableBoardTouch(enable);
	for (CoTuongPiece* p : _pool_piece)
	{
		p->setTouchEnabled(enable);
	}

}

void CoTuongBoard::enableBoardTouch(bool enable)
{
	_board_node->setTouchEnabled(enable);
}

void CoTuongBoard::clear()
{
	for (CoTuongPiece* p : _pool_piece)
	{
		p->setVisible(false);
	}
	_last_move_from->setVisible(false);
	_last_move_to->setVisible(false);
}

bool CoTuongBoard::isMoveSuggest(Vec2 row_col)
{
	for (int i = 0; i < _suggest_move.size(); i++)
	{
		if (_suggest_move[i].x == row_col.y && _suggest_move[i].y == row_col.x)
		{
			return true;
		}
	}

	return false;
}

void CoTuongBoard::setCellSize(Vec2 cell_size)
{
	_size_cell = cell_size;
}