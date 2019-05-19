#ifndef __INPUT_METHODS_H_
#define __INPUT_METHODS_H_

#include "error.h"
#include "board.h"

extern ErrorCodes parse_human_input(char *human_input, MoveCoordinates *move);
extern ErrorCodes parse_notation(char *notation, MoveCoordinates *move);

#endif /* __INPUT_METHODS_H_ */
