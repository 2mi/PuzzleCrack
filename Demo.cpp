// Puzzle.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include "Puzzle.h"

using namespace std;

int __cdecl main()
{
	vector<toward> act;
	string steps;
	vector<Sequence> store(16);//{7,0,5,1,11,6,9,8,12,14,13,4,10,15,2,3};/13
	PuzzleGame sapphire(4);

	for (auto &x : store) {
		cin >> (int&)x;
	}
	Sequence blank;
	cin >> blank;

	sapphire.Recover(&store[0], blank, act);
	sapphire.translateShow(act, steps);

	std::cout << steps.c_str() << std::endl;

	return 0;
}