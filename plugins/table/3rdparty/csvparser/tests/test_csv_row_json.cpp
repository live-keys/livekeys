#include "catch.hpp"
#include "csv.hpp"

#include <sstream>
using namespace csv;

/** Construct a CSVRow object for testing given column names and CSV fields */
CSVRow make_csv_row(std::vector<std::string> data, std::vector<std::string> col_names) {
    // Concatenate vector or strings into one large string
    using namespace csv::internals;

    std::stringstream raw_csv;
    auto writer = make_csv_writer(raw_csv);
    writer << col_names;
    writer << data;

    CSVReader reader(raw_csv);
    CSVRow row;
    reader.read_row(row);

    return row;
}

TEST_CASE("json_escape_string() Test", "[json_escape_string]") {
    using internals::json_escape_string;

    // Assert that special characters are escaped properly
    REQUIRE(json_escape_string("Quote\"Quote") == "Quote\\\"Quote");
    REQUIRE(json_escape_string("RSolidus\\RSolidus")
        == "RSolidus\\\\RSolidus");
    REQUIRE(json_escape_string("Backspace\bBackspace")
        == "Backspace\\bBackspace");
    REQUIRE(json_escape_string("Formfeed\fFormfeed")
        == "Formfeed\\fFormfeed");
    REQUIRE(json_escape_string("Newline\nNewline")
        == "Newline\\nNewline");
    REQUIRE(json_escape_string("CarriageReturn\rCarriageReturn")
        == "CarriageReturn\\rCarriageReturn");
    REQUIRE(json_escape_string("Tab\tTab")
        == "Tab\\tTab");

    // Assert that control characters are escaped properly
    REQUIRE(json_escape_string("Null\0Null")
        == "Null\u0000Null");
}

TEST_CASE("CSVRow to_json() Test", "[csv_row_to_json]") {
    CSVRow row = make_csv_row(
        { "Col 1", "Col 2" },   // Fields
        { "A", "B" }            // Column names
    );

    REQUIRE(row.to_json() == "{\"A\":\"Col 1\",\"B\":\"Col 2\"}");
}

TEST_CASE("CSVRow to_json() Test with Numbers", "[csv_numeric_row_to_json]") {
    CSVRow row = make_csv_row(
        { "1234.3", "234" },    // Fields
        { "A", "B"}             // Column names
    );

    REQUIRE(row.to_json() == "{\"A\":1234.3,\"B\":234}");
}

TEST_CASE("CSVRow to_json() Test - Mixed", "[csv_mixed_row_to_json]") {
    CSVRow row = make_csv_row(
        { "1234.3", "234", "ABCD", "AB1", "1337" },     // Fields
        { "A", "B", "C", "D", "E" }                     // Column names
    );

    SECTION("Full Row") {
        REQUIRE(row.to_json() == "{\"A\":1234.3,\"B\":234,\"C\":\"ABCD\",\"D\":\"AB1\",\"E\":1337}");
    }

    SECTION("Subset") {
        REQUIRE(row.to_json({ "B", "C" }) == "{\"B\":234,\"C\":\"ABCD\"}");
        REQUIRE(row.to_json({ "B", "A" }) == "{\"B\":234,\"A\":1234.3}");
    }
}

TEST_CASE("CSVRow to_json_array() Test() - Mixed", "[csv_mixed_row_to_json_array]") {
    CSVRow row = make_csv_row(
        { "1234.3", "234", "ABCD", "AB1", "1337" },     // Fields
        { "A", "B", "C", "D", "E" }                     // Column names
    );

    SECTION("Full Row") {
        REQUIRE(row.to_json_array() == "[1234.3,234,\"ABCD\",\"AB1\",1337]");
    }

    SECTION("Subset") {
        REQUIRE(row.to_json_array({ "B", "C" }) == "[234,\"ABCD\"]");
        REQUIRE(row.to_json_array({ "B", "A" }) == "[234,1234.3]");
    }
}

// Reported in: https://github.com/vincentlaucsb/csv-parser/issues/68
TEST_CASE("CSVRow to_json() with Wrong Columns", "[csv_json_wrong_cols]") {
    std::stringstream csv_string(R"(A,B,C,
123,345,678,)");

    auto format = CSVFormat();
    format.column_names({ "A", "B" });

    CSVReader reader(csv_string, format);
    CSVRow first_row;
    reader.read_row(first_row);

    // Since the column names provided were wrong, there won't be any data.
    // to_json() method should then produce an empty object instead of segfaulting.
    REQUIRE(first_row.to_json() == "{}");
    REQUIRE(first_row.to_json_array() == "[]");
}