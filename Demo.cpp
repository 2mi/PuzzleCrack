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
	vector<Sequence> store{ 6,7,15,13,5,2,10,0,8,11,4,3,12,14,9,1 };
	PuzzleGame sapphire(4);

	sapphire.RecoverVector(&store[0], 12, act);
	sapphire.translateShow(act, steps);

	std::cout << steps.c_str() << std::endl;

	return 0;
}