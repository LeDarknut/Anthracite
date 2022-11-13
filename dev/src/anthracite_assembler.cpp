#include <anthracite_assembler.hpp>

namespace ANTHRACITE {

	ASSEMBLED::ASSEMBLED(ENCODED &i_encoded) : encoded(&i_encoded), sectionList(), defList(), reqList() {

		/*
			Merge the machine code
			Solve relative symbols
			Construct the object file
		*/

		for (INST *f1_inst = encoded->instList.begin(); f1_inst < encoded->instList.end(); f1_inst++) {

			if (f1_inst->section <= s_GLB) continue;

			while (f1_inst->section >= sectionList.size()) {

				sectionList += STRING();

			}

			if (f1_inst->reqSym8.token.is()) {

				if (f1_inst->reqSym.section > s_GLB) {

					try {

						SDWORD f1_offset = symOffset(f1_inst);

						if (f1_offset < -128 || f1_offset > 127) {

							f1_inst->binary8.empty();
							f1_inst->reqSym8 = SYM();

						}
						else {

							f1_inst->binary = f1_inst->binary8;

						}

					}
					catch (BYTE f_e) {

						throw(traceToken(*encoded->solved->parsed->lexed, f1_inst->reqSym.token, "Symbol unreachable."));

					}
					
				}

			}

		}

		for (INST *f1_inst = encoded->instList.begin(); f1_inst < encoded->instList.end(); f1_inst++) {

			if (f1_inst->section <= s_GLB) continue;

			if (f1_inst->reqSym.token.is()) {

				if (f1_inst->reqSym.section > s_GLB) {

					try {

						SDWORD f1_offset = symOffset(f1_inst);

						if (f1_inst->reqSym8.token.is()) {

							f1_inst->binary[f1_inst->reqSym8.offset  ] = (f1_offset & 0x000000FF);

						}
						else {

							f1_inst->binary[f1_inst->reqSym.offset    ] = (f1_offset & 0x000000FF);
							f1_inst->binary[f1_inst->reqSym.offset + 1] = (f1_offset & 0x0000FF00) >> 8;
							f1_inst->binary[f1_inst->reqSym.offset + 2] = (f1_offset & 0x00FF0000) >> 16;
							f1_inst->binary[f1_inst->reqSym.offset + 3] = (f1_offset & 0xFF000000) >> 24;
							
						}

						f1_inst->reqSym = SYM();
						f1_inst->reqSym8 = SYM();

					}
					catch (BYTE f_e) {

						throw(traceToken(*encoded->solved->parsed->lexed, f1_inst->reqSym.token, "Symbol unreachable."));

					}

				}
				else {

					f1_inst->reqSym.roffset = f1_inst->binary.size() - 4 - f1_inst->reqSym.offset;

					f1_inst->reqSym.offset += sectionList[f1_inst->section].size();

				}

			}
			
			sectionList[f1_inst->section] += f1_inst->binary;

		}

		for (INST *f1_inst = encoded->instList.begin(); f1_inst < encoded->instList.end(); f1_inst++) {

			if (f1_inst->section <= s_GLB) continue;

			addDef(f1_inst);

			addReq(f1_inst);

		}
		
	}

	SDWORD ASSEMBLED::symOffset(INST *i_inst) {

		/*
			Compute the number of byte that separates a symbol target from its requested location
		*/

		SDWORD o_offset = 0;

		DWORD f_private = 0;

		INST *f_target = 0;

		for (INST *f1_inst = i_inst; f1_inst >= encoded->instList.begin(); f1_inst --) {

			if (f1_inst->section == s_IN || f1_inst->section == s_OUT) {

				if (f1_inst->section == s_OUT)
					f_private++;
				else if (f_private > 0)
					f_private--;

			}
			else if (f1_inst->section == i_inst->reqSym.section) {

				o_offset -= f1_inst->binary.size();

				if (!f_private) {

					for (SYM *f2_sym = f1_inst->defSymList.begin(); f2_sym < f1_inst->defSymList.end(); f2_sym++) {

						if (f2_sym->token.pointer == i_inst->reqSym.token.pointer) {

							if (i_inst->section == i_inst->reqSym.section) return o_offset;

							f_target = f1_inst;

						}

					}

				}

			}
			
		}

		if (f_target == 0) {

			o_offset = 0;

			f_private = 0;

			for (INST *f1_inst = i_inst + 1; f1_inst < encoded->instList.end(); f1_inst ++) {

				if (f1_inst->section == s_IN || f1_inst->section == s_OUT) {

					if (f1_inst->section == s_IN)
						f_private++;
					else if (f_private > 0)
						f_private--;

				}
				else if (f1_inst->section == i_inst->reqSym.section) {

					if (!f_private) {

						for (SYM *f2_sym = f1_inst->defSymList.begin(); f2_sym < f1_inst->defSymList.end(); f2_sym++) {

							if (f2_sym->token.pointer == i_inst->reqSym.token.pointer) {

								if (i_inst->section == i_inst->reqSym.section) return o_offset;

								f_target = f1_inst;

							}

						}

					}

					o_offset += f1_inst->binary.size();

				}
				
			}

		}

		if (f_target == 0) {

			throw(BYTE(1));

		}

		o_offset = 0;

		if (i_inst->section > i_inst->reqSym.section) {

			for (INST *f1_inst = encoded->instList.begin(); f1_inst < encoded->instList.end(); f1_inst ++) {

				if (
					(
						f1_inst->section < i_inst->section &&
						f1_inst->section > i_inst->reqSym.section
					) ||
					(
						f1_inst->section == i_inst->section &&
						f1_inst <= i_inst
					) ||
					(
						f1_inst->section == i_inst->reqSym.section &&
						f1_inst >= f_target
					)
				) {

					o_offset -= f1_inst->binary.size();

				}
				
			}

		}
		else {

			for (INST *f1_inst = encoded->instList.begin(); f1_inst < encoded->instList.end(); f1_inst ++) {

				if (
					(
						f1_inst->section > i_inst->section &&
						f1_inst->section < i_inst->reqSym.section
					) ||
					(
						f1_inst->section == i_inst->section &&
						f1_inst > i_inst
					) ||
					(
						f1_inst->section == i_inst->reqSym.section &&
						f1_inst < f_target
					)
				) {

					o_offset += f1_inst->binary.size();

				}
				
			}

		}

		return o_offset;

	}

	VOID ASSEMBLED::addDef(INST* i_inst) {

		/*
			Append a symbol definition to the symbol table
		*/

		DWORD f_offset = 0;

		for (INST *f1_inst = i_inst; f1_inst >= encoded->instList.begin(); f1_inst --) {

			if (f1_inst->section <= s_GLB) continue;

			if (f1_inst->section == i_inst->section) {

				f_offset += f1_inst->binary.size();

			}
			
		}

		for (INST *f1_inst = encoded->instList.begin(); f1_inst < encoded->instList.end(); f1_inst ++) {

			if (f1_inst->section < i_inst->section) {

				f_offset += f1_inst->binary.size();

			}
			
		}

		for (SYM* f1_sym = i_inst->defSymList.begin(); f1_sym < i_inst->defSymList.end(); f1_sym ++) {

			if (f1_sym->token.isGlb()) {

				defList += DEFT(i_inst->section, f_offset + f1_sym->offset, f1_sym->token);

			}

		}

		if (i_inst->reqSym.section == s_GLB) {

			DEFT* f1_deft = defList.begin();

			for (;f1_deft < defList.end(); f1_deft ++) {

				if (i_inst->reqSym.token.content == f1_deft->token.content) break;

			}

			if (f1_deft >= defList.end()) {

				defList += DEFT(s_GLB, 0, i_inst->reqSym.token);

			}

		}

	}

	VOID ASSEMBLED::addReq(INST* i_inst) {

		/*
			Append a symbol requirement to the redirection table
		*/

		DWORD f_offset = 0;

		for (DWORD f1_sectionId = 0; f1_sectionId < i_inst->section; f1_sectionId ++) {

			f_offset += sectionList[f1_sectionId].size();

		}

		if (i_inst->reqSym.section == s_GLB) {

			reqList += REQT(i_inst->section, f_offset + i_inst->reqSym.offset, i_inst->reqSym.roffset, i_inst->reqSym.token);

		}

	}

	STRING ASSEMBLED::assemble() {

		/*
			Construct the object file
		*/

		STRING o_string;

		STRING f_sectionTable = assemble_code();

		STRING f_reqTable = assemble_req();

		STRING f_defTable = assemble_def();
		
		o_string += WORD(0x8664);
		o_string += WORD(1);
		o_string += DWORD(0);
		o_string += DWORD(60 + f_sectionTable.size() + f_reqTable.size());
		o_string += DWORD(defList.size());
		o_string += WORD(0);
		o_string += WORD(0);
		o_string += DWORD(0);
		o_string += DWORD(0);
		o_string += DWORD(0);
		o_string += DWORD(0);
		o_string += DWORD(f_sectionTable.size());
		o_string += DWORD(60);
		o_string += DWORD(60 + f_sectionTable.size());
		o_string += DWORD(0);
		o_string += WORD(reqList.size());
		o_string += WORD(0);
		o_string += DWORD(0xE0500060);
		o_string += f_sectionTable;
		o_string += f_reqTable;
		o_string += f_defTable;

		return o_string;

	}

	STRING ASSEMBLED::assemble_code() {

		/*
			Merge the machine code
		*/

		STRING o_string;

		for (STRING* f1_section = sectionList.begin(); f1_section < sectionList.end(); f1_section ++) {

			o_string += *f1_section;

		}

		return o_string;

	}
	
	STRING ASSEMBLED::assemble_req() {

		/*
			Merge tthe redirection table
		*/

		STRING o_string;

		for (REQT* f1_req = reqList.begin(); f1_req < reqList.end(); f1_req ++) {

			o_string += DWORD(f1_req->offset);

			DEFT* f1_deft = defList.begin();

			for (; f1_deft < defList.end(); f1_deft ++) {

				if (f1_req->token.content == f1_deft->token.content) {

					o_string += DWORD(f1_deft - defList.begin());

					break;

				}

			}

			if (f1_deft >= defList.end())
				throw(traceToken(*encoded->solved->parsed->lexed, f1_req->token, "Symbol lost."));

			o_string += WORD(4 + f1_req->roffset);

		}

		return o_string;

	}
	
	STRING ASSEMBLED::assemble_def() {

		/*
			Merge the symbol table and the string table
		*/

		STRING o_string;

		STRING f_strTable;

		for (DEFT* f1_def = defList.begin(); f1_def < defList.end(); f1_def ++) {

			STRING f1_name = f1_def->token.content.from(1).to(-1);

			while (f1_name.size() < 8) {

				f1_name += BYTE(0x00);

			}

			if (f1_name.size() > 8) {

				o_string += DWORD(0);

				o_string += DWORD(f_strTable.size() + 4);

				f_strTable += f1_name + '\0';
			}
			else {

				o_string += f1_name;

			}

			o_string += DWORD(f1_def->offset);

			if (f1_def->section > s_GLB) {

				o_string += WORD(1);
				o_string += WORD(32);

			}
			else {

				o_string += WORD(0);
				o_string += WORD(0);

			}

			o_string += BYTE(2);
			o_string += BYTE(0);

		}

		o_string += DWORD(f_strTable.size() + 4);

		o_string += f_strTable;

		return o_string;

	}

	STRING ASSEMBLED::string() {

		return assemble().hex(" ");

	}

	VOID ASSEMBLED::save(STRING i_file) {

		/*
			Write to file
		*/

		std::ofstream f_file(i_file.c(), std::ifstream::binary);

		STRING f_binary = assemble();

		f_file.write(f_binary.c(), f_binary.size());
	}

}