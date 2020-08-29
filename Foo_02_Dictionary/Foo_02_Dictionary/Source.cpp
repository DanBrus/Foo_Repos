#include <string>
#include <iostream>
#include "TestDict.h"

void main(int) {
	TestDict<std::string, Dictionary<std::string, float>> test;

	Dictionary<std::string, TestDict<std::string, float>> *dict = &test;


	/*
	dict->Set(std::string("First"), 1);
	dict->Set(std::string("Second"), 2);
	dict->Set(std::string("Thrid"), 3);
	dict->Set(std::string("Fourts"), 4);
	dict->Set(std::string("Fivets"), 5);
	dict->Set(std::string("Sixth"), 6);
	dict->Set(std::string("Seventh"), 4);
	dict->Set(std::string("Eigth"), 5);
	dict->Set(std::string("Nineth"), 6);

	dict->IsSet(std::string("Sixth"));
	dict->IsSet(std::string("2"));

	try {
		dict->Get(std::string("First"));
		dict->Get(std::string("Second"));
		dict->Get(std::string("Thrid"));
		dict->Get(std::string("Fourts"));
		dict->Get(std::string("Fivets"));
		dict->Get(std::string("Sixth"));
		dict->Get(std::string("Seventh"));
		dict->Get(std::string("Eigth"));
		dict->Get(std::string("Nineth"));
		dict->Get(std::string("g"));
	}
	catch (NotFoundException<std::string> &except) {
		std::cout << except.GetKey() << std::endl;
	}
	*/
	while (1) {};
	return;
}