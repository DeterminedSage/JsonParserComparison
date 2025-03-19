import std.stdio;
import std.file;
import std.datetime;
import std.string;
import std.json;
import std.conv;
import std.process;
import core.sys.windows.windows;  // Windows API
import core.sys.windows.psapi;   // ✅ Import PSAPI for process memory info

void main() {
    // File path
    string filePath = "5mb_json.json";
    string json_string = readText(filePath);

    // Parsing
    writeln("--- jsoniopipe ---");
    auto startMemory = getMemoryUsageKB();
    auto start = Clock.currTime();
    JSONValue json = parseJSON(json_string);
    auto end = Clock.currTime();
    auto duration = end - start;
    writeln("Parsing time: ", duration.total!"msecs", " ms");

    auto endMemory = getMemoryUsageKB();
    writeln("Memory usage: ", endMemory - startMemory, " KB");

    // Validation Speed
    start = Clock.currTime();
    bool isValid = isValidJson(json_string);
    end = Clock.currTime();
    duration = end - start;
    writeln("Validation Time: ", duration.total!"msecs", " ms");

    // Serialization
    start = Clock.currTime();
    string serialized_string = json.toString();
    end = Clock.currTime();
    duration = end - start;
    writeln("Serialization time: ", duration.total!"msecs", " ms");

    // DOM access
    if (json.type == JSONType.object) {  
        start = Clock.currTime();
        auto glossary = json["glossary"];
        if (glossary.type == JSONType.object) {
            auto title = glossary["title"];
            if (title.type == JSONType.string) {
                string str_title = title.str;
            }
        }
        end = Clock.currTime();
        duration = end - start;
        writeln("DOM access time: ", duration.total!"usecs", " microseconds");
    }
}

// ✅ Fixed: Windows-compatible memory usage function
long getMemoryUsageKB() {
    PROCESS_MEMORY_COUNTERS memInfo;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &memInfo, cast(UINT)memInfo.sizeof))
        return cast(long) (memInfo.WorkingSetSize / 1024);

    return 0;
}

bool isValidJson(string json_string) {
    try {
        JSONValue json = parseJSON(json_string);
        return true;
    } catch (Exception e) {
        return false;
    }
}
