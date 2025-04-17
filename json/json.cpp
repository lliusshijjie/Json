#include <stdexcept>
#include <sstream>
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

Json::Json(int value) : m_type(json_int)
{
	m_value.m_int = value;
}

Json::Json(double value) : m_type(json_double)
{
	m_value.m_double = value;
}

Json::Json(const char *value) : m_type(json_string)
{
	m_value.m_string = new string(value);
}

Json::Json(const string &value) : m_type(json_string)
{
	m_value.m_string = new string(value);
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
		m_value.m_string = new string("");
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
	copy(other);
}

Json::operator bool()
{
	if (m_type != json_bool)
	{
		throw new logic_error("type error,not bool value");
	}
	return m_value.m_bool;
}

Json::operator int()
{
	if (m_type != json_int)
	{
		throw new logic_error("type error,not int value");
	}
	return m_value.m_int;
}
Json::operator double()
{
	if (m_type != json_double)
	{
		throw new logic_error("type error,not double value");
	}
	return m_value.m_double;
}
Json::operator string()
{
	if (m_type != json_string)
	{
		throw new logic_error("type error,not string value");
	}
	return *m_value.m_string;
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
		throw new logic_error("array[] index < 0");

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

void Json::operator=(const Json &other)
{
	clear();
	copy(other);
}

bool Json::operator==(const Json &other)
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
		return *(m_value.m_string) == *(other.m_value.m_string);
	case json_array:
		return m_value.m_array == other.m_value.m_array;
	case json_object:
		return m_value.m_object = other.m_value.m_object;
	default:
		break;
	}
	return false;
}
bool Json::operator!=(const Json &other)
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
		m_value.m_string = other.m_value.m_string;
		break;
	case json_array:
		m_value.m_array = other.m_value.m_array;
		break;
	case json_object:
		m_value.m_object = other.m_value.m_object;
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
		delete m_value.m_string;
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
		ss << m_value.m_double;
		break;
	case json_string:
		ss << '\"' << *(m_value.m_string) << '\"';
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
			ss << '\"' << it->first << '\"' << ':' << it->second.str();
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
		throw new logic_error("type error,not bool value");
	}
	return m_value.m_bool;
}
int Json::asInt() const
{
	if (m_type != json_int)
	{
		throw new logic_error("type error,not int value");
	}
	return m_value.m_int;
}
double Json::asDouble() const
{
	if (m_type != json_double)
	{
		throw new logic_error("type error,not double value");
	}
	return m_value.m_double;
}
string Json::asString() const
{
	if (m_type != json_string)
	{
		throw new logic_error("type error,not string value");
	}
	return *(m_value.m_string);
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