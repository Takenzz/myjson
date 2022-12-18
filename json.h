#include <string>
#include <memory>
#include <variant>
#include <vector>
#include <unordered_map>

namespace my_Json {
    using std::string;
    using std::move;
    using std::shared_ptr;
    using std::string;
    using std::vector;
    using std::initializer_list;
    using std::unordered_map;
    using std::pair;

    class Json;
    class JsonValue;

    typedef unordered_map<string, Json> object;
    typedef vector<Json>    array;

    struct JsonNull {

    };

    enum Jsontype{
        NUL = 0, NUMBER, BOOL, STRING, ARRAY, OBJECT
    };

    enum JsonError{
        N0_ERROR = 0, PARSE_INVALID_VALUE, PARSE_OVER_ONE_VALUE
    };

    class Json {
    public:
        Json() ;
        Json(std::nullptr_t v) ;
        Json(bool v) ;
        Json(double v) ;
        Json(int v);
        Json(string &v) ;
        Json(string &&v) ;
        Json(const char *v) ;
        Json(vector<Json> &v);
        Json(vector<Json> &&v);
        Json(initializer_list<Json> &v);
        Json(initializer_list<Json> &&v);
        Json(unordered_map<string,Json> &v);
        Json(unordered_map<string,Json> &&v) ;


        const Json  &operator[](size_t i);
        const Json  &operator[](string s);
        static Json parse(string &src);
        double number_value() const;
        bool   bool_value() const;
        string string_value() const;
        vector<Json> array_value() const;
        unordered_map<string, Json> object_value() const;
        string outputJson() const;

        Jsontype type() const;
    private:
        shared_ptr<JsonValue>  v_ptr;

        string output(std::nullptr_t)const;
        string output(bool v) const;
        string output(double v) const;
        string output(string v) const;
        string output(vector<Json> v) const;
        string output(unordered_map<string, Json> v) const;
    };

    class JsonValue{
    public:
        JsonValue() ;
        JsonValue(double v) ;
        JsonValue(int v) ;
        JsonValue(bool v) ;
        JsonValue(std::nullptr_t) ;
        JsonValue(string &v) ;
        JsonValue(string &&v) ;
        JsonValue(const char *v);
        JsonValue(vector<Json> &v);
        JsonValue(vector<Json> &&v);
        JsonValue(initializer_list<Json> &v);
        JsonValue(initializer_list<Json> &&v);
        JsonValue(unordered_map<string,Json> &v);
        JsonValue(unordered_map<string,Json> &&v);

        const Json & operator[](size_t i);
        const Json & operator[](string s);

        Jsontype Json_type();

        std::variant<std::nullptr_t , bool, double, string, vector<Json>,unordered_map<string, Json>> value;
        Jsontype type;
    };
};
