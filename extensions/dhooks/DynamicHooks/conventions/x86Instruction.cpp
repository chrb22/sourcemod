/**
* =============================================================================
* DynamicHooks
* Copyright (C) 2015 Robin Gohmert. All rights reserved.
* Copyright (C) 2018-2021 AlliedModders LLC.  All rights reserved.
* =============================================================================
*
* This software is provided 'as-is', without any express or implied warranty.
* In no event will the authors be held liable for any damages arising from 
* the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose, 
* including commercial applications, and to alter it and redistribute it 
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not 
* claim that you wrote the original software. If you use this software in a 
* product, an acknowledgment in the product documentation would be 
* appreciated but is not required.
*
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
*
* 3. This notice may not be removed or altered from any source distribution.
*
* asm.h/cpp from devmaster.net (thanks cybermind) edited by pRED* to handle gcc
* -fPIC thunks correctly
*
* Idea and trampoline code taken from DynDetours (thanks your-name-here).
*
* Adopted to provide similar features to SourceHook by AlliedModders LLC.
*/

// ============================================================================
// >> INCLUDES
// ============================================================================
#include "x86Instruction.h"
#include <string.h>


// ============================================================================
// >> x86Instruction
// ============================================================================
x86Instruction::x86Instruction(std::vector<DataTypeSized_t> &vecArgTypes, DataTypeSized_t returnType, int iAlignment) :
	ICallingConvention(vecArgTypes, returnType, iAlignment)
{
}

x86Instruction::~x86Instruction()
{
}

std::vector<Register_t> x86Instruction::GetRegisters()
{
	std::vector<Register_t> registers =
	{
		AL, CL, DL, BL, AH, CH, DH, BH,
		EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI,
		XMM0, XMM1, XMM2, XMM3, XMM4, XMM5, XMM6, XMM7,
		ST0
	};

	return registers;
}

int x86Instruction::GetPopSize()
{
	return 0;
}

int x86Instruction::GetArgStackSize()
{
	return 0;
}

void** x86Instruction::GetStackArgumentPtr(CRegisters* pRegisters)
{
	return NULL;
}

int x86Instruction::GetArgRegisterSize()
{
	int iArgRegisterSize = 0;

	for (size_t i = 0; i < m_vecArgTypes.size(); i++)
	{
		if (m_vecArgTypes[i].custom_register != None)
			iArgRegisterSize += m_vecArgTypes[i].size;
	}

	return iArgRegisterSize;
}

void* x86Instruction::GetArgumentPtr(unsigned int iIndex, CRegisters* pRegisters)
{
	if (iIndex >= m_vecArgTypes.size())
		return NULL;

	// Check if this argument wasn't passed in a register.
	if (m_vecArgTypes[iIndex].custom_register == None)
		return NULL;

	int iValue = m_vecArgTypes[iIndex].custom_register;
	if (Stack_Min <= iValue && iValue < Stack_Max) // range check in case the negative value is accidental
	{
		CRegister* pEBP = pRegisters->GetRegister(EBP);
		if (!pEBP)
			return NULL;

		return pEBP->GetValue<uint8_t*>() + (iValue - Stack);
	}

	CRegister *pRegister = pRegisters->GetRegister(m_vecArgTypes[iIndex].custom_register);
	if (!pRegister)
		return NULL;

	return pRegister->m_pAddress;
}

void x86Instruction::ArgumentPtrChanged(unsigned int iIndex, CRegisters* pRegisters, void* pArgumentPtr)
{
}

void* x86Instruction::GetReturnPtr(CRegisters* pRegisters)
{
	return NULL;
}

void x86Instruction::ReturnPtrChanged(CRegisters* pRegisters, void* pReturnPtr)
{
}