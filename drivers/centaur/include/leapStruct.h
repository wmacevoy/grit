struct leapData{
	float lx;
	float ly;
	float lz;
	float lroll;

	float rx;
	float ry;
	float rz;
	float rroll;

	leapData() {clear();}
	void clear()
	{
		lx = 0;
		ly = 0;
		lz = 0;
		lroll = 0;

		rx = 0;
		ry = 0;
		rz = 0;
		rroll = 0;
	}
};
