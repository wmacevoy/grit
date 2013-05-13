#pragma once

class Body {

protected:

	Body();
	bool started;

public:

	static Body& get();
	bool start();
	bool loop();
	void stop();

};

