// #include <iostream>
// #include <fstream>
// #include <sstream>
// #include <vector>
// #include <map>
// #include <string>
// #include <cassert>

// #include "Tokenizer.hpp"
// #include "Parser.hpp"
// #include "BytecodeInterpreter.hpp"
// #include "Instruction.hpp"

// using namespace std;
// using namespace simpleparser;
// using namespace bytecodeinterpreter;

// // Opcode names for debug display
// // static const char* sOpcodeNames[] = {
// //     "EXIT",
// //     "ADD_INT",
// //     "PUSH_INT",
// //     "POP_INT",
// //     "PRINT_INT",
// //     "COMP_INT_LT",
// //     "LOAD_INT",
// //     "STORE_INT",
// //     "JUMP_BY_IF_ZERO",
// //     "JUMP_BY",
// //     "LOAD_INT_BASEPOINTER_RELATIVE",
// //     "STORE_INT_BASEPOINTER_RELATIVE",
// //     "CALL",
// //     "RETURN"
// // };

// static const char* sOpcodeNames[] = {
//     "EXIT",
//     "ADD_INT",
//     "SUB_INT",
//     "MUL_INT",
//     "DIV_INT",
//     "PUSH_INT",
//     "POP_INT",
//     "PRINT_INT",
//     "COMP_INT_LT",
//     "COMP_INT_EQ",
//     "LOAD_INT",
//     "STORE_INT",
//     "JUMP_BY_IF_ZERO",
//     "JUMP_BY",
//     "LOAD_INT_BASEPOINTER_RELATIVE",
//     "STORE_INT_BASEPOINTER_RELATIVE",
//     "CALL",
//     "RETURN"
// };

// // ─────────────────────────────────────────────
// //  Compiler data structures
// // ─────────────────────────────────────────────

// struct Parameter {
//     string mName;
//     size_t mIndex;
// };

// struct CompiledFunction {
//     size_t mInstructionOffset;
//     size_t mNumArguments;
//     bool   mReturnsSomething;
// };

// // ─────────────────────────────────────────────
// //  Forward declarations
// // ─────────────────────────────────────────────

// void GenerateCodeForStatement(
//     const Statement&                    currStmt,
//     const map<string, int16_t>&         variableOffsets,
//     map<string, Parameter>              parameters,
//     vector<int16_t>&                    returnCmdJumpInstructions,
//     vector<Instruction>&                compiledCode,
//     map<string, CompiledFunction>&      functionNameToInstruction);

// void GenerateCodeForFunction(
//     const FunctionDefinition&           currFunc,
//     vector<Instruction>&                compiledCode,
//     map<string, CompiledFunction>&      functionNameToInstruction);

// // ─────────────────────────────────────────────
// //  Statement compiler
// // ─────────────────────────────────────────────

// void GenerateCodeForStatement(
//     const Statement&                    currStmt,
//     const map<string, int16_t>&         variableOffsets,
//     map<string, Parameter>              parameters,
//     vector<int16_t>&                    returnCmdJumpInstructions,
//     vector<Instruction>&                compiledCode,
//     map<string, CompiledFunction>&      functionNameToInstruction)
// {
//     switch (currStmt.mKind) {

//     case StatementKind::VARIABLE_DECLARATION:
//         if (currStmt.mType.mType == INT32 && !currStmt.mParameters.empty()) {
//             const auto& init = currStmt.mParameters[0];
//             if (init.mKind != StatementKind::LITERAL) {
//                 auto foundVar = variableOffsets.find(currStmt.mName);
//                 if (foundVar == variableOffsets.end())
//                     throw runtime_error(string("Unknown variable \"") + currStmt.mName + "\".");
//                 GenerateCodeForStatement(init, variableOffsets, parameters,
//                     returnCmdJumpInstructions, compiledCode, functionNameToInstruction);
//                 compiledCode.push_back(Instruction{STORE_INT_BASEPOINTER_RELATIVE, 0, foundVar->second});
//             }
//         }
//         break;

//     case StatementKind::FUNCTION_CALL:
//         if (currStmt.mName == "return") {
//             if (currStmt.mParameters.size() != 1)
//                 throw runtime_error("\"return\" expects one parameter.");
//             GenerateCodeForStatement(currStmt.mParameters[0], variableOffsets, parameters,
//                 returnCmdJumpInstructions, compiledCode, functionNameToInstruction);
//             compiledCode.push_back(Instruction{STORE_INT_BASEPOINTER_RELATIVE, 0,
//                 int16_t(-2 - (int)parameters.size())});
//             returnCmdJumpInstructions.push_back(compiledCode.size());
//             compiledCode.push_back(Instruction{JUMP_BY, 0, 0}); // patched later
//         }
//         else if (currStmt.mName == "print" || currStmt.mName == "printNum") {
//             if (currStmt.mParameters.size() != 1)
//                 throw runtime_error("\"print\" expects one parameter.");
//             GenerateCodeForStatement(currStmt.mParameters[0], variableOffsets, parameters,
//                 returnCmdJumpInstructions, compiledCode, functionNameToInstruction);
//             compiledCode.push_back(Instruction{PRINT_INT, 0, 0});
//         }
//         else {
//             auto foundFn = functionNameToInstruction.find(currStmt.mName);
//             if (foundFn == functionNameToInstruction.end())
//                 throw runtime_error(string("Unknown function \"") + currStmt.mName + "\".");
//             if (foundFn->second.mReturnsSomething)
//                 compiledCode.push_back(Instruction{PUSH_INT, 0, 0}); // reserve return slot
//             if (foundFn->second.mNumArguments != currStmt.mParameters.size())
//                 throw runtime_error(string("Wrong number of arguments to \"") + currStmt.mName + "\".");
//             for (auto& p : currStmt.mParameters)
//                 GenerateCodeForStatement(p, variableOffsets, parameters,
//                     returnCmdJumpInstructions, compiledCode, functionNameToInstruction);
//             size_t rel = foundFn->second.mInstructionOffset - compiledCode.size();
//             compiledCode.push_back(Instruction{CALL, 0, int16_t(rel)});
//             for (size_t i = 0; i < currStmt.mParameters.size(); ++i)
//                 compiledCode.push_back(Instruction{POP_INT, 0, 0});
//         }
//         break;

//     case StatementKind::LITERAL:
//         if (currStmt.mType.mType == INT32) {
//             int32_t v = stoi(currStmt.mName);
//             compiledCode.push_back(Instruction{PUSH_INT, 0, int16_t(v)});
//         }
//         break;

//     // case StatementKind::OPERATOR_CALL:
//     //     if (currStmt.mParameters.size() != 2)
//     //         throw runtime_error(string("Wrong number of operands for \"") + currStmt.mName + "\".");
//     //     if (currStmt.mName == "+" || currStmt.mName == "<") {
//     //         Opcode op = (currStmt.mName == "+") ? ADD_INT : COMP_INT_LT;
//     //         for (auto& p : currStmt.mParameters)
//     //             GenerateCodeForStatement(p, variableOffsets, parameters,
//     //                 returnCmdJumpInstructions, compiledCode, functionNameToInstruction);
//     //         compiledCode.push_back(Instruction{op, 0, 0});
//     //     }
//     //     else if (currStmt.mName == "=") {
//     //         auto foundVar = variableOffsets.find(currStmt.mParameters[0].mName);
//     //         if (foundVar == variableOffsets.end())
//     //             throw runtime_error(string("Unknown variable \"") + currStmt.mParameters[0].mName + "\".");
//     //         GenerateCodeForStatement(currStmt.mParameters[1], variableOffsets, parameters,
//     //             returnCmdJumpInstructions, compiledCode, functionNameToInstruction);
//     //         compiledCode.push_back(Instruction{STORE_INT_BASEPOINTER_RELATIVE, 0, foundVar->second});
//     //     }
//     //     break;

//     case StatementKind::OPERATOR_CALL:
//         if (currStmt.mParameters.size() != 2)
//             throw runtime_error(string("Wrong number of operands for \"") + currStmt.mName + "\".");
//         if (currStmt.mName == "=" ) {
//             auto foundVar = variableOffsets.find(currStmt.mParameters[0].mName);
//             if (foundVar == variableOffsets.end())
//                 throw runtime_error(string("Unknown variable \"") + currStmt.mParameters[0].mName + "\".");
//             GenerateCodeForStatement(currStmt.mParameters[1], variableOffsets, parameters,
//                 returnCmdJumpInstructions, compiledCode, functionNameToInstruction);
//             compiledCode.push_back(Instruction{STORE_INT_BASEPOINTER_RELATIVE, 0, foundVar->second});
//         } else {
//             // All other operators: push both operands then emit opcode
//             Opcode op;
//             if      (currStmt.mName == "+")  op = ADD_INT;
//             else if (currStmt.mName == "-")  op = SUB_INT;
//             else if (currStmt.mName == "*")  op = MUL_INT;
//             else if (currStmt.mName == "/")  op = DIV_INT;
//             else if (currStmt.mName == "<")  op = COMP_INT_LT;
//             else if (currStmt.mName == "==") op = COMP_INT_EQ;
//             else throw runtime_error(string("Unknown operator \"") + currStmt.mName + "\".");
//             for (auto& p : currStmt.mParameters)
//                 GenerateCodeForStatement(p, variableOffsets, parameters,
//                     returnCmdJumpInstructions, compiledCode, functionNameToInstruction);
//             compiledCode.push_back(Instruction{op, 0, 0});
//         }
//         break;

//     case StatementKind::VARIABLE_NAME: {
//         auto foundVar = variableOffsets.find(currStmt.mName);
//         if (foundVar != variableOffsets.end()) {
//             compiledCode.push_back(Instruction{LOAD_INT_BASEPOINTER_RELATIVE, 0, foundVar->second});
//             break;
//         }
//         auto foundParam = parameters.find(currStmt.mName);
//         if (foundParam != parameters.end()) {
//             compiledCode.push_back(Instruction{LOAD_INT_BASEPOINTER_RELATIVE, 0,
//                 int16_t(-1 - (int)parameters.size() + (int)foundParam->second.mIndex)});
//             break;
//         }
//         throw runtime_error(string("Unknown variable \"") + currStmt.mName + "\".");
//         break;
//     }

//     case StatementKind::WHILE_LOOP: {
//         size_t condOffset = compiledCode.size();
//         GenerateCodeForStatement(currStmt.mParameters[0], variableOffsets, parameters,
//             returnCmdJumpInstructions, compiledCode, functionNameToInstruction);
//         size_t jumpIfFalseIdx = compiledCode.size();
//         compiledCode.push_back(Instruction{JUMP_BY_IF_ZERO, 0, 0}); // patched later
//         for (auto it = currStmt.mParameters.begin() + 1; it != currStmt.mParameters.end(); ++it)
//             GenerateCodeForStatement(*it, variableOffsets, parameters,
//                 returnCmdJumpInstructions, compiledCode, functionNameToInstruction);
//         compiledCode.push_back(Instruction{JUMP_BY, 0, int16_t(condOffset - compiledCode.size())});
//         compiledCode[jumpIfFalseIdx].p2 = int16_t(compiledCode.size() - jumpIfFalseIdx);
//         break;
//     }

//     } // switch
// }

// // ─────────────────────────────────────────────
// //  Function compiler
// // ─────────────────────────────────────────────

// void GenerateCodeForFunction(
//     const FunctionDefinition&       currFunc,
//     vector<Instruction>&            compiledCode,
//     map<string, CompiledFunction>&  functionNameToInstruction)
// {
//     int numIntVars = 0;
//     vector<int16_t>        returnJumps;
//     map<string, int16_t>   varOffsets;
//     map<string, Parameter> params;

//     functionNameToInstruction[currFunc.mName] = CompiledFunction{
//         compiledCode.size(),
//         currFunc.mParameters.size(),
//         currFunc.mReturnsSomething
//     };

//     size_t paramIdx = 0;
//     for (auto& p : currFunc.mParameters)
//         params[p.mName] = Parameter{p.mName, paramIdx++};

//     // First pass: allocate stack space for local variables
//     for (auto& stmt : currFunc.mStatements) {
//         if (stmt.mKind == StatementKind::VARIABLE_DECLARATION && stmt.mType.mType == INT32) {
//             int32_t initVal = 0;
//             if (!stmt.mParameters.empty() && stmt.mParameters[0].mKind == StatementKind::LITERAL)
//                 initVal = stoi(stmt.mParameters[0].mName);
//             varOffsets[stmt.mName] = numIntVars++;
//             compiledCode.push_back(Instruction{PUSH_INT, 0, int16_t(initVal)});
//         }
//     }

//     // Second pass: generate code for all statements
//     for (auto& stmt : currFunc.mStatements)
//         GenerateCodeForStatement(stmt, varOffsets, params, returnJumps,
//             compiledCode, functionNameToInstruction);

//     // Patch all return jumps to point here
//     size_t epilogOffset = compiledCode.size();
//     for (auto idx : returnJumps)
//         compiledCode[idx].p2 = int16_t(epilogOffset - idx);

//     // Pop local variables
//     for (int i = 0; i < numIntVars; ++i)
//         compiledCode.push_back(Instruction{POP_INT, 0, 0});

//     compiledCode.push_back(Instruction{RETURN, 0, 0});
// }

// // ─────────────────────────────────────────────
// //  Run a source string end-to-end
// // ─────────────────────────────────────────────

// void runSource(const string& source, bool debugMode) {
//     // 1. Tokenize
//     Tokenizer tokenizer;
//     vector<Token> tokens = tokenizer.parse(source);

//     if (debugMode) {
//         cout << "\n=== TOKENS ===\n";
//         for (auto& t : tokens) t.debugPrint();
//     }

//     // 2. Parse
//     Parser parser;
//     parser.parse(tokens);

//     if (debugMode) {
//         cout << "\n=== AST ===\n";
//         parser.debugPrint();
//     }

//     // 3. Compile
//     vector<Instruction>           compiledCode;
//     map<string, CompiledFunction> functionNameToInstruction;
//     map<string, FunctionDefinition> functions = parser.GetFunctions();

//     for (auto& [name, func] : functions)
//         GenerateCodeForFunction(func, compiledCode, functionNameToInstruction);

//     if (debugMode) {
//         cout << "\n=== BYTECODE ===\n";
//         for (size_t i = 0; i < compiledCode.size(); ++i) {
//             auto& ins = compiledCode[i];
//             cout << "[" << i << "] "
//                  << sOpcodeNames[ins.opcode]
//                  << " p2=" << ins.p2 << "\n";
//         }
//     }

//     // 4. Execute
//     auto foundMain = functionNameToInstruction.find("main");
//     if (foundMain == functionNameToInstruction.end())
//         throw runtime_error("No 'main' function found.");

//     int16_t result = 0;
//     // Pass arguments matching main's parameter count
//     auto& mainFunc = foundMain->second;
//     vector<int16_t> mainArgs(mainFunc.mNumArguments, 0);
//     BytecodeInterpreter::Run(
//         compiledCode.data() + foundMain->second.mInstructionOffset,
//         mainArgs, &result);

//     cout << "\nResult: " << result << endl;
// }

// // ─────────────────────────────────────────────
// //  Entry point
// // ─────────────────────────────────────────────

// int main(int argc, const char* argv[]) {
//     cout << "CVM++ 0.1 — Custom Virtual Machine\n";
//     cout << "====================================\n";

//     bool debugMode = false;

//     // Check for --debug flag
//     for (int i = 1; i < argc; ++i)
//         if (string(argv[i]) == "--debug") debugMode = true;

//     // File runner mode
//     if (argc >= 2 && string(argv[1]) != "--debug") {
//         string filename = argv[1];
//         ifstream file(filename);
//         if (!file.is_open()) {
//             cerr << "Error: Cannot open file '" << filename << "'\n";
//             return 1;
//         }
//         stringstream ss;
//         ss << file.rdbuf();
//         try {
//             runSource(ss.str(), debugMode);
//         } catch (exception& e) {
//             cerr << "Error: " << e.what() << "\n";
//             return 1;
//         }
//         return 0;
//     }

//     // REPL mode
//     cout << "Type 'exit' to quit, '--debug' to toggle debug mode.\n\n";
//     string line, source;
//     while (true) {
//         cout << "cvm> ";
//         if (!getline(cin, line)) break;
//         if (line == "exit") break;
//         if (line == "--debug") {
//             debugMode = !debugMode;
//             cout << "Debug mode " << (debugMode ? "ON" : "OFF") << "\n";
//             continue;
//         }
//         if (line.empty()) {
//             if (!source.empty()) {
//                 try {
//                     runSource(source, debugMode);
//                 } catch (exception& e) {
//                     cerr << "Error: " << e.what() << "\n";
//                 }
//                 source.clear();
//             }
//             continue;
//         }
//         source += line + "\n";
//     }

//     return 0;
// }

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <cassert>

#include "Tokenizer.hpp"
#include "Parser.hpp"
#include "BytecodeInterpreter.hpp"
#include "Instruction.hpp"

using namespace std;
using namespace simpleparser;
using namespace bytecodeinterpreter;

// ─────────────────────────────────────────────
//  Opcode names for debug display
// ─────────────────────────────────────────────

static const char* sOpcodeNames[] = {
    "EXIT",
    "ADD_INT",
    "SUB_INT",
    "MUL_INT",
    "DIV_INT",
    "PUSH_INT",
    "POP_INT",
    "PRINT_INT",
    "COMP_INT_LT",
    "COMP_INT_EQ",
    "LOAD_INT",
    "STORE_INT",
    "JUMP_BY_IF_ZERO",
    "JUMP_BY",
    "LOAD_INT_BASEPOINTER_RELATIVE",
    "STORE_INT_BASEPOINTER_RELATIVE",
    "CALL",
    "RETURN"
};

// ─────────────────────────────────────────────
//  Compiler data structures
// ─────────────────────────────────────────────

struct Parameter {
    string mName;
    size_t mIndex;
};

struct CompiledFunction {
    size_t mInstructionOffset;
    size_t mNumArguments;
    bool   mReturnsSomething;
};

// ─────────────────────────────────────────────
//  Forward declarations
// ─────────────────────────────────────────────

void GenerateCodeForStatement(
    const Statement&                    currStmt,
    const map<string, int16_t>&         variableOffsets,
    map<string, Parameter>              parameters,
    vector<int16_t>&                    returnCmdJumpInstructions,
    vector<Instruction>&                compiledCode,
    map<string, CompiledFunction>&      functionNameToInstruction);

void GenerateCodeForFunction(
    const FunctionDefinition&           currFunc,
    vector<Instruction>&                compiledCode,
    map<string, CompiledFunction>&      functionNameToInstruction);

// ─────────────────────────────────────────────
//  Statement compiler
// ─────────────────────────────────────────────

void GenerateCodeForStatement(
    const Statement&                    currStmt,
    const map<string, int16_t>&         variableOffsets,
    map<string, Parameter>              parameters,
    vector<int16_t>&                    returnCmdJumpInstructions,
    vector<Instruction>&                compiledCode,
    map<string, CompiledFunction>&      functionNameToInstruction)
{
    switch (currStmt.mKind) {

    case StatementKind::VARIABLE_DECLARATION:
        if (currStmt.mType.mType == INT32 && !currStmt.mParameters.empty()) {
            const auto& init = currStmt.mParameters[0];
            if (init.mKind != StatementKind::LITERAL) {
                auto foundVar = variableOffsets.find(currStmt.mName);
                if (foundVar == variableOffsets.end())
                    throw runtime_error(string("Unknown variable \"") + currStmt.mName + "\".");
                GenerateCodeForStatement(init, variableOffsets, parameters,
                    returnCmdJumpInstructions, compiledCode, functionNameToInstruction);
                compiledCode.push_back(Instruction{STORE_INT_BASEPOINTER_RELATIVE, 0, foundVar->second});
            }
        }
        break;

    case StatementKind::FUNCTION_CALL:
        if (currStmt.mName == "return") {
            if (currStmt.mParameters.size() != 1)
                throw runtime_error("\"return\" expects one parameter.");
            GenerateCodeForStatement(currStmt.mParameters[0], variableOffsets, parameters,
                returnCmdJumpInstructions, compiledCode, functionNameToInstruction);
            compiledCode.push_back(Instruction{STORE_INT_BASEPOINTER_RELATIVE, 0,
                int16_t(-2 - (int)parameters.size())});
            returnCmdJumpInstructions.push_back(compiledCode.size());
            compiledCode.push_back(Instruction{JUMP_BY, 0, 0}); // patched later
        }
        else if (currStmt.mName == "print" || currStmt.mName == "printNum") {
            if (currStmt.mParameters.size() != 1)
                throw runtime_error("\"print\" expects one parameter.");
            GenerateCodeForStatement(currStmt.mParameters[0], variableOffsets, parameters,
                returnCmdJumpInstructions, compiledCode, functionNameToInstruction);
            compiledCode.push_back(Instruction{PRINT_INT, 0, 0});
        }
        else {
            auto foundFn = functionNameToInstruction.find(currStmt.mName);
            if (foundFn == functionNameToInstruction.end())
                throw runtime_error(string("Unknown function \"") + currStmt.mName + "\".");
            if (foundFn->second.mReturnsSomething)
                compiledCode.push_back(Instruction{PUSH_INT, 0, 0}); // reserve return slot
            if (foundFn->second.mNumArguments != currStmt.mParameters.size())
                throw runtime_error(string("Wrong number of arguments to \"") + currStmt.mName + "\".");
            for (auto& p : currStmt.mParameters)
                GenerateCodeForStatement(p, variableOffsets, parameters,
                    returnCmdJumpInstructions, compiledCode, functionNameToInstruction);
            size_t rel = foundFn->second.mInstructionOffset - compiledCode.size();
            compiledCode.push_back(Instruction{CALL, 0, int16_t(rel)});
            for (size_t i = 0; i < currStmt.mParameters.size(); ++i)
                compiledCode.push_back(Instruction{POP_INT, 0, 0});
        }
        break;

    case StatementKind::LITERAL:
        if (currStmt.mType.mType == INT32) {
            int32_t v = stoi(currStmt.mName);
            compiledCode.push_back(Instruction{PUSH_INT, 0, int16_t(v)});
        }
        break;

    case StatementKind::OPERATOR_CALL:
        if (currStmt.mParameters.size() != 2)
            throw runtime_error(string("Wrong number of operands for \"") + currStmt.mName + "\".");
        if (currStmt.mName == "=") {
            auto foundVar = variableOffsets.find(currStmt.mParameters[0].mName);
            if (foundVar == variableOffsets.end())
                throw runtime_error(string("Unknown variable \"") + currStmt.mParameters[0].mName + "\".");
            GenerateCodeForStatement(currStmt.mParameters[1], variableOffsets, parameters,
                returnCmdJumpInstructions, compiledCode, functionNameToInstruction);
            compiledCode.push_back(Instruction{STORE_INT_BASEPOINTER_RELATIVE, 0, foundVar->second});
        } else {
            Opcode op;
            if      (currStmt.mName == "+")  op = ADD_INT;
            else if (currStmt.mName == "-")  op = SUB_INT;
            else if (currStmt.mName == "*")  op = MUL_INT;
            else if (currStmt.mName == "/")  op = DIV_INT;
            else if (currStmt.mName == "<")  op = COMP_INT_LT;
            else if (currStmt.mName == "==") op = COMP_INT_EQ;
            else throw runtime_error(string("Unknown operator \"") + currStmt.mName + "\".");
            for (auto& p : currStmt.mParameters)
                GenerateCodeForStatement(p, variableOffsets, parameters,
                    returnCmdJumpInstructions, compiledCode, functionNameToInstruction);
            compiledCode.push_back(Instruction{op, 0, 0});
        }
        break;

    case StatementKind::VARIABLE_NAME: {
        auto foundVar = variableOffsets.find(currStmt.mName);
        if (foundVar != variableOffsets.end()) {
            compiledCode.push_back(Instruction{LOAD_INT_BASEPOINTER_RELATIVE, 0, foundVar->second});
            break;
        }
        auto foundParam = parameters.find(currStmt.mName);
        if (foundParam != parameters.end()) {
            compiledCode.push_back(Instruction{LOAD_INT_BASEPOINTER_RELATIVE, 0,
                int16_t(-1 - (int)parameters.size() + (int)foundParam->second.mIndex)});
            break;
        }
        throw runtime_error(string("Unknown variable \"") + currStmt.mName + "\".");
        break;
    }

    case StatementKind::WHILE_LOOP: {
        size_t condOffset = compiledCode.size();
        GenerateCodeForStatement(currStmt.mParameters[0], variableOffsets, parameters,
            returnCmdJumpInstructions, compiledCode, functionNameToInstruction);
        size_t jumpIfFalseIdx = compiledCode.size();
        compiledCode.push_back(Instruction{JUMP_BY_IF_ZERO, 0, 0}); // patched later
        for (auto it = currStmt.mParameters.begin() + 1; it != currStmt.mParameters.end(); ++it)
            GenerateCodeForStatement(*it, variableOffsets, parameters,
                returnCmdJumpInstructions, compiledCode, functionNameToInstruction);
        compiledCode.push_back(Instruction{JUMP_BY, 0, int16_t(condOffset - compiledCode.size())});
        compiledCode[jumpIfFalseIdx].p2 = int16_t(compiledCode.size() - jumpIfFalseIdx);
        break;
    }

    case StatementKind::BLOCK:
        for (auto& stmt : currStmt.mParameters)
            GenerateCodeForStatement(stmt, variableOffsets, parameters,
                returnCmdJumpInstructions, compiledCode, functionNameToInstruction);
        break;

    case StatementKind::IF_STATEMENT: {
        // mParameters[0] = condition
        // mParameters[1] = then-block
        // mParameters[2] = else-block

        // 1. emit condition
        GenerateCodeForStatement(currStmt.mParameters[0], variableOffsets, parameters,
            returnCmdJumpInstructions, compiledCode, functionNameToInstruction);

        // 2. JUMP_BY_IF_ZERO over then-block (patched later)
        size_t jumpOverThenIdx = compiledCode.size();
        compiledCode.push_back(Instruction{JUMP_BY_IF_ZERO, 0, 0});

        // 3. emit then-block
        GenerateCodeForStatement(currStmt.mParameters[1], variableOffsets, parameters,
            returnCmdJumpInstructions, compiledCode, functionNameToInstruction);

        // 4. JUMP_BY over else-block (patched later) — only if else exists
        size_t jumpOverElseIdx = compiledCode.size();
        bool hasElse = !currStmt.mParameters[2].mParameters.empty();
        if (hasElse)
            compiledCode.push_back(Instruction{JUMP_BY, 0, 0});

        // 5. patch condition jump to land here (start of else or after then)
        compiledCode[jumpOverThenIdx].p2 = int16_t(compiledCode.size() - jumpOverThenIdx);

        // 6. emit else-block and patch its jump
        if (hasElse) {
            GenerateCodeForStatement(currStmt.mParameters[2], variableOffsets, parameters,
                returnCmdJumpInstructions, compiledCode, functionNameToInstruction);
            compiledCode[jumpOverElseIdx].p2 = int16_t(compiledCode.size() - jumpOverElseIdx);
        }

        break;
    }

    } // switch
}

// ─────────────────────────────────────────────
//  Function compiler
// ─────────────────────────────────────────────

void GenerateCodeForFunction(
    const FunctionDefinition&       currFunc,
    vector<Instruction>&            compiledCode,
    map<string, CompiledFunction>&  functionNameToInstruction)
{
    int numIntVars = 0;
    vector<int16_t>        returnJumps;
    map<string, int16_t>   varOffsets;
    map<string, Parameter> params;

    functionNameToInstruction[currFunc.mName] = CompiledFunction{
        compiledCode.size(),
        currFunc.mParameters.size(),
        currFunc.mReturnsSomething
    };

    size_t paramIdx = 0;
    for (auto& p : currFunc.mParameters)
        params[p.mName] = Parameter{p.mName, paramIdx++};

    // First pass: allocate stack space for local variables
    for (auto& stmt : currFunc.mStatements) {
        if (stmt.mKind == StatementKind::VARIABLE_DECLARATION && stmt.mType.mType == INT32) {
            int32_t initVal = 0;
            if (!stmt.mParameters.empty() && stmt.mParameters[0].mKind == StatementKind::LITERAL)
                initVal = stoi(stmt.mParameters[0].mName);
            varOffsets[stmt.mName] = numIntVars++;
            compiledCode.push_back(Instruction{PUSH_INT, 0, int16_t(initVal)});
        }
    }

    // Second pass: generate code for all statements
    for (auto& stmt : currFunc.mStatements)
        GenerateCodeForStatement(stmt, varOffsets, params, returnJumps,
            compiledCode, functionNameToInstruction);

    // Patch all return jumps to point here
    size_t epilogOffset = compiledCode.size();
    for (auto idx : returnJumps)
        compiledCode[idx].p2 = int16_t(epilogOffset - idx);

    // Pop local variables
    for (int i = 0; i < numIntVars; ++i)
        compiledCode.push_back(Instruction{POP_INT, 0, 0});

    compiledCode.push_back(Instruction{RETURN, 0, 0});
}

// ─────────────────────────────────────────────
//  Run a source string end-to-end
// ─────────────────────────────────────────────

void runSource(const string& source, bool debugMode) {
    // 1. Tokenize
    Tokenizer tokenizer;
    vector<Token> tokens = tokenizer.parse(source);

    if (debugMode) {
        cout << "\n=== TOKENS ===\n";
        for (auto& t : tokens) t.debugPrint();
    }

    // 2. Parse
    Parser parser;
    parser.parse(tokens);

    if (debugMode) {
        cout << "\n=== AST ===\n";
        parser.debugPrint();
    }

    // 3. Compile
    vector<Instruction>             compiledCode;
    map<string, CompiledFunction>   functionNameToInstruction;
    map<string, FunctionDefinition> functions = parser.GetFunctions();

    for (auto& [name, func] : functions)
        GenerateCodeForFunction(func, compiledCode, functionNameToInstruction);

    if (debugMode) {
        cout << "\n=== BYTECODE ===\n";
        for (size_t i = 0; i < compiledCode.size(); ++i) {
            auto& ins = compiledCode[i];
            cout << "[" << i << "] "
                 << sOpcodeNames[ins.opcode]
                 << " p2=" << ins.p2 << "\n";
        }
    }

    // 4. Execute
    auto foundMain = functionNameToInstruction.find("main");
    if (foundMain == functionNameToInstruction.end())
        throw runtime_error("No 'main' function found.");

    int16_t result = 0;
    auto& mainFunc = foundMain->second;
    vector<int16_t> mainArgs(mainFunc.mNumArguments, 0);
    BytecodeInterpreter::Run(
        compiledCode.data() + foundMain->second.mInstructionOffset,
        mainArgs, &result);

    cout << "\nResult: " << result << endl;
}

// ─────────────────────────────────────────────
//  Entry point
// ─────────────────────────────────────────────

int main(int argc, const char* argv[]) {
    cout << "CVM++ 0.1 - Custom Virtual Machine\n";
    cout << "====================================\n";

    bool debugMode = false;

    for (int i = 1; i < argc; ++i)
        if (string(argv[i]) == "--debug") debugMode = true;

    // File runner mode
    if (argc >= 2 && string(argv[1]) != "--debug") {
        string filename = argv[1];
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Cannot open file '" << filename << "'\n";
            return 1;
        }
        stringstream ss;
        ss << file.rdbuf();
        try {
            runSource(ss.str(), debugMode);
        } catch (exception& e) {
            cerr << "Error: " << e.what() << "\n";
            return 1;
        }
        return 0;
    }

    // REPL mode
    cout << "Type 'exit' to quit, '--debug' to toggle debug mode.\n\n";
    string line, source;
    while (true) {
        cout << "cvm> ";
        if (!getline(cin, line)) break;
        if (line == "exit") break;
        if (line == "--debug") {
            debugMode = !debugMode;
            cout << "Debug mode " << (debugMode ? "ON" : "OFF") << "\n";
            continue;
        }
        if (line.empty()) {
            if (!source.empty()) {
                try {
                    runSource(source, debugMode);
                } catch (exception& e) {
                    cerr << "Error: " << e.what() << "\n";
                }
                source.clear();
            }
            continue;
        }
        source += line + "\n";
    }

    return 0;
}