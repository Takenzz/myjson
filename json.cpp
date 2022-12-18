#include "json.h"
#include <iostream>
#include <cassert>

namespace my_Json {
    using std::make_shared;
    using std::cout;

    JsonValue::JsonValue() : value(nullptr) {
        this->type = Jsontype::NUL;
    }

    JsonValue::JsonValue(std::nullptr_t v) : value(nullptr) {
        this->type = Jsontype::NUL;
    }

    JsonValue::JsonValue(bool v) : value(v) {
        this->type = Jsontype::BOOL;
    }

    JsonValue::JsonValue(double v) : value(v) {
        this->type = Jsontype::NUMBER;
    }

    JsonValue::JsonValue(int v) {
        this->type = Jsontype::NUMBER;
    }

    JsonValue::JsonValue(std::string &v) : value(v) {
        this->type = Jsontype::STRING;
    }

    JsonValue::JsonValue(std::string &&v) : value(std::forward<string>(v)) {
        this->type = Jsontype::STRING;
    }

    JsonValue::JsonValue(const char *v) :value(v) {
        this->type = Jsontype::STRING;
    }

    JsonValue::JsonValue(vector<Json> &v) : value(v) {
        this->type = Jsontype::ARRAY ;
    }

    JsonValue::JsonValue(vector<Json> &&v) : value(std::forward<vector<Json>>(v)) {
        this->type = Jsontype::ARRAY ;
    }

    JsonValue::JsonValue(initializer_list<my_Json::Json> &v) :value(v) {
        this->type = Jsontype::ARRAY ;
    }

    JsonValue::JsonValue(initializer_list<my_Json::Json> &&v) :value(std::forward<initializer_list<my_Json::Json>>(v)) {
        this->type = Jsontype::ARRAY;
    }

    JsonValue::JsonValue(unordered_map<std::string, my_Json::Json> &v) : value(v) {
        this->type = Jsontype::OBJECT;
    }

    JsonValue::JsonValue(unordered_map<std::string, my_Json::Json> &&v) : value(std::forward<unordered_map<std::string, my_Json::Json>>(v)){
        this->type = Jsontype::OBJECT;
    }

    const Json & JsonValue::operator[](size_t i) {
        return std::get<vector<Json>>(value)[i];
    }

    const Json &JsonValue::operator[](std::string s) {
        return std::get<unordered_map<string, Json>>(value)[s];
    }

    Jsontype JsonValue::Json_type() {
        return this->type;
    }

    Json::Json() : v_ptr(make_shared<JsonValue>()) {}

    Json::Json(bool v) : v_ptr(make_shared<JsonValue>(v)) {}

    Json::Json(double v) : v_ptr(make_shared<JsonValue>(v)) {}

    Json::Json(int v) : v_ptr(make_shared<JsonValue>(v)) {}

    Json::Json(std::string &v) : v_ptr(make_shared<JsonValue>(v)) {}

    Json::Json(std::string &&v) : v_ptr(make_shared<JsonValue>(std::forward<string>(v))) {}

    Json::Json(const char *v) : v_ptr(make_shared<JsonValue>(v)) {}

    Json::Json(vector<Json> &v) : v_ptr(make_shared<JsonValue>(v)){}

    Json::Json(vector<Json> &&v) : v_ptr(make_shared<JsonValue>(std::forward<vector<Json>>(v))) {}

    Json::Json(initializer_list<my_Json::Json> &v) : v_ptr(make_shared<JsonValue>(v)) {}

    Json::Json(initializer_list<my_Json::Json> &&v) : v_ptr(make_shared<JsonValue>(std::forward<initializer_list<my_Json::Json>>(v))) {}

    Json::Json(unordered_map<std::string, my_Json::Json> &v) :v_ptr(make_shared<JsonValue>(v)) {}

    Json::Json(unordered_map<std::string, my_Json::Json> &&v) :v_ptr(make_shared<JsonValue>(std::forward<unordered_map<std::string, my_Json::Json>>(v))) {}

    const Json &Json::operator[](size_t i) {
        return (*v_ptr)[i];
    }

    const Json &Json::operator[](std::string s) {
        return (*v_ptr)[s];
    }

    Jsontype Json::type() const {
        return v_ptr->Json_type();
    }

    double Json::number_value() const{
        return std::get<double>(v_ptr->value);
    }

    bool Json::bool_value() const{
        return std::get<bool>(v_ptr->value);
    }

    string Json::string_value() const{
        return std::get<string>(v_ptr->value);
    }

    vector<Json> Json::array_value() const {
        return std::get<vector<Json>>(v_ptr->value);
    }

    unordered_map<string, Json> Json::object_value() const {
        return std::get<unordered_map<string, Json>>(v_ptr->value);
    }

    class JsonParse {
    private:
        string &src;
        JsonError &err;
        int i = 0;
        int size ;

        char parse_whitespace(string &s) {
            while (s[i] == ' ' || s[i] == '\t' || s[i] == '\n' || s[i] == '\r') i++;
            return s[i];
        }

        bool Json_over_value(string &s) {
            char ch = parse_whitespace(s);
            if(ch != '\0') {
                err = JsonError::PARSE_OVER_ONE_VALUE;
                return true;
            }
            return false;
        }

        bool in_range(char target,char begin, char end) {
            if(target >= begin && target <= end) return true;
            else return false;
        }

        bool parse_unicode(string &s, unsigned &u) {
            u = 0;
            for(int j = 0; j < 4; j++) {
                char ch = s[i++];
                u <<= 4;
                if (ch >='0' && ch <='9') u |= ch  - '0';
                else if(ch >= 'A' && ch <= 'F')  u |= ch - 'A' + 10;
                else if(ch >= 'a' && ch <= 'f')  u |= ch - 'a' + 10;
                else return false;
            }
            return true;
        }

        void add_unicode(string &s,unsigned u) {
            if(u <= 0x7f) {
                s += u;
            }
            else if(u <= 0x7ff) {
                s += 0xc0 | ((u >> 6) & 0xff);
                s += 0x80 | (u        & 0x3f);
            }
            else if(u <= 0xffff) {
                s += 0xe0 | ((u >> 12) & 0xff);
                s += 0x80 | ((u >> 6)  & 0x3f);
                s += 0x80 | (u         & 0x3f);
            } else {
                assert(u <= 0x10fff);
                s += 0xf0 | ((u >> 18) & 0xff);
                s += 0x80 | ((u >> 12) & 0x3f);
                s += 0x80 | ((u >> 6)  & 0x3f);
                s += 0x80 | (u         & 0x3f);
            }


        }

        string to_string(string &s) {
            string res;
            unsigned u;
            unsigned u2;
            i++;
            while(1) {
                if(i == s.size()) {
                    err = JsonError::PARSE_INVALID_VALUE;
                    return res;
                }
                char ch = s[i++];

                if(ch == '\"') {
                    return res;
                }

                if(ch == '\\') {
                    ch = s[i++];
                    switch (ch) {
                        case '\"' : res += '\"'; break;
                        case '\\' : res += '\\'; break;
                        case '/'  : res += '/' ; break;
                        case 'b'  : res += '\b'; break;
                        case 'f'  : res += '\f'; break;
                        case 'n'  : res += '\n'; break;
                        case 'r'  : res += '\r'; break;
                        case 't'  : res += '\t'; break;
                        case 'u'  :
                            if(parse_unicode(s,u)) {
                                if(u >= 0xd800 && u <= 0xdbff) {
                                    if(s[i++] != '\\') {
                                        err = JsonError::PARSE_INVALID_VALUE;
                                        return res;
                                    }
                                    if(s[i++] != 'u') {
                                        err = JsonError::PARSE_INVALID_VALUE;
                                        return res;
                                    }
                                    if(!parse_unicode(s,u2)){
                                        err = JsonError::PARSE_INVALID_VALUE;
                                        return res;
                                    }
                                    if(u2 < 0xdc00 || u2 > 0xdfff) {
                                        err = JsonError::PARSE_INVALID_VALUE;
                                        return res;
                                    }
                                    u = (((u - 0xd800) << 10) | (u2 - 0xdc00)) + 0x1000;
                                }
                                add_unicode(res,u);
                            } else {
                                err = JsonError::PARSE_INVALID_VALUE;
                                return res;
                            }
                            break;
                        default :
                            err = JsonError::PARSE_INVALID_VALUE;
                            return res;
                    }
                    continue;
                }
                res += ch;
            }

        }

        Json parse_null(string &s) {
            if (s[i] != 'n' || s[i + 1] != 'u' || s[i + 2] != 'l' || s[i + 3] != 'l') {
                err = JsonError::PARSE_INVALID_VALUE;
                return Json();
            }
            i += 4;
            return Json();
        }

        Json parse_true(string &s) {
            if (s[i] != 't' || s[i + 1] != 'r' || s[i + 2] != 'u' || s[i + 3] != 'e') {
                err = JsonError::PARSE_INVALID_VALUE;
                return Json();
            }
            i += 4;
            return Json(true);
        }

        Json pares_false(string &s) {
            if (s[i] != 'f' || s[i + 1] != 'a' || s[i + 2] != 'l' || s[i + 3] != 's' || s[i + 4] != 'e') {
                err = JsonError::PARSE_INVALID_VALUE;
                return Json();
            }
            i += 5;
            return Json(false);
        }

        Json parse_number(string &s) {
            size_t start = i;
            if(s[i] == '-') i++;
            if(s[i] == '0') {
                i++;
            }else {
                if(!in_range(s[i], '1', '9')) {
                    err = JsonError::PARSE_INVALID_VALUE;
                    return Json();
                }
                while(in_range(s[i], '0', '9')) i++;
            }
            if(s[i] == '.') {
                i++;
                if(!in_range(s[i],'0','9')) {
                    err = JsonError::PARSE_INVALID_VALUE;
                    return Json();
                }
                while(in_range(s[i], '0', '9')) i++;
            }
            if(s[i] == 'E' || s[i] == 'e') {
                i++;
                if(s[i] == '+' || s[i] == '-') i++;
                if(!in_range(s[i], '0', '9')) {
                    err = JsonError::PARSE_INVALID_VALUE;
                    return Json();
                }
                while(in_range(s[i], '0', '9')) i++;
            }

            return Json(strtod(s.c_str() + start, nullptr));
        }

        Json parse_string(string &s) {
            return Json(to_string(s));
        }

        Json parse_array(string &s) {
            i++;
            vector<Json> res;
            if(s[i] == ']') return res;
            while(1) {
               Json tmp = parse_value();
               res.push_back(tmp);
                char ch = parse_whitespace(s);
                i++;
               if(ch == ',') continue;
               else if(ch == ']') return Json(res);
               else {
                   err = JsonError::PARSE_INVALID_VALUE;
                   return Json();
               }
            }

        }

        Json parse_object(string &s) {
            i++;
            unordered_map<string, Json> res;
            char ch;
            while(1) {
                ch = parse_whitespace(src);
                if(ch == '}') {
                    i++;
                    return Json(res);
                }
                if(ch != '\"') {
                    err = JsonError::PARSE_INVALID_VALUE;
                    return Json();
                }
                string tmp = to_string(src);
                if(err != JsonError::N0_ERROR) {
                    return  Json();
                }
                ch = parse_whitespace(src);
                if(ch != ':')  {
                    err = JsonError::PARSE_INVALID_VALUE;
                    return Json();
                }
                i++;
                Json tmp2 = parse_value();
                if(err != JsonError::N0_ERROR) {
                    return Json();
                }
                res[tmp] = tmp2;
                ch = parse_whitespace(src);
                if(ch == ',') i++;
            }
        }

        Json parse_value() {
            char ch = parse_whitespace(src);
            if (ch == 't') return parse_true(src);
            else if (ch == 'f') return pares_false(src);
            else if (ch == 'n') return parse_null(src);
            else if (in_range(ch, '0', '9') || ch == '-') return parse_number(src);
            else if (ch == '\"') return parse_string(src);
            else if (ch == '[')  return parse_array(src);
            else if (ch == '{')  return parse_object(src);
            else err = JsonError::PARSE_INVALID_VALUE;
            return Json();
        }
    public:
        JsonParse(string &s, JsonError &e) : src(s), err(e) {
            size = src.size();
        }

        Json parse() {
            Json res = parse_value();
            if(err == JsonError::N0_ERROR) {
                if(Json_over_value(src))
                    return Json();
            }
            return res;
        }
    };

    Json Json::parse(string &src) {
        JsonError err = JsonError::N0_ERROR ;
        Json res;
        JsonParse paser(src, err);
        res = paser.parse();
        if(err == JsonError::N0_ERROR)
            return res;
        else {
            printf("Parse failed,err is %d ", err);
            return res;
        }
    }

    string Json::output(std::nullptr_t) const{
        return "NULL";
    }

    string Json::output(bool v) const{
        if(v == true) return "true";
        else return "false";
    }

    string Json::output(double v) const{
        return std::to_string(v);
    }

    string Json::output(std::string v) const{
        string res;
        res += '\"';
        res += v;
        res += '\"';
        return res;
    }

    string Json::output(vector<my_Json::Json> v) const{
        string res;
        res += "[";
        for(auto &s:v) {
            res += '\n';
            Jsontype T = s.type();
            switch (T) {
                case Jsontype::NUL  :   res += output(nullptr);
                case Jsontype::BOOL :   res += output(s.bool_value()); break;
                case Jsontype::NUMBER : res += output(s.number_value());  break;
                case Jsontype::STRING : res += output(s.string_value());  break;
                case Jsontype::ARRAY :  res += output(s.array_value());  break;
                case Jsontype::OBJECT : res += output(s.object_value());  break;
            }
            res += ",";
        }
        if(v.size()) res.erase(res.end() - 1, res.end());
        res += "\n]";
        return  res;
    }

    string Json::output(unordered_map<std::string, my_Json::Json> v) const{
        string res;
        res += "{";
        for(auto it : v) {
            res += '\n';
            res += output(it.first);
            res += ": ";
            Jsontype T = it.second.type();
            switch (T) {
                case Jsontype::NUL  :   res += output(nullptr);
                case Jsontype::BOOL :   res += output(it.second.bool_value()); break;
                case Jsontype::NUMBER : res += output(it.second.number_value());  break;
                case Jsontype::STRING : res += output(it.second.string_value());  break;
                case Jsontype::ARRAY :  res += output(it.second.array_value());  break;
                case Jsontype::OBJECT : res += output(it.second.object_value());  break;
            }
            res += ",";
        }
        if(res.size() >= 3) res.erase(res.end() - 1, res.end());
        res += "\n}";
        return res;
    }

    string Json::outputJson() const{
        string res;
        Jsontype T = v_ptr->type;
        switch (T) {
            case Jsontype::NUL  :
                res = output(nullptr);
            case Jsontype::BOOL :
                res = output(this->bool_value());
                break;
            case Jsontype::NUMBER :
                res = output(this->number_value());
                break;
            case Jsontype::STRING :
                res = output(this->string_value());
                break;
            case Jsontype::ARRAY :
                res = output(this->array_value());
                break;
            case Jsontype::OBJECT :
                res = output(this->object_value());
                break;
        }
        return res;
    }

}