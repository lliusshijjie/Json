#include <stdexcept>
#include <limits>
#include "parser.h"

using namespace shijie::json;

Parser::Parser() : m_str(""),m_idx(0)
{}

void Parser::load(const string &str) {
	m_str = str;
	m_idx = 0;
}

void Parser::skip_white_space()
{
	while (m_idx < m_str.size() && (m_str[m_idx] == ' ' || m_str[m_idx] == '\n' || m_str[m_idx] == '\t' || m_str[m_idx] == '\r')) 
	{
		m_idx++;
	}
}
char Parser::get_next_token()
{
	skip_white_space();
	if (m_idx >= m_str.size()) throw std::logic_error("unexpected end of input");
	return m_str[m_idx++];
}

Json Parser::parse()
{
	char ch = get_next_token();
	switch (ch) 
	{
		case 'n':
			m_idx--;
			return parse_null();
		case 't':
		case 'f':
			m_idx--;
			return parse_bool();
		case '+': case '-': case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
			m_idx--;
			return parse_number();
		case '"': return Json(parse_string());
		case '[': return parse_array();
		case '{': return parse_object();
		default:
			break;
	}
	throw std::logic_error("unexpected char");
}

Json Parser::parse_null()
{
	if (m_idx + 3 >= m_str.size() || m_str.compare(m_idx, 4, "null") != 0) {
		throw std::logic_error("parse null error");
	}
	m_idx += 4;
	return Json();
}

Json Parser::parse_bool()
{
	if (m_idx + 3 < m_str.size() && m_str.compare(m_idx, 4, "true") == 0) {
		m_idx += 4;
		return Json(true);
	}
	else if (m_idx + 4 < m_str.size() && m_str.compare(m_idx, 5, "false") == 0) {
		m_idx += 5;
		return Json(false);
	}
	else throw std::logic_error("parse bool error");
}

Json Parser::parse_number()
{
	int pos = m_idx;
	// Handle optional sign
	if (m_idx < m_str.size() && (m_str[m_idx] == '-' || m_str[m_idx] == '+')) {
		m_idx++;
	}

	if (m_idx >= m_str.size() || m_str[m_idx] < '0' || m_str[m_idx] > '9') {
		throw std::logic_error("parse number error: invalid digit after sign");
	}

	// Parse integer part
	while (m_idx < m_str.size() && m_str[m_idx] >= '0' && m_str[m_idx] <= '9') m_idx++;

	bool is_double = false;
	// Parse fractional part
	if (m_idx < m_str.size() && m_str[m_idx] == '.') {
		is_double = true;
		m_idx++;
		if (m_idx >= m_str.size() || m_str[m_idx] < '0' || m_str[m_idx] > '9') {
			throw std::logic_error("parse number error: invalid digit after decimal point");
		}
		while (m_idx < m_str.size() && m_str[m_idx] >= '0' && m_str[m_idx] <= '9') m_idx++;
	}

	// Parse exponent part
	if (m_idx < m_str.size() && (m_str[m_idx] == 'e' || m_str[m_idx] == 'E')) {
		is_double = true;
		m_idx++;
		// Handle optional sign for exponent
		if (m_idx < m_str.size() && (m_str[m_idx] == '-' || m_str[m_idx] == '+')) {
			m_idx++;
		}
		if (m_idx >= m_str.size() || m_str[m_idx] < '0' || m_str[m_idx] > '9') {
			throw std::logic_error("parse number error: invalid digit in exponent");
		}
		while (m_idx < m_str.size() && m_str[m_idx] >= '0' && m_str[m_idx] <= '9') m_idx++;
	}

	string num_str = m_str.substr(pos, m_idx - pos);

	if (is_double) {
		// Use std::stod for double conversion, which handles scientific notation
		try {
			return Json(std::stod(num_str));
		} catch (const std::out_of_range& oor) {
			throw std::logic_error("parse number error: double out of range");
		} catch (const std::invalid_argument& ia) {
			throw std::logic_error("parse number error: invalid double argument");
		}
	} else {
		// Use std::stoll for long long conversion
		try {
			long long val = std::stoll(num_str);
			return Json(val);
		} catch (const std::out_of_range& oor) {
			throw std::logic_error("parse number error: long long out of range");
		} catch (const std::invalid_argument& ia) {
			throw std::logic_error("parse number error: invalid long long argument");
		}
	}
}

string Parser::parse_string()
{
	string out;
	while(true)
	{
		if (m_idx >= m_str.size()) throw std::logic_error("unexpected end in string");
		char ch = m_str[m_idx++];
        if (ch == '"') break;
        if (ch == '\\') {
			if (m_idx >= m_str.size()) throw std::logic_error("unexpected end in escape");
            ch = m_str[m_idx++];
            switch(ch) {
                case '"': out += '"'; break;
                case '\\': out += '\\'; break;
                case '/': out += '/'; break;
                case 'b': out += '\b'; break;
                case 'f': out += '\f'; break;
                case 'n': out += '\n'; break;
                case 'r': out += '\r'; break;
                case 't': out += '\t'; break;
				case 'u': {
					if (m_idx + 4 > m_str.size()) {
						throw std::logic_error("unicode escape error");
					}
					string hex = m_str.substr(m_idx, 4);
					m_idx += 4;
					int u = 0;
					for (char c : hex) {
						u *= 16;
						if (c >= '0' && c <= '9') {
							u += c - '0';
						} else if (c >= 'a' && c <= 'f') {
							u += c - 'a' + 10;
						} else if (c >= 'A' && c <= 'F') {
							u += c - 'A' + 10;
						} else {
							throw std::logic_error("unicode escape error");
						}
					}

					if (u <= 0x7F) {
						out += static_cast<char>(u);
					} else if (u <= 0x7FF) {
						out += static_cast<char>(0xC0 | (u >> 6));
						out += static_cast<char>(0x80 | (u & 0x3F));
					} else if (u <= 0xFFFF) {
						out += static_cast<char>(0xE0 | (u >> 12));
						out += static_cast<char>(0x80 | ((u >> 6) & 0x3F));
						out += static_cast<char>(0x80 | (u & 0x3F));
					}
					break;
				}
                default:
					out += ch;
					break;
            }
        } else {
            out += ch;
        }
	}
	return out;
}
Json Parser::parse_array()
{
	Json arr(Json::json_array);
	char ch = get_next_token();
	if (ch == ']') return arr;
	m_idx--;
	while(1) {
		arr.append(parse());
		ch = get_next_token();
		if (ch == ']') break;
		if (ch != ',') 
		{
			throw std::logic_error("parse array error");
		}
		// m_idx++;  // 移除此行
	}
	return arr;
}

Json Parser::parse_object()
{
	Json obj(Json::json_object);
	char ch = get_next_token();
	if (ch == '}') return obj;
	m_idx--;
	while (1) {
		ch = get_next_token();
		if (ch != '"') throw std::logic_error("parse object error");
		string key = parse_string();
		ch = get_next_token();
		if (ch != ':') throw std::logic_error("parse object error");
		obj[key] = parse();
		ch = get_next_token();
		if (ch == '}') break;
		if (ch != ',') throw std::logic_error("parse object error");
		// m_idx++;  // 移除此行
	}
	return obj;
}