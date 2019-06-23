#include <stdlib.h>
#include "moves.h"

extern MoveTurn g_turn;

MoveList *moves = NULL;
MoveList *current_move = NULL;
int move_count = 0;

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

void init_moves(void) {
	moves = NULL;
}

MoveList *add(MoveList **root, MoveCoordinates new_move) {
    MoveList **temp = root;
    MoveList *p_prev = NULL;
    
    while (*temp != NULL) {
        p_prev = (*temp);
        temp = &(*temp)->next;
    }
    
    *temp = malloc(sizeof(MoveList));
    if (*temp == NULL)
        return NULL;
	
    (*temp)->no = move_count++;
    (*temp)->mc = new_move;
    (*temp)->prev = p_prev;
    (*temp)->next = NULL;
    
    return *temp;
}

MoveList *add_to(MoveList **root, int index, MoveCoordinates new_move) {
    MoveList **temp = root;
    MoveList *p_prev = NULL;
    MoveList *p_next = NULL;
    
    while (index--) {
        p_prev = *(temp);
        temp = &(*temp)->next;
    }
    
    p_next = *temp;
    *temp = (MoveList *)malloc(sizeof(MoveList));
    if (*temp == NULL)
        return NULL;
    
	p_next->prev = *temp;
	(*temp)->no = move_count++;
    (*temp)->mc = new_move;
    (*temp)->next = p_next;
    (*temp)->prev = p_prev;
    
    return *temp;
}

MoveList *get(MoveList *root, int index) {
    MoveList *temp = root;
    int i = 0;
    
    while (i++ < index)
        temp = temp->next;
    
    return temp;
}

MoveList *get_prev(MoveList *node) {
	
	if (node->prev == NULL) {
		dlog("prev null");
		return node;
	}
	else if (node->prev->next == NULL) {
		dlog("endf");
	}
	else if (node->prev->prev == NULL) {
		dlog("endp");
	}
	
    return node->prev;
}

MoveList *get_next(MoveList *node) {
	
	if (node->next == NULL) {
		dlog("next null");
		return node;
	}
	else if (node->next->next == NULL) {
		dlog("endff");
	}
	else if (node->next->prev == NULL) {
		dlog("endpp");
	}
	
    return node->next;
}

MoveList *get_tail(MoveList *node) {
	
	if (node == NULL)
		return NULL;
	
    while (node->next != NULL) {
        node = node->next;
    }
    
    return node;
}

MoveList *get_head(MoveList *node) {
	
	if (node == NULL)
		return NULL;
	
    while (node->prev != NULL) {
        node = node->prev;
    }
    
    return node;
}

void remove_move(MoveList *node) {
    free(node);
}

void remove_from(MoveList *node) {
    MoveList *iter = node;
    
	if (iter == NULL)
		return;
	
    while (iter->next != NULL) {
        iter = iter->next;
    }
    
    while (iter != node) {
        iter = iter->prev;
        
        free(iter->next);
        iter->next = NULL;
    }
	iter = iter->prev;
	free(iter->next);
	iter->next = NULL;
}

void show_list(MoveList *root) {
	MoveList *iter = root;
	
	while (iter != NULL) {
		dlog("%d - from: %d-%d, to: %d-%d p: %p, n: %p", 
			iter->no, iter->mc.from.row, iter->mc.from.col, 
				iter->mc.to.row, iter->mc.to.col, iter->prev, iter->next);
		iter = iter->next;
	}
	
	if (current_move != NULL) {
		dlog("%d - current: %d-%d, %d-%d p: %p, n: %p", current_move->no, 
			current_move->mc.from.row, current_move->mc.from.col,
				current_move->mc.to.row, current_move->mc.to.col,
					current_move->prev, current_move->next);
	}
}
