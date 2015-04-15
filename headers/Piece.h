#pragma once
#include <cstdint>
#include "enums.h"

#define KINGVAL  	50000
#define QUEENVAL 	900
#define ROOKVAL  	500
#define BISHOPVAL 300
#define KNIGHTVAL 275
#define PAWNVAL   100

class piece
{
	private:
	public:
		uint32_t value;
		uint8_t x_pos;
		uint8_t y_pos;
		piece_type type;
		uint32_t moves;
		player_colour colour;
		piece(uint8_t x, uint8_t y, piece_type type_, player_colour colour_);
		piece();
		bool alive;
		void move_to(uint8_t x, uint8_t y);
		void move_back_to(uint8_t x, uint8_t y);
		void change_type(piece_type newtype);
		~piece();
};
