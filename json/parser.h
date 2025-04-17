#pragma once
#include <string>
#include "json.h"

using namespace std;

namespace shijie{
namespace json{
	class Parser {
		public:
			Parser();
			void load(const string &str);
			Json parse();

		private:
			void skip_white_space();
			char get_next_token();
			Json parse_null();
			Json parse_bool();
			Json parse_number();
			string parse_string();
			Json parse_array();
			Json parse_object();
		private:
		    string m_str;
			int m_idx;
	};
}
}