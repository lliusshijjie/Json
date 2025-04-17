#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

#include "json.h"
using namespace shijie::json;

int main() {
	ifstream fin("./test.json");
	stringstream ss;
	ss << fin.rdbuf();
	const string &str = ss.str();

	Json v;
	v.parse(str);

	int num = v["code"];
	string s = v["data"]["total"];

	cout << num << " " << s << "\n";
	cout << v.str() << "\n";
	cout << v.str() << "\n";
	return 0;
}