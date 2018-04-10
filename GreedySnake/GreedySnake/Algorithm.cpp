#include "Algorithm.h"
#include <algorithm>
#include <iostream>
#include <functional>

Algorithm::Algorithm(GLuint x, GLuint y)
	:row(x), col(y), FOOD(0), EMPTY((row + 1)*(col + 1)),
	SNAKE(2 * EMPTY)
{
	food = std::make_shared<Object>(NewFood().x, NewFood().y);
	board.resize(row, std::vector<int>(col, EMPTY));
}

void Algorithm::ResetBoard(std::list<Object> &psnake, Object &pfood,
	std::vector<std::vector<int> > &pboard) {
	for (auto &t : pboard)
		std::fill(t.begin(), t.end(), EMPTY);
	pboard[pfood.Index.x][pfood.Index.y] = FOOD;
	for (auto &t : psnake)
		pboard[t.Index.x][t.Index.y] = SNAKE;
}

glm::ivec2 Algorithm::NewFood() {
	glm::ivec2 loc;
	loc.x = rand() % row;
	loc.y = rand() % col;
	while (true) {
		bool found = false;
		for (auto &x : snake) {
			if (loc == x.Index) {
				found = true;
				break;
			}
		}
		if (!found)return loc;
		loc.x = rand() % row;
		loc.y = rand() % col;
	}
	return loc;
}

void Algorithm::Display() {
	for (auto &t : board) {
		for (auto &x : t) {
			std::cout << x << "-";
		}
		std::cout << "\n";
	}
}

bool Algorithm::RefreshBoard(std::list<Object> &psnake, Object &pfood,
	std::vector<std::vector<int> > &pboard) 
{	/*
		��ʳ����������ù�����ȱ�����������ɢ
		�Ӷ��õ�pboard��ÿ�����ӵ���food��·������
	*/
	std::queue<glm::ivec2> record;
	record.push(pfood.Index);
	std::vector<std::vector<bool>>visited;
	visited.resize(pboard.size(), std::vector<bool>(pboard[0].size(), false));
	visited[pfood.Index.x][pfood.Index.y] = true;
	glm::ivec2 cur;
	bool found = false;
	while (!record.empty()) {
		glm::ivec2 head = record.front();
		record.pop();
		//���ĸ�������չ
		for (auto x = 0; x < 4; ++x) {
			cur = glm::ivec2(head.x + dir[x][0], head.y + dir[x][1]);
			//�����߽���Ѿ����ʹ���
			if (!CouldMove(cur) || visited[cur.x][cur.y])continue;
			if (cur == psnake.front().Index)found = true;//�ҵ���ͷ
			if (pboard[cur.x][cur.y] < SNAKE) {//��������
				pboard[cur.x][cur.y] = pboard[head.x][head.y] + 1;
				record.push(cur);
				visited[cur.x][cur.y] = true;
			}
		}
	}
	return found;
}

bool Algorithm::make_move(glm::ivec2 step) {
	//ֱ�Ӽ���ǰ��
	snake.push_front(Object(step.x,step.y));
	//����ӵĲ���ʳ��λ��,ɾ�����һ��
	if (snake.front().Index != food->Index) {
		snake.pop_back();
	}
	else {//����Ե�ʳ��
		food->Index = NewFood();//���²���һ���µ�ʳ��
		return true;
	}
	return false;
}

glm::ivec2 Algorithm::FindSafeWay() {
	return ShortestMove(snake, board);
}

glm::ivec2 Algorithm::AnyPossibleWay() {
	glm::ivec2 ret = glm::ivec2(-1,-1);
	int minv = SNAKE;
	for (auto x = 0; x < 4; ++x) {
		glm::ivec2 tmp = glm::ivec2(snake.front().Index.x + dir[x][0], snake.front().Index.y + dir[x][1]);
		if (CouldMove(tmp) && minv > board[tmp.x][tmp.y]) {
			minv = board[tmp.x][tmp.y];
			ret = tmp;
		}
	}
	return ret;
}

//AI˼��
glm::ivec2 Algorithm::AIThinking() {
	ResetBoard(snake, *food, board);
	glm::ivec2 move;
	if (RefreshBoard(snake, *food, board))//���ԳԵ�ʳ��
		move = FindSafeWay();
	else move = AnyPossibleWay();
	return move;
}

void Algorithm::ResetSnakeAndFood() {
	snake.clear();
	snake.push_back(Object(row / 2 - 1, col / 2 - 1));
	snake.push_back(Object(row / 2 - 1, col / 2 + 0));
	snake.push_back(Object(row / 2 - 1, col / 2 + 1));
	food->Index = NewFood();
}