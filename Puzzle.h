#pragma once

// Puzzle.cpp : 定义控制台应用程序的入口点。
//

#include <queue>
#include <map>
#include <stdlib.h>

using std::priority_queue;
using std::vector;
using std::deque;
using std::string;
using std::map;
using Sequence = uint8_t;
using Hash = uint32_t;
using toward = uint8_t;
using price = uint8_t;// !too small

typedef struct __Container {
private:
	Sequence v[1];
public:
	__Container() {
	}
	__Container(Sequence *CM, int size) {
		for (int i = size - 1; i >= 0 ; --i) {
			v[i] = (CM[2 * i + 1] << 4 ) | CM[2 * i];
		}
	}
	__Container(const __Container &t, int size) {
		memcpy(v, t.v, size);
	}
	//__Block(const __Block &t) = delete;
	auto operator[](int index) {
		return (v[index >> 1] >> ((index & 1) << 2)) & 0xF;
	}
	void swap(int a, int b) {
		Sequence temp = operator[](a) ^ operator[](b);
		v[a >> 1] ^= temp << ((a & 1) << 2);
		v[b >> 1] ^= temp << ((b & 1) << 2);
	}

};
using Martix = __Container;

// axis
#define VERTICAL (0)
#define HORIZONTAL (1)
// direction
#define POSITIVE (0 << 1)
#define NEGATIVE (1 << 1)
typedef enum {
	UP = POSITIVE | VERTICAL,
	RIGHT = POSITIVE | HORIZONTAL,
	DOWN = NEGATIVE | VERTICAL,
	LEFT = NEGATIVE | HORIZONTAL
} __toward;

#define alloc_state()	reinterpret_cast<st_martix*>(operator new (size_of_state()))
#define initial_state(dst)	memset(dst, 0, size_of_state())
#define copy_martix(dst, src)	dst.Martix::__Container(src, total() / 2)
#define exchange(a, b)	st->Puzzle.swap(st_parent->blank, index)
#define total()	(this->rank * this->rank)
#define size_of_state() (sizeof(st_martix) - 1 + total() / 2)

/*
	Astar using in PuzzleGame
*/

typedef struct __st_martix {
	__st_martix* iParent;
	price Hx;
	toward Action;
	Sequence blank;
	Martix Puzzle;

	bool operator ()(__st_martix *&a, __st_martix *&b) const {
		return a->Hx > b->Hx;
	}
}st_martix;


class PuzzleGame
{
public:
	PuzzleGame(const uint8_t R = 4);
	~PuzzleGame();

	bool RecoverVector(Sequence *puzz, Sequence blank, vector<toward> &act);

	void translateShow(vector<toward>& act, string & txt);

protected:
	Hash get_martix_state(Martix &M);
	st_martix * get_from_openqueue();
	void put_into_openqueue(st_martix * st_parent);
	bool put_into_closedqueue(Hash hash, st_martix * st);
	int32_t Hx(int32_t fx);
	int32_t deltaHx(Martix &M, Sequence index, toward act);
	uint8_t Fx(Martix &M);

private:
	const uint8_t rank;
	priority_queue<st_martix*, deque<st_martix*>, st_martix> q_open;
	map<Hash, st_martix*> m_closed;
};

PuzzleGame::PuzzleGame(uint8_t R): rank(R)
{
	
}

PuzzleGame::~PuzzleGame()
{

}

bool PuzzleGame::RecoverVector(Sequence *puzz, Sequence blank, vector<toward> &steps)
{
	auto st = alloc_state();
	auto fx = 0;

	int8_t index;

	initial_state(st);
	{
		st->iParent = nullptr;
		st->blank = blank;
		copy_martix(st->Puzzle, puzz);
		st->Hx = Hx(fx = Fx(st->Puzzle)) - Hx(0);
	}


	//const static Hash fhash_table[] = { 0x1e6c5704,0x0b11ddf8,0x2487d50c };	fhash_table[rank - 3]
	const Hash final_hash = 0x0b11ddf8;

	for (auto Puzzle = &st->Puzzle; ; Puzzle = &st->Puzzle)
	{
		Hash hash = get_martix_state(*Puzzle);

		if (final_hash == hash) {
			break;
		}

		if (put_into_closedqueue(hash, st)) {
			put_into_openqueue(st);
		}

		st = get_from_openqueue();
	}


	for (; st->iParent; st = st->iParent) {
		steps.push_back(st->Action);
	}

	return true;
}

void PuzzleGame::translateShow(vector<toward> &act, string &txt)
{
	for (int i = act.size() - 1; i >= 0; --i) {
		static map<toward, string> direction { { UP, "↑ " },{ RIGHT, "→ " },{ DOWN, "↓ " },{ LEFT, "← " } };
		txt = txt + direction[act[i]];
	}
}

/*
	BFS with Prioity_Queue
*/

void PuzzleGame::put_into_openqueue(st_martix *st_parent)
{
	st_martix  *st;

	auto alloc = [&](toward act, uint8_t index) -> void __fastcall  {
		st = alloc_state();
		st->iParent = st_parent;
		st->Action = act;

		st->Hx = st_parent->Hx + Hx(deltaHx(st_parent->Puzzle, index, act));
		copy_martix(st->Puzzle, st_parent->Puzzle);
		exchange(st->Puzzle[st_parent->blank], st->Puzzle[index]);
		st->blank = index;

		q_open.push(st);
	};

	if (st_parent->blank > rank - 1) {
		alloc(DOWN, st_parent->blank - rank);
	}
	if (st_parent->blank % rank < rank - 1) {
		alloc(LEFT, st_parent->blank + 1);
	}
	if (st_parent->blank < total() - 1 - rank) {
		alloc(UP, st_parent->blank + rank);
	}
	if (st_parent->blank % rank > 0) {
		alloc(RIGHT, st_parent->blank - 1);
	}
}

bool PuzzleGame::put_into_closedqueue(Hash hash, st_martix* st)
{
	auto existed = m_closed.insert(map<Hash, st_martix*>::value_type(hash, st));
	return existed.second;
}

Hash PuzzleGame::get_martix_state(Martix &M)
{
	Hash hashCode = 0;

	for (int8_t i = total() - 1; i >= 0; --i)
	{	//SDBMhash => hashCode = 65539 * hashCode + M[i]
		hashCode = M[i] + (((hashCode << 10) + hashCode) << 6) - hashCode;
	}

	return hashCode & 0x7FFFFFFF;
}

st_martix* PuzzleGame::get_from_openqueue()
{
	const st_martix* stat = q_open.top();
	q_open.pop();
	return const_cast<st_martix*>(stat);
}

/*
	H(x) = aF(x) + bG(x)
*/

int32_t PuzzleGame::Hx(int32_t fx)
{
	constexpr static auto Gx = 1;

	return 5 * fx + 1 * Gx;
}

int32_t PuzzleGame::deltaHx(Martix &M, Sequence index, toward act)
{
	if (act & HORIZONTAL) {
		auto row_o = M[index] % rank, row_p = index % rank;
		return (row_o < row_p && (act & NEGATIVE) || row_p < row_o && !(act & NEGATIVE)) ? -1 : 1;
	}else{
		auto col_o = M[index] / rank, col_p = index / rank;
		return (col_o < col_p && !(act & NEGATIVE) || col_p < col_o && (act & NEGATIVE)) ? -1 : 1;
	}/*
	return (move & 1
		? (row_o < row_p && (move & 2) || row_p < row_o && !(move & 2)) ? -1 : 1
		: (col_o < col_p && !(move & 2) || col_p < col_o && (move & 2)) ? -1 : 1) \
		;*/
}

uint8_t PuzzleGame::Fx(Martix &M)
{
	uint8_t distance = 0;

	for (int8_t i = total() - 1; i >= 0; --i) {
		distance += abs(M[i] / rank - i / rank) + abs(M[i] % rank - i % rank);
	}

	return distance;
}