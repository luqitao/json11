/*
 * Define JSON11_TEST_CUSTOM_CONFIG to 1 if you want to build this tester into
 * your own unit-test framework rather than a stand-alone program.  By setting
 * The values of the variables included below, you can insert your own custom
 * code into this file as it builds, in order to make it into a test case for
 * your favorite framework.
 */
#if !JSON11_TEST_CUSTOM_CONFIG
#define JSON11_TEST_CPP_PREFIX_CODE
#define JSON11_TEST_CPP_SUFFIX_CODE
#define JSON11_TEST_STANDALONE_MAIN 1
#define JSON11_TEST_CASE(name) static void name()
#define JSON11_TEST_ASSERT(b) assert(b)
#ifdef NDEBUG
#undef NDEBUG//at now assert will work even in Release build
#endif
#endif // JSON11_TEST_CUSTOM_CONFIG

/*
 * Enable or disable code which demonstrates the behavior change in Xcode 7 / Clang 3.7,
 * introduced by DR1467 and described here: https://llvm.org/bugs/show_bug.cgi?id=23812
 * Defaults to on in order to act as a warning to anyone who's unaware of the issue.
 */
#ifndef JSON11_ENABLE_DR1467_CANARY
#define JSON11_ENABLE_DR1467_CANARY 1
#endif

/*
 * Beginning of standard source file, which makes use of the customizations above.
 */
#include <cassert>
#include <string>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <sstream>
#include "json11.hpp"
#include <list>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <type_traits>

#include <climits>

// Insert user-defined prefix code (includes, function declarations, etc)
// to set up a custom test suite
JSON11_TEST_CPP_PREFIX_CODE

using namespace json11;
using std::string;

// Check that Json has the properties we want.
#define CHECK_TRAIT(x) static_assert(std::x::value, #x)
CHECK_TRAIT(is_nothrow_constructible<Json>);
CHECK_TRAIT(is_nothrow_default_constructible<Json>);
CHECK_TRAIT(is_copy_constructible<Json>);
CHECK_TRAIT(is_nothrow_move_constructible<Json>);
CHECK_TRAIT(is_copy_assignable<Json>);
CHECK_TRAIT(is_nothrow_move_assignable<Json>);
CHECK_TRAIT(is_nothrow_destructible<Json>);

JSON11_TEST_CASE(json11_test) {

    std::cout << "int int64 test" << std::endl;

    const string str_it = R"({"message_id":105308320612483198,"msg_type":3,"order":0,
            "ques_id":0,"session_id":105308187502051928,"site_id":122062,
            "timestamp":1471140271,
            "visitor_id":9941658010949867158,"worker_id":133746})";
    string err_it;
    auto json_it = Json::parse(str_it, err_it);

    assert(json_it["message_id"].int64_value() == 105308320612483198);
    assert(json_it["message_id"].uint64_value() == 105308320612483198);
    assert(json_it["msg_type"].int64_value() == 3);
    assert(json_it["msg_type"].uint64_value() == 3);
    assert(json_it["order"].int64_value() == 0);
    assert(json_it["session_id"].int64_value() == 105308187502051928);
    assert(json_it["site_id"].int64_value() == 122062);
    assert(json_it["site_id"].int64_value() == 122062);
    std::cout << (signed long long)json_it["visitor_id"].int64_value() << std::endl;
    assert((unsigned long long)(json_it["visitor_id"].int64_value()) == 9941658010949867158ull);
    std::cout << "int int64 test passed!" << std::endl;

    std::cout << "uint uint64 test" << std::endl;
    const string str_it2 = R"({"int_max":2147483647,"int_min":-2147483648,"uint_max":4294967295, "uint_min":0,
                                 "i64_max":9223372036854775807,"i64_min":-9223372036854775808,"ui64_max":18446744073709551615, "ui_64min":0 })";
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

    std::cout << "int64 uint64 test passed!" << std::endl;

    const string simple_test =
        R"({"k1":"v1", "k2":42, "k3":["a",123,true,false,null]})";

    string err;
    auto json = Json::parse(simple_test, err);

    std::cout << "k1: " << json["k1"].string_value() << "\n";
    std::cout << "k3: " << json["k3"].dump() << "\n";

    for (auto &k : json["k3"].array_items()) {
        std::cout << "    - " << k.dump() << "\n";
    }

    const string comment_test = R"({
      // comment /* with nested comment */
      "a": 1,
      // comment
      // continued
      "b": "text",
      /* multi
         line
         comment */
      // and single-line comment
      "c": [1, 2, 3]
    })";

    string err_comment;
    auto json_comment = Json::parse(
      comment_test, err_comment, JsonParse::COMMENTS);
    if (!err_comment.empty()) {
        printf("Failed: %s\n", err_comment.c_str());
    } else {
        printf("Result: %s\n", json_comment.dump().c_str());
    }

    string failing_comment_test = R"({
      /* bad comment
      "a": 1,
      // another comment to make C parsers which don't understand raw strings happy */
    })";

    string err_failing_comment;
    auto json_failing_comment = Json::parse(
      failing_comment_test, err_failing_comment, JsonParse::COMMENTS);
    if (!err_failing_comment.empty()) {
        printf("Failed: %s\n", err_failing_comment.c_str());
    } else {
        printf("Result: %s\n", json_failing_comment.dump().c_str());
    }

    failing_comment_test = R"({
      / / bad comment })";

    json_failing_comment = Json::parse(
      failing_comment_test, err_failing_comment, JsonParse::COMMENTS);
    if (!err_failing_comment.empty()) {
        printf("Failed: %s\n", err_failing_comment.c_str());
    } else {
        printf("Result: %s\n", json_failing_comment.dump().c_str());
    }

    failing_comment_test = R"({// bad comment })";

    json_failing_comment = Json::parse(
      failing_comment_test, err_failing_comment, JsonParse::COMMENTS);
    if (!err_failing_comment.empty()) {
        printf("Failed: %s\n", err_failing_comment.c_str());
    } else {
        printf("Result: %s\n", json_failing_comment.dump().c_str());
    }

    failing_comment_test = R"({
          "a": 1
        }/)";

    json_failing_comment = Json::parse(
      failing_comment_test, err_failing_comment, JsonParse::COMMENTS);
    if (!err_failing_comment.empty()) {
        printf("Failed: %s\n", err_failing_comment.c_str());
    } else {
        printf("Result: %s\n", json_failing_comment.dump().c_str());
    }

    failing_comment_test = R"({/* bad
                                  comment *})";

    json_failing_comment = Json::parse(
      failing_comment_test, err_failing_comment, JsonParse::COMMENTS);
    if (!err_failing_comment.empty()) {
        printf("Failed: %s\n", err_failing_comment.c_str());
    } else {
        printf("Result: %s\n", json_failing_comment.dump().c_str());
    }

    std::list<int> l1 { 1, 2, 3 };
    std::vector<int> l2 { 1, 2, 3 };
    std::set<int> l3 { 1, 2, 3 };
    JSON11_TEST_ASSERT(Json(l1) == Json(l2));
    JSON11_TEST_ASSERT(Json(l2) == Json(l3));

    std::map<string, string> m1 { { "k1", "v1" }, { "k2", "v2" } };
    std::unordered_map<string, string> m2 { { "k1", "v1" }, { "k2", "v2" } };
    JSON11_TEST_ASSERT(Json(m1) == Json(m2));

    // Json literals
    Json obj = Json::object({
        { "k1", "v1" },
        { "k2", 42.0 },
        { "k3", Json::array({ "a", 123.0, true, false, nullptr }) },
    });

    std::cout << "obj: " << obj.dump() << "\n";

    JSON11_TEST_ASSERT(Json("a").number_value() == 0);
    JSON11_TEST_ASSERT(Json("a").string_value() == "a");
    JSON11_TEST_ASSERT(Json().number_value() == 0);

    JSON11_TEST_ASSERT(obj == json);
    JSON11_TEST_ASSERT(Json(42) == Json(42.0));
    JSON11_TEST_ASSERT(Json(42) != Json(42.1));

    const string unicode_escape_test =
        R"([ "blah\ud83d\udca9blah\ud83dblah\udca9blah\u0000blah\u1234" ])";

    const char utf8[] = "blah" "\xf0\x9f\x92\xa9" "blah" "\xed\xa0\xbd" "blah"
                        "\xed\xb2\xa9" "blah" "\0" "blah" "\xe1\x88\xb4";

    Json uni = Json::parse(unicode_escape_test, err);
    JSON11_TEST_ASSERT(uni[0].string_value().size() == (sizeof utf8) - 1);
    JSON11_TEST_ASSERT(std::memcmp(uni[0].string_value().data(), utf8, sizeof utf8) == 0);

    // Demonstrates the behavior change in Xcode 7 / Clang 3.7, introduced by DR1467
    // and described here: https://llvm.org/bugs/show_bug.cgi?id=23812
    if (JSON11_ENABLE_DR1467_CANARY) {
        Json nested_array = Json::array { Json::array { 1, 2, 3 } };
        JSON11_TEST_ASSERT(nested_array.is_array());
        JSON11_TEST_ASSERT(nested_array.array_items().size() == 1);
        JSON11_TEST_ASSERT(nested_array.array_items()[0].is_array());
        JSON11_TEST_ASSERT(nested_array.array_items()[0].array_items().size() == 3);
    }

    {
        const std::string good_json = R"( {"k1" : "v1"})";
        const std::string bad_json1 = good_json + " {";
        const std::string bad_json2 = good_json + R"({"k2":"v2", "k3":[)";
        struct TestMultiParse {
            std::string input;
            std::string::size_type expect_parser_stop_pos;
            size_t expect_not_empty_elms_count;
            Json expect_parse_res;
        } tests[] = {
            {" {", 0, 0, {}},
            {good_json, good_json.size(), 1, Json(std::map<string, string>{ { "k1", "v1" } })},
            {bad_json1, good_json.size() + 1, 1, Json(std::map<string, string>{ { "k1", "v1" } })},
            {bad_json2, good_json.size(), 1, Json(std::map<string, string>{ { "k1", "v1" } })},
            {"{}", 2, 1, Json::object{}},
        };
        for (const auto &tst : tests) {
            std::string::size_type parser_stop_pos;
            std::string err;
            auto res = Json::parse_multi(tst.input, parser_stop_pos, err);
            JSON11_TEST_ASSERT(parser_stop_pos == tst.expect_parser_stop_pos);
            JSON11_TEST_ASSERT(
                (size_t)std::count_if(res.begin(), res.end(),
                                      [](const Json& j) { return !j.is_null(); })
                == tst.expect_not_empty_elms_count);
            if (!res.empty()) {
                JSON11_TEST_ASSERT(tst.expect_parse_res == res[0]);
            }
        }
    }
    Json my_json = Json::object {
        { "key1", "value1" },
        { "key2", false },
        { "key3", Json::array { 1, 2, 3 } },
    };
    std::string json_str = my_json.dump();
    printf("%s\n", json_str.c_str());

    class Point {
    public:
        int x;
        int y;
        Point (int x, int y) : x(x), y(y) {}
        Json to_json() const { return Json::array { x, y }; }
    };

    std::vector<Point> points = { { 1, 2 }, { 10, 20 }, { 100, 200 } };
    std::string points_json = Json(points).dump();
    printf("%s\n", points_json.c_str());
}

#if JSON11_TEST_STANDALONE_MAIN

static void parse_from_stdin() {
    string buf;
    string line;
    while (std::getline(std::cin, line)) {
        buf += line + "\n";
    }

    string err;
    auto json = Json::parse(buf, err);
    if (!err.empty()) {
        printf("Failed: %s\n", err.c_str());
    } else {
        printf("Result: %s\n", json.dump().c_str());
    }
}

int main(int argc, char **argv) {
    if (argc == 2 && argv[1] == string("--stdin")) {
        parse_from_stdin();
        return 0;
    }

    json11_test();
}

#endif // JSON11_TEST_STANDALONE_MAIN

// Insert user-defined suffix code (function definitions, etc)
// to set up a custom test suite
JSON11_TEST_CPP_SUFFIX_CODE
