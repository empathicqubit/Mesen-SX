#include "stdafx.h"
#include "Debugger.h"
#include "MemoryManager.h"
#include "Ppu.h"
#include "MemoryDumper.h"
#include "BaseCartridge.h"
#include "VideoDecoder.h"
#include "DebugTypes.h"

MemoryDumper::MemoryDumper(shared_ptr<Ppu> ppu, shared_ptr<MemoryManager> memoryManager, shared_ptr<BaseCartridge> cartridge)
{
	_ppu = ppu;
	_memoryManager = memoryManager;
	_cartridge = cartridge;
}

void MemoryDumper::SetMemoryState(SnesMemoryType type, uint8_t *buffer, uint32_t length)
{
	if(length > GetMemorySize(type)) {
		return;
	}

	switch(type) {
		case SnesMemoryType::CpuMemory:
			break;
		
		case SnesMemoryType::PrgRom: memcpy(_cartridge->DebugGetPrgRom(), buffer, length); break;
		case SnesMemoryType::WorkRam: memcpy(_memoryManager->DebugGetWorkRam(), buffer, length); break;
		case SnesMemoryType::SaveRam: memcpy(_cartridge->DebugGetSaveRam(), buffer, length); break;
		case SnesMemoryType::VideoRam: memcpy(_ppu->GetVideoRam(), buffer, length); break;
		case SnesMemoryType::SpriteRam: memcpy(_ppu->GetSpriteRam(), buffer, length); break;
		case SnesMemoryType::CGRam: memcpy(_ppu->GetCgRam(), buffer, length); break;
	}
}

uint32_t MemoryDumper::GetMemorySize(SnesMemoryType type)
{
	switch(type) {
		case SnesMemoryType::CpuMemory: return 0x1000000;
		case SnesMemoryType::PrgRom: return _cartridge->DebugGetPrgRomSize();
		case SnesMemoryType::WorkRam: return MemoryManager::WorkRamSize;
		case SnesMemoryType::SaveRam: return _cartridge->DebugGetSaveRamSize();
		case SnesMemoryType::VideoRam: return Ppu::VideoRamSize;
		case SnesMemoryType::SpriteRam: return Ppu::SpriteRamSize;
		case SnesMemoryType::CGRam: return Ppu::CgRamSize;
	}
	return 0;
}

void MemoryDumper::GetMemoryState(SnesMemoryType type, uint8_t *buffer)
{
	switch(type) {
		case SnesMemoryType::CpuMemory:
			for(int i = 0; i <= 0xFFFFFF; i++) {
				buffer[i] = _memoryManager->Peek(i);
			}
			break;

		case SnesMemoryType::PrgRom: memcpy(buffer, _cartridge->DebugGetPrgRom(), _cartridge->DebugGetPrgRomSize()); break;
		case SnesMemoryType::WorkRam: memcpy(buffer, _memoryManager->DebugGetWorkRam(), MemoryManager::WorkRamSize); break;
		case SnesMemoryType::SaveRam: memcpy(buffer, _cartridge->DebugGetSaveRam(), _cartridge->DebugGetSaveRamSize()); break;
		case SnesMemoryType::VideoRam: memcpy(buffer, _ppu->GetVideoRam(), Ppu::VideoRamSize);	break;
		case SnesMemoryType::SpriteRam: memcpy(buffer, _ppu->GetSpriteRam(), Ppu::SpriteRamSize);	break;
		case SnesMemoryType::CGRam: memcpy(buffer, _ppu->GetCgRam(), Ppu::CgRamSize); break;
	}
}

void MemoryDumper::SetMemoryValues(SnesMemoryType memoryType, uint32_t address, uint8_t* data, uint32_t length)
{
	for(uint32_t i = 0; i < length; i++) {
		SetMemoryValue(memoryType, address+i, data[i], true);
	}
}

void MemoryDumper::SetMemoryValue(SnesMemoryType memoryType, uint32_t address, uint8_t value, bool disableSideEffects)
{
	if(address >= GetMemorySize(memoryType)) {
		return;
	}

	switch(memoryType) {
		case SnesMemoryType::CpuMemory: _memoryManager->Write(address, value, MemoryOperationType::Write); break;

		case SnesMemoryType::PrgRom: _cartridge->DebugGetPrgRom()[address] = value; break;
		case SnesMemoryType::WorkRam: _memoryManager->DebugGetWorkRam()[address] = value; break;
		case SnesMemoryType::SaveRam: _cartridge->DebugGetSaveRam()[address] = value; break;

		case SnesMemoryType::VideoRam: _ppu->GetVideoRam()[address & (Ppu::VideoRamSize - 1)] = value;
		case SnesMemoryType::SpriteRam: _ppu->GetSpriteRam()[address % Ppu::SpriteRamSize] = value; break;
		case SnesMemoryType::CGRam: _ppu->GetCgRam()[address & (Ppu::CgRamSize - 1)] = value; break;
	}
}

uint8_t MemoryDumper::GetMemoryValue(SnesMemoryType memoryType, uint32_t address, bool disableSideEffects)
{
	if(address >= GetMemorySize(memoryType)) {
		return 0;
	}

	switch(memoryType) {
		case SnesMemoryType::CpuMemory: return _memoryManager->Peek(address);

		case SnesMemoryType::PrgRom: return _cartridge->DebugGetPrgRom()[address];
		case SnesMemoryType::WorkRam: return  _memoryManager->DebugGetWorkRam()[address];
		case SnesMemoryType::SaveRam: return _cartridge->DebugGetSaveRam()[address];

		case SnesMemoryType::VideoRam: return _ppu->GetVideoRam()[address & (Ppu::VideoRamSize - 1)];
		case SnesMemoryType::SpriteRam: return _ppu->GetSpriteRam()[address % Ppu::SpriteRamSize];
		case SnesMemoryType::CGRam: return _ppu->GetCgRam()[address & (Ppu::CgRamSize - 1)];
	}

	return 0;
}