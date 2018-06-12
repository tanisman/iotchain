#include "vm.h"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <thread>
#include <iterator>
#include <string.h>
#if defined (BUILD_RP3)
#include <wiringPi.h>
#endif //BUILD_RP3

MCLVirtualMachine::MCLVirtualMachine(const std::vector<uint8_t>& bytecode)
	: bytecode_(bytecode)
{
#if defined (BUILD_RP3)
	static bool wiring_pi_setupped = false;
	if (!wiring_pi_setupped)
	{
		wiringPiSetup();
		wiring_pi_setupped = true;
	}
#endif //BUILD_RP3

	memset(registers_, 0, sizeof(registers_));
	registers_[SP] = sizeof(stack_);
}

MCLVirtualMachine::MCLVirtualMachine(const std::vector<char>& bytecode)
{
#if defined (BUILD_RP3)
	static bool wiring_pi_setuppedx = false;
	if (!wiring_pi_setuppedx)
	{
		wiringPiSetup();
		wiring_pi_setuppedx = true;
	}
#endif //BUILD_RP3

	std::copy(bytecode.begin(), bytecode.end(), std::back_inserter(bytecode_));
	memset(registers_, 0, sizeof(registers_));
	registers_[SP] = sizeof(stack_);
}


void MCLVirtualMachine::PrintRegisters()
{
	std::cout << "R0: 0x" << std::setfill('0') << std::setw(16) << std::hex << registers_[R0] << std::endl;
	std::cout << "R1: 0x" << std::setfill('0') << std::setw(16) << std::hex << registers_[R1] << std::endl;
	std::cout << "R2: 0x" << std::setfill('0') << std::setw(16) << std::hex << registers_[R2] << std::endl;
	std::cout << "R3: 0x" << std::setfill('0') << std::setw(16) << std::hex << registers_[R3] << std::endl;
	std::cout << "R4: 0x" << std::setfill('0') << std::setw(16) << std::hex << registers_[R4] << std::endl;
	std::cout << "R5: 0x" << std::setfill('0') << std::setw(16) << std::hex << registers_[R5] << std::endl;
	std::cout << "R6: 0x" << std::setfill('0') << std::setw(16) << std::hex << registers_[R6] << std::endl;
	std::cout << "R7: 0x" << std::setfill('0') << std::setw(16) << std::hex << registers_[R7] << std::endl;
	std::cout << "R8: 0x" << std::setfill('0') << std::setw(16) << std::hex << registers_[R8] << std::endl;
	std::cout << "R9: 0x" << std::setfill('0') << std::setw(16) << std::hex << registers_[R9] << std::endl;
	std::cout << "IP: 0x" << std::setfill('0') << std::setw(16) << std::hex << registers_[IP] << std::endl;
	std::cout << "SP: 0x" << std::setfill('0') << std::setw(16) << std::hex << registers_[SP] << std::endl;
	std::cout << "BP: 0x" << std::setfill('0') << std::setw(16) << std::hex << registers_[BP] << std::endl;
}

void MCLVirtualMachine::PrintStack()
{
	hex_dump(std::cout, stack_ + sizeof(stack_), 1024) << std::endl;
}

void MCLVirtualMachine::Execute()
{
	uint8_t opcode;
	do
	{
		bool op1, op2;
		opcode = this->ExtractOpcode(op1, op2);
		switch (opcode)
		{
		case STOP:
		{

		}
		break;
		case ADD:
		{
			uint64_t v1, v2;
			if (!op1)
				v1 = registers_[ExtractRegister()];
			else
				v1 = ExtractConstant();

			if (!op2)
				v2 = registers_[ExtractRegister()];
			else
				v2 = ExtractConstant();

			registers_[ExtractRegister()] = Add(v1, v2);
		}
		break;
		case SUB:
		{
			uint64_t v1, v2;
			if (!op1)
				v1 = registers_[ExtractRegister()];
			else
				v1 = ExtractConstant();

			if (!op2)
				v2 = registers_[ExtractRegister()];
			else
				v2 = ExtractConstant();

			registers_[ExtractRegister()] = Sub(v1, v2);

		}
		break;
		case MUL:
		{
			uint64_t v1, v2;
			if (!op1)
				v1 = registers_[ExtractRegister()];
			else
				v1 = ExtractConstant();

			if (!op2)
				v2 = registers_[ExtractRegister()];
			else
				v2 = ExtractConstant();

			registers_[ExtractRegister()] = Mul(v1, v2);
		}
		break;
		case SMUL:
		{
			uint64_t v1, v2;
			if (!op1)
				v1 = registers_[ExtractRegister()];
			else
				v1 = ExtractConstant();

			if (!op2)
				v2 = registers_[ExtractRegister()];
			else
				v2 = ExtractConstant();

			registers_[ExtractRegister()] = SMul(v1, v2);
		}
		break;
		case DIV:
		{
			uint64_t v1, v2;
			if (!op1)
				v1 = registers_[ExtractRegister()];
			else
				v1 = ExtractConstant();

			if (!op2)
				v2 = registers_[ExtractRegister()];
			else
				v2 = ExtractConstant();

			registers_[ExtractRegister()] = Div(v1, v2);
		}
		break;
		case SDIV:
		{
			uint64_t v1, v2;
			if (!op1)
				v1 = registers_[ExtractRegister()];
			else
				v1 = ExtractConstant();

			if (!op2)
				v2 = registers_[ExtractRegister()];
			else
				v2 = ExtractConstant();

			registers_[ExtractRegister()] = SDiv(v1, v2);
		}
		break;
		case MOD:
		{
			uint64_t v1, v2;
			if (!op1)
				v1 = registers_[ExtractRegister()];
			else
				v1 = ExtractConstant();

			if (!op2)
				v2 = registers_[ExtractRegister()];
			else
				v2 = ExtractConstant();

			registers_[ExtractRegister()] = Mod(v1, v2);
		}
		break;
		case SMOD:
		{
			uint64_t v1, v2;
			if (!op1)
				v1 = registers_[ExtractRegister()];
			else
				v1 = ExtractConstant();

			if (!op2)
				v2 = registers_[ExtractRegister()];
			else
				v2 = ExtractConstant();

			registers_[ExtractRegister()] = SMod(v1, v2);
		}
		break;
		case EXP:
		{
			uint64_t v1, v2;
			if (!op1)
				v1 = registers_[ExtractRegister()];
			else
				v1 = ExtractConstant();

			if (!op2)
				v2 = registers_[ExtractRegister()];
			else
				v2 = ExtractConstant();

			registers_[ExtractRegister()] = Exp(v1, v2);
		}
		break;
		case NEG:
		{
			uint64_t v1;
			if (!op1)
				v1 = registers_[ExtractRegister()];
			else
				v1 = ExtractConstant();

			registers_[ExtractRegister()] = Neg(v1);
		}
		break;
		case LT:
		{
			uint64_t v1, v2;
			if (!op1)
				v1 = registers_[ExtractRegister()];
			else
				v1 = ExtractConstant();

			if (!op2)
				v2 = registers_[ExtractRegister()];
			else
				v2 = ExtractConstant();

			uint32_t address = ExtractAddress();
			if (LowerThan(v1, v2))
				registers_[IP] = address;
		}
		break;
		case LE:
		{
			uint64_t v1, v2;
			if (!op1)
				v1 = registers_[ExtractRegister()];
			else
				v1 = ExtractConstant();

			if (!op2)
				v2 = registers_[ExtractRegister()];
			else
				v2 = ExtractConstant();

			uint32_t address = ExtractAddress();
			if (LowerEqual(v1, v2))
				registers_[IP] = address;
		}
		break;
		case GT:
		{
			uint64_t v1, v2;
			if (!op1)
				v1 = registers_[ExtractRegister()];
			else
				v1 = ExtractConstant();

			if (!op2)
				v2 = registers_[ExtractRegister()];
			else
				v2 = ExtractConstant();

			uint32_t address = ExtractAddress();
			if (GreaterThan(v1, v2))
				registers_[IP] = address;
		}
		break;
		case GE:
		{
			uint64_t v1, v2;
			if (!op1)
				v1 = registers_[ExtractRegister()];
			else
				v1 = ExtractConstant();

			if (!op2)
				v2 = registers_[ExtractRegister()];
			else
				v2 = ExtractConstant();

			uint32_t address = ExtractAddress();
			if (GreaterEqual(v1, v2))
				registers_[IP] = address;
		}
		break;
		case EQ:
		{
			uint64_t v1, v2;
			if (!op1)
				v1 = registers_[ExtractRegister()];
			else
				v1 = ExtractConstant();

			if (!op2)
				v2 = registers_[ExtractRegister()];
			else
				v2 = ExtractConstant();

			uint32_t address = ExtractAddress();
			if (Equal(v1, v2))
				registers_[IP] = address;
		}
		break;
		case NOT:
		{
			uint64_t v1;
			if (!op1)
				v1 = registers_[ExtractRegister()];
			else
				v1 = ExtractConstant();

			registers_[ExtractRegister()] = Not(v1);
		}
		break;
		case STORE:
		{
			Register r = ExtractRegister();
			uint32_t address;
			if (!op2)
				address = registers_[ExtractRegister()];
			else
				address = ExtractAddress();

			*reinterpret_cast<uint64_t*>(stack_ + address) = registers_[r];
		}
		break;
		case LOAD:
		{
			Register r = ExtractRegister();
			uint32_t address;
			if (!op2)
				address = registers_[ExtractRegister()];
			else
				address = ExtractAddress();

			registers_[r] = *reinterpret_cast<uint64_t*>(stack_ + address);
		}
		break;
		case JMP:
		{
			uint32_t address;
			if (!op1) //read from register
				address = registers_[ExtractRegister()];
			else //read address
				address = ExtractAddress();

			if (!op2) //relative address
				address += registers_[IP] - 1;

			registers_[IP] = address;
		}
		break;
		case CALL:
		{
			uint32_t address;
			if (!op1) //read from register
				address = registers_[ExtractRegister()];
			else //read address
				address = ExtractAddress();

			if (!op2) //relative address
				address += registers_[IP] - 1;

			uint64_t retn_size = ExtractConstant();

			Push(retn_size);
			Push(registers_[IP]);

			registers_[IP] = address;
		}
		break;
		case RET:
		{
			uint32_t address = Pop();
			uint32_t retn_size = Pop();

			registers_[SP] += retn_size;
			registers_[IP] = address;
		}
		break;
		case PUSH:
		{
			uint64_t v1;
			if (!op1)
				v1 = registers_[ExtractRegister()];
			else
				v1 = ExtractConstant();

			Push(v1);
		}
		break;
		case POP:
		{
			registers_[ExtractRegister()] = Pop();
		}
		break;
		case MOV:
		{
			uint64_t v2;
			Register reg = ExtractRegister();

			if (!op2)
				v2 = registers_[ExtractRegister()];
			else
				v2 = ExtractConstant();

			registers_[reg] = v2;
		}
		break;
		case SYSCALL:
		{
			switch ((SYSCALL_TABLE)registers_[R0])
			{
			case SYSCALL_TABLE::Sleep:
				std::this_thread::sleep_for(std::chrono::milliseconds(registers_[R1]));
				break;
#if defined (BUILD_RP3)
			case WPort:
				pinMode(registers_[R1], OUTPUT);
				digitalWrite(registers_[R1], registers_[R2]);
				break;
			case RPort:
				pinMode(registers_[R1], INPUT);
				registers_[R2] = digitalRead(registers_[R1]);
				break;
#endif //BUILD_RP3
			case SYSCALL_TABLE::Stout:
			{
				uint32_t size = (registers_[R1] - 1) / 8 + 1;
				for (int i = 0; i < size; i++)
				{
					uint64_t val = Pop();
					for (int j = 0; j < (i == size - 1 ? registers_[R1] % 8 : 8); j++)
						putchar(*(reinterpret_cast<char*>(&val) + j));
				}
			}
			break;
			}
		}
		break;
		default:
		{
			std::cout << "Illegal instruction: 0x" << std::setfill('0') << std::setw(2) << std::hex << opcode << std::endl;
		}
		break;
		}

	} while (opcode != STOP);
}

Opcode MCLVirtualMachine::ExtractOpcode(bool& op1, bool& op2)
{
	uint8_t opc = bytecode_[registers_[IP]++];
	op1 = opc & 1;
	op2 = opc & 2;
	return static_cast<Opcode>(opc >> 2);
}

Register MCLVirtualMachine::ExtractRegister()
{
	return static_cast<Register>(bytecode_[registers_[IP]++]);
}

uint32_t MCLVirtualMachine::ExtractAddress()
{
	uint32_t a1 = bytecode_[registers_[IP]++];
	uint32_t a2 = bytecode_[registers_[IP]++] << 8;
	uint32_t a3 = bytecode_[registers_[IP]++] << 16;

	return a1 | a2 | a3;
}

uint64_t MCLVirtualMachine::ExtractConstant()
{
	uint64_t val = *reinterpret_cast<uint64_t*>(bytecode_.data() + registers_[IP]);
	registers_[IP] += 8;
	return val;
}

uint64_t MCLVirtualMachine::Add(uint64_t v1, uint64_t v2)
{
	return v1 + v2;
}

uint64_t MCLVirtualMachine::Sub(uint64_t v1, uint64_t v2)
{
	return v1 - v2;
}

uint64_t MCLVirtualMachine::Mul(uint64_t v1, uint64_t v2)
{
	return v1 * v2;
}

uint64_t MCLVirtualMachine::SMul(uint64_t v1, uint64_t v2)
{
	return static_cast<int64_t>(v1) * static_cast<int64_t>(v2);
}

uint64_t MCLVirtualMachine::Div(uint64_t v1, uint64_t v2)
{
	return v1 / v2;
}

uint64_t MCLVirtualMachine::SDiv(uint64_t v1, uint64_t v2)
{
	return static_cast<int64_t>(v1) / static_cast<int64_t>(v2);
}

uint64_t MCLVirtualMachine::Mod(uint64_t v1, uint64_t v2)
{
	return v1 % v2;
}

uint64_t MCLVirtualMachine::SMod(uint64_t v1, uint64_t v2)
{
	return static_cast<int64_t>(v1) % static_cast<int64_t>(v2);
}

uint64_t MCLVirtualMachine::Exp(uint64_t v1, uint64_t v2)
{
	int temp;
	if (v2 == 0)
		return 1;

	temp = Exp(v1, v2 / 2);
	if ((v2 % 2) == 0)
		return temp * temp;
	else
		return v1 * temp * temp;
}

uint64_t MCLVirtualMachine::Neg(uint64_t v1)
{
	return static_cast<int64_t>(v1) * -1;
}
bool MCLVirtualMachine::LowerThan(uint64_t v1, uint64_t v2)
{
	return v1 < v2;
}

bool MCLVirtualMachine::LowerEqual(uint64_t v1, uint64_t v2)
{
	return v1 <= v2;
}

bool MCLVirtualMachine::GreaterThan(uint64_t v1, uint64_t v2)
{
	return v1 > v2;
}

bool MCLVirtualMachine::GreaterEqual(uint64_t v1, uint64_t v2)
{
	return v1 >= v2;
}

bool MCLVirtualMachine::Equal(uint64_t v1, uint64_t v2)
{
	return v1 == v2;
}

uint64_t MCLVirtualMachine::Not(uint64_t v1)
{
	return ~v1;
}

void MCLVirtualMachine::Push(uint64_t v1)
{
	registers_[SP] -= 8;
	*reinterpret_cast<uint64_t*>(stack_ + registers_[SP]) = v1;
}
uint64_t MCLVirtualMachine::Pop()
{
	uint64_t value = *reinterpret_cast<uint64_t*>(stack_ + registers_[SP]);
	registers_[SP] += 8;
	return value;
}


MCLVirtualMachine::~MCLVirtualMachine()
{
}

