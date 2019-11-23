#include <iostream>

using namespace std;
#include "ConsoleGameEngine.h"
class FlappyBird : public olcConsoleGameEngine {
public:
	FlappyBird() {
		m_sAppName = L"Flappy Bird";
	}


private:	
	float birdVelocity = 0.0f;
	float birdAcceleration = 0.0f;
	float gravity = 50.0f;
	float birdY = 0.0f;
	boolean hasCollided = false;
	list<int> sections;
	float sectionWidth;
	float levelPosition;
	boolean reset = false;
protected:
	virtual bool OnUserCreate() {
		birdY = ScreenHeight() / 2.0f;
		sections = { 0,0,0,0,0 };
		sectionWidth = (float)ScreenWidth() / (float)(sections.size() - 1);
		return true;
	}

	virtual bool OnUserUpdate(float timeElapsed) {
		if (reset)
		{
			hasCollided = false;
			reset = false;
			birdY = ScreenHeight() / 2.0f;
			sections = { 0,0,0,0,0 };
			sectionWidth = (float)ScreenWidth() / (float)(sections.size() - 1);
		}
		if (hasCollided)
		{
			if (m_keys[VK_SPACE].bPressed)
			{
				reset = true;
			}
		}
		else
		{
			//Clear the screen on each update
			Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');

			int birdX = ScreenWidth() / 3.0;

			if (m_keys[VK_SPACE].bPressed && birdVelocity >= gravity / 11.0f)
			{
				birdAcceleration = 0.0f;

				birdVelocity -= gravity / 2.0f;

			}
			else
			{
				birdAcceleration += gravity * timeElapsed;
			}

			if (birdAcceleration > gravity)
				birdAcceleration = gravity;

			birdVelocity += birdAcceleration * timeElapsed;

			birdY += birdVelocity * timeElapsed;

			levelPosition += 10.0 * timeElapsed;

			if (birdVelocity > 0)
			{
				DrawString(birdX, birdY + 0, L"\\\\\\");
				DrawString(birdX, birdY + 1, L"<\\\\\\=Q");
			}
			else
			{
				DrawString(birdX, birdY + 0, L"///");
				DrawString(birdX, birdY + 1, L"<///=Q");
			}

			if (levelPosition > sectionWidth)
			{
				levelPosition -= sectionWidth;
				sections.pop_front();
				int i = rand() % (ScreenHeight() - 20);
				if (i <= 10) i = 0;
				sections.push_back(i);
			}

			int section = 0;
			for (auto s : sections)
			{
				if (s != 0)
				{
					Fill(section * sectionWidth + 10 - levelPosition, ScreenHeight() - s, section * sectionWidth + 15 - levelPosition, ScreenHeight(), PIXEL_SOLID, FG_GREEN);
					Fill(section * sectionWidth + 10 - levelPosition, 0, section * sectionWidth + 15 - levelPosition, ScreenHeight() - s - 15, PIXEL_SOLID, FG_GREEN);
				}

				section++;
			}

			hasCollided = birdY < 2 || birdY > ScreenHeight() - 2 						  
						  || m_bufScreen[(int)(birdY + 0) * ScreenWidth() + birdX + 6].Char.UnicodeChar != L' '
				          || m_bufScreen[(int)(birdY + 1) * ScreenWidth() + birdX + 6].Char.UnicodeChar != L' ';

		}

		
		return true;
	}
};

int main()
{	
	FlappyBird bird;
	bird.ConstructConsole(80, 48, 16, 16);
	bird.Start();
	return 0;
}