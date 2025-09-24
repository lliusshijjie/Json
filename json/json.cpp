#include <stdexcept>
#include <sstream>
#include <iomanip>
#include "json.h"
#include "parser.h"

using namespace std;
using namespace shijie::json;

Json::Json() : m_type(json_null)
{
}

Json::Json(bool value) : m_type(json_bool)
{
	m_value.m_bool = value;
}

Json::Json(long long value) : m_type(json_int)
{
	m_value.m_int = value;
}

Json::Json(double value) : m_type(json_double)
{
	m_value.m_double = value;
}

Json::Json(const char *value) : m_type(json_string)
{
	new (&m_value.m_string) string(value);
}

Json::Json(const string &value) : m_type(json_string)
{
	new (&m_value.m_string) string(value);
}

Json::Json(Type type) : m_type(type)
{
	switch (m_type)
	{
	case json_null:
		break;
	case json_bool:
		m_value.m_bool = false;
		break;
	case json_int:
		m_value.m_int = 0;
		break;
	case json_double:
		m_value.m_double = 0.0;
		break;
	case json_string:
		new (&m_value.m_string) string("");
		break;
	case json_array:
		m_value.m_array = new vector<Json>();
		break;
	case json_object:
		m_value.m_object = new map<string, Json>();
		break;
	default:
		break;
	}
}

Json::Json(const Json &other)
{
	m_type = json_null;
	copy(other);
}

Json::Json(Json&& other) : m_type(other.m_type)
{
	if (m_type == json_string) {
		new (&m_value.m_string) string(std::move(*reinterpret_cast<string*>(&other.m_value.m_string)));
		reinterpret_cast<string*>(&other.m_value.m_string)->~string();
	} else {
		m_value = other.m_value;
	}
	other.m_type = json_null;
}

Json::~Json()
{
	clear();
}
Json::operator bool() const
{
	if (m_type != json_bool)
	{
		throw std::logic_error("type error,not bool value");
	}
	return m_value.m_bool;
}

Json::operator int() const
{
	if (m_type != json_int)
	{
		throw std::logic_error("type error,not int value");
	}
	return m_value.m_int;
}
Json::operator double() const
{
	if (m_type != json_double)
	{
		throw std::logic_error("type error,not double value");
	}
	return m_value.m_double;
}

Json::operator long long() const
{
	if (m_type != json_int)
	{
		throw std::logic_error("type error,not long long value");
	}
	return m_value.m_int;
}
Json::operator string() const
{
	if (m_type != json_string)
	{
		throw std::logic_error("type error,not string value");
	}
	return *reinterpret_cast<const string*>(&m_value.m_string);
}

Json &Json::operator[](int index)
{
	if (m_type != json_array)
	{
		clear();
		m_type = json_array;
		m_value.m_array = new vector<Json>();
	}
	if (index < 0)
		throw std::logic_error("array[] index < 0");

	int size = (m_value.m_array)->size();
	if (index >= size)
	{
		for (int i = size; i <= index; i++)
		{
			(m_value.m_array)->push_back(Json());
		}
	}

	return (m_value.m_array)->at(index);
}
void Json::append(const Json &other)
{
	if (m_type != json_array)
	{
		clear();
		m_type = json_array;
		m_value.m_array = new vector<Json>();
	}

	(m_value.m_array)->push_back(other);
}

Json &Json::operator[](const char *key)
{
	string name(key);
	return (*(this))[name];
}
Json &Json::operator[](const string &key)
{
	if (m_type != json_object)
	{
		clear();
		m_type = json_object;
		m_value.m_object = new map<string, Json>();
	}
	return (*(m_value.m_object))[key];
}

Json& Json::operator=(const Json &other)
{
	if (this != &other) {
		clear();
		copy(other);
	}
	return *this;
}

Json& Json::operator=(Json&& other)
{
	if (this != &other) {
		clear();
		m_type = other.m_type;
		if (m_type == json_string) {
			new (&m_value.m_string) string(std::move(*reinterpret_cast<string*>(&other.m_value.m_string)));
			reinterpret_cast<string*>(&other.m_value.m_string)->~string();
		} else {
			m_value = other.m_value;
		}
		other.m_type = json_null;
	}
	return *this;
}
bool Json::operator==(const Json &other) const
{
	if (m_type != other.m_type)
	{
		return false;
	}
	switch (m_type)
	{
	case json_null:
		return true;
	case json_bool:
		return m_value.m_bool == other.m_value.m_bool;
	case json_int:
		return m_value.m_int == other.m_value.m_int;
	case json_double:
		return m_value.m_double == other.m_value.m_double;
	case json_string:
		return *reinterpret_cast<const string*>(&m_value.m_string) == *reinterpret_cast<const string*>(&other.m_value.m_string);
	case json_array:
		if (m_value.m_array->size() != other.m_value.m_array->size()) return false;
		for (size_t i = 0; i < m_value.m_array->size(); ++i) {
			if (! ((*m_value.m_array)[i] == (*other.m_value.m_array)[i])) return false;
		}
		return true;
	case json_object:
		if (m_value.m_object->size() != other.m_value.m_object->size()) return false;
		for (const auto& pair : *m_value.m_object) {
			auto it = other.m_value.m_object->find(pair.first);
			if (it == other.m_value.m_object->end() || ! (pair.second == it->second)) return false;
		}
		return true;
	default:
		break;
	}
	return false;
}
bool Json::operator!=(const Json &other) const
{
	return !((*this) == other);
}

void Json::copy(const Json &other)
{
	m_type = other.m_type;
	switch (m_type)
	{
	case json_null:
		break;
	case json_bool:
		m_value.m_bool = other.m_value.m_bool;
		break;
	case json_int:
		m_value.m_int = other.m_value.m_int;
		break;
	case json_double:
		m_value.m_double = other.m_value.m_double;
		break;
	case json_string:
		new (&m_value.m_string) string(*reinterpret_cast<const string*>(&other.m_value.m_string));
		break;
	case json_array:
		m_value.m_array = new vector<Json>();
		m_value.m_array->reserve(other.m_value.m_array->size());
		for (const auto& elem : *other.m_value.m_array) {
			m_value.m_array->push_back(Json(elem));
		}
		break;
	case json_object:
		m_value.m_object = new map<string, Json>();
		for (const auto& pair : *other.m_value.m_object) {
			(*m_value.m_object)[pair.first] = Json(pair.second);
		}
		break;
	default:
		break;
	}
}

void Json::clear()
{
	switch (m_type)
	{
	case json_null:
		break;
	case json_bool:
		m_value.m_bool = false;
		break;
	case json_int:
		m_value.m_int = 0;
		break;
	case json_double:
		m_value.m_double = 0.0;
		break;
	case json_string:
		reinterpret_cast<string*>(&m_value.m_string)->~string();
		break;
	case json_array:
	{
		for (auto it = (m_value.m_array)->begin(); it != (m_value.m_array)->end(); it++)
		{
			it->clear();
		}
		delete m_value.m_array;
		break;
	}
	case json_object:
	{
		for (auto it = (m_value.m_object)->begin(); it != (m_value.m_object)->end(); it++)
		{
			it->second.clear();
		}
		delete m_value.m_object;
		break;
	}
	default:
		break;
	}
	m_type = json_null;
}

static string escape_string(const string& s) {
    stringstream ss;
    for (char c : s) {
        switch (c) {
            case '"': ss << "\\\""; break;
            case '\\': ss << "\\\\"; break;
            case '\b': ss << "\\b"; break;
            case '\f': ss << "\\f"; break;
            case '\n': ss << "\\n"; break;
            case '\r': ss << "\\r"; break;
            case '\t': ss << "\\t"; break;
            default:    
                if (iscntrl(static_cast<unsigned char>(c))) {
                    ss << "\\u" << hex << setw(4) << setfill('0') << static_cast<int>(c);
                } else {
                    ss << c;
                }
                break;
        }
    }
    return ss.str();
}

string Json::str() const
{
	stringstream ss;
	switch (m_type)
	{
	case json_null:
		ss << "null";
		break;
	case json_bool:
		ss << (m_value.m_bool ? "true" : "false");
		break;
	case json_int:
		ss << m_value.m_int;
		break;
	case json_double:
		ss << std::fixed << std::setprecision(15) << m_value.m_double;
		break;
	case json_string:
		ss << '\"' << escape_string(*reinterpret_cast<const string*>(&m_value.m_string)) << '\"';
		break;
	case json_array:
	{
		ss << '[';
		for (auto it = (m_value.m_array)->begin(); it != (m_value.m_array)->end(); it++)
		{
			if (it != (m_value.m_array)->begin())
			{
				ss << ',';
			}
			ss << it->str();
		}
		ss << ']';
	}
	break;
	case json_object:
	{
		ss << '{';
		for (auto it = (m_value.m_object)->begin(); it != (m_value.m_object)->end(); it++)
		{
			if (it != (m_value.m_object)->begin())
			{
				ss << ',';
			}
			ss << '\"' << escape_string(it->first) << '\"' << ':' << it->second.str();
		}
		ss << '}';
	}
	default:
		break;
	}
	return ss.str();
}

bool Json::asBool() const
{
	if (m_type != json_bool)
	{
		throw std::logic_error("type error,not bool value");
	}
	return m_value.m_bool;
}
int Json::asInt() const
{
	if (m_type != json_int)
	{
		throw std::logic_error("type error,not int value");
	}
	return m_value.m_int;
}

long long Json::asInt64() const
{
	if (m_type != json_int)
	{
		throw std::logic_error("type error,not int value");
	}
	return m_value.m_int;
}
double Json::asDouble() const
{
	if (m_type != json_double)
	{
		throw std::logic_error("type error,not double value");
	}
	return m_value.m_double;
}
string Json::asString() const
{
	if (m_type != json_string)
	{
		throw std::logic_error("type error,not string value");
	}
	return *reinterpret_cast<const string*>(&m_value.m_string);
}

bool Json::has(int index)
{
	if (m_type != json_array)
	{
		return false;
	}

	int size = (m_value.m_array)->size();
	return (index >= 0 && index < size);
}
bool Json::has(const char *key)
{
	string name(key);
	return has(name);
}
bool Json::has(const string &key)
{
	if (m_type != json_object)
		return false;
	return (m_value.m_object)->find(key) != m_value.m_object->end();
}

void Json::remove(int index)
{
	if (m_type != json_array)
		return;
	int size = (m_value.m_array)->size();
	if (index < 0 || index >= size)
		return;
	(m_value.m_array)->at(index).clear();
	(m_value.m_array)->erase((m_value.m_array)->begin() + index);
}
void Json::remove(const char *key)
{
	string name(key);
	remove(name);
}
void Json::remove(const string &key)
{
	auto it = (m_value.m_object)->find(key);
	if (it == (m_value.m_object)->end())
		return;
	(*(m_value.m_object))[key].clear();
	(m_value.m_object)->erase(key);
}

void Json::parse(const string &str)
{
	Parser p;
	p.load(str);
	*this = p.parse();
}
Json::iterator Json::begin() {
	if (m_type != json_array) throw std::logic_error("not an array");
	return m_value.m_array->begin();
}
Json::iterator Json::end() {
	if (m_type != json_array) throw std::logic_error("not an array");
	return m_value.m_array->end();
}

Json::const_iterator Json::begin() const {
	if (m_type != json_array) throw std::logic_error("not an array");
	return m_value.m_array->begin();
}
Json::const_iterator Json::end() const {
	if (m_type != json_array) throw std::logic_error("not an array");
	return m_value.m_array->end();
}