#include "moves.h"

extern MoveTurn g_turn;

Coordinates find_starting_point(Coordinates target, Piece attacker) {
    
    switch (attacker) {
        case KNIGHT:
            return find_knight(target, g_turn);
        case BISHOP:
			return find_cross(target, g_turn, BISHOP);
        case ROOK:
			return find_straight(target, g_turn, ROOK);
        case QUEEN:
		{
            Coordinates o = find_straight(target, g_turn, QUEEN);
			if (o.col != COL_COUNT)
				return o;
			
			o = find_cross(target, g_turn, QUEEN);
			return o;
		}
        case KING:
            return find_king(target, g_turn);
		case BLANK:
			return find_king(target, g_turn);
		default:
			return find_king(target, g_turn);
    }
}

Coordinates find_knight(Coordinates target, PieceColor color) {
    int kv[8][2] = {{1,2}, {1,-2}, {-1,2}, {-1,-2},
                {2,1}, {2,-1}, {-2,1}, {-2,-1}};
    
    Coordinates start = {ROW_COUNT, COL_COUNT};
    uint8_t count = 0;

    for (int i = 0; i < 8; i++) {
        if ((target.row + kv[i][0] >= _1_) && (target.row + kv[i][0] <= _8_) &&
            (target.col + kv[i][1] >= _A_) && (target.col + kv[i][1] <= _H_) &&
            (board[target.row + kv[i][0]][target.col + kv[i][1]].color == color) &&
            (board[target.row + kv[i][0]][target.col + kv[i][1]].type == KNIGHT)) 
        {
            dlog("knight\n");
            if (++count > 1) {
                start.row = ROW_COUNT;
                start.col = COL_COUNT;
                break;
            }
            
            start.col = target.col + kv[i][1];
            start.row = target.row + kv[i][0];
        }
    }

    return start;
}

/* Kale ve/veya Vezir aramak için */
/* TODO: 4 kareyi tek seferde kontrol et */
Coordinates find_straight(Coordinates target, PieceColor color, Piece types) {
    Coordinates start = target;
	uint8_t l = 0, r = 0, f = 0, b = 0;
	uint8_t count = 0;
	
	for (int i = 1; i < 8; i++) {
		
		if (count > 1) {
			start.row = ROW_COUNT;
			start.col = COL_COUNT;
			break;
		}
		
		if (!l && ((target.col - i) >= _A_) && 
			(board[target.row][target.col - i].type & types) && 
			(board[target.row][target.col - i].color == color))
		{
			start.col = target.col - i;
			count++;
			l = 1;
		}
		else if (board[target.row][target.col - i].type != BLANK) {
			l = 1;
		}
		
		if (!r && ((target.col + i) <= _H_) && 
			(board[target.row][target.col + i].type & types) &&
			(board[target.row][target.col + i].color == color))
		{
			start.col = target.col + i;
			count++;
			r = 1;
		}
		else if (board[target.row][target.col + i].type != BLANK) {
			r = 1;
		}
		
		if (!f && ((target.row + i) <= _H_) && 
			(board[target.row + i][target.col].type & types) &&
			(board[target.row + i][target.col].color == color))
		{
			start.row = target.row + i;
			count++;
			f = 1;
		}
		else if (board[target.row + i][target.col].type != BLANK) {
			f = 1;	
		}
		
		if (!b && ((target.row - i) >= _A_) && 
			(board[target.row - i][target.col].type & types) &&
			(board[target.row - i][target.col].color == color))
		{
			start.row = target.row - i;
			count++;
			b = 1;
		}
		else if (board[target.row - i][target.col].type != BLANK) {
			b = 1;
		}
	}
	
	if (target.row == start.row && target.col == start.col) {
		start.col = COL_COUNT;
		start.row = ROW_COUNT;
	}
	
    return start;
}

/* Fil ve/veya Vezir aramak için */
/* TODO: 4 kareyi tek seferde kontrol et */
Coordinates find_cross(Coordinates target, PieceColor color, Piece types) {
    Coordinates start = target;
	uint8_t l = 0, r = 0, f = 0, b = 0;
	uint8_t count = 0;
	
	for (int i = 1; i < 8; i++) {
		
		if (count > 1) {
			start.row = ROW_COUNT;
			start.col = COL_COUNT;
			break;
		}
		
		/* Sağ-ileri */
		if (!l && ((target.row + i) <= _8_) && ((target.col + i) <= _H_) &&
			(board[target.row + i][target.col + i].type & types) && 
			(board[target.row + i][target.col + i].color == color))
		{
			start.row = target.row + i;
			start.col = target.col + i;
			count++;
			l = 1;
		}
		else if (board[target.row + i][target.col + i].type != BLANK) {
			l = 1;
		}
		
		/* Sağ-geri */
		if (!r && ((target.row - i) >= _1_) && ((target.col + i) <= _H_) &&
			(board[target.row - i][target.col + i].type & types) &&
			(board[target.row - i][target.col + i].color == color))
		{
			start.row = target.row - i;
			start.col = target.col + i;
			count++;
			r = 1;
		}
		else if (board[target.row - i][target.col + i].type != BLANK) {
			r = 1;
		}
		
		/* Sol-geri */
		if (!f && ((target.row - i) >= _A_) && ((target.col - i) >= _1_) &&
			(board[target.row - i][target.col - i].type & types) &&
			(board[target.row - i][target.col - i].color == color))
		{
			start.row = target.row - i;
			start.col = target.col - i;
			count++;
			f = 1;
		}
		else if (board[target.row - i][target.col - i].type != BLANK) {
			f = 1;	
		}
		
		/* Sol-ileri */
		if (!b && ((target.row + i) <= _8_) && ((target.col - i) >= _A_) &&
			(board[target.row + i][target.col - i].type & types) &&
			(board[target.row + i][target.col - i].color == color))
		{
			start.row = target.row + i;
			start.col = target.col - i;
			count++;
			b = 1;
		}
		else if (board[target.row + i][target.col - i].type != BLANK) {
			b = 1;
		}
	}
	
	if (target.row == start.row && target.col == start.col) {
		start.col = COL_COUNT;
		start.row = ROW_COUNT;
	}
	
	return start;
}

Coordinates find_king(Coordinates target, PieceColor color) {
	int kv[11][2] = {{1,-1}, {1,0}, {1,1}, {0,1}, 
                {-1,1}, {-1,0}, {-1,-1}, {0,-1}, 
				{0,-2}, {0,2}};
    
    Coordinates start = {ROW_COUNT, COL_COUNT};

    for (int i = 0; i < 11; i++) {
        if ((board[target.row + kv[i][0]][target.col + kv[i][1]].color == color) &&
            (board[target.row + kv[i][0]][target.col + kv[i][1]].type == KING)) 
        {
            dlog("king\n");
            
			start.row = target.row + kv[i][0];
            start.col = target.col + kv[i][1];
        }
    }

    return start;
}
