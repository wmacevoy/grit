struct leapData{
	float lx;
	float ly;
	float lz;
	float lroll;
	float lpitch;

	float rx;
	float ry;
	float rz;
	float rroll;
	float rpitch;

	leapData() {clear();}
	void clear()
	{
		lx = 0;
		ly = 0;
		lz = 0;
		lroll = 0;
		lpitch = 0;

		rx = 0;
		ry = 0;
		rz = 0;
		rroll = 0;
		rpitch = 0;
	}
};
