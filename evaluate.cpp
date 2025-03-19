#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#include <psapi.h> // Include this header for PROCESS_MEMORY_COUNTERS and GetProcessMemoryInfo
#else
#include <unistd.h> // Required for sysconf
#endif
#include <cstdint> // For uint64_t

// Include RapidJSON headers
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

// Include simdjson headers
#include "simdjson.h"

using namespace std;
using namespace rapidjson;
using namespace std::chrono;

// Function to get memory usage (resident set size) in KB
long getMemoryUsageKB() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS memCounter;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &memCounter, sizeof(memCounter))) {
        return memCounter.WorkingSetSize / 1024;
    }
    return 0;
#else
    long rss = 0L;
    FILE* fp = fopen("/proc/self/statm", "r");
    if (fp == nullptr) {
        return 0; // Error
    }
    if (fscanf(fp, "%*s%ld", &rss) != 1) {
         fclose(fp);
        return 0;
    }
    fclose(fp);
    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024L;
    return rss * page_size_kb;
#endif
}

// Function to check validation speed
bool isValidJson(const string& json_string, bool isSimdJson) {
    if (isSimdJson) {
        simdjson::dom::parser parser;
        auto error = parser.parse(json_string);
        return !error.error();
    }
    Document d;
    ParseResult ok = d.Parse(json_string.c_str());
    return ok;
}

int main() {
    // File path
    const string filePath = "5mb_json.json";
    string json_string;
    {
        ifstream inputFile(filePath);
        if (!inputFile.is_open()) {
            cerr << "Error opening file." << endl;
            return 1;
        }

        json_string = string((istreambuf_iterator<char>(inputFile)),
                       istreambuf_iterator<char>());
        inputFile.close();
    }

    // RapidJSON parsing
    {
        cout << "--- RapidJSON ---" << endl;
        // Memory Usage
        long startMemory = getMemoryUsageKB();

        auto start = high_resolution_clock::now();
        FILE* fp = fopen(filePath.c_str(), "rb");
        if (fp == nullptr) {
            cerr << "Error opening file for RapidJSON." << endl;
            return 1;
        }
        char readBuffer[65536];
        FileReadStream is(fp, readBuffer, sizeof(readBuffer));
       
        Document d;
        d.ParseStream(is);
        fclose(fp);
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Parsing time: " << duration.count() / 1000 << " ms" << endl;

        long endMemory = getMemoryUsageKB();
        cout << "Memory usage: " << endMemory - startMemory << " KB" << endl;

        // Validation Speed
        auto startValidation = high_resolution_clock::now();
        bool isValid = isValidJson(json_string, false);
        auto endValidation = high_resolution_clock::now();
        auto validationDuration = duration_cast<microseconds>(endValidation - startValidation);
        cout << "Validation Time : " << validationDuration.count() / 1000 << " ms" << endl;

        // Serialization
        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        start = high_resolution_clock::now();
        d.Accept(writer);
        end = high_resolution_clock::now();
        duration = duration_cast<microseconds>(end - start);
        cout << "Serialization time: " << duration.count() / 1000 << " ms" << endl;

        if (d.HasParseError()) {
            cout << "Parse Error" << endl;
            cout << "Error code: " << d.GetParseError() << endl;
            cout << "Error offset: " << d.GetErrorOffset() << endl;
        }
        // DOM access
        if (d.IsObject() && d.HasMember("glossary") && d["glossary"].IsObject()) {
            start = high_resolution_clock::now();
            const Value& glossary = d["glossary"];
            const Value& title = glossary["title"];
            if (title.IsString()) {
                string str_title = title.GetString();
            }
            end = high_resolution_clock::now();
            duration = duration_cast<microseconds>(end - start);
            cout << "DOM access time: " << duration.count() << " microseconds" << endl;
        }
    }

    // simdjson parsing
    {
        cout << "--- simdjson ---" << endl;
        long startMemory = getMemoryUsageKB();
        auto start = high_resolution_clock::now();
        simdjson::dom::parser parser;

        simdjson::dom::element json;
        auto error = parser.parse(json_string).get(json);
        if (error) {
            cerr << "simdjson parse error: " << error << endl;
            return 1;
        }

        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Parsing time: " << duration.count() / 1000 << " ms" << endl;

        long endMemory = getMemoryUsageKB();
        cout << "Memory usage: " << endMemory - startMemory << " KB" << endl;

        // Validation Speed
        auto startValidation = high_resolution_clock::now();
        bool isValid = isValidJson(json_string, true);
        auto endValidation = high_resolution_clock::now();
        auto validationDuration = duration_cast<microseconds>(endValidation - startValidation);
        cout << "Validation Time : " << validationDuration.count() / 1000 << " ms" << endl;

        // Serialization
        start = high_resolution_clock::now();
        string serialized_string = simdjson::to_string(json);
        end = high_resolution_clock::now();
        duration = duration_cast<microseconds>(end - start);
        cout << "Serialization time: " << duration.count() / 1000 << " ms" << endl;

        // DOM access
        if (json.is_object()) {
            start = high_resolution_clock::now();
            auto glossary_result = json["glossary"];
            if (glossary_result.error()) {
                cerr << "simdjson DOM access error getting glossary key: " << glossary_result.error() << endl;
                return 1;
            }
            simdjson::dom::object glossary;
            error = glossary_result.get(glossary);
            if (error) {
                cerr << "simdjson DOM access error : " << error << endl;
                return 1;
            }
            string str_title;
            auto title_result = glossary["title"];
            if (title_result.error()) {
                cerr << "simdjson DOM access error getting title key: " << title_result.error() << endl;
                return 1;
            }
            string_view stringView;
            error = title_result.get(stringView);
            if (!error) {
                str_title = string(stringView);
            } else {
                cerr << "simdjson DOM access error: " << error << endl;
                return 1;
            }
            end = high_resolution_clock::now();
            duration = duration_cast<microseconds>(end - start);
            cout << "DOM access time: " << duration.count() << " microseconds" << endl;
        }
    }

    return 0;
}
