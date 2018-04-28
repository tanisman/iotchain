#ifndef MCL_VM_H
#define MCL_VM_H
#include <ChainThings.h>
#include <cstdint>
#include <vector>

#undef OVERFLOW

_CHAIN_THINGS_BEGIN
_NAMESPACE_BEGIN(VM)

enum Opcode : uint8_t
{
	HALT = 0,
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

class MCLVirtualMachine
{
public:
	MCLVirtualMachine(const std::vector<uint8_t>& bytecode);
	~MCLVirtualMachine();
	void Execute();
	void PrintRegisters();
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

_NAMESPACE_END(VM)
_CHAIN_THINGS_END

#endif //MCL_VM_H