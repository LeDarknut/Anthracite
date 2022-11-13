#include <anthracite_encoder.hpp>

namespace ANTHRACITE {

	ENCODED::ENCODED(SOLVED &i_solved) : solved(&i_solved), instList() {

		/*
			Convert basic instruction to list of machine code instructions
		*/

		LIST<SWORD> f_sectionList;

		for (LINE* f1_line = solved->lineList.begin(); f1_line < solved->lineList.end(); f1_line++) {

			if (!f1_line->section.is()) {

				if (f1_line->section.content == "{") {

					instList += INST(s_IN);

				}
				else {

					instList += INST(s_OUT);

				}

				continue;

			}

			if (size(f1_line->section.value) > 1) {

				throw(traceToken(*solved->parsed->lexed, f1_line->section, "Section being to big."));

			}

			STRING f1_sectionValue = resize(f1_line->section.value, 1);

			SWORD f1_sectionId = 0;

			f1_sectionId |= DWORD(f1_sectionValue[0]);

			if (f_sectionList.size() == 0) {

				f_sectionList += f1_sectionId;

			}
			
			else for (SWORD* f2_section = f_sectionList.begin(); f2_section < f_sectionList.end(); f2_section ++) {
				
				if (f1_sectionId < *f2_section) {

					LIST<SWORD> f1_sectionList;

					for (SWORD* f3_section = f_sectionList.begin(); f3_section < f_sectionList.end(); f3_section ++) {
						
						if (f3_section == f2_section) {

							f1_sectionList += f1_sectionId;

						}

						f1_sectionList += *f3_section;

					}

					f_sectionList = f1_sectionList;

					break;
					
				}

				if (f1_sectionId == *f2_section) break;

				if (f2_section + 1 >= f_sectionList.end()) {

					f_sectionList += f1_sectionId;

				}

			}

			STRING f1_name = f1_line->name.content;
			BYTE f1_mode = 0;
			BYTE f1_mode2 = 0;
			BYTE f1_cond = 0x80;

			if (
				f1_name.size() > 2 &&
				f1_name[-2] == '_' &&
				(
					f1_name[-1] == 'B' ||
					f1_name[-1] == 'W' ||
					f1_name[-1] == 'D' ||
					f1_name[-1] == 'Q'
				)
			) {

				if (f1_name[-1] == 'B') f1_cond = 0x02;

				f1_mode = f1_name[-1];
				f1_name = f1_name.to(-2);

			}

			if (
				f1_name.size() > 2 &&
				f1_name[-2] == '_'
			) {

				if		(f1_name[-1] == 'O') f1_cond = 0x00;
				else if	(f1_name[-1] == 'B') f1_cond = 0x02;
				else if	(f1_name[-1] == 'C') f1_cond = 0x02;
				else if	(f1_name[-1] == 'E') f1_cond = 0x04;
				else if	(f1_name[-1] == 'Z') f1_cond = 0x04;
				else if	(f1_name[-1] == 'A') f1_cond = 0x07;
				else if	(f1_name[-1] == 'S') f1_cond = 0x08;
				else if	(f1_name[-1] == 'P') f1_cond = 0x0A;
				else if	(f1_name[-1] == 'L') f1_cond = 0x0C;
				else if	(f1_name[-1] == 'G') f1_cond = 0x0F;
				else throw(traceLine(*solved, *f1_line, "Wrong method '" + f1_name + "'."));

				f1_name = f1_name.to(-2);

			}
			else if (
				f1_name.size() > 3 &&
				f1_name[-3] == '_'
			) {

				if (
					(f1_name[-1] == 'B' && (f1_name[-2] == 'W' || f1_name[-2] == 'D' || f1_name[-2] == 'Q')) ||
					(f1_name[-1] == 'W' && (f1_name[-2] == 'D' || f1_name[-2] == 'Q')) ||
					(f1_name[-1] == 'D' && f1_name[-2] == 'Q')
				) {

					f1_mode = f1_name[-1];
					f1_mode2 = f1_name[-2];					

				}
				else if		(f1_name[-2] == 'N') {

					if		(f1_name[-1] == 'O') f1_cond = 0x01;
					else if	(f1_name[-1] == 'B') f1_cond = 0x03;
					else if	(f1_name[-1] == 'C') f1_cond = 0x03;
					else if	(f1_name[-1] == 'E') f1_cond = 0x05;
					else if	(f1_name[-1] == 'Z') f1_cond = 0x05;
					else if	(f1_name[-1] == 'A') f1_cond = 0x06;
					else if	(f1_name[-1] == 'S') f1_cond = 0x09;
					else if	(f1_name[-1] == 'P') f1_cond = 0x0B;
					else if	(f1_name[-1] == 'L') f1_cond = 0x0D;
					else if	(f1_name[-1] == 'G') f1_cond = 0x0E;
					else throw(traceLine(*solved, *f1_line, "Wrong method '" + f1_name + "'."));

				}
				else if		(f1_name[-1] == 'E') {

					if		(f1_name[-2] == 'A') f1_cond = 0x03;
					else if	(f1_name[-2] == 'B') f1_cond = 0x06;
					else if	(f1_name[-2] == 'P') f1_cond = 0x0A;
					else if	(f1_name[-2] == 'G') f1_cond = 0x0D;
					else if	(f1_name[-2] == 'L') f1_cond = 0x0E;
					else throw(traceLine(*solved, *f1_line, "Wrong method '" + f1_name + "'."));

				}
				else if		(f1_name[-1] == 'O') {

					if		(f1_name[-2] == 'P') f1_cond = 0x0B;
					else throw(traceLine(*solved, *f1_line, "Wrong method '" + f1_name + "'."));

				}
				else throw(traceLine(*solved, *f1_line, "Wrong method '" + f1_name + "'."));

				f1_name = f1_name.to(-3);

			}
			else if (
				f1_name.size() > 4 &&
				f1_name[-4] == '_'
			) {

				if		(f1_name[-3] == 'N' && f1_name[-1] == 'E') {

					if		(f1_name[-2] == 'A') f1_cond = 0x02;
					else if	(f1_name[-2] == 'B') f1_cond = 0x07;
					else if	(f1_name[-2] == 'G') f1_cond = 0x0C;
					else if	(f1_name[-2] == 'L') f1_cond = 0x0F;
					else throw(traceLine(*solved, *f1_line, "Wrong method '" + f1_name + "'."));

				}
				else throw(traceLine(*solved, *f1_line, "Wrong method '" + f1_name + "'."));

				f1_name = f1_name.to(-4);

			}

			if		(f1_name == "NOP"	) instList += NOP(*f1_line);

			else if (f1_name == "FILE"	) instList += FILE(*f1_line);
			else if (f1_name == "STRING") instList += STR(*f1_line);
			else if (f1_name == "SPACE") instList += SPACE(*f1_line);
			else if (f1_name == "BYTE"	) instList += B(*f1_line);
			else if (f1_name == "WORD"	) instList += W(*f1_line);
			else if (f1_name == "DWORD"	) instList += D(*f1_line);
			else if (f1_name == "QWORD"	) instList += Q(*f1_line);

			else if (f1_name == "NOT" 	&& f1_mode != 0) instList += ALTER(*f1_line, f1_mode, 0xF6, 0x10);
			else if (f1_name == "NEG"	&& f1_mode != 0) instList += ALTER(*f1_line, f1_mode, 0xF6, 0x18);
			else if (f1_name == "INC"	&& f1_mode != 0) instList += ALTER(*f1_line, f1_mode, 0xFE, 0x00);
			else if (f1_name == "DEC"	&& f1_mode != 0) instList += ALTER(*f1_line, f1_mode, 0xFE, 0x08);
			else if (f1_name == "ROL"	&& f1_mode != 0) instList += SHIFT(*f1_line, f1_mode, 0x00);
			else if (f1_name == "ROR"	&& f1_mode != 0) instList += SHIFT(*f1_line, f1_mode, 0x08);
			else if (f1_name == "RCL"	&& f1_mode != 0) instList += SHIFT(*f1_line, f1_mode, 0x10);
			else if (f1_name == "RCR"	&& f1_mode != 0) instList += SHIFT(*f1_line, f1_mode, 0x18);
			else if (f1_name == "SHL"	&& f1_mode != 0) instList += SHIFT(*f1_line, f1_mode, 0x20);
			else if (f1_name == "SHR"	&& f1_mode != 0) instList += SHIFT(*f1_line, f1_mode, 0x28);
			else if (f1_name == "SAL"	&& f1_mode != 0) instList += SHIFT(*f1_line, f1_mode, 0x30);
			else if (f1_name == "SAR"	&& f1_mode != 0) instList += SHIFT(*f1_line, f1_mode, 0x38);
			else if (f1_name == "ADD"	&& f1_mode != 0) instList += ARITH(*f1_line, f1_mode, 0x00);
			else if (f1_name == "OR"	&& f1_mode != 0) instList += ARITH(*f1_line, f1_mode, 0x08);
			else if (f1_name == "ADC"	&& f1_mode != 0) instList += ARITH(*f1_line, f1_mode, 0x10);
			else if (f1_name == "SBB"	&& f1_mode != 0) instList += ARITH(*f1_line, f1_mode, 0x18);
			else if (f1_name == "AND"	&& f1_mode != 0) instList += ARITH(*f1_line, f1_mode, 0x20);
			else if (f1_name == "SUB"	&& f1_mode != 0) instList += ARITH(*f1_line, f1_mode, 0x28);
			else if (f1_name == "XOR"	&& f1_mode != 0) instList += ARITH(*f1_line, f1_mode, 0x30);
			else if (f1_name == "CMP"	&& f1_mode != 0) instList += ARITH(*f1_line, f1_mode, 0x38);
			else if (f1_name == "TEST"	&& f1_mode != 0) instList += TEST(*f1_line, f1_mode);
			else if (f1_name == "MUL"	&& f1_mode != 0) instList += MUL(*f1_line, f1_mode, 0x20);
			else if (f1_name == "SMUL"	&& f1_mode != 0) instList += MUL(*f1_line, f1_mode, 0x28);
			else if (f1_name == "DIV"	&& f1_mode != 0) instList += MUL(*f1_line, f1_mode, 0x30);
			else if (f1_name == "SDIV"	&& f1_mode != 0) instList += MUL(*f1_line, f1_mode, 0x38);
			else if (f1_name == "MOV"	&& f1_mode != 0) instList += MOV(*f1_line, f1_mode);
			else if (f1_name == "MOVSX"	&& f1_mode != 0 && f1_mode2 != 0) instList += MOVX(*f1_line, f1_mode, f1_mode2, 0x08);
			else if (f1_name == "MOVZX"	&& f1_mode != 0 && f1_mode2 != 0) instList += MOVX(*f1_line, f1_mode, f1_mode2, 0x00);
			else if (f1_name == "XCHG"	&& f1_mode != 0) instList += XCHG(*f1_line, f1_mode);
			else if (f1_name == "PUSH"	&& f1_mode != 0) instList += PUSH(*f1_line, f1_mode);
			else if (f1_name == "POP"	&& f1_mode != 0) instList += POP(*f1_line, f1_mode);

			else if (f1_name == "LEA"	) instList += LEA(*f1_line);

			else if (f1_name == "JMP"	) instList += JUMP(*f1_line);
			else if (f1_name == "CALL"	) instList += CALL(*f1_line);
			else if (f1_name == "RETN"	) instList += RETN(*f1_line);
			
			else if	(f1_name == "STACK_ENTER"	) instList += FENTER(*f1_line);
			else if (f1_name == "STACK_LEAVE"	) instList += FLEAVE(*f1_line);
			else if (f1_name == "STACK_GROW"	) instList += FGROW(*f1_line);
			else if (f1_name == "STACK_SHRINK"	) instList += FSHRINK(*f1_line);
			else if (f1_name == "STACK_ALIGN"	) instList += FALIGN(*f1_line);

			else if (f1_name == "CJMP"	&& f1_cond != 0x80) instList += CJUMP(*f1_line, f1_cond);
			else if (f1_name == "CSET"	&& f1_cond != 0x80) instList += CSET(*f1_line, f1_cond);
			else if (f1_name == "CMOV"	&& f1_cond != 0x80 && f1_mode > 'B') instList += CMOV(*f1_line, f1_cond, f1_mode);

			else if (f1_name == "SHLD"	&& f1_mode > 'B') instList += SHIFTD(*f1_line, f1_mode, 0xA4);
			else if (f1_name == "SHRD"	&& f1_mode > 'B') instList += SHIFTD(*f1_line, f1_mode, 0xAC);

			else if (f1_name == "BT"	&& f1_mode > 'B') instList += BT(*f1_line, f1_mode, 0x00);
			else if (f1_name == "BTS"	&& f1_mode > 'B') instList += BT(*f1_line, f1_mode, 0x08);
			else if (f1_name == "BTR"	&& f1_mode > 'B') instList += BT(*f1_line, f1_mode, 0x10);
			else if (f1_name == "BTC"	&& f1_mode > 'B') instList += BT(*f1_line, f1_mode, 0x18);

			else if (f1_name == "BSF"	&& f1_mode > 'B') instList += BS(*f1_line, f1_mode, 0x00);
			else if (f1_name == "BSR"	&& f1_mode > 'B') instList += BS(*f1_line, f1_mode, 0x01);

			else if (f1_name == "BCNT"	&& f1_mode > 'B') instList += CNT(*f1_line, f1_mode, 0x08);
			else if (f1_name == "ZCNT"	&& f1_mode > 'B') instList += CNT(*f1_line, f1_mode, 0x0D);

			else throw(traceLine(*solved, *f1_line, "Wrong method '" + f1_line->name.content + "'."));

			instList[-1].section = f1_sectionId;
			
			for (TOKEN* f2_token = f1_line->symList.begin(); f2_token < f1_line->symList.end(); f2_token ++) {

				instList[-1].defSymList += SYM(*f2_token, f1_sectionId);

			}

		}

		for (INST* f1_inst = instList.begin(); f1_inst < instList.end(); f1_inst++) {

			if (f1_inst->section <= s_GLB) continue;

			for (SWORD* f2_section = f_sectionList.begin(); f2_section < f_sectionList.end(); f2_section ++) {
				
				if (f1_inst->section == *f2_section) {

					f1_inst->section = SWORD(f2_section - f_sectionList.begin());

				}

			}

			for (SYM* f2_sym = f1_inst->defSymList.begin(); f2_sym < f1_inst->defSymList.end(); f2_sym ++) {

				for (SWORD* f3_section = f_sectionList.begin(); f3_section < f_sectionList.end(); f3_section ++) {

					if (f2_sym->section == *f3_section) {

						f2_sym->section = SWORD(f3_section - f_sectionList.begin());

					}

				}

			}

		}

		for (INST* f1_inst = instList.begin(); f1_inst < instList.end(); f1_inst++) {

			if (!f1_inst->reqSym.token.is()) continue;

			BOOL f_found = false;

			for (INST *f2_inst = instList.begin(); f2_inst < instList.end(); f2_inst++) {

				for (SYM* f3_sym = f2_inst->defSymList.begin(); f3_sym < f2_inst->defSymList.end(); f3_sym ++) {

					if (f3_sym->token.pointer == f1_inst->reqSym.token.pointer) {

						f1_inst->reqSym.section = f3_sym->section;

						f_found = true;

						break;

					}

				}

				if (f_found) break;

			}

			if (!f_found) {

				if (f1_inst->reqSym.token.isGlb()) {
					
					f1_inst->reqSym.section = s_GLB;

				}
				else {

					throw(traceToken(*solved->parsed->lexed, f1_inst->reqSym.token, "Symbol unreachable."));

				}

			}

		}

	}

	INST ENCODED::NOP(LINE &i_line) {

		return INST();

	}

	INST ENCODED::FILE(LINE &i_line) {

		INST o_instruction;

		if (i_line.argList.size() != 1) {

			throw(traceLine(*solved, i_line, "Wrong argument number."));
		}

		ARGV *f_d = arg(i_line.argList, "d", 0);

		if (f_d == 0) {

			throw(traceLine(*solved, i_line, "Argument 'd' missing."));
		}

		if (f_d->name[0] != '#') {

			throw(traceLine(*solved, i_line, "Argument 'd' having wrong type."));
		}

		std::ifstream f_file(f_d->value.value.c(), std::ifstream::binary);

		if (f_file.fail()) {

			throw(traceLine(*solved, i_line, "File '" + f_d->value.value + "' not found."));
		}

		char f_char;

		while (f_file.get(f_char)) {

			o_instruction.binary += BYTE(f_char);

		}

		return o_instruction;
	}

	INST ENCODED::STR(LINE &i_line) {

		INST o_instruction;

		if (i_line.argList.size() != 1) {

			throw(traceLine(*solved, i_line, "Wrong argument number."));
		}

		ARGV *f_d = arg(i_line.argList, "d", 0);

		if (f_d == 0) {

			throw(traceLine(*solved, i_line, "Argument 'd' missing."));
		}

		if (f_d->name[0] != '#') {

			throw(traceLine(*solved, i_line, "Argument 'd' having wrong type."));
		}

		o_instruction.binary += f_d->value.value;

		return o_instruction;
	}
	
	INST ENCODED::SPACE(LINE &i_line) {

		INST o_instruction;

		if (i_line.argList.size() != 1) {

			throw(traceLine(*solved, i_line, "Wrong argument number."));
		}

		ARGV *f_d = arg(i_line.argList, "d", 0);

		if (f_d == 0) {

			throw(traceLine(*solved, i_line, "Argument 'd' missing."));
		}

		if (f_d->name[0] != '#') {

			throw(traceLine(*solved, i_line, "Argument 'd' having wrong type."));
		}

		DWORD f_num = f_d->value.value[0];

		if (f_d->value.value.size() >= 2) f_num |= (f_d->value.value[1] << 8);
		if (f_d->value.value.size() >= 3) f_num |= (f_d->value.value[2] << 16);
		if (f_d->value.value.size() >= 4) f_num |= (f_d->value.value[3] << 24);

		if (f_d->value.value.size() > 4) {

			throw(traceLine(*solved, i_line, "Argument 'd' being too big."));
		}

		while (f_num > 0) {

			o_instruction.binary += BYTE(0);

			f_num --;

		}		

		return o_instruction;
	}

	INST ENCODED::B(LINE &i_line) {

		INST o_instruction;

		if (i_line.argList.size() != 1) {

			throw(traceLine(*solved, i_line, "Wrong argument number."));
		}

		ARGV *f_d = arg(i_line.argList, "d", 0);

		if (f_d == 0) {

			throw(traceLine(*solved, i_line, "Argument 'd' missing."));
		}

		if (f_d->name[0] != '#') {

			throw(traceLine(*solved, i_line, "Argument 'd' having wrong type."));
		}

		if (size(f_d->value.value) <= 1) {

			o_instruction.binary = resize(f_d->value.value, 1);

		}
		else throw(traceLine(*solved, i_line, "Argument 'd' being too big."));

		return o_instruction;
	}

	INST ENCODED::W(LINE &i_line) {

		INST o_instruction;

		if (i_line.argList.size() != 1) {

			throw(traceLine(*solved, i_line, "Wrong argument number."));
		}

		ARGV *f_d = arg(i_line.argList, "d", 0);

		if (f_d == 0) {

			throw(traceLine(*solved, i_line, "Argument 'd' missing."));
		}

		if (f_d->name[0] != '#') {

			throw(traceLine(*solved, i_line, "Argument 'd' having wrong type."));
		}

		if (size(f_d->value.value) <= 2) {

			o_instruction.binary = resize(f_d->value.value, 2);

		}
		else throw(traceLine(*solved, i_line, "Argument 'd' being too big."));

		return o_instruction;
	}

	INST ENCODED::D(LINE &i_line) {

		INST o_instruction;

		if (i_line.argList.size() != 1) {

			throw(traceLine(*solved, i_line, "Wrong argument number."));
		}

		ARGV *f_d = arg(i_line.argList, "d", 0);

		if (f_d == 0) {

			throw(traceLine(*solved, i_line, "Argument 'd' missing."));
		}

		if (f_d->name[0] != '#') {

			throw(traceLine(*solved, i_line, "Argument 'd' having wrong type."));
		}

		if (size(f_d->value.value) <= 4) {

			o_instruction.binary = resize(f_d->value.value, 4);

		}
		else throw(traceLine(*solved, i_line, "Argument 'd' being too big."));

		return o_instruction;
	}

	INST ENCODED::Q(LINE &i_line) {

		INST o_instruction;

		if (i_line.argList.size() != 1) {

			throw(traceLine(*solved, i_line, "Wrong argument number."));
		}

		ARGV *f_d = arg(i_line.argList, "d", 0);

		if (f_d == 0) {

			throw(traceLine(*solved, i_line, "Argument 'd' missing."));
		}

		if (f_d->name[0] != '#') {

			throw(traceLine(*solved, i_line, "Argument 'd' having wrong type."));
		}

		if (size(f_d->value.value) <= 8) {

			o_instruction.binary = resize(f_d->value.value, 8);

		}
		else throw(traceLine(*solved, i_line, "Argument 'd' being too big."));

		return o_instruction;
	}

	INST ENCODED::ALTER(LINE &i_line, BYTE i_mode, BYTE i_op1, BYTE i_op2) {

		INST o_instruction;

		if (i_mode != 'B') {

			i_op1 += 0x01;
		}

		if (i_mode == 'W') {

			o_instruction.binary += BYTE(0x66);
		}

		if (i_line.argList.size() == 1) {

			ARGV *f_t = arg(i_line.argList, "t", 0);

			if (f_t == 0) {

				throw(traceLine(*solved, i_line, "Argument 't' missing."));
			}

			if (f_t->name[0] == '%') {

				o_instruction.binary += R(i_line, i_mode, f_t, i_op1, i_op2);
			}
			else if (f_t->name[0] == '$') {

				o_instruction.binary += M(i_line, i_mode, f_t, i_op1, i_op2);

				if (f_t->value.is()) {

					o_instruction.reqSym = SYM(f_t->value, s_ERR, o_instruction.binary.size() - 4);

				}
			}
			else {

				throw(traceLine(*solved, i_line, "Argument 't' having wrong type."));
			}
		}
		else {

			throw(traceLine(*solved, i_line, "Wrong argument number."));
		}

		return o_instruction;
	}

	INST ENCODED::SHIFT(LINE &i_line, BYTE i_mode, BYTE i_op1) {

		INST o_instruction;

		BYTE f_op2 = 0xC0;

		if (i_mode != 'B') {

			f_op2 += 0x01;
		}

		if (i_mode == 'W') {

			o_instruction.binary += BYTE(0x66);
		}

		if (i_line.argList.size() == 2) {

			ARGV *f_t = arg(i_line.argList, "t", 0);

			if (f_t == 0)
				throw(traceLine(*solved, i_line, "Argument 't' missing."));

			ARGV *f_v = arg(i_line.argList, "v", 1);

			if (f_v == 0)
				throw(traceLine(*solved, i_line, "Argument 'v' missing."));

			if (f_v->name[0] == '%') {

				if (f_v->value.value[0] == 1) {

					f_op2 += 0x12;

				}
				else throw(traceLine(*solved, i_line, "Argument 'v' not being %11."));
			}
			else if (f_v->name[0] == '#') {

				if (f_v->value.value[0] == 1)
					f_op2 += 0x10;

				else {

					if (size(f_v->value.value) > 1) {

						throw(traceLine(*solved, i_line, "Argument 'v' being too big."));

					}

					if (isNeg(f_v->value.value)) {

						i_op1 ^= 0x08;

						f_v->value.value[0] = ~f_v->value.value[0];

						f_v->value.value[0] ++;

						if (f_v->value.value[0] == 1) {

							f_op2 += 0x10;

						}

					}

					f_v->value.value = resize(f_v->value.value, 1);
				}
			}
			else throw(traceLine(*solved, i_line, "Argument 'v' having wrong type."));

			if (f_t->name[0] == '%')
				o_instruction.binary += R(i_line, i_mode, f_t, f_op2, i_op1);

			else if (f_t->name[0] == '$') {

				o_instruction.binary += M(i_line, i_mode, f_t, f_op2, i_op1);

				if (f_t->value.is()) {

					o_instruction.reqSym = SYM(f_t->value, s_ERR, o_instruction.binary.size() - 4);

				}
			}
			else throw(traceLine(*solved, i_line, "Argument 't' having wrong type."));

			if (f_v->value.value[0] != 1)
				o_instruction.binary += BYTE(f_v->value.value[0]);
		}
		else throw(traceLine(*solved, i_line, "Wrong argument number."));

		return o_instruction;
	}

	INST ENCODED::SHIFTD(LINE &i_line, BYTE i_mode, BYTE i_op1) {

		INST o_instruction;

		if (i_mode == 'W') {

			o_instruction.binary += BYTE(0x66);
		}

		if (i_line.argList.size() == 3) {

			ARGV *f_t = arg(i_line.argList, "t", 0);

			if (f_t == 0)
				throw(traceLine(*solved, i_line, "Argument 't' missing."));

			ARGV *f_s = arg(i_line.argList, "s", 1);

			if (f_s == 0)
				throw(traceLine(*solved, i_line, "Argument 's' missing."));

			if (f_s->name[0] != '%')
				throw(traceLine(*solved, i_line, "Argument 's' having wrong type."));

			ARGV *f_v = arg(i_line.argList, "v", 2);

			if (f_v == 0)
				throw(traceLine(*solved, i_line, "Argument 'v' missing."));

			if (f_v->name[0] == '%') {

				if (f_v->value.value[0] == 1) {

					i_op1 |= 0x01;

					f_v->value.value.empty();

				}
				else throw(traceLine(*solved, i_line, "Argument 'v' not being %11."));
			}
			else if (f_v->name[0] == '#') {

				if (size(f_v->value.value) > 1) {

					throw(traceLine(*solved, i_line, "Argument 'v' being too big."));

				}

				if (isNeg(f_v->value.value)) {

					i_op1 ^= 0x08;

					f_v->value.value[0] = ~f_v->value.value[0];

					f_v->value.value[0] ++;

				}

				f_v->value.value = resize(f_v->value.value, 1);
				
			}
			else throw(traceLine(*solved, i_line, "Argument 'v' having wrong type."));

			if (f_t->name[0] == '%')
				o_instruction.binary += RR(i_line, i_mode, f_t, f_s, i_op1, 0, true);

			else if (f_t->name[0] == '$') {

				o_instruction.binary += MR(i_line, i_mode, f_t, f_s, i_op1, 0, true);

				if (f_t->value.is()) {

					o_instruction.reqSym = SYM(f_t->value, s_ERR, o_instruction.binary.size() - 4);

				}
			}
			else throw(traceLine(*solved, i_line, "Argument 't' having wrong type."));

			o_instruction.binary += f_v->value.value;
		}
		else throw(traceLine(*solved, i_line, "Wrong argument number."));

		return o_instruction;
	}
	
	INST ENCODED::BT(LINE &i_line, BYTE i_mode, BYTE i_op1) {

		INST o_instruction;

		if (i_mode == 'W') {

			o_instruction.binary += BYTE(0x66);

		}

		if (i_line.argList.size() == 2) {

			ARGV *f_t = arg(i_line.argList, "t", 0);

			if (f_t == 0)
				throw(traceLine(*solved, i_line, "Argument 't' missing."));

			ARGV *f_v = arg(i_line.argList, "v", 1);

			if (f_v == 0)
				throw(traceLine(*solved, i_line, "Argument 'v' missing."));

			if (f_t->name[0] == '%') {

				if (f_v->name[0] == '%') {

					o_instruction.binary += RR(i_line, i_mode, f_t, f_v, 0xA3 + i_op1, 0, true);

				}
				else if (f_v->name[0] == '#') {

					if (size(f_v->value.value) > 1) {

						throw(traceLine(*solved, i_line, "Argument 'v' being too big."));

					}

					o_instruction.binary += R(i_line, i_mode, f_t, 0xBA, 0x20 + i_op1, true);

					o_instruction.binary += resize(f_v->value.value, 1);
					
				}
				else throw(traceLine(*solved, i_line, "Argument 'v' having wrong type."));

			}
			else if (f_t->name[0] == '$') {

				if (f_v->name[0] == '%') {

					o_instruction.binary += MR(i_line, i_mode, f_t, f_v, 0xA3 + i_op1, 0, true);

				}
				else if (f_v->name[0] == '#') {

					if (size(f_v->value.value) > 1) {

						throw(traceLine(*solved, i_line, "Argument 'v' being too big."));

					}

					o_instruction.binary += M(i_line, i_mode, f_t, 0xBA, 0x20 + i_op1, true);
					
				}
				else throw(traceLine(*solved, i_line, "Argument 'v' having wrong type."));

				if (f_t->value.is()) {

					o_instruction.reqSym = SYM(f_t->value, s_ERR, o_instruction.binary.size() - 4);

				}

				if (f_v->name[0] == '#') {

					o_instruction.binary += resize(f_v->value.value, 1);

				}
				
			}
			else throw(traceLine(*solved, i_line, "Argument 't' having wrong type."));

		}
		else throw(traceLine(*solved, i_line, "Wrong argument number."));

		return o_instruction;
	}

	INST ENCODED::BS(LINE &i_line, BYTE i_mode, BYTE i_op1) {

		INST o_instruction;

		if (i_mode == 'W') {

			o_instruction.binary += BYTE(0x66);

		}

		if (i_line.argList.size() == 2) {

			ARGV *f_t = arg(i_line.argList, "t", 0);

			if (f_t == 0)
				throw(traceLine(*solved, i_line, "Argument 't' missing."));

			if (f_t->name[0] != '%')
				throw(traceLine(*solved, i_line, "Argument 't' having wrong type."));

			ARGV *f_s = arg(i_line.argList, "s", 1);

			if (f_s == 0)
				throw(traceLine(*solved, i_line, "Argument 's' missing."));

			if (f_s->name[0] == '%') {

				o_instruction.binary += RR(i_line, i_mode, f_s, f_t, 0xBC + i_op1, 0, true);

			}
			else if (f_s->name[0] == '$') {

				o_instruction.binary += MR(i_line, i_mode, f_s, f_t, 0xBC + i_op1, 0, true);

				if (f_s->value.is()) {

					o_instruction.reqSym = SYM(f_s->value, s_ERR, o_instruction.binary.size() - 4);

				}
				
			}
			else throw(traceLine(*solved, i_line, "Argument 's' having wrong type."));

		}
		else throw(traceLine(*solved, i_line, "Wrong argument number."));

		return o_instruction;
	}

	INST ENCODED::CNT(LINE& i_line, BYTE i_mode, BYTE i_op1) {

		INST o_instruction;

		if (i_mode == 'W') {

			o_instruction.binary += BYTE(0x66);

		}

		o_instruction.binary += BYTE(0xF3);

		if (i_line.argList.size() == 2) {

			ARGV *f_t = arg(i_line.argList, "t", 0);

			if (f_t == 0)
				throw(traceLine(*solved, i_line, "Argument 't' missing."));

			if (f_t->name[0] != '%')
				throw(traceLine(*solved, i_line, "Argument 't' having wrong type."));

			ARGV *f_s = arg(i_line.argList, "s", 1);

			if (f_s == 0)
				throw(traceLine(*solved, i_line, "Argument 's' missing."));

			if (f_s->name[0] == '%') {

				o_instruction.binary += RR(i_line, i_mode, f_s, f_t, 0xB0 | i_op1, 0, true);

			}
			else if (f_s->name[0] == '$') {

				o_instruction.binary += MR(i_line, i_mode, f_s, f_t, 0xB0 | i_op1, 0, true);

				if (f_s->value.is()) {

					o_instruction.reqSym = SYM(f_s->value, s_ERR, o_instruction.binary.size() - 4);

				}
				
			}
			else throw(traceLine(*solved, i_line, "Argument 's' having wrong type."));

		}
		else throw(traceLine(*solved, i_line, "Wrong argument number."));

		return o_instruction;

	}

	INST ENCODED::ARITH(LINE &i_line, BYTE i_mode, BYTE i_op1) {

		INST o_instruction;

		if (i_mode == 'W') {

			o_instruction.binary += BYTE(0x66);
		}

		if (i_line.argList.size() == 2) {

			ARGV *f_t = arg(i_line.argList, "t", 0);

			if (f_t == 0)
				throw(traceLine(*solved, i_line, "Argument 't' missing."));

			ARGV *f_s = arg(i_line.argList, "s", 1);

			if (f_s == 0)
				throw(traceLine(*solved, i_line, "Argument 's' missing."));

			if (f_s->name[0] == '#') {

				if (i_mode == 'B') {

					if (size(f_s->value.value) <= 1) {

						f_s->value.value = resize(f_s->value.value, 1);

					}
					else throw(traceLine(*solved, i_line, "Argument 's' being too big."));

				}
				if (i_mode == 'W') {

					if (f_s->value.value.size() <= 1) {

						f_s->value.value = resize(f_s->value.value, 1);

					}
					else if (size(f_s->value.value) <= 2) {

						f_s->value.value = resize(f_s->value.value, 2);

					}
					else throw(traceLine(*solved, i_line, "Argument 's' being too big."));

				}
				if (i_mode == 'D') {

					if (f_s->value.value.size() <= 1) {

						f_s->value.value = resize(f_s->value.value, 1);

					}
					else if (size(f_s->value.value) <= 4) {

						f_s->value.value = resize(f_s->value.value, 4);

					}
					else throw(traceLine(*solved, i_line, "Argument 's' being too big."));

				}
				if (i_mode == 'Q') {

					if (f_s->value.value.size() <= 1) {

						f_s->value.value = resize(f_s->value.value, 1);

					}
					else if (size(f_s->value.value) <= 4) {

						f_s->value.value = resize(f_s->value.value, 4);

					}
					else throw(traceLine(*solved, i_line, "Argument 's' being too big."));

				}
			}

			if (f_t->name[0] == '%') {

				if (f_s->name[0] == '#') {

					if (f_t->value.value.isZero()) {

						if (i_mode == 'B')
							o_instruction.binary += BYTE(i_op1 | 0x04);

						else {

							if (f_s->value.value.size() == 1)
								o_instruction.binary += R(i_line, i_mode, f_t, 0x83, i_op1);
							else {

								if (i_mode == 'Q')
									o_instruction.binary += REX_000(0x08);

								o_instruction.binary += BYTE(i_op1 | 0x05);
							}
						}
					}
					else {

						if (f_s->value.value.size() == 1) {

							if (i_mode == 'B')
								o_instruction.binary += R(i_line, i_mode, f_t, 0x80, i_op1);

							else
								o_instruction.binary += R(i_line, i_mode, f_t, 0x83, i_op1);
						}
						else
							o_instruction.binary += R(i_line, i_mode, f_t, 0x81, i_op1);
					}

					o_instruction.binary += f_s->value.value;
				}
				else if (f_s->name[0] == '%') {

					if (i_mode == 'B')
						o_instruction.binary += RR(i_line, i_mode, f_t, f_s, i_op1);

					else
						o_instruction.binary += RR(i_line, i_mode, f_t, f_s, i_op1 | 0x01);
				}
				else if (f_s->name[0] == '$') {

					if (i_mode == 'B')
						o_instruction.binary += MR(i_line, i_mode, f_s, f_t, i_op1 | 0x02);

					else
						o_instruction.binary += MR(i_line, i_mode, f_s, f_t, i_op1 | 0x03);

					if (f_s->value.is()) {

						o_instruction.reqSym = SYM(f_s->value, s_ERR, o_instruction.binary.size() - 4);
					}
				}
				else throw(traceLine(*solved, i_line, "Argument 's' having wrong type."));
			}
			else if (f_t->name[0] == '$') {

				if (f_s->name[0] == '#') {

					if (f_s->value.value.size() == 1) {

						if (i_mode == 'B')
							o_instruction.binary += M(i_line, i_mode, f_t, 0x80, i_op1);

						else
							o_instruction.binary += M(i_line, i_mode, f_t, 0x83, i_op1);
					}
					else
						o_instruction.binary += M(i_line, i_mode, f_t, 0x81, i_op1);

					if (f_t->value.is()) {

						o_instruction.reqSym = SYM(f_t->value, s_ERR, o_instruction.binary.size() - 4);
					}

					o_instruction.binary += f_s->value.value;
				}
				else if (f_s->name[0] == '%') {

					if (i_mode == 'B')
						o_instruction.binary += MR(i_line, i_mode, f_t, f_s, i_op1);

					else
						o_instruction.binary += MR(i_line, i_mode, f_t, f_s, i_op1 | 0x01);

					if (f_t->value.is()) {

						o_instruction.reqSym = SYM(f_t->value, s_ERR, o_instruction.binary.size() - 4);
					}
				}
				else throw(traceLine(*solved, i_line, "Argument 's' having wrong type."));
			}
			else throw(traceLine(*solved, i_line, "Argument 't' having wrong type."));
		}
		else throw(traceLine(*solved, i_line, "Wrong argument number."));

		return o_instruction;
	}

	INST ENCODED::MUL(LINE &i_line, BYTE i_mode, BYTE i_op2) {

		INST o_instruction;

		ARGV *f_s = arg(i_line.argList, "s", 0);

		if (f_s == 0)
			throw(traceLine(*solved, i_line, "Argument 's' missing."));

		ARGV *f_v = arg(i_line.argList, "v", 1);

		if (i_line.argList.size() == 1 || !f_v || f_v->value.value.isZero()) {

			if 	(f_s->name[0] == '%') {

				if (i_mode == 'B') {

					if (i_op2 == 0x30) {

						o_instruction.binary += BYTE(0x30);
						o_instruction.binary += BYTE(0xE4);

					}
					else if (i_op2 == 0x38) {

						o_instruction.binary += BYTE(0x66);
						o_instruction.binary += BYTE(0x98);

					}

					o_instruction.binary += R(i_line, i_mode, f_s, 0xF6, i_op2);

					o_instruction.binary += BYTE(0x88);
					o_instruction.binary += BYTE(0xE2);

				}
				else if (i_mode == 'W') {

					if (i_op2 == 0x30) {

						o_instruction.binary += BYTE(0x66);
						o_instruction.binary += BYTE(0x31);
						o_instruction.binary += BYTE(0xD2);

					}
					else if (i_op2 == 0x38) {

						o_instruction.binary += BYTE(0x66);
						o_instruction.binary += BYTE(0x99);

					}

					o_instruction.binary += BYTE(0x66);

					o_instruction.binary += R(i_line, i_mode, f_s, 0xF7, i_op2);

				}
				else if (i_mode == 'D') {

					if (i_op2 == 0x30) {

						o_instruction.binary += BYTE(0x31);
						o_instruction.binary += BYTE(0xD2);

					}
					else if (i_op2 == 0x38) {

						o_instruction.binary += BYTE(0x99);

					}

					o_instruction.binary += R(i_line, i_mode, f_s, 0xF7, i_op2);

				}
				else {

					if (i_op2 == 0x30) {

						o_instruction.binary += BYTE(0x31);
						o_instruction.binary += BYTE(0xD2);

					}
					else if (i_op2 == 0x38) {

						o_instruction.binary += BYTE(0x48);
						o_instruction.binary += BYTE(0x99);

					}

					o_instruction.binary += R(i_line, i_mode, f_s, 0xF7, i_op2);

				}
			}
			else if (f_s->name[0] == '$') {

				if (i_mode == 'B') {

					if (i_op2 == 0x30) {

						o_instruction.binary += BYTE(0x30);
						o_instruction.binary += BYTE(0xE4);

					}
					else if (i_op2 == 0x38) {

						o_instruction.binary += BYTE(0x66);
						o_instruction.binary += BYTE(0x98);

					}

					o_instruction.binary += M(i_line, i_mode, f_s, 0xF6, i_op2);

					o_instruction.binary += BYTE(0x88);
					o_instruction.binary += BYTE(0xE2);

				}
				else if (i_mode == 'W') {

					if (i_op2 == 0x30) {

						o_instruction.binary += BYTE(0x66);
						o_instruction.binary += BYTE(0x31);
						o_instruction.binary += BYTE(0xD2);

					}
					else if (i_op2 == 0x38) {

						o_instruction.binary += BYTE(0x66);
						o_instruction.binary += BYTE(0x99);

					}

					o_instruction.binary += BYTE(0x66);

					o_instruction.binary += M(i_line, i_mode, f_s, 0xF7, i_op2);

				}
				else if (i_mode == 'D') {

					if (i_op2 == 0x30) {

						o_instruction.binary += BYTE(0x31);
						o_instruction.binary += BYTE(0xD2);

					}
					else if (i_op2 == 0x38) {

						o_instruction.binary += BYTE(0x99);

					}

					o_instruction.binary += M(i_line, i_mode, f_s, 0xF7, i_op2);

				}
				else {

					if (i_op2 == 0x30) {

						o_instruction.binary += BYTE(0x31);
						o_instruction.binary += BYTE(0xD2);

					}
					else if (i_op2 == 0x38) {

						o_instruction.binary += BYTE(0x48);
						o_instruction.binary += BYTE(0x99);

					}

					o_instruction.binary += M(i_line, i_mode, f_s, 0xF7, i_op2);

				}

				if (f_s->value.is()) {

					o_instruction.reqSym = SYM(f_s->value, s_ERR, o_instruction.binary.size() - 4);
				}
			
			}
			else throw(traceLine(*solved, i_line, "Argument 's' having wrong type."));
		}
		else if (i_line.argList.size() == 2) {

			if (f_v == 0)
				throw(traceLine(*solved, i_line, "Argument 'v' missing."));

			if (f_v->name[0] == '%') {

				if (f_v->value.value[0] != 1)
					throw(traceLine(*solved, i_line, "Argument 'v' not being %11."));

			}
			else if (f_v->name[0] == '#') {

				if (size(f_v->value.value) > 1) {

					throw(traceLine(*solved, i_line, "Argument 'v' being too big."));

				}

				if (isNeg(f_v->value.value)) {

					throw(traceLine(*solved, i_line, "Argument 'v' being negative."));

				}

				f_v->value.value = resize(f_v->value.value, 1);
				
			}

			if 	(f_s->name[0] == '%') {

				if (i_mode == 'B') {

					if (i_op2 >= 0x30) {
					
						if (i_op2 == 0x30) {

							o_instruction.binary += BYTE(0x30);
							o_instruction.binary += BYTE(0xE4);

						}
						else {

							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0x98);

						}

						if (f_v->name[0] == '%') {

							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0xD3);
							o_instruction.binary += BYTE(0xE0);

						}
						else if (f_v->value.value[0] == 1) {

							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0xD1);
							o_instruction.binary += BYTE(0xE0);

						}
						else {

							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0xC1);
							o_instruction.binary += BYTE(0xE0);
							o_instruction.binary += f_v->value.value;

						}

					}

					o_instruction.binary += R(i_line, i_mode, f_s, 0xF6, i_op2);

					if (i_op2 < 0x30) {

						if (f_v->name[0] == '%') {

							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0xD3);
							o_instruction.binary += BYTE(0xE8);

						}
						else if (f_v->value.value[0] == 1) {

							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0xD1);
							o_instruction.binary += BYTE(0xE8);

						}
						else {

							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0xC1);
							o_instruction.binary += BYTE(0xE8);
							o_instruction.binary += f_v->value.value;

						}

					}
					
					o_instruction.binary += BYTE(0x88);
					o_instruction.binary += BYTE(0xE2);

				}
				else if (i_mode == 'W') {

					if (i_op2 >= 0x30) {
					
						if (i_op2 == 0x30) {

							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0x31);
							o_instruction.binary += BYTE(0xD2);

						}
						else {

							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0x99);

						}

						if (f_v->name[0] == '%') {

							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xA5);
							o_instruction.binary += BYTE(0xC2);
							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0xD3);
							o_instruction.binary += BYTE(0xE0);

						}
						else if (f_v->value.value[0] == 1) {

							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xA4);
							o_instruction.binary += BYTE(0xC2);
							o_instruction.binary += f_v->value.value;
							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0xD1);
							o_instruction.binary += BYTE(0xE0);

						}
						else {

							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xA4);
							o_instruction.binary += BYTE(0xC2);
							o_instruction.binary += f_v->value.value;
							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0xC1);
							o_instruction.binary += BYTE(0xE0);
							o_instruction.binary += f_v->value.value;

						}

					}

					o_instruction.binary += BYTE(0x66);

					o_instruction.binary += R(i_line, i_mode, f_s, 0xF7, i_op2);

					if (i_op2 < 0x30) {

						if (f_v->name[0] == '%') {

							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xAD);
							o_instruction.binary += BYTE(0xD0);

						}
						else {

							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xAC);
							o_instruction.binary += BYTE(0xD0);
							o_instruction.binary += f_v->value.value;

						}

					}

				}
				else if (i_mode == 'D') {

					if (i_op2 >= 0x30) {
					
						if (i_op2 == 0x30) {

							o_instruction.binary += BYTE(0x31);
							o_instruction.binary += BYTE(0xD2);

						}
						else {

							o_instruction.binary += BYTE(0x99);

						}

						if (f_v->name[0] == '%') {

							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xA5);
							o_instruction.binary += BYTE(0xC2);
							o_instruction.binary += BYTE(0xD3);
							o_instruction.binary += BYTE(0xE0);

						}
						else if (f_v->value.value[0] == 1) {

							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xA4);
							o_instruction.binary += BYTE(0xC2);
							o_instruction.binary += f_v->value.value;
							o_instruction.binary += BYTE(0xD1);
							o_instruction.binary += BYTE(0xE0);

						}
						else {

							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xA4);
							o_instruction.binary += BYTE(0xC2);
							o_instruction.binary += f_v->value.value;
							o_instruction.binary += BYTE(0xC1);
							o_instruction.binary += BYTE(0xE0);
							o_instruction.binary += f_v->value.value;

						}

					}

					o_instruction.binary += R(i_line, i_mode, f_s, 0xF7, i_op2);

					if (i_op2 < 0x30) {

						if (f_v->name[0] == '%') {

							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xAD);
							o_instruction.binary += BYTE(0xD0);

						}
						else {

							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xAC);
							o_instruction.binary += BYTE(0xD0);
							o_instruction.binary += f_v->value.value;

						}

					}

				}
				else {

					if (i_op2 >= 0x30) {
					
						if (i_op2 == 0x30) {

							o_instruction.binary += BYTE(0x31);
							o_instruction.binary += BYTE(0xD2);

						}
						else {

							o_instruction.binary += BYTE(0x48);
							o_instruction.binary += BYTE(0x99);

						}

						if (f_v->name[0] == '%') {

							o_instruction.binary += BYTE(0x48);
							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xA5);
							o_instruction.binary += BYTE(0xC2);
							o_instruction.binary += BYTE(0x48);
							o_instruction.binary += BYTE(0xD3);
							o_instruction.binary += BYTE(0xE0);

						}
						else if (f_v->value.value[0] == 1) {

							o_instruction.binary += BYTE(0x48);
							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xA4);
							o_instruction.binary += BYTE(0xC2);
							o_instruction.binary += f_v->value.value;
							o_instruction.binary += BYTE(0x48);
							o_instruction.binary += BYTE(0xD1);
							o_instruction.binary += BYTE(0xE0);

						}
						else {

							o_instruction.binary += BYTE(0x48);
							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xA4);
							o_instruction.binary += BYTE(0xC2);
							o_instruction.binary += f_v->value.value;
							o_instruction.binary += BYTE(0x48);
							o_instruction.binary += BYTE(0xC1);
							o_instruction.binary += BYTE(0xE0);
							o_instruction.binary += f_v->value.value;

						}

					}

					o_instruction.binary += R(i_line, i_mode, f_s, 0xF7, i_op2);

					if (i_op2 < 0x30) {

						if (f_v->name[0] == '%') {

							o_instruction.binary += BYTE(0x48);
							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xAD);
							o_instruction.binary += BYTE(0xD0);

						}
						else {

							o_instruction.binary += BYTE(0x48);
							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xAC);
							o_instruction.binary += BYTE(0xD0);
							o_instruction.binary += f_v->value.value;

						}

					}

				}
			
			}
			else if (f_s->name[0] == '$') {

				if (i_mode == 'B') {

					if (i_op2 >= 0x30) {
					
						if (i_op2 == 0x30) {

							o_instruction.binary += BYTE(0x30);
							o_instruction.binary += BYTE(0xE4);

						}
						else {

							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0x98);

						}

						if (f_v->name[0] == '%') {

							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0xD3);
							o_instruction.binary += BYTE(0xE0);

						}
						else if (f_v->value.value[0] == 1) {

							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0xD1);
							o_instruction.binary += BYTE(0xE0);

						}
						else {

							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0xC1);
							o_instruction.binary += BYTE(0xE0);
							o_instruction.binary += f_v->value.value;

						}

					}

					o_instruction.binary += M(i_line, i_mode, f_s, 0xF6, i_op2);

					if (i_op2 < 0x30) {

						if (f_v->name[0] == '%') {

							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0xD3);
							o_instruction.binary += BYTE(0xE8);

						}
						else if (f_v->value.value[0] == 1) {

							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0xD1);
							o_instruction.binary += BYTE(0xE8);

						}
						else {

							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0xC1);
							o_instruction.binary += BYTE(0xE8);
							o_instruction.binary += f_v->value.value;

						}

					}
				
					o_instruction.binary += BYTE(0x88);
					o_instruction.binary += BYTE(0xE2);

				}
				else if (i_mode == 'W') {

					if (i_op2 >= 0x30) {
					
						if (i_op2 == 0x30) {

							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0x31);
							o_instruction.binary += BYTE(0xD2);

						}
						else {

							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0x99);

						}

						if (f_v->name[0] == '%') {

							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xA5);
							o_instruction.binary += BYTE(0xC2);
							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0xD3);
							o_instruction.binary += BYTE(0xE0);

						}
						else if (f_v->value.value[0] == 1) {

							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xA4);
							o_instruction.binary += BYTE(0xC2);
							o_instruction.binary += f_v->value.value;
							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0xD1);
							o_instruction.binary += BYTE(0xE0);

						}
						else {

							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xA4);
							o_instruction.binary += BYTE(0xC2);
							o_instruction.binary += f_v->value.value;
							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0xC1);
							o_instruction.binary += BYTE(0xE0);
							o_instruction.binary += f_v->value.value;

						}

					}

					o_instruction.binary += BYTE(0x66);

					o_instruction.binary += M(i_line, i_mode, f_s, 0xF7, i_op2);

					if (i_op2 < 0x30) {

						if (f_v->name[0] == '%') {

							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xAD);
							o_instruction.binary += BYTE(0xC2);
							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0xD3);
							o_instruction.binary += BYTE(0xE8);

						}
						else if (f_v->value.value[0] == 1) {

							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xAC);
							o_instruction.binary += BYTE(0xC2);
							o_instruction.binary += f_v->value.value;
							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0xD1);
							o_instruction.binary += BYTE(0xE8);

						}
						else {

							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xAC);
							o_instruction.binary += BYTE(0xC2);
							o_instruction.binary += f_v->value.value;
							o_instruction.binary += BYTE(0x66);
							o_instruction.binary += BYTE(0xC1);
							o_instruction.binary += BYTE(0xE8);
							o_instruction.binary += f_v->value.value;

						}

					}

				}
				else if (i_mode == 'D') {

					if (i_op2 >= 0x30) {
					
						if (i_op2 == 0x30) {

							o_instruction.binary += BYTE(0x31);
							o_instruction.binary += BYTE(0xD2);

						}
						else {

							o_instruction.binary += BYTE(0x99);

						}

						if (f_v->name[0] == '%') {

							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xA5);
							o_instruction.binary += BYTE(0xC2);
							o_instruction.binary += BYTE(0xD3);
							o_instruction.binary += BYTE(0xE0);

						}
						else if (f_v->value.value[0] == 1) {

							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xA4);
							o_instruction.binary += BYTE(0xC2);
							o_instruction.binary += f_v->value.value;
							o_instruction.binary += BYTE(0xD1);
							o_instruction.binary += BYTE(0xE0);

						}
						else {

							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xA4);
							o_instruction.binary += BYTE(0xC2);
							o_instruction.binary += f_v->value.value;
							o_instruction.binary += BYTE(0xC1);
							o_instruction.binary += BYTE(0xE0);
							o_instruction.binary += f_v->value.value;

						}

					}

					o_instruction.binary += M(i_line, i_mode, f_s, 0xF7, i_op2);

					if (i_op2 < 0x30) {

						if (f_v->name[0] == '%') {

							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xAD);
							o_instruction.binary += BYTE(0xC2);
							o_instruction.binary += BYTE(0xD3);
							o_instruction.binary += BYTE(0xE8);

						}
						else if (f_v->value.value[0] == 1) {

							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xAC);
							o_instruction.binary += BYTE(0xC2);
							o_instruction.binary += f_v->value.value;
							o_instruction.binary += BYTE(0xD1);
							o_instruction.binary += BYTE(0xE8);

						}
						else {

							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xAC);
							o_instruction.binary += BYTE(0xC2);
							o_instruction.binary += f_v->value.value;
							o_instruction.binary += BYTE(0xC1);
							o_instruction.binary += BYTE(0xE8);
							o_instruction.binary += f_v->value.value;

						}

					}

				}
				else {

					if (i_op2 >= 0x30) {
					
						if (i_op2 == 0x30) {

							o_instruction.binary += BYTE(0x31);
							o_instruction.binary += BYTE(0xD2);

						}
						else {

							o_instruction.binary += BYTE(0x48);
							o_instruction.binary += BYTE(0x99);

						}

						if (f_v->name[0] == '%') {

							o_instruction.binary += BYTE(0x48);
							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xA5);
							o_instruction.binary += BYTE(0xC2);
							o_instruction.binary += BYTE(0x48);
							o_instruction.binary += BYTE(0xD3);
							o_instruction.binary += BYTE(0xE0);

						}
						else if (f_v->value.value[0] == 1) {

							o_instruction.binary += BYTE(0x48);
							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xA4);
							o_instruction.binary += BYTE(0xC2);
							o_instruction.binary += f_v->value.value;
							o_instruction.binary += BYTE(0x48);
							o_instruction.binary += BYTE(0xD1);
							o_instruction.binary += BYTE(0xE0);

						}
						else {

							o_instruction.binary += BYTE(0x48);
							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xA4);
							o_instruction.binary += BYTE(0xC2);
							o_instruction.binary += f_v->value.value;
							o_instruction.binary += BYTE(0x48);
							o_instruction.binary += BYTE(0xC1);
							o_instruction.binary += BYTE(0xE0);
							o_instruction.binary += f_v->value.value;

						}

					}

					o_instruction.binary += M(i_line, i_mode, f_s, 0xF7, i_op2);

					if (i_op2 < 0x30) {

						if (f_v->name[0] == '%') {

							o_instruction.binary += BYTE(0x48);
							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xAD);
							o_instruction.binary += BYTE(0xC2);
							o_instruction.binary += BYTE(0x48);
							o_instruction.binary += BYTE(0xD3);
							o_instruction.binary += BYTE(0xE8);

						}
						else if (f_v->value.value[0] == 1) {

							o_instruction.binary += BYTE(0x48);
							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xAC);
							o_instruction.binary += BYTE(0xC2);
							o_instruction.binary += f_v->value.value;
							o_instruction.binary += BYTE(0x48);
							o_instruction.binary += BYTE(0xD1);
							o_instruction.binary += BYTE(0xE8);

						}
						else {

							o_instruction.binary += BYTE(0x48);
							o_instruction.binary += BYTE(0x0F);
							o_instruction.binary += BYTE(0xAC);
							o_instruction.binary += BYTE(0xC2);
							o_instruction.binary += f_v->value.value;
							o_instruction.binary += BYTE(0x48);
							o_instruction.binary += BYTE(0xC1);
							o_instruction.binary += BYTE(0xE8);
							o_instruction.binary += f_v->value.value;

						}

					}

				}

				if (f_s->value.is()) {

					o_instruction.reqSym = SYM(f_s->value, s_ERR, o_instruction.binary.size() - 4);
				}
			
			}
			else throw(traceLine(*solved, i_line, "Argument 's' having wrong type."));
		}
		else throw(traceLine(*solved, i_line, "Wrong argument number."));

		return o_instruction;
	}

	INST ENCODED::JUMP(LINE &i_line) {

		INST o_instruction;

		if (i_line.argList.size() == 1) {

			ARGV *f_t = arg(i_line.argList, "t", 0);

			if (f_t == 0)
				throw(traceLine(*solved, i_line, "Argument 't' missing."));

			if (f_t->name[0] == '%')
				o_instruction.binary += R(i_line, 'D', f_t, 0xFF, 0x20);

			else if (f_t->name[0] == '$') {

				if (!f_t->value.is())
					throw(traceLine(*solved, i_line, "Argument 't' having wrong type."));

				o_instruction.binary += BYTE(0xE9);
				o_instruction.binary += BYTE(0x00);
				o_instruction.binary += BYTE(0x00);
				o_instruction.binary += BYTE(0x00);
				o_instruction.binary += BYTE(0x00);

				o_instruction.reqSym = SYM(f_t->value, s_ERR, o_instruction.binary.size() - 4);

				o_instruction.binary8 += BYTE(0xEB);
				o_instruction.binary8 += BYTE(0x00);

				o_instruction.reqSym8 = SYM(f_t->value, s_ERR, o_instruction.binary8.size() - 1);

			}
			else throw(traceLine(*solved, i_line, "Argument 't' having wrong type."));
		}
		else throw(traceLine(*solved, i_line, "Wrong argument number."));

		return o_instruction;
	}

	INST ENCODED::CALL(LINE &i_line) {

		INST o_instruction;

		if (i_line.argList.size() == 1) {

			ARGV *f_t = arg(i_line.argList, "t", 0);

			if (f_t == 0)
				throw(traceLine(*solved, i_line, "Argument 't' missing."));

			if (f_t->name[0] == '%')
				o_instruction.binary += R(i_line, 'D', f_t, 0xFF, 0x10);

			else if (f_t->name[0] == '$') {

				if (!f_t->value.is())
					throw(traceLine(*solved, i_line, "Argument 't' having wrong type."));

				o_instruction.binary += BYTE(0xE8);
				o_instruction.binary += BYTE(0x00);
				o_instruction.binary += BYTE(0x00);
				o_instruction.binary += BYTE(0x00);
				o_instruction.binary += BYTE(0x00);

				o_instruction.reqSym = SYM(f_t->value, s_ERR, o_instruction.binary.size() - 4);

			}
			else throw(traceLine(*solved, i_line, "Argument 't' having wrong type."));
		}
		else throw(traceLine(*solved, i_line, "Wrong argument number."));

		return o_instruction;
	}

	INST ENCODED::RETN(LINE &i_line) {

		INST o_instruction;

		if		(i_line.argList.size() == 0) {

			o_instruction.binary += BYTE(0xC3);

		}
		else if (i_line.argList.size() == 1) {

			ARGV *f_d = arg(i_line.argList, "d", 0);

			if (f_d == 0)
				throw(traceLine(*solved, i_line, "Argument 'd' missing."));

			if (f_d->name[0] != '#')
				throw(traceLine(*solved, i_line, "Argument 'd' having wrong type."));

			if (size(f_d->value.value) > 2)
				throw(traceLine(*solved, i_line, "Argument 'd' being too big."));

			if (f_d->value.value.isZero()) {

				o_instruction.binary += BYTE(0xC3);

			}
			else {

				f_d->value.value = resize(f_d->value.value, 2);

				o_instruction.binary += BYTE(0xC2);

				o_instruction.binary += f_d->value.value;

			}

		}
		else throw(traceLine(*solved, i_line, "Wrong argument number."));

		return o_instruction;
	}

	INST ENCODED::CJUMP(LINE &i_line, BYTE i_op1) {

		INST o_instruction;

		if (i_line.argList.size() == 1) {

			ARGV *f_t = arg(i_line.argList, "t", 0);

			if (f_t == 0)
				throw(traceLine(*solved, i_line, "Argument 't' missing."));

			if (f_t->name[0] != '$')
				throw(traceLine(*solved, i_line, "Argument 't' having wrong type."));

			if (!f_t->value.is())
				throw(traceLine(*solved, i_line, "Argument 't' having wrong type."));

			o_instruction.binary += BYTE(0x0F);
			o_instruction.binary += BYTE(0x80 | i_op1);
			o_instruction.binary += BYTE(0x00);
			o_instruction.binary += BYTE(0x00);
			o_instruction.binary += BYTE(0x00);
			o_instruction.binary += BYTE(0x00);

			o_instruction.reqSym = SYM(f_t->value, s_ERR, o_instruction.binary.size() - 4);

			o_instruction.binary8 += BYTE(0x70 | i_op1);
			o_instruction.binary8 += BYTE(0x00);

			o_instruction.reqSym8 = SYM(f_t->value, s_ERR, o_instruction.binary8.size() - 1);

		}
		else throw(traceLine(*solved, i_line, "Wrong argument number."));

		return o_instruction;
	}

	INST ENCODED::CSET(LINE &i_line, BYTE i_op1) {

		INST o_instruction;

		if (i_line.argList.size() == 1) {

			ARGV *f_t = arg(i_line.argList, "t", 0);

			if (f_t == 0)
				throw(traceLine(*solved, i_line, "Argument 't' missing."));

			if		(f_t->name[0] == '%') {

				o_instruction.binary += R(i_line, 'B', f_t, 0x90 | i_op1, 0x00, true);

			}
			else if	(f_t->name[0] == '$') {

				o_instruction.binary += M(i_line, 'B', f_t, 0x90 | i_op1, 0x00, true);
				
			}
			else throw(traceLine(*solved, i_line, "Argument 't' having wrong type."));

		}
		else throw(traceLine(*solved, i_line, "Wrong argument number."));

		return o_instruction;
	}

	INST ENCODED::CMOV(LINE &i_line, BYTE i_op1, BYTE i_mode) {

		INST o_instruction;

		if (i_mode == 'W') {

			o_instruction.binary += BYTE(0x66);

		}

		if (i_line.argList.size() == 2) {

			ARGV *f_t = arg(i_line.argList, "t", 0);

			if (f_t == 0)
				throw(traceLine(*solved, i_line, "Argument 't' missing."));

			if (f_t->name[0] != '%')
				throw(traceLine(*solved, i_line, "Argument 't' having wrong type."));

			ARGV *f_s = arg(i_line.argList, "s", 1);

			if (f_s == 0)
				throw(traceLine(*solved, i_line, "Argument 's' missing."));

			if		(f_s->name[0] == '%') {

				o_instruction.binary += RR(i_line, i_mode, f_s, f_t, 0x40 | i_op1, 0x00, true);

			}
			else if	(f_s->name[0] == '$') {

				o_instruction.binary += MR(i_line, i_mode, f_s, f_t, 0x40 | i_op1, 0x00, true);
				
			}
			else throw(traceLine(*solved, i_line, "Argument 's' having wrong type."));

		}
		else throw(traceLine(*solved, i_line, "Wrong argument number."));

		return o_instruction;
	}

	INST ENCODED::TEST(LINE &i_line, BYTE i_mode) {

		INST o_instruction;

		if (i_mode == 'W') {

			o_instruction.binary += BYTE(0x66);
		}

		if (i_line.argList.size() == 2) {

			ARGV *f_t = arg(i_line.argList, "t", 0);

			if (f_t == 0)
				throw(traceLine(*solved, i_line, "Argument 't' missing."));

			ARGV *f_s = arg(i_line.argList, "s", 1);

			if (f_s == 0)
				throw(traceLine(*solved, i_line, "Argument 's' missing."));

			if (f_s->name[0] == '#') {

				if (i_mode == 'B') {

					if (size(f_s->value.value) <= 1) {

						f_s->value.value = resize(f_s->value.value, 1);

					}
					else throw(traceLine(*solved, i_line, "Argument 's' being too big."));

				}
				if (i_mode == 'W') {

					if (size(f_s->value.value) <= 2) {

						f_s->value.value = resize(f_s->value.value, 2);

					}
					else throw(traceLine(*solved, i_line, "Argument 's' being too big."));

				}
				if (i_mode == 'D') {

					if (size(f_s->value.value) <= 4) {

						f_s->value.value = resize(f_s->value.value, 4);

					}
					else throw(traceLine(*solved, i_line, "Argument 's' being too big."));

				}
				if (i_mode == 'Q') {

					if (size(f_s->value.value) <= 4) {

						f_s->value.value = resize(f_s->value.value, 4);

					}
					else throw(traceLine(*solved, i_line, "Argument 's' being too big."));

				}
			}

			if (f_t->name[0] == '%') {

				if (f_s->name[0] == '#') {

					if (f_t->value.value.isZero()) {

						if (i_mode == 'B')
							o_instruction.binary += BYTE(0xA8);

						else {

							if (i_mode == 'Q')
								o_instruction.binary += REX_000(0x08);

							o_instruction.binary += BYTE(0xA9);
						}
					}
					else {

						if (i_mode == 'B')
							o_instruction.binary += R(i_line, i_mode, f_t, 0xF6);

						else
							o_instruction.binary += R(i_line, i_mode, f_t, 0xF7);
					}

					o_instruction.binary += f_s->value.value;
				}
				else if (f_s->name[0] == '%') {

					if (i_mode == 'B')
						o_instruction.binary += RR(i_line, i_mode, f_t, f_s, 0x84);

					else
						o_instruction.binary += RR(i_line, i_mode, f_t, f_s, 0x85);
				}
				else if (f_s->name[0] == '$') {

					if (i_mode == 'B')
						o_instruction.binary += MR(i_line, i_mode, f_s, f_t, 0x84);

					else
						o_instruction.binary += MR(i_line, i_mode, f_s, f_t, 0x85);

					if (f_s->value.is()) {

						o_instruction.reqSym = SYM(f_s->value, s_ERR, o_instruction.binary.size() - 4);
					}
				}
				else throw(traceLine(*solved, i_line, "Argument 's' having wrong type."));
			}
			else if (f_t->name[0] == '$') {

				if (f_s->name[0] == '#') {

					if (i_mode == 'B')
						o_instruction.binary += M(i_line, i_mode, f_t, 0xF6);

					else
						o_instruction.binary += M(i_line, i_mode, f_t, 0xF7);

					if (f_t->value.is()) {

						o_instruction.reqSym = SYM(f_t->value, s_ERR, o_instruction.binary.size() - 4);
					}

					o_instruction.binary += f_s->value.value;
				}
				else if (f_s->name[0] == '%') {

					if (i_mode == 'B')
						o_instruction.binary += MR(i_line, i_mode, f_t, f_s, 0x84);

					else
						o_instruction.binary += MR(i_line, i_mode, f_t, f_s, 0x85);

					if (f_t->value.is()) {

						o_instruction.reqSym = SYM(f_t->value, s_ERR, o_instruction.binary.size() - 4);
					}
				}
				else throw(traceLine(*solved, i_line, "Argument 's' having wrong type."));
			}
			else throw(traceLine(*solved, i_line, "Argument 't' having wrong type."));
		}
		else throw(traceLine(*solved, i_line, "Wrong argument number."));

		return o_instruction;
	}

	INST ENCODED::MOV(LINE &i_line, BYTE i_mode) {

		INST o_instruction;

		if (i_mode == 'W') {

			o_instruction.binary += BYTE(0x66);
		}

		if (i_line.argList.size() == 2) {

			ARGV *f_t = arg(i_line.argList, "t", 0);

			if (f_t == 0)
				throw(traceLine(*solved, i_line, "Argument 't' missing."));

			ARGV *f_s = arg(i_line.argList, "s", 1);

			if (f_s == 0)
				throw(traceLine(*solved, i_line, "Argument 's' missing."));

			if (f_s->name[0] == '#') {

				if (i_mode == 'B') {

					if (size(f_s->value.value) <= 1) {

						f_s->value.value = resize(f_s->value.value, 1);

					}
					else throw(traceLine(*solved, i_line, "Argument 's' being too big."));

				}
				if (i_mode == 'W') {

					if (size(f_s->value.value) <= 2) {

						f_s->value.value = resize(f_s->value.value, 2);

					}
					else throw(traceLine(*solved, i_line, "Argument 's' being too big."));

				}
				if (i_mode == 'D') {

					if (size(f_s->value.value) <= 4) {

						f_s->value.value = resize(f_s->value.value, 4);

					}
					else throw(traceLine(*solved, i_line, "Argument 's' being too big."));

				}
				if (i_mode == 'Q') {

					if (f_t->name[0] == '%') {

						if (size(f_s->value.value) <= 4 && !isNeg(f_s->value.value)) {

							f_s->value.value = resize(f_s->value.value, 4);
							i_mode = 'D';

						}
						else if (size(f_s->value.value) <= 8) {

							f_s->value.value = resize(f_s->value.value, 8);

						}
						else throw(traceLine(*solved, i_line, "Argument 's' being too big."));

					}
					else {

						if (size(f_s->value.value) <= 4) {

							f_s->value.value = resize(f_s->value.value, 4);

						}
						else throw(traceLine(*solved, i_line, "Argument 's' being too big."));
						
					}
				}
			}

			if (f_t->name[0] == '%') {

				if (f_s->name[0] == '#') {

					if (f_s->value.value.isZero())
						o_instruction.binary += RR(i_line, i_mode, f_t, f_t, 0x31);
					else {

						if (i_mode == 'B') {

							if (reg(f_t) >= 4)
								o_instruction.binary += REX_00R(reg(f_t));

							else
								o_instruction.binary += OREX_00R(reg(f_t));

							o_instruction.binary += ARG_0R(reg(f_t), 0xB0);

						}
						else {

							if (i_mode == 'Q')
								o_instruction.binary += REX_00R(reg(f_t), 0x08);

							else
								o_instruction.binary += OREX_00R(reg(f_t));

							o_instruction.binary += ARG_0R(reg(f_t), 0xB8);

						}
						o_instruction.binary += f_s->value.value;
					}
				}
				else if (f_s->name[0] == '%') {

					if (f_t->value.value[0] == f_s->value.value[0]) {

						o_instruction.binary.empty();

						return o_instruction;
					}

					if (i_mode == 'B')
						o_instruction.binary += RR(i_line, i_mode, f_t, f_s, 0x88);

					else
						o_instruction.binary += RR(i_line, i_mode, f_t, f_s, 0x89);
				}
				else if (f_s->name[0] == '$') {

					if (i_mode == 'B')
						o_instruction.binary += MR(i_line, i_mode, f_s, f_t, 0x8A);

					else
						o_instruction.binary += MR(i_line, i_mode, f_s, f_t, 0x8B);

					if (f_s->value.is()) {

						o_instruction.reqSym = SYM(f_s->value, s_ERR, o_instruction.binary.size() - 4);
					}
				}
				else throw(traceLine(*solved, i_line, "Argument 's' having wrong type."));
			}
			else if (f_t->name[0] == '$') {

				if (f_s->name[0] == '#') {

					if (i_mode == 'B')
						o_instruction.binary += M(i_line, i_mode, f_t, 0xC6);

					else
						o_instruction.binary += M(i_line, i_mode, f_t, 0xC7);

					if (f_t->value.is()) {

						o_instruction.reqSym = SYM(f_t->value, s_ERR, o_instruction.binary.size() - 4);
					}

					o_instruction.binary += f_s->value.value;
				}
				else if (f_s->name[0] == '%') {

					if (i_mode == 'B')
						o_instruction.binary += MR(i_line, i_mode, f_t, f_s, 0x88);

					else
						o_instruction.binary += MR(i_line, i_mode, f_t, f_s, 0x89);

					if (f_t->value.is()) {

						o_instruction.reqSym = SYM(f_t->value, s_ERR, o_instruction.binary.size() - 4);
					}
				}
				else throw(traceLine(*solved, i_line, "Argument 's' having wrong type."));
			}
			else throw(traceLine(*solved, i_line, "Argument 't' having wrong type."));
		}
		else throw(traceLine(*solved, i_line, "Wrong argument number."));

		return o_instruction;
	}

	INST ENCODED::MOVX(LINE& i_line, BYTE i_mode1, BYTE i_mode2, BYTE i_op1) {

		INST o_instruction;

		if (i_mode2 == 'W') {

			o_instruction.binary += BYTE(0x66);

		}

		if (i_line.argList.size() == 2) {

			ARGV *f_t = arg(i_line.argList, "t", 0);

			if (f_t == 0)
				throw(traceLine(*solved, i_line, "Argument 't' missing."));

			if (f_t->name[0] != '%')
				throw(traceLine(*solved, i_line, "Argument 't' having wrong type."));

			ARGV *f_s = arg(i_line.argList, "s", 1);

			if (f_s == 0)
				throw(traceLine(*solved, i_line, "Argument 's' missing."));

			if (f_s->name[0] == '%') {

				if 	(i_mode1 == 'B') {

					o_instruction.binary += RR(i_line, i_mode2, f_s, f_t, 0xB6 | i_op1, 0, true);

				}
				else if (i_mode1 == 'W') {

					o_instruction.binary += RR(i_line, i_mode2, f_s, f_t, 0xB7 | i_op1, 0, true);

				}
				else if (i_mode1 == 'D') {

					if (i_op1 == 0x00) {

						o_instruction.binary += RR(i_line, 'D', f_s, f_t, 0x8B, 0, false);

					}
					else {

						o_instruction.binary += RR(i_line, 'Q', f_s, f_t, 0x63, 0, false);

					}

				}

			}
			else if (f_s->name[0] == '$') {

				if 	(i_mode1 == 'B') {

					o_instruction.binary += MR(i_line, i_mode2, f_s, f_t, 0xB6 | i_op1, 0, true);

				}
				else if (i_mode1 == 'W') {

					o_instruction.binary += MR(i_line, i_mode2, f_s, f_t, 0xB7 | i_op1, 0, true);

				}
				else if (i_mode1 == 'D') {

					if (i_op1 == 0x00) {

						o_instruction.binary += MR(i_line, 'D', f_s, f_t, 0x8B, 0, false);

					}
					else {

						o_instruction.binary += MR(i_line, 'Q', f_s, f_t, 0x63, 0, false);

					}

				}

				if (f_s->value.is()) {

					o_instruction.reqSym = SYM(f_s->value, s_ERR, o_instruction.binary.size() - 4);

				}
				
			}
			else throw(traceLine(*solved, i_line, "Argument 's' having wrong type."));

		}
		else throw(traceLine(*solved, i_line, "Wrong argument number."));

		return o_instruction;

	}

	INST ENCODED::XCHG(LINE &i_line, BYTE i_mode) {

		INST o_instruction;

		if (i_mode == 'W') {

			o_instruction.binary += BYTE(0x66);
		}

		if (i_line.argList.size() == 2) {

			ARGV *f_t = arg(i_line.argList, "t", 0);

			if (f_t == 0)
				throw(traceLine(*solved, i_line, "Argument 't' missing."));

			ARGV *f_s = arg(i_line.argList, "s", 1);

			if (f_s == 0)
				throw(traceLine(*solved, i_line, "Argument 's' missing."));

			if (f_t->name[0] == '%') {

				if (f_s->name[0] == '%') {

					if (f_t->value.value[0] == f_s->value.value[0])
						return INST();

					if (i_mode == 'B')
						o_instruction.binary += RR(i_line, i_mode, f_t, f_s, 0x86);

					else if (f_t->value.value.isZero()) {

						if (i_mode == 'Q')
							o_instruction.binary += REX_00R(reg(f_s), 0x08);

						else
							o_instruction.binary += OREX_00R(reg(f_s));

						o_instruction.binary += ARG_0R(reg(f_s), 0x90);
					}
					else if (f_s->value.value.isZero()) {

						if (i_mode == 'Q')
							o_instruction.binary += REX_00R(reg(f_t), 0x08);

						else
							o_instruction.binary += OREX_00R(reg(f_t));

						o_instruction.binary += ARG_0R(reg(f_t), 0x90);
					}
					else
						o_instruction.binary += RR(i_line, i_mode, f_t, f_s, 0x87);
				}
				else if (f_s->name[0] == '$') {

					if (i_mode == 'B')
						o_instruction.binary += MR(i_line, i_mode, f_s, f_t, 0x86);

					else
						o_instruction.binary += MR(i_line, i_mode, f_s, f_t, 0x87);

					if (f_s->value.is()) {

						o_instruction.reqSym = SYM(f_s->value, s_ERR, o_instruction.binary.size() - 4);
					}
				}
				else throw(traceLine(*solved, i_line, "Argument 's' having wrong type."));
			}
			else if (f_t->name[0] == '$') {

				if (f_s->name[0] == '%') {

					if (i_mode == 'B')
						o_instruction.binary += MR(i_line, i_mode, f_t, f_s, 0x86);

					else
						o_instruction.binary += MR(i_line, i_mode, f_t, f_s, 0x87);

					if (f_t->value.is()) {

						o_instruction.reqSym = SYM(f_t->value, s_ERR, o_instruction.binary.size() - 4);
					}
				}
				else throw(traceLine(*solved, i_line, "Argument 's' having wrong type."));
			}
			else throw(traceLine(*solved, i_line, "Argument 't' having wrong type."));
		}
		else throw(traceLine(*solved, i_line, "Wrong argument number."));

		return o_instruction;
	}

	INST ENCODED::LEA(LINE &i_line) {

		INST o_instruction;

		if (i_line.argList.size() == 2) {

			ARGV *f_t = arg(i_line.argList, "t", 0);

			if (f_t == 0)
				throw(traceLine(*solved, i_line, "Argument 't' missing."));

			if (f_t->name[0] != '%')
				throw(traceLine(*solved, i_line, "Argument 't' having wrong type."));

			ARGV *f_s = arg(i_line.argList, "s", 1);

			if (f_s == 0)
				throw(traceLine(*solved, i_line, "Argument 's' missing."));

			if (f_s->name[0] != '$')
				throw(traceLine(*solved, i_line, "Argument 's' having wrong type."));

			o_instruction.binary += MR(i_line, 'Q', f_s, f_t, 0x8D);

			if (f_s->value.is()) {

				o_instruction.reqSym = SYM(f_s->value, s_ERR, o_instruction.binary.size() - 4);

			}
		}
		else throw(traceLine(*solved, i_line, "Wrong argument number."));

		return o_instruction;
	}

	INST ENCODED::PUSH(LINE &i_line, BYTE i_mode) {

		INST o_instruction;

		if (i_line.argList.size() == 1) {

			ARGV *f_s = arg(i_line.argList, "s", 0);

			if (f_s == 0)
				throw(traceLine(*solved, i_line, "Argument 's' missing."));

			if (f_s->name[0] == '#') {

				if (i_mode == 'Q') {

					if (size(f_s->value.value) <= 1 && !isNeg(f_s->value.value)) {

						f_s->value.value = resize(f_s->value.value, 1);

						o_instruction.binary += BYTE(0x6A);

					}
					else if (size(f_s->value.value) <= 4) {

						f_s->value.value = resize(f_s->value.value, 4);

						o_instruction.binary += BYTE(0x68);

					}
					else throw(traceLine(*solved, i_line, "Argument 's' being too big."));

				}
				else if (i_mode == 'B') {

					if (size(f_s->value.value) <= 1) {

						f_s->value.value = resize(f_s->value.value, 1);

					}
					else throw(traceLine(*solved, i_line, "Argument 'd' being too big."));

					o_instruction.binary += BYTE(0x48);
					o_instruction.binary += BYTE(0x83);
					o_instruction.binary += BYTE(0xEC);
					o_instruction.binary += BYTE(0x01);

					o_instruction.binary += BYTE(0xC6);
					o_instruction.binary += BYTE(0x04);
					o_instruction.binary += BYTE(0x24);
				}
				else if (i_mode == 'W') {

					if (size(f_s->value.value) <= 2) {

						f_s->value.value = resize(f_s->value.value, 2);

					}
					else throw(traceLine(*solved, i_line, "Argument 'd' being too big."));

					o_instruction.binary += BYTE(0x48);
					o_instruction.binary += BYTE(0x83);
					o_instruction.binary += BYTE(0xEC);
					o_instruction.binary += BYTE(0x02);

					o_instruction.binary += BYTE(0x66);
					o_instruction.binary += BYTE(0xC7);
					o_instruction.binary += BYTE(0x04);
					o_instruction.binary += BYTE(0x24);
				}
				else if (i_mode == 'D') {

					if (size(f_s->value.value) <= 4) {

						f_s->value.value = resize(f_s->value.value, 4);

					}
					else throw(traceLine(*solved, i_line, "Argument 'd' being too big."));

					o_instruction.binary += BYTE(0x48);
					o_instruction.binary += BYTE(0x83);
					o_instruction.binary += BYTE(0xEC);
					o_instruction.binary += BYTE(0x04);

					o_instruction.binary += BYTE(0xC7);
					o_instruction.binary += BYTE(0x04);
					o_instruction.binary += BYTE(0x24);
				}

				o_instruction.binary += f_s->value.value;
			}
			else if (f_s->name[0] == '%') {

				if (i_mode == 'Q') {

					o_instruction.binary += OREX_00R(reg(f_s));
					o_instruction.binary += ARG_0R(reg(f_s), 0x50);
				}
				else if (i_mode == 'B') {

					o_instruction.binary += BYTE(0x48);
					o_instruction.binary += BYTE(0x83);
					o_instruction.binary += BYTE(0xEC);
					o_instruction.binary += BYTE(0x01);

					if (reg(f_s) >= 4)
						o_instruction.binary += REX_R00(reg(f_s));

					else
						o_instruction.binary += OREX_R00(reg(f_s));

					o_instruction.binary += BYTE(0x88);
					o_instruction.binary += ARG_R0(reg(f_s), 0x04);
					o_instruction.binary += BYTE(0x24);
				}
				else if (i_mode == 'W') {

					o_instruction.binary += BYTE(0x48);
					o_instruction.binary += BYTE(0x83);
					o_instruction.binary += BYTE(0xEC);
					o_instruction.binary += BYTE(0x02);

					o_instruction.binary += BYTE(0x66);
					o_instruction.binary += OREX_R00(reg(f_s));
					o_instruction.binary += BYTE(0x89);
					o_instruction.binary += ARG_R0(reg(f_s), 0x04);
					o_instruction.binary += BYTE(0x24);
				}
				else if (i_mode == 'D') {

					o_instruction.binary += BYTE(0x48);
					o_instruction.binary += BYTE(0x83);
					o_instruction.binary += BYTE(0xEC);
					o_instruction.binary += BYTE(0x04);

					o_instruction.binary += OREX_R00(reg(f_s));
					o_instruction.binary += BYTE(0x89);
					o_instruction.binary += ARG_R0(reg(f_s), 0x04);
					o_instruction.binary += BYTE(0x24);
				}
			}
			else throw(traceLine(*solved, i_line, "Argument 's' having wrong type."));
		}
		else throw(traceLine(*solved, i_line, "Wrong argument number."));

		return o_instruction;
	}

	INST ENCODED::POP(LINE &i_line, BYTE i_mode) {

		INST o_instruction;

		if (i_line.argList.size() == 1) {

			ARGV *f_t = arg(i_line.argList, "t", 0);

			if (f_t == 0)
				throw(traceLine(*solved, i_line, "Argument 't' missing."));

			if (f_t->name[0] != '%')
				throw(traceLine(*solved, i_line, "Argument 't' having wrong type."));

			if (i_mode == 'Q') {

				o_instruction.binary += OREX_00R(reg(f_t));
				o_instruction.binary += ARG_0R(reg(f_t), 0x58);
			}
			else if (i_mode == 'B') {

				if (reg(f_t) >= 4)
					o_instruction.binary += REX_R00(reg(f_t));

				else
					o_instruction.binary += OREX_R00(reg(f_t));

				o_instruction.binary += BYTE(0x8A);
				o_instruction.binary += ARG_R0(reg(f_t), 0x04);
				o_instruction.binary += BYTE(0x24);

				o_instruction.binary += BYTE(0x48);
				o_instruction.binary += BYTE(0x83);
				o_instruction.binary += BYTE(0xC4);
				o_instruction.binary += BYTE(0x01);
			}
			else if (i_mode == 'W') {

				o_instruction.binary += BYTE(0x66);
				o_instruction.binary += OREX_R00(reg(f_t));
				o_instruction.binary += BYTE(0x8B);
				o_instruction.binary += ARG_R0(reg(f_t), 0x04);
				o_instruction.binary += BYTE(0x24);

				o_instruction.binary += BYTE(0x48);
				o_instruction.binary += BYTE(0x83);
				o_instruction.binary += BYTE(0xC4);
				o_instruction.binary += BYTE(0x02);
			}
			else if (i_mode == 'D') {

				o_instruction.binary += OREX_R00(reg(f_t));
				o_instruction.binary += BYTE(0x8B);
				o_instruction.binary += ARG_R0(reg(f_t), 0x04);
				o_instruction.binary += BYTE(0x24);

				o_instruction.binary += BYTE(0x48);
				o_instruction.binary += BYTE(0x83);
				o_instruction.binary += BYTE(0xC4);
				o_instruction.binary += BYTE(0x04);
			}
		}
		else throw(traceLine(*solved, i_line, "Wrong argument number."));

		return o_instruction;
	}

	INST ENCODED::FENTER(LINE &i_line) {

		INST o_instruction;

		if (i_line.argList.size() == 0) {

			o_instruction.binary += BYTE(0x55);
			o_instruction.binary += BYTE(0x48);
			o_instruction.binary += BYTE(0x89);
			o_instruction.binary += BYTE(0xE5);

		}
		else throw(traceLine(*solved, i_line, "Wrong argument number."));

		return o_instruction;
	}

	INST ENCODED::FLEAVE(LINE &i_line) {

		INST o_instruction;

		if (i_line.argList.size() == 0) {

			o_instruction.binary += BYTE(0x48);
			o_instruction.binary += BYTE(0x89);
			o_instruction.binary += BYTE(0xEC);
			o_instruction.binary += BYTE(0x5D);
		}
		else throw(traceLine(*solved, i_line, "Wrong argument number."));

		return o_instruction;
	}

	INST ENCODED::FGROW(LINE &i_line) {

		INST o_instruction;

		if (i_line.argList.size() == 1) {

			ARGV *f_v = arg(i_line.argList, "v", 0);

			if (f_v == 0)
				throw(traceLine(*solved, i_line, "Argument 'v' missing."));

			if (f_v->name[0] == '#') {

				o_instruction.binary += BYTE(0x48);

				if (f_v->value.value.size() <= 1) {

					f_v->value.value = resize(f_v->value.value, 1);

					o_instruction.binary += BYTE(0x83);
				}
				else if (size(f_v->value.value) <= 4) {

					f_v->value.value = resize(f_v->value.value, 4);

					o_instruction.binary += BYTE(0x81);
				}
				else throw(traceLine(*solved, i_line, "Argument 's' being too big."));

				o_instruction.binary += BYTE(0xEC);
				o_instruction.binary += f_v->value.value;
			}
			else throw(traceLine(*solved, i_line, "Argument 'v' having wrong type."));
		}
		else throw(traceLine(*solved, i_line, "Wrong argument number."));

		return o_instruction;
	}

	INST ENCODED::FSHRINK(LINE &i_line) {

		INST o_instruction;

		if (i_line.argList.size() == 1) {

			ARGV *f_v = arg(i_line.argList, "v", 0);

			if (f_v == 0)
				throw(traceLine(*solved, i_line, "Argument 'v' missing."));

			if (f_v->name[0] == '#') {

				o_instruction.binary += BYTE(0x48);

				if (f_v->value.value.size() <= 1) {

					f_v->value.value = resize(f_v->value.value, 1);

					o_instruction.binary += BYTE(0x83);
				}
				else if (size(f_v->value.value) <= 4) {

					f_v->value.value = resize(f_v->value.value, 4);

					o_instruction.binary += BYTE(0x81);
				}
				else throw(traceLine(*solved, i_line, "Argument 's' being too big."));

				o_instruction.binary += BYTE(0xC4);
				o_instruction.binary += f_v->value.value;
			}
			else throw(traceLine(*solved, i_line, "Argument 'v' having wrong type."));
		}
		else throw(traceLine(*solved, i_line, "Wrong argument number."));

		return o_instruction;
	}

	INST ENCODED::FALIGN(LINE &i_line) {

		INST o_instruction;

		if (i_line.argList.size() == 0) {

			o_instruction.binary += BYTE(0x40);
			o_instruction.binary += BYTE(0x80);
			o_instruction.binary += BYTE(0xE4);
			o_instruction.binary += BYTE(0xF0);
		}
		else throw(traceLine(*solved, i_line, "Wrong argument number."));

		return o_instruction;
	}

	STRING ENCODED::resize(STRING &i_binary, DWORD i_size) {

		STRING o_binary = i_binary;

		if (o_binary.size() > i_size) {

			o_binary = o_binary.to(i_size);

		}
		else if (o_binary.size() < i_size) {

			BYTE f_filler = 0x00;

			if (o_binary.size() > 0 && o_binary[-1] >= 0x80)
				f_filler = 0xFF;

			while (o_binary.size() < i_size) {

				o_binary += f_filler;

			}

		}

		return o_binary;
	}

	DWORD ENCODED::size(STRING &i_binary) {

		for (BYTE* f1_char = i_binary.end() - 1; f1_char >= i_binary.begin(); f1_char --) {

			if (*f1_char != 0x00) {

				return f1_char + 1 - i_binary.begin();

			}

		}

		return 0;

	}

	BOOL ENCODED::isNeg(STRING& i_binary) {

		if (i_binary.size() == 0) return false;

		return (i_binary[-1] >= 0x80);

	}

	BYTE ENCODED::reg(ARGV *i_r) {

		/*
			Register number from argument
		*/

		BYTE o_byte = i_r->value.value[0];

		if		(o_byte < 8 ) o_byte += 8;
		else if (o_byte < 10) o_byte -= 2;
		else if (o_byte < 14) o_byte -= 10;
		else throw(tracePointer(*solved->parsed->lexed->loaded, i_r->value.pointer, "Invalid register."));

		return o_byte;
	}

	STRING ENCODED::M(LINE &i_line, BYTE i_mode, ARGV *i_m, BYTE i_op1, BYTE i_op2, BOOL i_sec) {

		/*
			Memory operand scheme
		*/

		STRING o_string;

		if (i_m->value.is()) {

			if (i_mode == 'Q') {

				o_string += REX_000(0x08);
			}

			if (i_sec) {

				o_string += BYTE(0x0F);

			}
			
			o_string += BYTE(i_op1);
			o_string += ARG_00(0x05 | i_op2);
			o_string += BYTE(0x00);
			o_string += BYTE(0x00);
			o_string += BYTE(0x00);
			o_string += BYTE(0x00);
		}
		else {

			ARGV *f_mb = arg(i_m->argList, "b");
			ARGV *f_mi = arg(i_m->argList, "i");
			ARGV *f_mm = arg(i_m->argList, "m");
			ARGV *f_mo = arg(i_m->argList, "o");

			BYTE f_mbOp = 0;
			BYTE f_miOp = 0;
			BYTE f_mmOp = 0x00;
			BYTE f_moOp = 0x00;
			STRING f_moStr;

			if (f_mb) {

				f_mbOp = reg(f_mb);
			}
			else {

				if (i_m->argList[0].value.content == "SB") {

					f_mbOp = 5;
				}
				else {

					f_mbOp = 4;
				}
			}

			if (f_mi) {

				f_miOp = reg(f_mi);
			}

			if (f_mm) {

				if (f_mm->value.value[0] == 0)
					f_mmOp = 0;
				else if (f_mm->value.value[0] == 2)
					f_mmOp = 0x40;
				else if (f_mm->value.value[0] == 4)
					f_mmOp = 0x80;
				else if (f_mm->value.value[0] == 8)
					f_mmOp = 0xC0;
			}

			if (f_mo) {

				if (size(f_mo->value.value) <= 1) {

					f_moOp = 0x40;

					f_moStr = resize(f_mo->value.value, 1);

					if (f_mo->value.value.isZero()) {

						f_moOp = 0x00;

						f_mo = 0;
					}

					if (!f_mb) {

						f_moOp = 0x40;
					}

				}
				else if (size(f_mo->value.value) <= 4) {

					f_moOp = 0x80;

					f_moStr = resize(f_mo->value.value, 4);

				}
				else throw(traceLine(*solved, i_line, "Argument '" + i_m->name.sub(1, i_m->name.size() - 1) + ".o' being too big."));

			}
			else if ((f_mbOp & 0b111) == 5) {

				f_moOp = 0x40;

				f_moStr = BYTE(0x00);
			}

			if (f_mi) {

				if (i_mode == 'Q')
					o_string += REX_0RR(f_miOp, f_mbOp, 0x08);

				else
					o_string += OREX_0RR(f_miOp, f_mbOp);

				if (i_sec) {

					o_string += BYTE(0x0F);

				}
			

				o_string += BYTE(i_op1);

				o_string += ARG_00(0x04 | f_moOp | i_op2);
				o_string += ARG_RR(f_miOp, f_mbOp, f_mmOp);
			}
			else {

				if (i_mode == 'Q')
					o_string += REX_00R(f_mbOp, 0x08);

				else
					o_string += OREX_00R(f_mbOp);

				if (i_sec) {

					o_string += BYTE(0x0F);

				}			

				o_string += BYTE(i_op1);
				o_string += ARG_0R(f_mbOp, f_moOp | i_op2);

				if ((f_mbOp & 0b111) == 4)
					o_string += BYTE(0x24);
			}

			o_string += f_moStr;
		}

		return o_string;
	}

	STRING ENCODED::MR(LINE &i_line, BYTE i_mode, ARGV *i_m, ARGV *i_r, BYTE i_op1, BYTE i_op2, BOOL i_sec) {

		/*
			Memory + register operand scheme
		*/

		STRING o_string;

		BYTE f_r = reg(i_r);

		if (i_m->value.is()) {

			if (i_mode == 'Q') {

				o_string += REX_R00(f_r, 0x08);
			}
			else if (
				i_mode == 'B' &&
				f_r >= 4
			) {

				o_string += REX_R00(f_r);

			}
			else {

				o_string += OREX_R00(f_r);
			}

			if (i_sec) {

				o_string += BYTE(0x0F);

			}
				
			o_string += BYTE(i_op1);
			o_string += ARG_R0(f_r, 0x05 | i_op2);
			o_string += BYTE(0x00);
			o_string += BYTE(0x00);
			o_string += BYTE(0x00);
			o_string += BYTE(0x00);
		}
		else {

			ARGV *f_mb = arg(i_m->argList, "b");
			ARGV *f_mi = arg(i_m->argList, "i");
			ARGV *f_mm = arg(i_m->argList, "m");
			ARGV *f_mo = arg(i_m->argList, "o");

			BYTE f_mbOp = 0;
			BYTE f_miOp = 0;
			BYTE f_mmOp = 0x00;
			BYTE f_moOp = 0x00;
			STRING f_moStr;

			if (f_mb) {

				f_mbOp = reg(f_mb);
			}
			else {

				if (i_m->argList[0].value.content == "SB") {

					f_mbOp = 5;
				}
				else {

					f_mbOp = 4;
				}
			}

			if (f_mi) {

				f_miOp = reg(f_mi);
			}

			if (f_mm) {

				if (f_mm->value.value[0] == 0)
					f_mmOp = 0;
				else if (f_mm->value.value[0] == 2)
					f_mmOp = 0x40;
				else if (f_mm->value.value[0] == 4)
					f_mmOp = 0x80;
				else if (f_mm->value.value[0] == 8)
					f_mmOp = 0xC0;
			}

			if (f_mo) {

				if (size(f_mo->value.value) <= 1) {

					f_moOp = 0x40;

					f_moStr = resize(f_mo->value.value, 1);

					if (f_mo->value.value.isZero()) {

						f_moOp = 0x00;

						f_mo = 0;
					}

					if (!f_mb) {

						f_moOp = 0x40;
					}

				}
				else if (size(f_mo->value.value) <= 4) {

					f_moOp = 0x80;

					f_moStr = resize(f_mo->value.value, 4);
				}
				else {

					throw(traceLine(*solved, i_line, "Argument '" + i_m->name.sub(1, i_m->name.size() - 1) + ".o' being too big."));
				}
			}
			else if ((f_mbOp & 0b111) == 5) {

				f_moOp = 0x40;

				f_moStr = BYTE(0x00);
			}

			if (f_mi) {

				if (i_mode == 'Q')
					o_string += REX_RRR(f_r, f_miOp, f_mbOp, 0x08);

				else if (
					i_mode == 'B' &&
					f_r >= 4
				)

					o_string += REX_RRR(f_r, f_miOp, f_mbOp);

				else
					o_string += OREX_RRR(f_r, f_miOp, f_mbOp);

				if (i_sec) {

					o_string += BYTE(0x0F);

				}
				
				o_string += BYTE(i_op1);

				o_string += ARG_R0(f_r, 0x04 | f_moOp | i_op2);
				o_string += ARG_RR(f_miOp, f_mbOp, f_mmOp);
			}
			else {

				if (i_mode == 'Q')
					o_string += REX_R0R(f_r, f_mbOp, 0x08);

				else if (
					i_mode == 'B' &&
					f_r >= 4
				)

					o_string += REX_R0R(f_r, f_mbOp);

				else
					o_string += OREX_R0R(f_r, f_mbOp);

				if (i_sec) {

					o_string += BYTE(0x0F);

				}
				
				o_string += BYTE(i_op1);
				o_string += ARG_RR(f_r, f_mbOp, f_moOp | i_op2);

				if ((f_mbOp & 0b111) == 4)
					o_string += BYTE(0x24);
			}

			o_string += f_moStr;
		}

		return o_string;
	}

	STRING ENCODED::R(LINE &i_line, BYTE i_mode, ARGV *i_r, BYTE i_op1, BYTE i_op2, BOOL i_sec) {

		/*
			Register operand scheme
		*/

		STRING o_string;

		BYTE f_r = reg(i_r);

		if (i_mode == 'Q')
			o_string += REX_00R(f_r, 0x08);

		else if (
			i_mode == 'B' &&
			f_r >= 4
		)
			o_string += REX_00R(f_r);

		else
			o_string += OREX_00R(f_r);

		if (i_sec) {

			o_string += BYTE(0x0F);

		}
		
		o_string += BYTE(i_op1);
		o_string += ARG_0R(f_r, i_op2 | 0xC0);

		return o_string;
	}

	STRING ENCODED::RR(LINE &i_line, BYTE i_mode, ARGV *i_r1, ARGV *i_r2, BYTE i_op1, BYTE i_op2, BOOL i_sec) {

		/*
			Register + register operand scheme
		*/

		STRING o_string;

		BYTE f_r1 = reg(i_r1);
		BYTE f_r2 = reg(i_r2);

		if (i_mode == 'Q')
			o_string += REX_R0R(f_r2, f_r1, 0x08);

		else if (
			i_mode == 'B' &&
			(
				f_r1 >= 4 ||
				f_r2 >= 4
			)
		)
			o_string += REX_R0R(f_r2, f_r1);

		else
			o_string += OREX_R0R(f_r2, f_r1);

		if (i_sec) {

			o_string += BYTE(0x0F);

		}
		
		o_string += BYTE(i_op1);
		o_string += ARG_RR(f_r2, f_r1, i_op2 | 0xC0);

		return o_string;
	}

	STRING ENCODED::OREX_00R(BYTE i_r3, BYTE i_v) {

		/*
			Optional REX byte
		*/

		STRING o_string;

		if (i_r3 > 7) {

			o_string += BYTE(0x40 | ((i_r3 & 0b1000) >> 3) | i_v);
		}

		return o_string;
	}

	STRING ENCODED::OREX_0R0(BYTE i_r2, BYTE i_v) {

		/*
			Optional REX byte
		*/

		STRING o_string;

		if (i_r2 > 7) {

			o_string += BYTE(0x40 | ((i_r2 & 0b1000) >> 2) | i_v);
		}

		return o_string;
	}

	STRING ENCODED::OREX_R00(BYTE i_r1, BYTE i_v) {

		/*
			Optional REX byte
		*/

		STRING o_string;

		if (i_r1 > 7) {

			o_string += BYTE(0x40 | ((i_r1 & 0b1000) >> 1) | i_v);
		}

		return o_string;
	}

	STRING ENCODED::OREX_0RR(BYTE i_r2, BYTE i_r3, BYTE i_v) {

		/*
			Optional REX byte
		*/

		STRING o_string;

		if (i_r2 > 7 || i_r3 > 7) {

			o_string += BYTE(0x40 | ((i_r2 & 0b1000) >> 2) | ((i_r3 & 0b1000) >> 3) | i_v);
		}

		return o_string;
	}

	STRING ENCODED::OREX_R0R(BYTE i_r1, BYTE i_r3, BYTE i_v) {

		/*
			Optional REX byte
		*/

		STRING o_string;

		if (i_r1 > 7 || i_r3 > 7) {

			o_string += BYTE(0x40 | ((i_r1 & 0b1000) >> 1) | ((i_r3 & 0b1000) >> 3) | i_v);
		}

		return o_string;
	}

	STRING ENCODED::OREX_RR0(BYTE i_r1, BYTE i_r2, BYTE i_v) {

		/*
			Optional REX byte
		*/

		STRING o_string;

		if (i_r1 > 7 || i_r2 > 7) {

			o_string += BYTE(0x40 | ((i_r1 & 0b1000) >> 1) | ((i_r2 & 0b1000) >> 2) | i_v);
		}

		return o_string;
	}

	STRING ENCODED::OREX_RRR(BYTE i_r1, BYTE i_r2, BYTE i_r3, BYTE i_v) {

		/*
			Optional REX byte
		*/

		STRING o_string;

		if (i_r1 > 7 || i_r2 > 7 || i_r3 > 7) {

			o_string += BYTE(0x40 | ((i_r1 & 0b1000) >> 1) | ((i_r2 & 0b1000) >> 2) | ((i_r3 & 0b1000) >> 3) | i_v);
		}

		return o_string;
	}

	STRING ENCODED::REX_000(BYTE i_v) {

		/*
			Mandatory REX byte
		*/

		STRING o_string;

		o_string += BYTE(0x40 | i_v);

		return o_string;
	}

	STRING ENCODED::REX_00R(BYTE i_r3, BYTE i_v) {

		/*
			Mandatory REX byte
		*/

		STRING o_string;

		o_string += BYTE(0x40 | ((i_r3 & 0b1000) >> 3) | i_v);

		return o_string;
	}

	STRING ENCODED::REX_0R0(BYTE i_r2, BYTE i_v) {

		/*
			Mandatory REX byte
		*/

		STRING o_string;

		o_string += BYTE(0x40 | ((i_r2 & 0b1000) >> 2) | i_v);

		return o_string;
	}

	STRING ENCODED::REX_R00(BYTE i_r1, BYTE i_v) {

		/*
			Mandatory REX byte
		*/

		STRING o_string;

		o_string += BYTE(0x40 | ((i_r1 & 0b1000) >> 1) | i_v);

		return o_string;
	}

	STRING ENCODED::REX_0RR(BYTE i_r2, BYTE i_r3, BYTE i_v) {

		/*
			Mandatory REX byte
		*/

		STRING o_string;

		o_string += BYTE(0x40 | ((i_r2 & 0b1000) >> 2) | ((i_r3 & 0b1000) >> 3) | i_v);

		return o_string;
	}

	STRING ENCODED::REX_R0R(BYTE i_r1, BYTE i_r3, BYTE i_v) {

		/*
			Mandatory REX byte
		*/

		STRING o_string;

		o_string += BYTE(0x40 | ((i_r1 & 0b1000) >> 1) | ((i_r3 & 0b1000) >> 3) | i_v);

		return o_string;
	}

	STRING ENCODED::REX_RR0(BYTE i_r1, BYTE i_r2, BYTE i_v) {

		/*
			Mandatory REX byte
		*/

		STRING o_string;

		o_string += BYTE(0x40 | ((i_r1 & 0b1000) >> 1) | ((i_r2 & 0b1000) >> 2) | i_v);

		return o_string;
	}

	STRING ENCODED::REX_RRR(BYTE i_r1, BYTE i_r2, BYTE i_r3, BYTE i_v) {

		/*
			Mandatory REX byte
		*/

		STRING o_string;

		o_string += BYTE(0x40 | ((i_r1 & 0b1000) >> 1) | ((i_r2 & 0b1000) >> 2) | ((i_r3 & 0b1000) >> 3) | i_v);

		return o_string;
	}

	STRING ENCODED::ARG_00(BYTE i_v) {

		/*
			void argument byte
		*/

		STRING o_string;

		o_string += BYTE(i_v);

		return o_string;
	}

	STRING ENCODED::ARG_0R(BYTE i_r2, BYTE i_v) {

		/*
			right register argument byte
		*/

		STRING o_string;

		o_string += BYTE((i_r2 & 0b00000111) | i_v);

		return o_string;
	}

	STRING ENCODED::ARG_R0(BYTE i_r1, BYTE i_v) {

		/*
			left register argument byte
		*/

		STRING o_string;

		o_string += BYTE(((i_r1 << 3) & 0b00111000) | i_v);

		return o_string;
	}

	STRING ENCODED::ARG_RR(BYTE i_r1, BYTE i_r2, BYTE i_v) {

		/*
			both register argument byte
		*/

		STRING o_string;

		o_string += BYTE(((i_r1 << 3) & 0b00111000) | (i_r2 & 0b00000111) | i_v);

		return o_string;
	}

	STRING ENCODED::string() {

		STRING o_string;

		for (INST *f1_inst = instList.begin(); f1_inst < instList.end(); f1_inst++) {

			for (SYM *f2_sym = f1_inst->defSymList.begin(); f2_sym < f1_inst->defSymList.end(); f2_sym ++) {

				o_string += f2_sym->token.content;

				o_string += " :\n";

			}

			if (f1_inst->binary.size() > 0) {

				o_string += f1_inst->binary.hex(" ");

				if (f1_inst->binary8.size() > 0) {

					o_string += " | ";

					o_string += f1_inst->binary8.hex(" ");

				}

				if (f1_inst->reqSym.token.is()) {

					o_string += " : ";

					o_string += f1_inst->reqSym.token.content;

				}

				if (f1_inst + 1 < instList.end()) {

					o_string += "\n";

				}

			}

		}

		return o_string;
		
	}

}