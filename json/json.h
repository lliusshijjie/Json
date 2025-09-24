#pragma once 
#include <string>
#include <vector>
#include <map>

using namespace std;

namespace shijie{
    namespace json{
		class Json 
		{
			public:
				enum Type
				{
					json_null = 0,
					json_bool,
					json_int,
					json_double,
					json_string,
					json_array,
					json_object
				};

				Json();
				Json(bool value);
				Json(long long value);
				Json(double value);
				Json(const char * value);
				Json(const string &value);
				Json(Type type);
				Json(const Json &other);
				Json(Json&& other);
				~Json();

				explicit operator bool() const;
                explicit operator int() const;
                explicit operator double() const;
                explicit operator string() const;
				explicit operator long long() const;

				Json &operator[](int index);
				void append(const Json &other);
				
				Json &operator[](const char *key);
				Json &operator[](const string &key);

				Json& operator=(const Json &other);
				Json& operator=(Json&& other);
				bool operator == (const Json &other) const;
				bool operator != (const Json &other) const;
				void copy(const Json &other);
				void clear();

				typedef std::vector<Json>::iterator iterator;
				typedef std::vector<Json>::const_iterator const_iterator;

				iterator begin();
				iterator end();

				const_iterator begin() const;
				const_iterator end() const;

				bool isNull() const { return m_type == json_null;}
				bool isBool() const { return m_type == json_bool;}
				bool isInt() const { return m_type == json_int;}
				bool isDouble() const { return m_type == json_double;}
				bool isString() const { return m_type == json_string;}
				bool isArray() const { return m_type == json_array;}
				bool isObject() const { return m_type == json_object;}

				bool asBool() const ;
				int asInt() const;
				long long asInt64() const;
				double asDouble() const;
				string asString() const;

				bool has(int index);
				bool has(const char *key);
				bool has(const string &key);

				void remove(int index);
				void remove(const char *key);
				void remove(const string &key);

				void parse(const string &str);

				string str() const;
				
			private:
			    union Value
				{
					bool m_bool;
					long long m_int;
					double m_double;
                    std::aligned_storage<sizeof(std::string)>::type m_string;
					vector<Json> *m_array;
					map<string, Json> *m_object;
				};

				Type m_type;
				Value m_value;
		};
	}
}
