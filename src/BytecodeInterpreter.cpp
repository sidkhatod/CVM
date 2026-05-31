#include "BytecodeInterpreter.hpp"
#include <iostream>
#include <stdexcept>

namespace bytecodeinterpreter {

    using namespace std;

    InstructionFunction gInstructionFunctions[NUM_INSTRUCTIONS] = {
        ExitInstruction,
        AddIntInstruction,
        SubIntInstruction,
        MulIntInstruction,
        DivIntInstruction,
        PushIntInstruction,
        PopIntInstruction,
        PrintIntInstruction,
        CompareIntLessThanInstruction,
        CompareIntEqualInstruction,
        LoadIntInstruction,
        StoreIntInstruction,
        JumpByIfZeroInstruction,
        JumpByInstruction,
        LoadIntBasepointerRelativeInstruction,
        StoreIntBasepointerRelativeInstruction,
        CallInstruction,
        ReturnInstruction,
    };

    /*static*/ void BytecodeInterpreter::Run(Instruction* code, vector<int16_t> arguments, int16_t* result) {
        InterpreterRegisters registers{ .currentInstruction = code };

        if (result)
            registers.stack.push_back(0);

        registers.stack.insert(registers.stack.end(), arguments.begin(), arguments.end());
        registers.stack.push_back(0); // saved baseIndex
        registers.returnAddressStack.push_back(nullptr);
        registers.baseIndex = registers.stack.size();

        while (registers.currentInstruction != nullptr)
            gInstructionFunctions[registers.currentInstruction->opcode](registers);

        size_t numArgs = arguments.size();
        while (numArgs--)
            registers.stack.pop_back();

        if (result)
            *result = registers.stack[0];
    }

    void ExitInstruction(InterpreterRegisters& registers) {
        registers.currentInstruction = nullptr;
    }

    void AddIntInstruction(InterpreterRegisters& registers) {
        int16_t rhs = registers.stack.back(); registers.stack.pop_back();
        int16_t lhs = registers.stack.back(); registers.stack.pop_back();
        registers.stack.push_back(lhs + rhs);
        ++registers.currentInstruction;
    }

    void SubIntInstruction(InterpreterRegisters& registers) {
        int16_t rhs = registers.stack.back(); registers.stack.pop_back();
        int16_t lhs = registers.stack.back(); registers.stack.pop_back();
        registers.stack.push_back(lhs - rhs);
        ++registers.currentInstruction;
    }

    void MulIntInstruction(InterpreterRegisters& registers) {
        int16_t rhs = registers.stack.back(); registers.stack.pop_back();
        int16_t lhs = registers.stack.back(); registers.stack.pop_back();
        registers.stack.push_back(lhs * rhs);
        ++registers.currentInstruction;
    }

    void DivIntInstruction(InterpreterRegisters& registers) {
        int16_t rhs = registers.stack.back(); registers.stack.pop_back();
        int16_t lhs = registers.stack.back(); registers.stack.pop_back();
        if (rhs == 0)
            throw runtime_error("Division by zero.");
        registers.stack.push_back(lhs / rhs);
        ++registers.currentInstruction;
    }

    void PushIntInstruction(InterpreterRegisters& registers) {
        registers.stack.push_back(registers.currentInstruction->p2);
        ++registers.currentInstruction;
    }

    void PopIntInstruction(InterpreterRegisters& registers) {
        registers.stack.pop_back();
        ++registers.currentInstruction;
    }

    void PrintIntInstruction(InterpreterRegisters& registers) {
        int16_t number = registers.stack.back();
        registers.stack.pop_back();
        cout << number << endl;
        ++registers.currentInstruction;
    }

    void CompareIntLessThanInstruction(InterpreterRegisters& registers) {
        int16_t rhs = registers.stack.back(); registers.stack.pop_back();
        int16_t lhs = registers.stack.back(); registers.stack.pop_back();
        registers.stack.push_back(int16_t(lhs < rhs));
        ++registers.currentInstruction;
    }

    void CompareIntEqualInstruction(InterpreterRegisters& registers) {
        int16_t rhs = registers.stack.back(); registers.stack.pop_back();
        int16_t lhs = registers.stack.back(); registers.stack.pop_back();
        registers.stack.push_back(int16_t(lhs == rhs));
        ++registers.currentInstruction;
    }

    void LoadIntInstruction(InterpreterRegisters& registers) {
        registers.stack.push_back(registers.stack[registers.currentInstruction->p2]);
        ++registers.currentInstruction;
    }

    void StoreIntInstruction(InterpreterRegisters& registers) {
        registers.stack[registers.currentInstruction->p2] = registers.stack.back();
        registers.stack.pop_back();
        ++registers.currentInstruction;
    }

    void JumpByIfZeroInstruction(InterpreterRegisters& registers) {
        int16_t condition = registers.stack.back();
        registers.stack.pop_back();
        if (condition == 0)
            registers.currentInstruction += registers.currentInstruction->p2;
        else
            ++registers.currentInstruction;
    }

    void JumpByInstruction(InterpreterRegisters& registers) {
        registers.currentInstruction += registers.currentInstruction->p2;
    }

    void LoadIntBasepointerRelativeInstruction(InterpreterRegisters& registers) {
        registers.stack.push_back(
            registers.stack[registers.currentInstruction->p2 + registers.baseIndex]);
        ++registers.currentInstruction;
    }

    void StoreIntBasepointerRelativeInstruction(InterpreterRegisters& registers) {
        registers.stack[registers.currentInstruction->p2 + registers.baseIndex] =
            registers.stack.back();
        registers.stack.pop_back();
        ++registers.currentInstruction;
    }

    void CallInstruction(InterpreterRegisters& registers) {
        registers.stack.push_back(int16_t(registers.baseIndex));
        registers.returnAddressStack.push_back(registers.currentInstruction + 1);
        registers.baseIndex = registers.stack.size();
        registers.currentInstruction += registers.currentInstruction->p2;
    }

    void ReturnInstruction(InterpreterRegisters& registers) {
        Instruction* returnAddress = registers.returnAddressStack.back();
        registers.returnAddressStack.pop_back();
        registers.baseIndex = registers.stack.back();
        registers.stack.pop_back();
        registers.currentInstruction = returnAddress;
    }

}