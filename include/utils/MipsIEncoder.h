/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

// MIPS-I instruction encoder (Don't need a full assembler :)).
//
// "Stolen" from https://github.com/brad-lin/FreePSXBoot/blob/master/builder/stage2/common/util/encoder.hh
// which subsequently was taken from PCSX-R.
//

#ifndef ELFLDR_MIPSIENCODER_H
#define ELFLDR_MIPSIENCODER_H

#include <stdint.h>

namespace elfldr::mips {

	// clang-format off

	/**
	 * Registers.
	 */
	enum class Reg {
		R0, AT, V0, V1, A0, A1, A2, A3,  // 00 to 07
		T0, T1, T2, T3, T4, T5, T6, T7,  // 08 to 0f
		S0, S1, S2, S3, S4, S5, S6, S7,  // 10 to 17
		T8, T9, K0, K1, GP, SP, S8, RA,  // 18 to 1f
	};
	// clang-format on

	// Encoding helpers

	constexpr uint32_t iclass(uint32_t v) {
		return v << 26;
	}

	constexpr uint32_t dstVal(Reg r) {
		return uint32_t(r) << 11;
	}

	constexpr uint32_t tgtVal(Reg r) {
		return uint32_t(r) << 16;
	}

	constexpr uint32_t srcVal(Reg r) {
		return uint32_t(r) << 21;
	}

	// ALU
	constexpr uint32_t add(Reg dst, Reg src, Reg tgt) {
		return dstVal(dst) | tgtVal(tgt) | srcVal(src) | 0b100000;
	}

	constexpr uint32_t addu(Reg dst, Reg src, Reg tgt) {
		return dstVal(dst) | tgtVal(tgt) | srcVal(src) | 0b100001;
	}

	constexpr uint32_t addi(Reg tgt, Reg src, int16_t value) {
		uint32_t v = value;
		v &= 0xffff;
		return iclass(0b001000) | srcVal(src) | tgtVal(tgt) | v;
	}

	constexpr uint32_t addiu(Reg tgt, Reg src, int16_t value) {
		uint32_t v = value;
		v &= 0xffff;
		return iclass(0b001001) | srcVal(src) | tgtVal(tgt) | v;
	}

	constexpr uint32_t andd(Reg dst, Reg src, Reg tgt) {
		return dstVal(dst) | tgtVal(tgt) | srcVal(src) | 0b100100;
	}

	constexpr uint32_t andi(Reg tgt, Reg src, uint16_t value) {
		return iclass(0b001100) | srcVal(src) | tgtVal(tgt) | value;
	}

	constexpr uint32_t lui(Reg tgt, uint16_t value) {
		return iclass(0b001111) | tgtVal(tgt) | value;
	}

	constexpr uint32_t nor(Reg dst, Reg src, Reg tgt) {
		return dstVal(dst) | tgtVal(tgt) | srcVal(src) | 0b100111;
	}

	constexpr uint32_t orr(Reg dst, Reg src, Reg tgt) {
		return dstVal(dst) | tgtVal(tgt) | srcVal(src) | 0b100101;
	}

	constexpr uint32_t ori(Reg tgt, Reg src, uint16_t value) {
		return iclass(0b001101) | srcVal(src) | tgtVal(tgt) | value;
	}

	constexpr uint32_t slt(Reg dst, Reg src, Reg tgt) {
		return dstVal(dst) | tgtVal(tgt) | srcVal(src) | 0b101010;
	}

	constexpr uint32_t sltu(Reg dst, Reg src, Reg tgt) {
		return dstVal(dst) | tgtVal(tgt) | srcVal(src) | 0b101011;
	}

	constexpr uint32_t slti(Reg tgt, Reg src, int16_t value) {
		uint32_t v = value;
		v &= 0xffff;
		return iclass(0b001010) | srcVal(src) | tgtVal(tgt) | v;
	}

	constexpr uint32_t sltiu(Reg tgt, Reg src, uint16_t value) {
		return iclass(0b001011) | srcVal(src) | tgtVal(tgt) | value;
	}

	constexpr uint32_t sub(Reg dst, Reg src, Reg tgt) {
		return dstVal(dst) | tgtVal(tgt) | srcVal(src) | 0b100010;
	}

	constexpr uint32_t subu(Reg dst, Reg src, Reg tgt) {
		return dstVal(dst) | tgtVal(tgt) | srcVal(src) | 0b100011;
	}

	constexpr uint32_t xorr(Reg dst, Reg src, Reg tgt) {
		return dstVal(dst) | tgtVal(tgt) | srcVal(src) | 0b100110;
	}

	constexpr uint32_t xori(Reg tgt, Reg src, uint16_t value) {
		return iclass(0b001110) | srcVal(src) | tgtVal(tgt) | value;
	}

	// shifts
	constexpr uint32_t sll(Reg dst, Reg tgt, uint16_t sa) {
		return dstVal(dst) | tgtVal(tgt) | (sa << 6) | 0b000000;
	}

	constexpr uint32_t sllv(Reg dst, Reg tgt, Reg src) {
		return dstVal(dst) | tgtVal(tgt) | srcVal(src) | 0b000100;
	}

	constexpr uint32_t sra(Reg dst, Reg tgt, uint16_t sa) {
		return dstVal(dst) | tgtVal(tgt) | (sa << 6) | 0b000011;
	}

	constexpr uint32_t srav(Reg dst, Reg tgt, Reg src) {
		return dstVal(dst) | tgtVal(tgt) | srcVal(src) | 0b000111;
	}

	constexpr uint32_t srl(Reg dst, Reg tgt, uint16_t sa) {
		return dstVal(dst) | tgtVal(tgt) | (sa << 6) | 0b000010;
	}

	constexpr uint32_t srlv(Reg dst, Reg tgt, Reg src) {
		return dstVal(dst) | tgtVal(tgt) | srcVal(src) | 0b000110;
	}

	// Div/Mul

	constexpr uint32_t div(Reg src, Reg tgt) {
		return tgtVal(tgt) | srcVal(src) | 0b011010;
	}

	constexpr uint32_t divu(Reg src, Reg tgt) {
		return tgtVal(tgt) | srcVal(src) | 0b011011;
	}

	constexpr uint32_t mfhi(Reg dst) {
		return dstVal(dst) | 0b010000;
	}

	constexpr uint32_t mflo(Reg dst) {
		return dstVal(dst) | 0b010010;
	}

	constexpr uint32_t mthi(Reg dst) {
		return dstVal(dst) | 0b010001;
	}

	constexpr uint32_t mtlo(Reg dst) {
		return dstVal(dst) | 0b010011;
	}

	constexpr uint32_t mult(Reg src, Reg tgt) {
		return tgtVal(tgt) | srcVal(src) | 0b011000;
	}

	constexpr uint32_t multu(Reg src, Reg tgt) {
		return tgtVal(tgt) | srcVal(src) | 0b011001;
	}

	// Branching

	constexpr uint32_t beq(Reg src, Reg tgt, int16_t offset) {
		uint32_t o = offset >> 2;
		o &= 0xffff;
		return iclass(0b000100) | tgtVal(tgt) | srcVal(src) | o;
	}

	constexpr uint32_t bgez(Reg src, int16_t offset) {
		uint32_t o = offset >> 2;
		o &= 0xffff;
		return iclass(0b000001) | tgtVal(Reg(0b00001)) | srcVal(src) | o;
	}
	constexpr uint32_t bgezal(Reg src, int16_t offset) {
		uint32_t o = offset >> 2;
		o &= 0xffff;
		return iclass(0b000001) | tgtVal(Reg(0b10001)) | srcVal(src) | o;
	}

	constexpr uint32_t bgtz(Reg src, int16_t offset) {
		uint32_t o = offset >> 2;
		o &= 0xffff;
		return iclass(0b000111) | tgtVal(Reg(0b00000)) | srcVal(src) | o;
	}

	constexpr uint32_t blez(Reg src, int16_t offset) {
		uint32_t o = offset >> 2;
		o &= 0xffff;
		return iclass(0b000110) | tgtVal(Reg(0b00000)) | srcVal(src) | o;
	}

	constexpr uint32_t bltz(Reg src, int16_t offset) {
		uint32_t o = offset >> 2;
		o &= 0xffff;
		return iclass(0b000001) | tgtVal(Reg(0b00000)) | srcVal(src) | o;
	}

	constexpr uint32_t bltzal(Reg src, int16_t offset) {
		uint32_t o = offset >> 2;
		o &= 0xffff;
		return iclass(0b000001) | tgtVal(Reg(0b10000)) | srcVal(src) | o;
	}

	constexpr uint32_t bne(Reg src, Reg tgt, int16_t offset) {
		uint32_t o = offset >> 2;
		o &= 0xffff;
		return iclass(0b000101) | tgtVal(tgt) | srcVal(src) | o;
	}

	constexpr uint32_t brk(uint32_t code) {
		return (code << 6) | 0b001101;
	}

	constexpr uint32_t j(uint32_t addr) {
		return iclass(0b000010) | ((addr >> 2) & 0x03ffffff);
	}

	constexpr uint32_t jal(uint32_t addr) {
		return iclass(0b000011) | ((addr >> 2) & 0x03ffffff);
	}

	constexpr uint32_t jalr(Reg src, Reg dst = Reg::RA) {
		return dstVal(dst) | srcVal(src) | 0b001001;
	}

	constexpr uint32_t jr(Reg src) {
		return srcVal(src) | 0b001000;
	}

	constexpr uint32_t syscall() {
		return 0b001100;
	}

	constexpr uint32_t rfe() {
		return 0x42000010;
	}

	// Memory load/store

	constexpr uint32_t lb(Reg tgt, int16_t offset, Reg src) {
		uint32_t o = offset;
		o &= 0xffff;
		return iclass(0b100000) | tgtVal(tgt) | srcVal(src) | o;
	}

	constexpr uint32_t lbu(Reg tgt, int16_t offset, Reg src) {
		uint32_t o = offset;
		o &= 0xffff;
		return iclass(0b100100) | tgtVal(tgt) | srcVal(src) | o;
	}

	constexpr uint32_t lh(Reg tgt, int16_t offset, Reg src) {
		uint32_t o = offset;
		o &= 0xffff;
		return iclass(0b100001) | tgtVal(tgt) | srcVal(src) | o;
	}

	constexpr uint32_t lhu(Reg tgt, int16_t offset, Reg src) {
		uint32_t o = offset;
		o &= 0xffff;
		return iclass(0b100101) | tgtVal(tgt) | srcVal(src) | o;
	}

	constexpr uint32_t lw(Reg tgt, int16_t offset, Reg src) {
		uint32_t o = offset;
		o &= 0xffff;
		return iclass(0b100011) | tgtVal(tgt) | srcVal(src) | o;
	}

	constexpr uint32_t lwl(Reg tgt, int16_t offset, Reg src) {
		uint32_t o = offset;
		o &= 0xffff;
		return iclass(0b100010) | tgtVal(tgt) | srcVal(src) | o;
	}

	constexpr uint32_t lwr(Reg tgt, int16_t offset, Reg src) {
		uint32_t o = offset;
		o &= 0xffff;
		return iclass(0b100110) | tgtVal(tgt) | srcVal(src) | o;
	}

	constexpr uint32_t sb(Reg tgt, int16_t offset, Reg src) {
		uint32_t o = offset;
		o &= 0xffff;
		return iclass(0b101000) | tgtVal(tgt) | srcVal(src) | o;
	}

	constexpr uint32_t sh(Reg tgt, int16_t offset, Reg src) {
		uint32_t o = offset;
		o &= 0xffff;
		return iclass(0b101001) | tgtVal(tgt) | srcVal(src) | o;
	}

	constexpr uint32_t sw(Reg tgt, int16_t offset, Reg src) {
		uint32_t o = offset;
		o &= 0xffff;
		return iclass(0b101011) | tgtVal(tgt) | srcVal(src) | o;
	}

	constexpr uint32_t swl(Reg tgt, int16_t offset, Reg src) {
		uint32_t o = offset;
		o &= 0xffff;
		return iclass(0b101010) | tgtVal(tgt) | srcVal(src) | o;
	}

	constexpr uint32_t swr(Reg tgt, int16_t offset, Reg src) {
		uint32_t o = offset;
		o &= 0xffff;
		return iclass(0b101110) | tgtVal(tgt) | srcVal(src) | o;
	}

	// NOP is 0x0 on MIPS.

	constexpr uint32_t nop() {
		return 0x0;
	}

} // namespace elfldr::mips

#endif // ELFLDR_MIPSIENCODER_H
