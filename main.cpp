#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

#include "json/json.h"
using namespace shijie::json;

int main() {
	ifstream fin("./test.json");
	stringstream ss;
	ss << fin.rdbuf();
	const string &str = ss.str();

	Json v;
	v.parse(str);

	// P0 验证
	// int num = v["code"];
	// string s = v["data"]["total"];
	// cout << num << endl;
	// cout << s << endl;

	// P0 验证 (修复后)
	int num = static_cast<int>(v["code"]);
	string s = static_cast<string>(v["data"]["total"]);
	cout << num << endl;
	cout << s << endl;

	// P2 验证：字符串转义
	Json escaped_json;
	escaped_json.parse("{\"key\": \"Hello\\nWorld!\\t\\u0040\\u0022\\\\\\/\\b\\f\\r\"}");
	cout << "Escaped String Test: " << static_cast<string>(escaped_json["key"]) << endl;

	// P2 验证：数字解析
	Json number_json;
	number_json.parse("{\"sci\": 1.23e-4, \"plus\": +5, \"big_int\": 9223372036854775807, \"big_neg_int\": -9223372036854775808}");
	cout << "Scientific Notation Test: " << static_cast<double>(number_json["sci"]) << endl;
	cout << "Optional Plus Test: " << static_cast<int>(number_json["plus"]) << endl;
	cout << "Big Integer Test: " << static_cast<long long>(number_json["big_int"]) << endl;
	cout << "Big Negative Integer Test: " << static_cast<long long>(number_json["big_neg_int"]) << endl;

	// P2 验证：字符串序列化和浮点数格式化
	Json serialization_json;
	serialization_json["my_string"] = "\"Hello\\\nWorld\"\t";
	serialization_json["my_double"] = 3.141592653589793;
	cout << "Serialization Test: " << serialization_json.str() << endl;

	cout << v.str() << "\n";
	cout << v.str() << "\n";

	return 0;
}