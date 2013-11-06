class joystick
{
public:
	int32_t x1, y1, x2, y2;
	bool button1, button2;
	joystick()
	{
		x1 = 0;
		y1 = 0;
		x2 = 0;
		y2 = 0;
		button1 = false;
		button2 = false;
	}
	void setX1(int32_t newX)
	{
		//MAP to max and min neck
		x1 = newX;
	}
	void setY1(int32_t newY)
	{
		//MAP to max and min neck
		y1 = newY;
	}
	void setX2(int32_t newX)
	{
		//MAP to max and min neck
		x2 = newX;
	}
	void setY2(int32_t newY)
	{
		//MAP to max and min neck
		y2 = newY;
	}
	void setButtonDown(int button)
	{
		switch(button)
		{
		case 1:
			button1 = true;
			break;
		case 2:
			button2 = true;
			break;
		}
	}
	void setButtonUp(int button)
	{
		switch(button)
		{
		case 1:
			button1 = false;
			break;
		case 2:
			button2 = false;
			break;
		}
	}
	void clear()
	{
		x1 = 0;
		y1 = 0;
		x2 = 0;
		y2 = 0;
		button1 = false;
		button2 = false;
	}
};
