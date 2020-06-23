#pragma once

#define TABLE_HEIGHT 5

class Table {
	public:
		float table[2][TABLE_HEIGHT];
		
		float position;
		float deltaX;
		
		void reset();
		void calculate(Function function);
		
		void scrollUp(Function function);
		void scrollDown(Function function);
		
		void draw();
};
