#pragma once
#include <cstdint>
#include <vector>
#include <iostream>
#include <iomanip>
#include <cctype>
#include <algorithm>
#undef OVERFLOW
//#define BUILD_RP3

enum Opcode : uint8_t
{
	STOP = 0,
	ADD,
	SUB,
	MUL,
	SMUL,
	DIV,
	SDIV,
	MOD,
	SMOD,
	EXP,
	NEG,
	LT,
	LE,
	GT,
	GE,
	EQ,
	NOT,
	STORE,
	LOAD,
	JMP,
	CALL,
	RET,
	PUSH,
	POP,
	MOV,
	SYSCALL
};

enum Register : uint8_t
{
	R0 = 0,
	R1,
	R2,
	R3,
	R4,
	R5,
	R6,
	R7,
	R8,
	R9,
	IP,
	SP,
	BP
};

enum class SYSCALL_TABLE
{
	Sleep = 0,
	WPort = 1,
	RPort = 2,
	Stout = 3,
};

static std::ostream& hex_dump(std::ostream& os, const void *buffer,
	std::size_t bufsize, bool showPrintableChars = true)
{
	if (buffer == nullptr) {
		return os;
	}
	auto oldFormat = os.flags();
	auto oldFillChar = os.fill();
	constexpr std::size_t maxline{ 16 };
	// create a place to store text version of string
	char renderString[maxline + 1];
	char *rsptr{ renderString };
	// convenience cast
	const unsigned char *buf{ reinterpret_cast<const unsigned char *>(buffer) };

	for (std::size_t linecount = maxline; bufsize; --bufsize, --buf) {
		os << std::setw(2) << std::setfill('0') << std::hex
			<< static_cast<unsigned>(*buf) << ' ';
		*rsptr++ = std::isprint(*buf) ? *buf : '.';
		if (--linecount == 0) {
			*rsptr++ = '\0';  // terminate string
			if (showPrintableChars) {
				os << " | " << renderString;
			}
			os << '\n';
			rsptr = renderString;
			linecount = std::min(maxline, bufsize);
		}
	}
	// emit newline if we haven't already
	if (rsptr != renderString) {
		if (showPrintableChars) {
			for (*rsptr++ = '\0'; rsptr != &renderString[maxline + 1]; ++rsptr) {
				os << "   ";
			}
			os << " | " << renderString;
		}
		os << '\n';
	}

	os.fill(oldFillChar);
	os.flags(oldFormat);
	return os;
}


class MCLVirtualMachine
{
public:
	MCLVirtualMachine(const std::vector<uint8_t>& bytecode);
	MCLVirtualMachine(const std::vector<char>& bytecode);
	~MCLVirtualMachine();
	void Execute();
	void PrintRegisters();
	void PrintStack();
protected:
	Opcode ExtractOpcode(bool& op1, bool& op2);
	Register ExtractRegister();
	uint32_t ExtractAddress();
	uint64_t ExtractConstant();
private:
	uint64_t Add(uint64_t v1, uint64_t v2);
	uint64_t Sub(uint64_t v1, uint64_t v2);
	uint64_t Mul(uint64_t v1, uint64_t v2);
	uint64_t SMul(uint64_t v1, uint64_t v2);
	uint64_t Div(uint64_t v1, uint64_t v2);
	uint64_t SDiv(uint64_t v1, uint64_t v2);
	uint64_t Mod(uint64_t v1, uint64_t v2);
	uint64_t SMod(uint64_t v1, uint64_t v2);
	uint64_t Exp(uint64_t v1, uint64_t v2);
	uint64_t Neg(uint64_t v1);
	bool LowerThan(uint64_t v1, uint64_t v2);
	bool LowerEqual(uint64_t v1, uint64_t v2);
	bool GreaterThan(uint64_t v1, uint64_t v2);
	bool GreaterEqual(uint64_t v1, uint64_t v2);
	bool Equal(uint64_t v1, uint64_t v2);
	uint64_t Not(uint64_t v1);
	void Push(uint64_t v1);
	uint64_t Pop();
private:
	std::vector<uint8_t> bytecode_;
	char stack_[0x1000000]; //16 MB
	uint64_t registers_[10 + 3];
};


