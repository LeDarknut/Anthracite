#include <anthracite_solver.hpp>

namespace ANTHRACITE {

	SOLVED::SOLVED(PARSED& i_parsed) : parsed(&i_parsed), lineList() {

		EXPR f_root;

		f_root.block = parsed->block;

		f_root.target += 0;

		LIST<EXPR*> f_scopeList;

		f_scopeList += &f_root;

		lineList += solve_block(f_scopeList, parsed->block);

	}

	LIST<LINE> SOLVED::solve_block(LIST<EXPR*> i_scopeList, BLOCK i_block, LIST<TOKEN> i_forbidden) {
		
		LIST<LINE> o_lineList;
		
		for (EXPR* f1_expr = i_block.exprList.begin(); f1_expr < i_block.exprList.end(); f1_expr ++) {
			
			LIST<TOKEN> f1_symList;

			BOOL f1_isNullable = false;

			LIST<LINE> f1_nullableLineList;

			for (DEF* f2_def = f1_expr->defList.begin(); f2_def < f1_expr->defList.end(); f2_def ++) {

				f1_symList += f2_def->name;
			}

			if		(f1_expr->isOpr()) {

				LINE f1_line;

				try {

					f1_line.section = solve_section(i_scopeList, f1_expr->section);

				} catch (BYTE f_e) {continue;}

				f1_line.name = f1_expr->target[0];

				f1_line.symList += f1_symList;

				LIST<LIST<ARG>> f1_args = solve_args(i_scopeList, f1_expr->args);

				try {

					f1_line.argList = argify(f1_args);

				}
				catch (BYTE f_e) {

					if (f_e == 2) {

						f1_line.name = TOKEN(t_LBL, 0, "NOP");

						f1_line.argList.empty();

					}
					else throw(tracePath(*parsed, f1_expr->target, "Invalid function."));

				}

				o_lineList += f1_line;

			}
			else if	(f1_expr->isMtd()) {

				EXPR f1_cexpr;

				LIST<LIST<ARG>> f1_cargs = solve_args(i_scopeList, f1_expr->args);

				f1_cexpr.args = f1_cargs;

				LIST<FOUND> f1_functionFoundList = find_path(i_scopeList, f1_expr->target);

				if (f1_functionFoundList.size() == 0)
					throw(tracePath(*parsed, f1_expr->target, "Function not found."));

				for (FOUND* f2_functionFound = f1_functionFoundList.begin(); f2_functionFound < f1_functionFoundList.end(); f2_functionFound ++) {

					if (!f2_functionFound->expr || f2_functionFound->expr->isPrivate()) continue;

					EXPR f2_fexpr = *f2_functionFound->expr;

					if (f2_functionFound->prefixed.size() > 0) {

						FOUND& f2_prefixed = f2_functionFound->prefixed[-1];

						f1_cexpr.args = LIST<LIST<ARG>>(toArg(f2_prefixed)) + f1_cargs;

					}

					f2_fexpr.args = solve_args(f2_functionFound->scopeList, f2_fexpr.args);
					
					try {
						
						f2_fexpr.args = solve_fargs(f2_fexpr.args, f1_cexpr.args);
						
					}
					catch (BYTE f_e) {

						if (f_e == 2) {

							f1_isNullable = true;

							if (f1_symList.size() > 0) {

								if (f2_functionFound->expr->section.size() == 0)
									throw(tracePath(*parsed, f2_functionFound->expr->target, "Block section missing."));

								f1_nullableLineList += LINE(f1_symList, TOKEN(t_LBL, 0, "NOP"), solve_section(f2_functionFound->scopeList + &f2_fexpr, f2_functionFound->expr->section));

							}

							continue;

						}
						else {
						
							if (f2_functionFound + 1 >= f1_functionFoundList.end()) {

								if (f1_isNullable) {

									o_lineList += f1_nullableLineList;

									break;

								}
								else throw(tracePath(*parsed, f1_expr->target, "Invalid argument list."));

							}
							
							continue;

						}
						
					}

					if (f1_symList.size() > 0) {

						if (f2_functionFound->expr->section.size() == 0)
							throw(tracePath(*parsed, f2_functionFound->expr->target, "Block section missing."));

						o_lineList += LINE(f1_symList, TOKEN(t_LBL, 0, "NOP"), solve_section(f2_functionFound->scopeList + &f2_fexpr, f2_functionFound->expr->section));

					}

					for (TOKEN* f3_forbidden = i_forbidden.begin(); f3_forbidden < i_forbidden.end(); f3_forbidden ++) {

						if (f2_functionFound->expr->target[-1].pointer == f3_forbidden->pointer)
							throw(tracePath(*parsed, f1_expr->target, "Self-referencing function."));

					}

					o_lineList += LINE(LIST<TOKEN>(), TOKEN(), TOKEN(t_ERR, 0, "{"));

					o_lineList += solve_block(f2_functionFound->scopeList + &f2_fexpr, f2_fexpr.block, i_forbidden + f2_functionFound->expr->target[-1]);

					o_lineList += LINE(LIST<TOKEN>(), TOKEN(), TOKEN(t_ERR, 0, "}"));

					break;

				}

			}
			else if	(f1_expr->isDcl() && !f1_expr->isHidden()) {

				if (f1_symList.size() > 0) {

					if (f1_expr->section.size() == 0)
						throw(tracePath(*parsed, f1_expr->target, "Block section missing."));

					o_lineList += LINE(f1_symList, TOKEN(t_LBL, 0, "NOP"), solve_section(i_scopeList, f1_expr->section));

				}

				f1_expr->args = solve_args(i_scopeList, f1_expr->args);
				
				try {
					
					f1_expr->args = solve_fargs(f1_expr->args, f1_expr->args);

				} catch (BYTE f_e) {continue;}

				o_lineList += solve_block(i_scopeList + f1_expr, f1_expr->block, i_forbidden);

			}

		}

		return o_lineList;

	}

	LIST<ARGV> SOLVED::argify(LIST<LIST<ARG>>& i_args) {

		LIST<ARGV> o_argList;

		for (LIST<ARG>* f1_argList = i_args.begin(); f1_argList < i_args.end(); f1_argList ++) {

			BOOL f1_arg = false;

			for (ARG* f2_arg = f1_argList->begin(); f2_arg < f1_argList->end(); f2_arg ++) {

				if (f2_arg->isNul()) {

					if (!f1_arg && f2_arg + 1 >= f1_argList->end()) throw(BYTE(2));

					continue;

				}

				if (f1_arg) throw(BYTE(1));

				f1_arg = true;

				STRING f2_name;

				if (!f2_arg->name.is()) {

					if		(f2_arg->isBox()) {

						f2_name = "@";

					}
					else if	(f2_arg->isMem()) {

						f2_name = "$";

					}
					else if	(f2_arg->isReg()) {

						f2_name = "%";

					}
					else {

						f2_name = "#";

					}

				}
				else {

					f2_name = f2_arg->name.content;

				}

				if (f2_arg->isLtr() && f2_arg->isBox()) {
						
					o_argList += ARGV(f2_name, TOKEN(), argify(f2_arg->args));

				}
				else if (f2_arg->isLtr() && f2_arg->isMem()) {

					if (f2_arg->args.size() > 0) {

						LIST<ARGV> f2_SymbolArgList;

						f2_SymbolArgList += ARGV("_OPR", f2_arg->args[0][0].target[0]);

						if (f2_arg->args[0][1].is()) {
							
							f2_SymbolArgList += ARGV("%b", f2_arg->args[0][1].target[-1]);

						}
						if (f2_arg->args[0][2].is()) {
							
							f2_SymbolArgList += ARGV("%i", f2_arg->args[0][2].target[-1]);

						}
						if (f2_arg->args[0][3].is()) {
							
							f2_SymbolArgList += ARGV("#m", f2_arg->args[0][3].target[-1]);

						}
						if (f2_arg->args[0][4].is()) {
							
							f2_SymbolArgList += ARGV("#o", f2_arg->args[0][4].target[-1]);

						}

						o_argList += ARGV(f2_name, TOKEN(), f2_SymbolArgList);

					}
					else {

						o_argList += ARGV(f2_name, f2_arg->target[0]);

					}

				}
				else {

					o_argList += ARGV(f2_name, f2_arg->target[-1]);

				}

			}

		}

		return o_argList;

	}

	LIST<LIST<ARG>> SOLVED::solve_args(LIST<EXPR*> i_scopeList, LIST<LIST<ARG>>& i_args, LIST<ARG*> i_forbidden) {

		LIST<LIST<ARG>> o_args = i_args;

		for (LIST<ARG>* f1_argList = o_args.begin(); f1_argList < o_args.end(); f1_argList ++) {

			for (ARG* f2_arg = f1_argList->begin(); f2_arg < f1_argList->end(); f2_arg ++) {

				*f2_arg = solve_arg(i_scopeList, *f2_arg, i_forbidden);

			}

		}

		return o_args;

	}

	ARG SOLVED::solve_arg(LIST<EXPR*> i_scopeList, ARG& i_arg, LIST<ARG*> i_forbidden) {

		ARG o_arg;

		if (!i_arg.is() || 
			(i_arg.isLtr() && (i_arg.isVal() || i_arg.isReg() || i_arg.isSct())) ||
			 i_arg.isNul()) o_arg = i_arg;
		else if	(i_arg.isLtr()) {

			if (i_arg.args.size() > 0) {

				o_arg = i_arg;
				
				o_arg.args = solve_args(i_scopeList, i_arg.args, i_forbidden);

			}
			else {

				o_arg = i_arg;

			}

		}
		else if (i_arg.isCal()) {

			LIST<LIST<ARG>> f_args = solve_args(i_scopeList, i_arg.args, i_forbidden);

			o_arg.type = (i_arg.type ^ a_00C0) | a_00L0;

			if		(i_arg.isReg()) {

				if		(f_args[0][0].target[0].content == "MOST") {

					LIST<WORD> f_regList;

					for (BYTE f1_reg = 0; f1_reg <= 13; f1_reg ++) {

						f_regList += 0;

					}

					for (ARG* f1_arg = f_args[0].begin() + 2; f1_arg < f_args[0].end(); f1_arg ++) {

						f_regList[f1_arg->target[0].value[0]] ++;

					}

					BYTE f_newReg = 0;

					WORD f_regMax = 0;

					for (BYTE f1_reg = 0; f1_reg <= 13; f1_reg ++) {

						if (f_regList[f1_reg] <= f_regMax) continue;

						f_regMax = f_regList[f1_reg];

						f_newReg = f1_reg;

					}

					o_arg.target = TOKEN(t_LTR, 0, STRING(), STRING(BYTE(f_newReg)));

				}
				else if (f_args[0][0].target[0].content == "LEAST") {

					LIST<WORD> f_regList;

					for (BYTE f1_reg = 0; f1_reg <= 13; f1_reg ++) {

						f_regList += 0;

					}

					for (ARG* f1_arg = f_args[0].begin() + 2; f1_arg < f_args[0].end(); f1_arg ++) {

						f_regList[f1_arg->target[0].value[0]] ++;

					}

					BYTE f_newReg = 0;

					WORD f_regMin = 65535;

					for (BYTE f1_reg = 0; f1_reg <= 13; f1_reg ++) {

						if (f_regList[f1_reg] > f_regMin) continue;

						f_regMin = f_regList[f1_reg];

						f_newReg = f1_reg;

					}

					o_arg.target = TOKEN(t_LTR, 0, STRING(), STRING(BYTE(f_newReg)));

				}

			}
			else if	(i_arg.isVal()) {

				o_arg.target = f_args[0][1].target[0];

				if		(f_args[0][0].target[0].content == "OR") {

					for (ARG* f1_arg = f_args[0].begin() + 2; f1_arg < f_args[0].end(); f1_arg ++) {

						while (f1_arg->target[0].value.size() > o_arg.target[0].value.size()) {

							o_arg.target[0].value += o_arg.target[0].value[-1];

						}

						for (DWORD f2_byteId = 0; f2_byteId < f1_arg->target[0].value.size(); f2_byteId ++) {

							o_arg.target[0].value[f2_byteId] |= f1_arg->target[0].value[f2_byteId];

						}

					}

				}
				else if (f_args[0][0].target[0].content == "AND") {

					for (ARG* f1_arg = f_args[0].begin() + 2; f1_arg < f_args[0].end(); f1_arg ++) {

						while (f1_arg->target[0].value.size() > o_arg.target[0].value.size()) {

							o_arg.target[0].value += o_arg.target[0].value[-1];

						}

						for (DWORD f2_byteId = 0; f2_byteId < f1_arg->target[0].value.size(); f2_byteId ++) {

							o_arg.target[0].value[f2_byteId] &= f1_arg->target[0].value[f2_byteId];

						}

					}

				}
				else if (f_args[0][0].target[0].content == "XOR") {

					for (ARG* f1_arg = f_args[0].begin() + 2; f1_arg < f_args[0].end(); f1_arg ++) {

						while (f1_arg->target[0].value.size() > o_arg.target[0].value.size()) {

							o_arg.target[0].value += o_arg.target[0].value[-1];

						}

						for (DWORD f2_byteId = 0; f2_byteId < f1_arg->target[0].value.size(); f2_byteId ++) {

							o_arg.target[0].value[f2_byteId] ^= f1_arg->target[0].value[f2_byteId];

						}

					}

				}
				else if (f_args[0][0].target[0].content == "ADD") {

					for (ARG* f1_arg = f_args[0].begin() + 2; f1_arg < f_args[0].end(); f1_arg ++) {

						do {

							if (o_arg.target[0].value[-1] >= 0x80)
								o_arg.target[0].value += BYTE(0xFF);

							else
								o_arg.target[0].value += BYTE(0x00);

						} while (f1_arg->target[0].value.size() + 1 > o_arg.target[0].value.size());

						do {

							if (f1_arg->target[0].value[-1] >= 0x80)
								f1_arg->target[0].value += BYTE(0xFF);

							else
								f1_arg->target[0].value += BYTE(0x00);

						} while (f1_arg->target[0].value.size() < o_arg.target[0].value.size());

						WORD f1_subAdd = 0;

						for (DWORD f2_byteId = 0; f2_byteId < f1_arg->target[0].value.size(); f2_byteId ++) {

							f1_subAdd += o_arg.target[0].value[f2_byteId] + f1_arg->target[0].value[f2_byteId];

							o_arg.target[0].value[f2_byteId] = f1_subAdd;

							f1_subAdd = f1_subAdd >> 8;

						}

						while (o_arg.target[0].value.size() >= 2 &&
							 ((o_arg.target[0].value[-1] == 0x00 && o_arg.target[0].value[-2] < 0x80) ||
							  (o_arg.target[0].value[-1] == 0xFF && o_arg.target[0].value[-2] >= 0x80))) {

							o_arg.target[0].value -= 1;

						}

					}

				}
				else if (f_args[0][0].target[0].content == "APPEND") {

					for (ARG* f1_arg = f_args[0].begin() + 2; f1_arg < f_args[0].end(); f1_arg ++) {

						o_arg.target[0].value += f1_arg->target[0].value;

					}

				}
				else if (f_args[0][0].target[0].content == "SIZE") {

					DWORD f1_size = o_arg.target[0].value.size();

					o_arg.target[0].value.empty();

					while (f1_size > 0) {

						o_arg.target[0].value += BYTE(f1_size & 0xFF);

						f1_size = f1_size >> 8;

					}

				}
				else if (f_args[0][0].target[0].content == "NOT") {

					for (BYTE* f2_byte = o_arg.target[0].value.begin(); f2_byte < o_arg.target[0].value.end(); f2_byte ++) {

						*f2_byte = ~*f2_byte;

					}

				}
				else if (f_args[0][0].target[0].content == "NEG") {

					BOOL f1_carry = true;

					for (BYTE* f2_byte = o_arg.target[0].value.begin(); f2_byte < o_arg.target[0].value.end(); f2_byte ++) {

						*f2_byte = ~*f2_byte;

						if (f1_carry) {

							(*f2_byte) ++;

							f1_carry = (*f2_byte == 0x00);

						}

					}

				}

			}

		}
		else if	(i_arg.isTrg()) {

			LIST<FOUND> f2_foundList = find_path(i_scopeList, i_arg.target, LIST<FOUND>(), i_forbidden);

			if (f2_foundList.size() == 0)
				throw(tracePath(*parsed, i_arg.target, "Symbol not found."));

			if (f2_foundList.size() > 1 && isDifferent(f2_foundList[0], f2_foundList.from(1)))
				throw(tracePath(*parsed, i_arg.target, "Ambiguous symbol found."));
			
			if (f2_foundList[0].arg) {

				o_arg = solve_arg(f2_foundList[0].scopeList, *f2_foundList[0].arg, f2_foundList[0].forbidden);

			}
			else {

				o_arg.type = a_00L0 | a_000M ;

				o_arg.target = f2_foundList[0].def->name;

			}

		}
		else if	(i_arg.isGlb()) {
		
			LIST<FOUND> f2_foundList = find_glob(i_scopeList.to(1), i_arg.target[0], LIST<FOUND>(), i_forbidden);

			if (f2_foundList.size() == 0) {

				if (!i_arg.isMem())
					throw(tracePath(*parsed, i_arg.target, "Symbol not found."));

				o_arg = i_arg;

			}
			else {

				if (f2_foundList.size() > 1 && isDifferent(f2_foundList[0], f2_foundList.from(1)))
					throw(tracePath(*parsed, i_arg.target, "Ambiguous symbol found."));

				if (f2_foundList[0].arg) {

					o_arg = solve_arg(f2_foundList[0].scopeList, *f2_foundList[0].arg, f2_foundList[0].forbidden);

				}
				else {

					o_arg.type = a_00L0 | a_000M;

					o_arg.target = f2_foundList[0].def->name;

				}

			}

		}

		o_arg.name = i_arg.name;

		o_arg.prefix = i_arg.prefix;

		o_arg.type = (o_arg.type ^ (o_arg.type & (a_1000 | a_0100))) | (i_arg.type & (a_1000 | a_0100));
		
		return o_arg;

	}

	LIST<LIST<ARG>> SOLVED::solve_fargs(LIST<LIST<ARG>> i_fargs, LIST<LIST<ARG>> i_cargs) {

		LIST<LIST<ARG>>& o_fargs = i_fargs;

		LIST<LIST<ARG>>& f_cargsLeft = i_cargs;

		for (LIST<ARG>* f1_fargList = o_fargs.begin(); f1_fargList < o_fargs.end(); f1_fargList ++) {

			BOOL f1_found = false;

			for (ARG* f2_farg = f1_fargList->begin(); f2_farg < f1_fargList->end(); f2_farg ++) {

				for (LIST<ARG>* f3_cargList = f_cargsLeft.begin(); f3_cargList < f_cargsLeft.end(); f3_cargList ++) {

					for (ARG* f4_carg = f3_cargList->begin(); f4_carg < f3_cargList->end(); f4_carg ++) {

						if (f4_carg->name.is()) {

							if ((f2_farg->type & a_0001) != (f4_carg->type & a_0001) ||
								f2_farg->name.content != f4_carg->name.content) continue;

							f1_found = true;

							*f1_fargList = solve_fargList(*f1_fargList, *f3_cargList);

							f_cargsLeft = f_cargsLeft.to(f3_cargList) + f_cargsLeft.from(f3_cargList + 1);

							break;

						}

					}

					if (f1_found) break;

				}

				if (f1_found) break;

			}

			if (f1_found) continue;

			for (LIST<ARG>* f2_cargList = f_cargsLeft.begin(); f2_cargList < f_cargsLeft.end(); f2_cargList ++) {

				for (ARG* f3_carg = f2_cargList->begin(); f3_carg < f2_cargList->end(); f3_carg ++) {

					if (f3_carg->name.is()) break;

					if (f3_carg + 1 >= f2_cargList->end()) {

						f1_found = true;

						*f1_fargList = solve_fargList(*f1_fargList, *f2_cargList);

						f_cargsLeft = f_cargsLeft.to(f2_cargList) + f_cargsLeft.from(f2_cargList + 1);

					}

				}

				if (f1_found) break;

			}

			if (f1_found) continue;

			*f1_fargList = solve_fargList(*f1_fargList);

		}

		if (f_cargsLeft.size() > 0) throw(BYTE(1));

		return o_fargs;

	}

	LIST<ARG> SOLVED::solve_fargList(LIST<ARG> i_fargList, LIST<ARG> i_cargList) {

		LIST<ARG>& o_fargList = i_fargList;

		LIST<ARG>& f_cargLeft = i_cargList;

		LIST<ARG*> f_fargLeft;

		BOOL f_null = (o_fargList.size() > 0);

		for (ARG* f1_farg = o_fargList.begin(); f1_farg < o_fargList.end(); f1_farg ++) {

			if (!f1_farg->is()) {

				f_null = false;

				for (ARG* f2_carg = f_cargLeft.begin(); f2_carg < f_cargLeft.end(); f2_carg ++) {

					if (!f2_carg->is()) {

						f_cargLeft = f_cargLeft.to(f2_carg) + f_cargLeft.from(f2_carg + 1);

						break;

					}

				}

				continue;

			}

			BOOL f1_found = false;

			for (ARG* f2_carg = f_cargLeft.begin(); f2_carg < f_cargLeft.end(); f2_carg ++) {

				if (!f2_carg->name.is()) continue;

				if (f1_farg->name.content != f2_carg->name.content) continue;

				if (f1_farg->isBox()) {

					try {

						f1_farg->args = solve_fargs(f1_farg->args, f2_carg->args);

					} catch (BYTE f_e) {continue;}

					f1_found = true;

					f1_farg->target.empty();
					f1_farg->type = a_00L0 | a_000B;

				}
				else {
				
					f1_found = true;

					if (f1_farg->isFrc() &&
					   (f1_farg->isVal() || f1_farg->isReg() || f1_farg->isSct()) && 
						f1_farg->target[0].value != f2_carg->target[0].value) throw(BYTE(1));

					f1_farg->args = f2_carg->args;
					f1_farg->target = f2_carg->target;
					f1_farg->type = f2_carg->type;

				}

				if (!f1_farg->isNul()) {

					f_null = false;

				}

				f_cargLeft = f_cargLeft.to(f2_carg) + f_cargLeft.from(f2_carg + 1);

				break;

			}

			if (f1_found) continue;

			for (ARG* f2_carg = f_cargLeft.begin(); f2_carg < f_cargLeft.end(); f2_carg ++) {

				if (!f2_carg->name.is()) {

					if ((f1_farg->type & a_0001) != (f2_carg->type & a_0001)) continue;

					if (f1_farg->isBox()) {

						try {

							f1_farg->args = solve_fargs(f1_farg->args, f2_carg->args);

						} catch (BYTE f_e) {continue;}

						f1_found = true;

						f1_farg->target.empty();
						f1_farg->type = a_00L0 | a_000B;

					}
					else {

						f1_found = true;

						if (f1_farg->isFrc() &&
						   (f1_farg->isVal() || f1_farg->isReg() || f1_farg->isSct()) && 
							f1_farg->target[0].value != f2_carg->target[0].value) throw(BYTE(1));

						f1_farg->args = f2_carg->args;
						f1_farg->target = f2_carg->target;
						f1_farg->type = f2_carg->type;

					}

					if (!f1_farg->isNul()) {

						f_null = false;

					}

					f_cargLeft = f_cargLeft.to(f2_carg) + f_cargLeft.from(f2_carg + 1);

					break;

				}

			}

			if (f1_found) continue;

			if (f1_farg->isOpt()) {

				f_fargLeft += f1_farg;

			}
			else {

				nullArg(*f1_farg);

			}

		}

		if (f_cargLeft.size() > 0) throw(BYTE(1));

		for (ARG** f1_fargLeft = f_fargLeft.begin(); f1_fargLeft < f_fargLeft.end(); f1_fargLeft ++) {

			if (!f_null) {

				nullArg(**f1_fargLeft);

			}
			else {

				if (!(*f1_fargLeft)->isNul()) {

					f_null = false;

				}

			}

		}
		
		if (f_null) throw(BYTE(2));

		return o_fargList;

	}

	TOKEN SOLVED::solve_section(LIST<EXPR*> i_scopeList, LIST<TOKEN> i_section) {

		LIST<FOUND> f_sectionFoundList;

		if (i_section.size() == 1 && i_section[0].isLtr()) {

			return i_section[0];

		}
		
		if (i_section.size() == 1 && i_section[0].isGlb()) {

			f_sectionFoundList = find_glob(i_scopeList.to(1), i_section[0]);

		}
		else {

			f_sectionFoundList = find_path(i_scopeList, i_section);

		}

		if (f_sectionFoundList.size() == 0)
			throw(tracePath(*parsed, i_section, "Symbol not found."));

		if (f_sectionFoundList.size() > 1 && isDifferent(f_sectionFoundList[0], f_sectionFoundList.from(1)))
			throw(tracePath(*parsed, i_section, "Ambiguous symbol found."));

		if (!f_sectionFoundList[0].arg)
			throw(tracePath(*parsed, i_section, "Symbol not found."));

		ARG f_arg = solve_arg(f_sectionFoundList[0].scopeList, *f_sectionFoundList[0].arg, f_sectionFoundList[0].forbidden);

		if (f_arg.isNul()) throw(BYTE(1));

		return f_arg.target[0];

	}

	ARG SOLVED::toArg(FOUND& i_found) {

		ARG o_arg;

		if (i_found.arg) {
			
			o_arg = solve_arg(i_found.scopeList, *i_found.arg, i_found.forbidden);

			o_arg.name = TOKEN();

			o_arg.prefix.empty();

		}
		else {

			o_arg.type = a_00L0 | a_000M;

			o_arg.target = i_found.def->name;

		}

		return o_arg;

	}

	VOID SOLVED::nullArg(ARG& i_arg) {

		if (i_arg.isBox()) {

			for (LIST<ARG>* f1_argList = i_arg.args.begin(); f1_argList < i_arg.args.end(); f1_argList ++) {

				for (ARG* f2_arg = f1_argList->begin(); f2_arg < f1_argList->end(); f2_arg ++) {

					nullArg(*f2_arg);

				}

			}

		}
		else {

			i_arg.args.empty();
			i_arg.target.empty();
			i_arg.type = (i_arg.type & a_0001) | a_00N0;

		}

	}

	BOOL SOLVED::isDifferent(FOUND& i_found, LIST<FOUND> i_foundList) {

		if (i_found.def) {

			for (FOUND* f1_found = i_foundList.begin(); f1_found < i_foundList.end(); f1_found ++) {

				if (!f1_found->def) return true;
				
				if (i_found.def->name.pointer != f1_found->def->name.pointer) return true;

			}

		}
		else if (i_found.arg) {

			ARG f_arg = solve_arg(i_found.scopeList, *i_found.arg, i_found.forbidden);

			for (FOUND* f1_found = i_foundList.begin(); f1_found < i_foundList.end(); f1_found ++) {

				if (!f1_found->arg) return true;

				ARG f1_arg = solve_arg(f1_found->scopeList, *f1_found->arg, f1_found->forbidden);

				if (isDifferent(f_arg, f1_arg)) return true;

			}

		}

		return false;

	}

	BOOL SOLVED::isDifferent(ARG& i_arg1, ARG& i_arg2) {

		if (i_arg1.type != i_arg2.type ||
			i_arg1.target.size() != i_arg2.target.size() ||
			i_arg1.args.size() != i_arg2.args.size()) return true;

		if (i_arg1.target.size() > 0) {

			if (i_arg1.target[0].isMem()) {

				if (i_arg1.target[0].pointer != i_arg2.target[0].pointer) return true;

			}
			else {

				if (i_arg1.target[0].value != i_arg2.target[0].value) return true;

			}

		}

		for (DWORD f1_argListId = 0; f1_argListId < i_arg1.args.size(); f1_argListId ++) {

			if (i_arg1.args[f1_argListId].size() != i_arg2.args[f1_argListId].size()) return true;

			for (DWORD f2_argId = 0; f2_argId < i_arg1.args[f1_argListId].size(); f2_argId ++) {

				if (isDifferent(i_arg1.args[f1_argListId][f2_argId], i_arg2.args[f1_argListId][f2_argId])) return true;

			}

		}

		return false;

	}

	LIST<FOUND> SOLVED::find_path(LIST<EXPR*> i_scopeList, LIST<TOKEN> i_path, LIST<FOUND> i_prefixed, LIST<ARG*> i_forbidden) {
		
		LIST<FOUND> o_foundList;

		if (i_path.size() == 0) {
			
			return o_foundList;

		}
		
		while (i_path.size() > 1 && !i_path[0].is()) {

			while (true) {

				if (i_scopeList[-1] == i_scopeList[0] || i_scopeList[-1]->isRoot()) break;

				i_scopeList -= 1;

			}

			if (!i_path[1].is()) {

				if (i_scopeList[-1] == i_scopeList[0])
					throw(tracePath(*parsed, i_path, "Path cannot ascend beyond root."));

				i_scopeList -= 1;

			}

			i_path = i_path.from(1);

		}

		if (i_path.size() == 0) {
			
			return o_foundList + FOUND(i_scopeList, i_scopeList[-1], 0, 0, i_prefixed, i_forbidden);

		}

		TOKEN* f_subPath = i_path.begin() + 1;

		for (; f_subPath < i_path.end() && f_subPath->is(); f_subPath ++);

		if ((f_subPath - 1)->isLbl()) {

			LIST<FOUND> f_foundList = find_block(i_scopeList, i_path.to(f_subPath), i_prefixed, i_forbidden);

			if (f_subPath != i_path.end()) {
				
				for (FOUND* f1_found = f_foundList.begin(); f1_found < f_foundList.end(); f1_found ++) {

					o_foundList += find_path(f1_found->scopeList + f1_found->expr, i_path.from(f_subPath + 1), i_prefixed, i_forbidden);

				}

			}
			else {

				o_foundList += f_foundList;

			}

		}
		else {

			LIST<FOUND> f_foundList = find_def(i_scopeList, i_path.to(f_subPath), i_prefixed, i_forbidden);

			if (f_subPath != i_path.end()) {

				for (FOUND* f1_found = f_foundList.begin(); f1_found < f_foundList.end(); f1_found ++) {

					if (f1_found->def) {
					
						if (f1_found->def->prefix.size() == 0)
							throw(tracePath(*parsed, f1_found->def->name, "Prefix expected."));

						o_foundList += find_path(f1_found->scopeList, f1_found->def->prefix + i_path.from(f_subPath + 1), i_prefixed + *f1_found, f1_found->forbidden);

					}
					else if (f1_found->arg) {

						if (f1_found->arg->prefix.size() == 0)
							throw(tracePath(*parsed, f1_found->arg->name, "Prefix expected."));

						o_foundList += find_path(f1_found->scopeList, f1_found->arg->prefix + i_path.from(f_subPath + 1), i_prefixed + *f1_found, f1_found->forbidden);

					}

				}

			}
			else {
				
				o_foundList += f_foundList;

			}
		
		}

		return o_foundList;

	}

	LIST<FOUND> SOLVED::find_block(LIST<EXPR*> i_scopeList, LIST<TOKEN> i_path, LIST<FOUND> i_prefixed, LIST<ARG*> i_forbidden) {

		LIST<FOUND> o_foundList;

		for (EXPR* f1_expr = i_scopeList[-1]->block.exprList.begin(); f1_expr < i_scopeList[-1]->block.exprList.end(); f1_expr ++) {

			if	(!f1_expr->isDcl() || f1_expr->target[0].content != i_path[0].content) continue;

			if (i_path.size() == 1) {
				
				o_foundList += FOUND(i_scopeList, f1_expr, 0, 0, i_prefixed, i_forbidden);

			}
			else {
				
				o_foundList += find_block(i_scopeList + f1_expr, i_path.from(1), i_prefixed, i_forbidden);

			}

		}

		return o_foundList;

	}

	LIST<FOUND> SOLVED::find_def(LIST<EXPR*> i_scopeList, LIST<TOKEN> i_path, LIST<FOUND> i_prefixed, LIST<ARG*> i_forbidden) {

		LIST<FOUND> o_foundList;

		o_foundList += find_arg(i_scopeList, i_path, i_scopeList[-1]->args, i_prefixed, i_forbidden);

		for (ARG* f1_arg = i_scopeList[-1]->block.argList.begin(); f1_arg < i_scopeList[-1]->block.argList.end(); f1_arg ++) {
			
			if (f1_arg->name.content != i_path[0].content) continue;

			if (i_path.size() == 1) {

				for (ARG** f3_forbidden = i_forbidden.begin(); f3_forbidden < i_forbidden.end(); f3_forbidden ++) {

					if ((*f3_forbidden)->name.pointer == f1_arg->name.pointer)
						throw(tracePath(*parsed, f1_arg->name, "Self-referencing symbol."));

				}
				
				o_foundList += FOUND(i_scopeList, 0, f1_arg, 0, i_prefixed, i_forbidden + f1_arg);

			}
			else if (f1_arg->isBox()) {

				o_foundList += find_arg(i_scopeList, i_path.from(1), f1_arg->args, i_prefixed, i_forbidden);

			}

		}

		if (i_path.size() == 1 && i_path[0].isMem()) {

			for (EXPR* f1_expr = i_scopeList[-1]->block.exprList.begin(); f1_expr < i_scopeList[-1]->block.exprList.end(); f1_expr ++) {

				for (DEF* f2_def = f1_expr->defList.begin(); f2_def < f1_expr->defList.end(); f2_def ++) {

					if (f2_def->name.content != i_path[0].content) continue;

					o_foundList += FOUND(i_scopeList, 0, 0, f2_def, i_prefixed, i_forbidden);

				}

			}

		}

		return o_foundList;

	}

	LIST<FOUND> SOLVED::find_arg(LIST<EXPR*> i_scopeList, LIST<TOKEN> i_path, LIST<LIST<ARG>>& i_args, LIST<FOUND> i_prefixed, LIST<ARG*> i_forbidden) {

		LIST<FOUND> o_foundList;

		for (LIST<ARG>* f1_argList = i_args.begin(); f1_argList < i_args.end(); f1_argList ++) {

			for (ARG* f2_arg = f1_argList->begin(); f2_arg < f1_argList->end(); f2_arg ++) {

				if (f2_arg->name.content != i_path[0].content) continue;

				if (i_path.size() == 1) {

					o_foundList += FOUND(i_scopeList - 1, 0, f2_arg, 0, i_prefixed, i_forbidden);

				}
				else if (f2_arg->isBox()) {
					
					o_foundList += find_arg(i_scopeList, i_path.from(1), f2_arg->args, i_prefixed, i_forbidden);

				}

			}

		}

		return o_foundList;

	}

	LIST<FOUND> SOLVED::find_glob(LIST<EXPR*> i_scopeList, TOKEN& i_glob, LIST<FOUND> i_prefixed, LIST<ARG*> i_forbidden) {

		LIST<FOUND> o_foundList;

		for (ARG* f1_arg = i_scopeList[-1]->block.argList.begin(); f1_arg < i_scopeList[-1]->block.argList.end(); f1_arg ++) {

			if (!f1_arg->name.isGlb() ||f1_arg->name.content != i_glob.content) continue;

			o_foundList += FOUND(i_scopeList, 0, f1_arg, 0, i_prefixed, i_forbidden);

		}

		for (EXPR* f1_expr = i_scopeList[-1]->block.exprList.begin(); f1_expr < i_scopeList[-1]->block.exprList.end(); f1_expr ++) {

			for (DEF* f2_def = f1_expr->defList.begin(); f2_def < f1_expr->defList.end(); f2_def ++) {

				if (f2_def->name.content != i_glob.content) continue;

				o_foundList += FOUND(i_scopeList, 0, 0, f2_def, i_prefixed, i_forbidden);

			}

			if (f1_expr->block.exprList.size() > 0) {

				o_foundList += find_glob(i_scopeList + f1_expr, i_glob, i_prefixed, i_forbidden);

			}

		}

		return o_foundList;

	}

	STRING SOLVED::string() {

		STRING o_string;

		for (LINE* f1_line = lineList.begin(); f1_line < lineList.end(); f1_line ++) {

			if (!f1_line->section.is()) continue;

			for (TOKEN* f2_sym = f1_line->symList.begin(); f2_sym < f1_line->symList.end(); f2_sym ++) {

				if (f2_sym->isGlb()) {

					o_string += "$[";
					
					o_string += f2_sym->content;

					o_string += "]";

				}
				else {

					o_string += f2_sym->content;

				}

				o_string += " :\n";

			}

			o_string += "&";
			
			o_string += f1_line->section.content;

			o_string += "->";
			
			o_string += string_path(f1_line->name);
			
			o_string += "(";
			
			o_string += string_argList(f1_line->argList);
			
			o_string += ");";

			if (f1_line + 1 != lineList.end()) {

				o_string += "\n";

			}

		}

		return o_string;

	}

	STRING SOLVED::string_argList(LIST<ARGV> i_argList) {

		STRING o_string;

		for (ARGV* f1_arg = i_argList.begin(); f1_arg < i_argList.end(); f1_arg ++) {

			o_string += f1_arg->name;
			
			o_string += " : ";

			if (f1_arg->argList.size() > 0) {

				if (f1_arg->name[0] == '@') {
					
					o_string += "(";
					
					o_string += string_argList(f1_arg->argList);
					
					o_string += ")";

				}
				else {

					o_string += "$[";

					ARGV* f1_b = arg(f1_arg->argList, "b");
					ARGV* f1_i = arg(f1_arg->argList, "{");
					ARGV* f1_m = arg(f1_arg->argList, "m");
					ARGV* f1_o = arg(f1_arg->argList, "}");

					if (f1_b) {
						
						o_string += "ADDR: ";

						if (f1_b->value.isLtr()) {

							o_string += "%";

						}
						
						o_string += f1_b->value.content;

					}
					else {
						
						o_string += "STACK: ";

					}

					if (f1_i) {

						if (f1_b) {
							
							o_string += " + ";

						}

						if (f1_i->value.isLtr()) {

							o_string += "%";

						}

						o_string += f1_i->value.content;

						if (f1_m) {
							
							o_string += " * ";
							
							o_string += BYTE(f1_m->value.value[0] + '0');

						}

					}

					if (f1_o) {

						if (f1_b || f1_i) o_string += " + ";
						
						o_string += "0x";
						
						o_string += f1_o->value.value.hex();

					}

					o_string += "]";

				}

			}
			else {

				if		(f1_arg->value.isLtr()) {

					if (f1_arg->name[0] == '#') {
					
						o_string += "0x";
						
						o_string += f1_arg->value.value.hex();

					}
					else {

						o_string += "%";

						if (f1_arg->value.value[0] < 10) {
						
							o_string += BYTE(f1_arg->value.value[0] + '0');

						}
						else {

							o_string += "1";

							o_string += BYTE(f1_arg->value.value[0] - 10 + '0');

						}

					}

				}
				else if	(f1_arg->value.isMem()) {
					
					o_string += f1_arg->value.content;

				}
				else if	(f1_arg->value.isGlb()) {
					
					o_string += "$[";
					
					o_string += f1_arg->value.content;

					o_string += "]";

				}

			}
			
			if (f1_arg + 1 < i_argList.end()) {
				
				o_string += ", ";

			}

		}

		return o_string;

	}

	ARGV* arg(LIST<ARGV>& i_argList, STRING i_argName) {

		for (ARGV* f1_arg = i_argList.begin(); f1_arg < i_argList.end(); f1_arg ++) {
			
			if (f1_arg->name.from(1) == i_argName) {
				
				return f1_arg;

			}

		}

		return 0;

	}

	ARGV* arg(LIST<ARGV>& i_argList, STRING i_argName, DWORD i_index) {

		for (ARGV* f1_arg = i_argList.begin(); f1_arg < i_argList.end(); f1_arg ++) {
			
			if (f1_arg->name.from(1) == i_argName) {
				
				return f1_arg;

			}

		}

		if (i_index < i_argList.size() && i_argList[i_index].name.size() == 1) {
			
			return &i_argList[i_index];

		}

		return 0;

	}

	ERROR traceLine(SOLVED& i_solved, LINE& i_line, STRING i_message) {

		return traceToken(*i_solved.parsed->lexed, i_line.section, i_message);

	}

}