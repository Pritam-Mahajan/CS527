#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <cxxabi.h>

using namespace std;

struct FunctionInfo
{
    string functionName;
    int stackAllocation;
};

string demangle(const string &mangledName)
{
    int status;
    char *demangledName = abi::__cxa_demangle(mangledName.c_str(), nullptr, nullptr, &status);

    if (status == 0)
    {
        string result(demangledName);
        free(demangledName);

        size_t pos = result.find("(");
        if (pos != string::npos)
        {
            result = result.substr(0, pos);
        }

        return result;
    }
    else
    {
        return mangledName;
    }
}

vector<FunctionInfo> extractFunctions(const string &assemblyCode)
{
    vector<FunctionInfo> functions;
    regex functionNameRegex(R"(\s*(_[a-zA-Z0-9_]+):)");
    regex stackAllocationRegex(R"(\s*subq\s+\$([0-9]+),\s+%rsp)");
    std::regex movlStackAllocationRegex(R"(\s*movl\s+\$([0-9]+),\s+%eax)");

    istringstream codeStream(assemblyCode);
    string line;

    while (getline(codeStream, line))
    {
        smatch match;
        if (regex_match(line, match, functionNameRegex))
        {
            FunctionInfo function;
            function.functionName = match[1].str().substr(1);
            function.stackAllocation = 0;
            functions.push_back(function);
        }

        if (!functions.empty() && regex_match(line, match, stackAllocationRegex))
        {
            int allocationSize = stoi(match[1].str());
            functions.back().stackAllocation += allocationSize;
        }

        if (!functions.empty() && std::regex_match(line, match, movlStackAllocationRegex))
        {
            int allocationSize = std::stoi(match[1].str());
            functions.back().stackAllocation += allocationSize;
        }
    }

    return functions;
}

int main()
{
    const char *cCodeFileName = "Sample.c";
    const char *assemblyFileName = "output_asm.s";

    string compileCommand = "g++ -S " + string(cCodeFileName) + " -o " + string(assemblyFileName);
    int result = system(compileCommand.c_str());

    if (result != 0)
    {
        cerr << "Error generating assembly code." << endl;
        return result;
    }

    cout << "Assembly code generated successfully. See " << assemblyFileName << endl;

    ifstream assemblyFile("output_asm.s");
    stringstream buffer;
    buffer << assemblyFile.rdbuf();
    string assemblyCode = buffer.str();
    vector<FunctionInfo> functions = extractFunctions(assemblyCode);

    cout << "Extracted Functions and Stack Allocations:\n";
    for (const auto &function : functions)
    {
        cout << "Function: " << demangle(function.functionName) << ", Stack Allocation: " << function.stackAllocation << " bytes\n";
    }

    ofstream outputFile("fun_info.txt");
    if (outputFile.is_open())
    {
        for (const auto &function : functions)
        {
            outputFile << "Function: " << demangle(function.functionName) << ", Stack Allocation: " << function.stackAllocation << " bytes\n";
        }
        outputFile.close();
        cout << "Output written to 'fun_info.txt'\n";
    }
    else
    {
        cerr << "Unable to open output file.\n";
    }

    return 0;
}
