json11
------

#### modified
The original json11 from Dropbox doesnot support integer more than 2^54 for the compatible with javascript cases. But in most development cases, int64_t and uint64_t are very important types. So modified to support these ones.

    std::cout << "uint uint64 test" << std::endl;
    const string str_it2 = R"({"int_max":2147483647,"int_min":-2147483648,"uint_max":4294967295, "uint_min":0, "i64_max":9223372036854775807,"i64_min":-9223372036854775808,"ui64_max":18446744073709551615, "ui_64min":0 })";
    auto json_uit = Json::parse(str_it2, err_it);
    std::cout << "ORIGIN:" << str_it2 << std::endl;
    string str_it2_ret;
    json_uit.dump(str_it2_ret);
    std::cout << "DUMP:" << str_it2_ret << std::endl;

    assert(json_uit["int_max"].int64_value() == INT_MAX);
    assert(json_uit["int_min"].int64_value() == INT_MIN);
    assert(json_uit["uint_max"].uint64_value() == UINT_MAX);
    assert(json_uit["uint_min"].uint64_value() == 0);

    assert(json_uit["i64_max"].int64_value() == LLONG_MAX);
    assert(json_uit["i64_min"].int64_value() == LLONG_MIN);
    assert(json_uit["ui64_max"].uint64_value() == ULLONG_MAX);
    assert(json_uit["ui64_min"].uint64_value() == 0);

#### end

json11 is a tiny JSON library for C++11, providing JSON parsing and serialization.

The core object provided by the library is json11::Json. A Json object represents any JSON
value: null, bool, number (int or double), string (std::string), array (std::vector), or
object (std::map).

Json objects act like values. They can be assigned, copied, moved, compared for equality or
order, and so on. There are also helper methods Json::dump, to serialize a Json to a string, and
Json::parse (static) to parse a std::string as a Json object.

It's easy to make a JSON object with C++11's new initializer syntax:

    Json my_json = Json::object {
        { "key1", "value1" },
        { "key2", false },
        { "key3", Json::array { 1, 2, 3 } },
    };
    std::string json_str = my_json.dump();

There are also implicit constructors that allow standard and user-defined types to be
automatically converted to JSON. For example:

    class Point {
    public:
        int x;
        int y;
        Point (int x, int y) : x(x), y(y) {}
        Json to_json() const { return Json::array { x, y }; }
    };

    std::vector<Point> points = { { 1, 2 }, { 10, 20 }, { 100, 200 } };
    std::string points_json = Json(points).dump();

JSON values can have their values queried and inspected:

    Json json = Json::array { Json::object { { "k", "v" } } };
    std::string str = json[0]["k"].string_value();

More documentation is still to come. For now, see json11.hpp.
