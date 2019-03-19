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
	vector<Sequence> store{ 7,0,5,1,11,6,9,8,12,14,13,4,10,15,2,3 };//2,1,7,5,0,3,4,8,6
	PuzzleGame sapphire(4);

	sapphire.RecoverVector(&store[0], 8, act);
	sapphire.translateShow(act, steps);

	std::cout << steps.c_str() << std::endl;

	return 0;
}