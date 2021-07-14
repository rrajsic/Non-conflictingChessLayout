#include <vector>
#include <algorithm>
#include "Pieces.h"
#include "EnumFunctions.h"
#include "EnumLayout.h"
#include "Chessboard.h"
#include "Engine.h"


/////////////////////////////Main engine functions////////////////////////////////////////////////
bool Engine::calculatePossibleLayouts(const Functions function) {
	bool isSuccess = false;
	int counter(1);
	int num_of_permutations = m_pieces_all_permutations.size();

	while (!m_pieces_all_permutations.empty()) {
		Chessboard* board = new Chessboard();
		std::vector<Piece*> temp_pieces = std::move(m_pieces_all_permutations.back());
		std::cout << "Calculating permutations: " << counter << "/" << num_of_permutations << "\n";

		switch (function) {
			case Functions::DISPLAY_ALL_LAYOUTS:
			case Functions::DISPLAY_FUNDEMENTAL_LAYOUTS:
				if (saveLayouts(*board, temp_pieces, 0, temp_pieces.size(), Layouts::EVERY))
					isSuccess = true;
				break;

			case Functions::DISPLAY_FIRST_POSSIBLE_LAYOUT:
				if (saveLayouts(*board, temp_pieces, 0, temp_pieces.size(), Layouts::FIRST)) 
					isSuccess = true;
				break;

			default:
				return -2;
		}
		m_pieces_all_permutations.pop_back();
		counter++;
	}

	return isSuccess;
}

bool Engine::saveLayouts(Chessboard& board, std::vector<Piece*> pieces, int piece_index, int piece_count, Layouts combinations) {
	bool isSuccess = false;
	Chessboard temp_board = std::move(board);

	for (int i = 0; i < g_board_size; i++) 
		for (int j = 0; j < g_board_size; j++) 
			if (temp_board(i,j) == Type::UNOCCUPIED) {
				if (pieces[piece_index]->placePiece(i, j, temp_board) == SUCCESS) {
					if (piece_index == (piece_count - 1)) {

						m_temp_boards.emplace_back(new Chessboard(std::move(temp_board)));
						isSuccess = true;
						if (combinations == Layouts::FIRST) 
							return isSuccess;
						
					} else {
						if (saveLayouts(temp_board, pieces, piece_index + 1, piece_count, combinations)) {

							isSuccess = true;
							if (combinations == Layouts::FIRST) 
								return isSuccess;
						}	
					}
				}
				temp_board = std::move(board);
			}

	return isSuccess;
}

void Engine::filterBoards(const Functions function) {

	switch (function) {
	case Functions::DISPLAY_ALL_LAYOUTS:
		for (auto board : m_temp_boards) 
			if (!doesBoardExist(*board))m_boards.emplace_back(std::move(board));	
		break;

	case Functions::DISPLAY_FUNDEMENTAL_LAYOUTS:
		for (auto board : m_temp_boards) 
			if (!doesRotatedOrReflectedBoardExist(*board))m_boards.emplace_back(std::move(board));
		break;

	case Functions::DISPLAY_FIRST_POSSIBLE_LAYOUT:
		m_boards.push_back(m_temp_boards.back());
		break;

	default:
		std::cout << "Error impossible function call\n";
		break;
	}
}

////////////////////////////Vector Functions/////////////////////////////////////////////////

void Engine::pushPiece(Piece* piece) {
	m_pieces.push_back(piece);
}

void Engine::setPiecesPermutations() {
	std::cout << "Permuting..." << std::endl;
	int counter(0);
	std::sort(m_pieces.begin(), m_pieces.end(),[](Piece*& p1, Piece*& p2) {
		return p1->getType() < p2->getType();
		});

	do {
		if (!doesVectorExist(m_pieces))
			m_pieces_all_permutations.emplace_back(std::move(m_pieces));
		counter++;
	} while (std::next_permutation(begin(m_pieces), end(m_pieces), [](Piece *&p1,Piece* &p2) {
		return p1->getType() < p2->getType();
		}));
}

bool Engine::doesVectorExist(std::vector<Piece*> v) {
	for (auto i : m_pieces_all_permutations)
		if (areVectorsEqual(i, v))return true;
	
	return false;
}

bool Engine::areVectorsEqual(std::vector<Piece*> v1, std::vector<Piece*> v2) {
	for (int i = 0; i < v1.size(); i++) 
		if (v1[i]->getType() != v2[i]->getType())return false;
	
	return true;
}

///////////////////////////////Rotating board functions///////////////////////////////////////

bool Engine::doesBoardExist(Chessboard& board) {

	for (auto existingBoard : m_boards) {
		if (board.equals(*existingBoard))
			return true;
	}
	return false;
}

bool Engine::doesRotatedOrReflectedBoardExist(Chessboard& const board) {

	for (auto existingBoard : m_boards) {
		if (board.equals(*existingBoard))
			return true;

		Chessboard rotated_tempBoard = *existingBoard;
		Chessboard reflected_tempBoard = *existingBoard;

		for (int i = 0; i < 4; i++) {
			rotated_tempBoard=std::move(rotateBoard90Degrees(rotated_tempBoard));
			reflected_tempBoard=std::move(reflectBoardVerticaly(rotated_tempBoard));
		
			if (board.equals(std::move(rotated_tempBoard)) || board.equals(std::move(reflected_tempBoard))) 
				return true;
		}
	}

	return false;
}

Chessboard Engine::rotateBoard90Degrees(Chessboard &board) {
	Chessboard temp = std::move(board);
	for (int i = 0; i < g_board_size; i++) {
		for (int j = 0; j < g_board_size; j++) {
			temp(i,j) = board(g_board_size - 1 - j, i);
		}
	}
	
	return std::move(temp);
}

Chessboard Engine::reflectBoardVerticaly(Chessboard &board) {
	Chessboard temp = std::move(board);
	for (int i = 0; i < g_board_size; i++) {
		for (int j = 0; j < g_board_size; j++) {
			temp(i,j) = board(g_board_size - 1 - i, j);
		}
	}

	return std::move(temp);
}

