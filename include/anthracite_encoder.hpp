#ifndef DEF_ANTHRACITE_ENCODER
#define DEF_ANTHRACITE_ENCODER

#include <anthracite.hpp>
#include <anthracite_solver.hpp>

namespace ANTHRACITE {

	struct	SYM;
	struct	INST;

	struct	ENCODED;

	const SWORD s_GLB 		= -1;
	const SWORD s_IN 		= -2;
	const SWORD s_OUT 		= -3;
	const SWORD s_ERR 		= -4;

	struct	SYM {

		TOKEN				token;
		SWORD				section;
		DWORD				offset;
		DWORD				roffset;

		SYM(TOKEN i_token = TOKEN(), SWORD i_section = s_ERR, DWORD i_offset = 0, DWORD i_roffset = 0) : token(i_token), section(i_section), offset(i_offset), roffset(i_roffset) {}
		SYM(const SYM& i_symbol) : token(i_symbol.token), section(i_symbol.section), offset(i_symbol.offset), roffset(i_symbol.roffset) {}

	};

	struct	INST {

		SWORD				section;
		LIST<SYM>			defSymList;
		STRING				binary;
		SYM					reqSym;
		STRING				binary8;
		SYM					reqSym8;

		INST(SWORD i_section = s_ERR, LIST<SYM> i_defSymList = LIST<SYM>(), STRING i_binary = STRING(), SYM i_reqSym = SYM(), STRING i_binary8 = STRING(), SYM i_reqSym8 = SYM()) : section(i_section), defSymList(i_defSymList), binary(i_binary), reqSym(i_reqSym), binary8(i_binary8), reqSym8(i_reqSym8) {}
		INST(const INST& i_inst) : section(i_inst.section), defSymList(i_inst.defSymList), binary(i_inst.binary), reqSym(i_inst.reqSym), binary8(i_inst.binary8), reqSym8(i_inst.reqSym8) {}

	};

	struct	ENCODED {

		public :

			SOLVED*				solved;
			LIST<INST>			instList;

			ENCODED() {};
			ENCODED(SOLVED& i_solved);

			STRING	string();

		private :

			STRING resize(STRING& i_binary, DWORD i_size);
			DWORD size(STRING& i_binary);
			BOOL isNeg(STRING& i_binary);

			BYTE reg(ARGV* i_r);

			STRING OREX_00R(BYTE i_r3, BYTE i_v = 0);
			STRING OREX_0R0(BYTE i_r2, BYTE i_v = 0);
			STRING OREX_R00(BYTE i_r1, BYTE i_v = 0);
			STRING OREX_0RR(BYTE i_r2, BYTE i_r3, BYTE i_v = 0);
			STRING OREX_R0R(BYTE i_r1, BYTE i_r3, BYTE i_v = 0);
			STRING OREX_RR0(BYTE i_r1, BYTE i_r2, BYTE i_v = 0);
			STRING OREX_RRR(BYTE i_r1, BYTE i_r2, BYTE i_r3, BYTE i_v = 0);
			STRING REX_000(BYTE i_v = 0);
			STRING REX_00R(BYTE i_r3, BYTE i_v = 0);
			STRING REX_0R0(BYTE i_r2, BYTE i_v = 0);
			STRING REX_R00(BYTE i_r1, BYTE i_v = 0);
			STRING REX_0RR(BYTE i_r2, BYTE i_r3, BYTE i_v = 0);
			STRING REX_R0R(BYTE i_r1, BYTE i_r3, BYTE i_v = 0);
			STRING REX_RR0(BYTE i_r1, BYTE i_r2, BYTE i_v = 0);
			STRING REX_RRR(BYTE i_r1, BYTE i_r2, BYTE i_r3, BYTE i_v = 0);
			STRING ARG_00(BYTE i_v = 0);
			STRING ARG_0R(BYTE i_r2, BYTE i_v = 0);
			STRING ARG_R0(BYTE i_r1, BYTE i_v = 0);
			STRING ARG_RR(BYTE i_r1, BYTE i_r2, BYTE i_v = 0);
			
			STRING M(LINE& i_line, BYTE i_mode, ARGV* i_m, BYTE i_op1, BYTE i_op2 = 0x00, BOOL i_sec = false);
			STRING MR(LINE& i_line, BYTE i_mode, ARGV* i_m, ARGV* i_r, BYTE i_op1, BYTE i_op2 = 0x00, BOOL i_sec = false);
			STRING R(LINE& i_line, BYTE i_mode, ARGV* i_r, BYTE i_op1, BYTE i_op2 = 0x00, BOOL i_sec = false);
			STRING RR(LINE& i_line, BYTE i_mode, ARGV* i_r1, ARGV* i_r2, BYTE i_op1, BYTE i_op2 = 0x00, BOOL i_sec = false);

			INST NOP(LINE& i_line);

			INST ALTER(LINE& i_line, BYTE i_mode, BYTE i_op1, BYTE i_op2);
			INST SHIFT(LINE& i_line, BYTE i_mode, BYTE i_op1);
			INST SHIFTD(LINE& i_line, BYTE i_mode, BYTE i_op1);
			INST BT(LINE& i_line, BYTE i_mode, BYTE i_op1);
			INST BS(LINE& i_line, BYTE i_mode, BYTE i_op1);
			INST CNT(LINE& i_line, BYTE i_mode, BYTE i_op1);
			INST ARITH(LINE& i_line, BYTE i_mode, BYTE i_op1);
			INST MUL(LINE& i_line, BYTE i_mode, BYTE i_op2);
			INST PUSH(LINE& i_line, BYTE i_mode);
			INST POP(LINE& i_line, BYTE i_mode);
			
			INST JUMP(LINE& i_line);
			INST CALL(LINE& i_line);
			INST RETN(LINE& i_line);
			INST CJUMP(LINE& i_line, BYTE i_op1);
			INST CSET(LINE& i_line, BYTE i_op1);
			INST CMOV(LINE& i_line, BYTE i_op1, BYTE i_mode);

			INST TEST(LINE& i_line, BYTE i_mode);
			INST MOV(LINE& i_line, BYTE i_mode);
			INST MOVX(LINE& i_line, BYTE i_mode1, BYTE i_mode2, BYTE i_op1);
			INST XCHG(LINE& i_line, BYTE i_mode);

			INST LEA(LINE& i_line);

			INST FENTER(LINE& i_line);
			INST FLEAVE(LINE& i_line);
			INST FGROW(LINE& i_line);
			INST FSHRINK(LINE& i_line);
			INST FALIGN(LINE& i_line);

			INST FILE(LINE& i_line);
			INST SPACE(LINE& i_line);
			INST STR(LINE& i_line);
			INST B(LINE& i_line);
			INST W(LINE& i_line);
			INST D(LINE& i_line);
			INST Q(LINE& i_line);
			
	};

}

#include <anthracite_encoder.cpp>

#endif