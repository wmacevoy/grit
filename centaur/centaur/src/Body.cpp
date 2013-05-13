#include "Body.h"

Body::Body()
:	started(false)
{}

Body& Body::get(){
	static Body body;
	return body;
}

bool Body::start(){
	return true;
}

bool Body::loop(){
	return true;
}

void Body::stop(){

}



