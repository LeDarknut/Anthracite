#include <anthracite_parser.hpp>

namespace ANTHRACITE {

	PARSED::PARSED(LEXED& i_lexed) : lexed(&i_lexed), block() {

		TOKEN* f_token = lexed->tokenList.begin();

		block = parse_block(f_token);

	}

	EXPR PARSED::parse_expr(TOKEN*& i_token) {

		EXPR o_expr(0);

		expect(i_token);

		TOKEN* f_token = i_token;

		while (f_token->content != "{" && f_token->content != ";") next(f_token);

		BOOL f_def = (f_token->content == "{");

		while (
			i_token->content == "[" ||
			(
				inRange(i_token + 1) &&
				i_token->isMem() &&
				(
					(i_token + 1)->content == ":" ||
			 		(
						inRange(i_token + 4) &&
						i_token->content.size() == 1 &&
						(i_token + 1)->content == "[" &&
						(i_token + 2)->isGlb() &&
						(i_token + 3)->content == "]" &&
						(i_token + 4)->content == ":"
					)
				)
			)
		) {

			DEF f1_def;

			if (i_token->content == "[") {

				next(i_token);

				f1_def.prefix = parse_path(i_token);

				if (f1_def.prefix.size() == 0)
					throw(traceToken(*lexed, *i_token, "Prefix cannot be empty."));

				if (!f1_def.prefix[-1].isLbl())
					throw(traceToken(*lexed, *i_token, "Prefix must end with a block."));

				if (i_token->content != "]")
					throw(traceToken(*lexed, *i_token, "Prefix' end operator ']' expected."));

				next(i_token);

			}

			if (!i_token->isMem())
				throw(traceToken(*lexed, *i_token, "Symbol name expected."));

			if (i_token->content.size() == 1) {

				next(i_token);

				if (i_token->content != "[")
					throw(traceToken(*lexed, *i_token, "Global declaration operator '[' expected."));

				next(i_token);

				if (!i_token->isGlb())
					throw(traceToken(*lexed, *i_token, "Global declaration name expected."));

				f1_def.name = *i_token;

				next(i_token);

				if (i_token->content != "]")
					throw(traceToken(*lexed, *i_token, "Global declaration operator ']' expected."));

			}
			else {

				f1_def.name = *i_token;

			}

			next(i_token);

			if (i_token->content != ":")
				throw(traceToken(*lexed, *i_token, "Declaration operator ':' expected."));

			o_expr.defList += f1_def;

			next(i_token);

		}

		if (f_def) {

			o_expr.type |= e_D000;

			if (i_token->content == ".") {

				o_expr.type |= e_0R00;

				next(i_token);

			}

			if (!i_token->isLbl())
				throw(traceToken(*lexed, *i_token, "Block name expected."));

			o_expr.target = *i_token;

			next(i_token);

			if (i_token->content == "(") {

				o_expr.args = parse_args(i_token, 2);

				expect(i_token);

			}
			else {

				o_expr.type |= e_000P;

			}

			while (i_token->content != "{") {

				if		(i_token->content == "?") {

					o_expr.type |= e_00H0;

					next(i_token);

				}
				else if	(i_token->content == "!") {

					o_expr.type |= e_000P;

					next(i_token);

				}
				else {

					if (o_expr.section.size() > 0)
						throw(traceToken(*lexed, *i_token, "Ambiguous block's section."));

					o_expr.section = parse_path(i_token);

					if (!o_expr.section[-1].isSct())
						throw(traceToken(*lexed, *i_token, "Block's section expected."));

					expect(i_token);

				}

			}

			o_expr.block = parse_block(i_token);
			
			if (inRange(i_token) && i_token->content == ";") i_token ++;

		}
		else {

			if (i_token->isSct() && i_token->content.size() == 1) {

				o_expr.section += parse_arg(i_token, 0).target;

				expect(i_token);

				if (i_token->content != "->")
					throw(traceToken(*lexed, *i_token, "Section operator '->' expected."));

				next(i_token);

			}

			o_expr.target = parse_path(i_token);

			for (TOKEN* f1_token = o_expr.target.end() - 1; f1_token > o_expr.target.begin(); f1_token --) {

				if (!f1_token->is() && (f1_token - 1)->isSct()) {

					if (o_expr.section.size() > 0)
						throw(traceToken(*lexed, *i_token, "Section cannot be prefixed expected."));

					o_expr.section = o_expr.target.to(f1_token);

					o_expr.target = o_expr.target.from(f1_token + 1);

					break;

				}

			}

			if (o_expr.section.size() > 0 && o_expr.target.size() != 1)
				throw(traceToken(*lexed, *i_token, "Basic instruction's name has length one."));

			if (o_expr.target.size() == 0)
				throw(traceToken(*lexed, *i_token, "Function name expected."));

			o_expr.args = parse_args(i_token, 1);

			expect(i_token);

			if (i_token->content != ";")
				throw(traceToken(*lexed, *i_token, "Expression's end operator ';' expected."));

			i_token ++;

		}

		return o_expr;

	}

	ARG PARSED::parse_arg(TOKEN*& i_token, BYTE i_defMode) {

		ARG o_arg(0);

		expect(i_token);

		BYTE f_defMode = i_defMode;

		if (i_token->content == "[") {

			if (f_defMode == 0)
				throw(traceToken(*lexed, *i_token, "Prefix unexpected."));

			f_defMode = 2;

			next(i_token);

			o_arg.prefix = parse_path(i_token);

			if (o_arg.prefix.size() == 0)
				throw(traceToken(*lexed, *i_token, "Prefix cannot be empty."));

			if (!o_arg.prefix[-1].isLbl())
				throw(traceToken(*lexed, *i_token, "Prefix must end with a block."));

			if (i_token->content != "]")
				throw(traceToken(*lexed, *i_token, "Prefix' end operator ']' expected."));

			next(i_token);

		}
		
		if (f_defMode == 1) {

			if (
				(
					inRange(i_token + 1) &&
					i_token->isSym() &&
					(i_token + 1)->content == ":"
				) ||
				(
					inRange(i_token + 4) &&
					i_token->isSym() &&
					i_token->content.size() == 1 &&
					(i_token + 1)->content == "[" &&
					(i_token + 2)->isGlb() &&
					(i_token + 3)->content == "]" &&
					(i_token + 4)->content == ":"
				)
			) {

				f_defMode = 2;

			}
			else {

				f_defMode = 0;

			}

		}

		if (f_defMode == 2) {

			if (i_token->isSct() && o_arg.prefix.size() > 0)
				throw(traceToken(*lexed, *i_token, "Section definition cannot be prefixed."));

			if (i_token->content.size() == 1) {

				next(i_token);

				if (i_token->content != "[")
					throw(traceToken(*lexed, *i_token, "Global declaration operator '[' expected."));

				next(i_token);

				if (!i_token->isGlb())
					throw(traceToken(*lexed, *i_token, "Global declaration expected."));

				o_arg.type |= (i_token - 2)->type;

				o_arg.name = *i_token;

				next(i_token);

				if (i_token->content != "]")
					throw(traceToken(*lexed, *i_token, "Global declaration operator ']' expected."));

			}
			else {

				o_arg.type |= i_token->type;

				o_arg.name = *i_token;

			}

			i_token ++;

			while (
				inRange(i_token) &&
				(
					i_token->content == "?" ||
					i_token->content == "!"
				)
			) {

				if (i_token->content == "?") {

					if (o_arg.isOpt())
						throw(traceToken(*lexed, *i_token, "Operator '?' unexpected."));

					o_arg.type |= a_O000;

				}
				else {

					if (o_arg.isFrc())
						throw(traceToken(*lexed, *i_token, "Operator '!' unexpected."));

					o_arg.type |= a_0F00;

				}

				i_token ++;

			}

		}

		if (
			f_defMode == 0 ||
			(
				inRange(i_token) &&
				i_token->content == ":"
			)
		) {

			if (
				inRange(i_token) &&
				i_token->content == ":"
			) {

				i_token ++;

			}

			if 		(!inRange(i_token)) {

				o_arg.type |= a_00N0;

			}
			else if	(!i_token->isSym()) {
				
				if	 (i_token->isLbl() || i_token->content == ".") {

					o_arg.type |= a_00T0;

					o_arg.target = parse_path(i_token);

					if (o_arg.target.size() == 0)
						throw(traceToken(*lexed, *i_token, "Target cannot be empty."));

					if (!o_arg.target[-1].isSym())
						throw(traceToken(*lexed, *i_token, "Target must end with a definition."));

					o_arg.type |= o_arg.target[-1].type;

				}
				else if	(i_token->isLtr()) {

					if (f_defMode == 2) {

						if (!o_arg.isVal())
							throw(traceToken(*lexed, *i_token, "Number or string unexpected."));

					}
					else o_arg.type |= a_000V;

					o_arg.type |= a_00L0;

					o_arg.target = *i_token;

					i_token ++;

				}
				else if (i_token->content == "(") {

					if (f_defMode == 2) {

						if (!o_arg.isBox())
							throw(traceToken(*lexed, *i_token, "Argument operator '(' unexpected."));

					}
					else o_arg.type |= a_000B;

					o_arg.type |= a_00L0;

					o_arg.args = parse_args(i_token, i_defMode);

				}
				else o_arg.type |= a_00N0;

			}
			else if (i_token->content.size() == 1) {

				if (f_defMode == 2) {

					if ((o_arg.type & a_0001) != i_token->type)
						throw(traceToken(*lexed, *i_token, "Wrong object type."));

				}
				else o_arg.type |= i_token->type;

				next(i_token);

				if		(i_token->content == "[") {

					next(i_token);

					if (i_token->isGlb()) {

						o_arg.type |= a_00G0;

						o_arg.target = *i_token;

						next(i_token);

						if (i_token->content != "]")
							throw(traceToken(*lexed, *i_token, "Global declaration operator ']' expected."));

						i_token ++;

					}
					else {

						i_token -= 2;

						if		(i_token->isMem()) {

							o_arg.args = parse_sym(i_token);

						}
						else if	(i_token->isVal()) {

							o_arg.args = parse_val(i_token);

						}
						else throw(traceToken(*lexed, *i_token, "Operator '[' unexpected."));

						if		(o_arg.args.size() == 0) {

							o_arg.type |= a_00N0;

						}
						else if (o_arg.args[0][0].target.size() == 2) {

							o_arg.type |= a_00L0;

						}
						else {

							o_arg.type |= a_00C0;

						}

					}

				}
				else if	(i_token->isLtr()) {

					o_arg.type |= a_00L0;

					if (!(i_token - 1)->isReg() && !(i_token - 1)->isSct())
						throw(traceToken(*lexed, *i_token, "Register or section expected."));

					o_arg.target = *i_token;

					i_token ++;				

				}
				else throw(traceToken(*lexed, *i_token, "Global declaration operator '[' expected."));			

			}
			else {

				o_arg.target = parse_path(i_token);

				if (o_arg.target.size() == 0)
					throw(traceToken(*lexed, *i_token, "target cannot be empty."));

				if (!o_arg.target[-1].isSym())
					throw(traceToken(*lexed, *i_token, "Target must end with a definition."));

				if (f_defMode == 2) {

					if ((o_arg.type & a_0001) != o_arg.target[-1].type)
						throw(traceToken(*lexed, *i_token, "Wrong object type."));

				}
				else o_arg.type |= o_arg.target[-1].type;

				o_arg.type |= a_00T0;

			}

		}

		return o_arg;

	}

	BLOCK PARSED::parse_block(TOKEN*& i_token) {

		BLOCK o_block;

		expect(i_token);

		if (i_token != lexed->tokenList.begin() && i_token->content != "{")
			throw(traceToken(*lexed, *i_token, "Block's start operator '{' expected."));

		if (i_token != lexed->tokenList.begin()) {

			next(i_token);

		}

		while (inRange(i_token) && i_token->content != "}") {

			TOKEN* f1_token = i_token;

			BOOL f1_isExpr = false;

			while (
				f1_token->content != "{" &&
				f1_token->content != ";" &&
				f1_token->content != "("
			) {

				next(f1_token);

			}

			f1_isExpr = (f1_token->content == "{" ||
						(f1_token->content == "(" && (f1_token - 1)->isLbl()));


			if (f1_isExpr) {

				o_block.exprList += parse_expr(i_token);

			}
			else {

				o_block.argList += parse_arg(i_token, 2);

				expect(i_token);

				if (i_token->content != ";")
					throw(traceToken(*lexed, *i_token, "Line end operator ';' expected."));

				i_token ++;

			}		

		}

		if (inRange(i_token) && i_token->content == "}") {

			i_token ++;

		}

		return o_block;

	}

	LIST<LIST<ARG>> PARSED::parse_args(TOKEN*& i_token, BYTE i_defMode) {

		LIST<LIST<ARG>> o_args;

		expect(i_token + 1);

		if (i_token->content != "(")
			throw(traceToken(*lexed, *i_token, "Argument's operator '(' expected."));

		if ((i_token + 1)->content == ")") {

			i_token += 2;

			return o_args;

		}

		while (i_token->content != ")") {

			LIST<ARG> f1_argList;

			do {

				next(i_token);

				if (i_token->content == "|") {

					f1_argList += ARG();

				}
				else if (i_token->content == ',' || i_token->content == ')') {

					f1_argList += ARG();
					break;

				}
				else {

					f1_argList += parse_arg(i_token, i_defMode);

				}

				expect(i_token);

				if (i_token->content != "," && i_token->content != "|" && i_token->content != ")")
					throw(traceToken(*lexed, *i_token, "Argument end operator ',' or '|' or ')' expected."));

			} while (i_token->content == "|");

			o_args += f1_argList;

		}

		i_token ++;

		return o_args;

	}

	LIST<TOKEN> PARSED::parse_path(TOKEN*& i_token) {

		LIST<TOKEN> o_path;

		expect(i_token);

		if (i_token->isOpr() && i_token->content != ".")
			throw(traceToken(*lexed, *i_token, "Path expected."));

		if (!i_token->isSym() && !i_token->isLbl() && i_token->content != ".")
			throw(traceToken(*lexed, *i_token, "Wrong symbol syntax."));

		while (i_token->content == ".") {

			o_path += TOKEN();
			
			next(i_token);

		}

		while (inRange(i_token + 1) &&
			 ((i_token + 1)->content == "." || (i_token + 1)->content == "->")) {

			if ((i_token + 1)->content == "." && !i_token->isLbl() && !i_token->isBox())
				throw(traceToken(*lexed, *i_token, "Container expected."));

			o_path += *i_token;

			i_token ++;

			expect(i_token);

			if (i_token->content == "->") {
				
				o_path += TOKEN();

			}

			i_token ++;

		}

		expect(i_token);

		if (
			i_token->isSym() ||
			i_token->isLbl()
		) {

			o_path += *i_token;

			i_token ++;

		}

		return o_path;

	}

	LIST<LIST<ARG>> PARSED::parse_sym(TOKEN*& i_token) {

		LIST<LIST<ARG>> o_args;

		expect(i_token + 2);

		if ((!i_token->isSym() && i_token->content.size() != 1) ||
			(i_token + 1)->content != "[")
			throw(traceToken(*lexed, *i_token, "Symbol operator '$[' expected."));

		i_token += 2;

		if (i_token->content == "]") {

			return o_args;

		}

		o_args += LIST<ARG>();

		o_args[0] += ARG(a_ERR, LIST<TOKEN>(), TOKEN(), *i_token);

		if (i_token->content != "ABS" &&
			i_token->content != "SP" &&
			i_token->content != "SB")
			throw(traceToken(*lexed, *i_token, "In valid sym operation '" + i_token->content + "'."));

		o_args[0][0].target += TOKEN();

		o_args[0] += ARG();
		o_args[0] += ARG();
		o_args[0] += ARG();
		o_args[0] += ARG();

		ARG f1_tmp;

		next(i_token);

		if (i_token->content != ":")
			throw(traceToken(*lexed, *i_token, "Box operator ':' expected."));

		while (true) {

			f1_tmp = parse_arg(i_token, 0);

			if (f1_tmp.isReg()) {

				if (i_token->content == "*" || o_args[0][0].target[0].content == "SP" || o_args[0][0].target[0].content == "SB" || o_args[0][1].is()) {

					if (o_args[0][2].is())
						throw(traceToken(*lexed, *i_token, "Address index already set."));

					o_args[0][2] = f1_tmp;

					if (i_token->content == "*") {

						next(i_token);

						f1_tmp = parse_arg(i_token, 0);

						if (f1_tmp.type != (a_000V | a_00L0))
							throw(traceToken(*lexed, *i_token, "Address index factor expected."));

						if (f1_tmp.target[0].value.size() > 2 ||
						   (f1_tmp.target[0].value[0] != 1 &&
							f1_tmp.target[0].value[0] != 2 &&
							f1_tmp.target[0].value[0] != 4 &&
							f1_tmp.target[0].value[0] != 8))
							throw(traceToken(*lexed, *i_token, "Address index factor expected to be '1' or '2' or '4' or '8'."));

						o_args[0][3] = f1_tmp;

					}

				}
				else {

					if (o_args[0][1].is())
						throw(traceToken(*lexed, *i_token, "Address base already set."));

					o_args[0][1] = f1_tmp;

				}


			}
			else if (f1_tmp.isVal()) {

				if (o_args[0][4].is())
					throw(traceToken(*lexed, *i_token, "Address offset already set."));

				o_args[0][4] = f1_tmp;

			}

			expect(i_token);

			if (i_token->content != "," && i_token->content != "]")
				throw(traceToken(*lexed, *i_token, "Symbol operator ',' expected."));

			if (i_token->content == "]") break;

			next(i_token);

		}

		if (o_args[0][0].target[0].content == "ABS" && !o_args[0][1].is())
			throw(traceToken(*lexed, *i_token, "Address base expected."));

		i_token ++;

		return o_args;

	}

	LIST<LIST<ARG>> PARSED::parse_val(TOKEN*& i_token) {

		LIST<LIST<ARG>> o_args;

		expect(i_token + 2);

		if ((!i_token->isVal() && i_token->content.size() != 1) ||
			(i_token + 1)->content != "[")
			throw(traceToken(*lexed, *i_token, "Value operator '#[' expected."));

		i_token += 2;

		if (i_token->content == "]") {

			return o_args;

		}

		o_args += LIST<ARG>();

		o_args[0] += ARG(a_ERR, LIST<TOKEN>(), TOKEN(), *i_token);

		if (i_token->content != "OR" &&
			i_token->content != "AND" &&
			i_token->content != "XOR" &&
			i_token->content != "ADD" &&
			i_token->content != "APPEND" &&
			i_token->content != "SIZE" &&
			i_token->content != "NOT" &&
			i_token->content != "NEG")
			throw(traceToken(*lexed, *i_token, "In valid box operation '" + i_token->content + "'."));

		next(i_token);

		if (i_token->content != ":")
			throw(traceToken(*lexed, *i_token, "Value operator ':' expected."));

		next(i_token);

		ARG f_tmp;

		while (true) {

			f_tmp = parse_arg(i_token, 0);

			if (!f_tmp.isVal())
				throw(traceToken(*lexed, *i_token, "Value expected."));

			o_args[0] += f_tmp;

			expect(i_token);

			if (i_token->content != "," && i_token->content != "]")
				throw(traceToken(*lexed, *i_token, "Value operator ',' expected."));

			if (i_token->content == "]") break;

			next(i_token);

		}

		if (o_args[0].size() != 2 &&
		   (o_args[0][0].target[0].content == "SIZE" ||
			o_args[0][0].target[0].content == "NOT" ||
			o_args[0][0].target[0].content == "NEG"))
			throw(traceToken(*lexed, *i_token, "Value operator '" + o_args[0][0].target[0].content + "' can only handle one operand."));

		if (o_args[0].size() < 3 &&
		   (o_args[0][0].target[0].content == "OR" ||
			o_args[0][0].target[0].content == "AND" ||
			o_args[0][0].target[0].content == "XOR" ||
			o_args[0][0].target[0].content == "ADD" ||
			o_args[0][0].target[0].content == "APPEND"))
			throw(traceToken(*lexed, *i_token, "Value operator '" + o_args[0][0].target[0].content + "' can only handle at least two operand."));

		i_token ++;

		return o_args;

	}

	VOID PARSED::next(TOKEN*& i_token) {

		i_token ++;

		if (i_token >= lexed->tokenList.end())
			throw(traceToken(*lexed, lexed->tokenList[-1], "Unexpected file's end."));

	}

	VOID PARSED::expect(TOKEN* i_token) {

		if (i_token >= lexed->tokenList.end())
			throw(traceToken(*lexed, lexed->tokenList[-1], "Unexpected file's end."));

	}

	BOOL PARSED::inRange(TOKEN* i_token) {

		return (i_token < lexed->tokenList.end());

	}
	
	BOOL ARG::is() {

		return (type != a_ERR);

	}

	BOOL ARG::isMem() {

		return ((type & a_0001) == a_000M);

	}
	
	BOOL ARG::isReg() {

		return ((type & a_0001) == a_000R);

	}
	
	BOOL ARG::isVal() {

		return ((type & a_0001) == a_000V);

	}
	
	BOOL ARG::isSct() {

		return ((type & a_0001) == a_000S);

	}
	
	BOOL ARG::isBox() {

		return ((type & a_0001) == a_000B);

	}
	
	BOOL ARG::isTrg() {

		return ((type & a_0010) == a_00T0);

	}
	
	BOOL ARG::isGlb() {

		return ((type & a_0010) == a_00G0);

	}
	
	BOOL ARG::isCal() {

		return ((type & a_0010) == a_00C0);

	}
	
	BOOL ARG::isLtr() {

		return ((type & a_0010) == a_00L0);

	}

	BOOL ARG::isNul() {

		return ((type & a_0010) == a_00N0);

	}
	
	BOOL ARG::isOpt() {

		return ((type & a_1000) == a_O000);

	}
	
	BOOL ARG::isFrc() {

		return ((type & a_0100) == a_0F00);

	}

	BOOL EXPR::is() {

		return (type != e_ERR);

	}

	BOOL EXPR::isDcl() {

		return ((type & e_1000) == e_D000);

	}
	
	BOOL EXPR::isMtd() {

		return ((type & e_1000) != e_D000 && section.size() == 0);

	}
	
	BOOL EXPR::isOpr() {

		return ((type & e_1000) != e_D000 && section.size() > 0);

	}
	
	BOOL EXPR::isRoot() {

		return ((type & e_0100) == e_0R00);

	}
	
	BOOL EXPR::isHidden() {

		return ((type & e_0010) == e_00H0);

	}
	
	BOOL EXPR::isPrivate() {

		return ((type & e_0001) == e_000P);

	}
	
	STRING PARSED::string() {

		return string_block(block);

	}

	STRING string_expr(EXPR i_expr, DWORD i_tab) {

		STRING o_string;

		for (DEF* f1_def = i_expr.defList.begin(); f1_def < i_expr.defList.end(); f1_def ++) {

			if (f1_def->prefix.size() > 0) {

				o_string += "[";
				o_string += string_path(f1_def->prefix);
				o_string += "] ";

			}

			if (f1_def->name.isGlb()) {

				o_string += "$[";
				o_string += f1_def->name.content;
				o_string += "]";

			}
			else {

				o_string += f1_def->name.content;

			}

			o_string += " :\n";
			o_string += string_tab(i_tab);

		}

		if		(i_expr.isOpr()) {

			if (i_expr.section[0].isLtr()) {

				o_string += "&";
				o_string += i_expr.section[0].content;

			}
			else if (i_expr.section[0].isGlb()) {

				o_string += "&[";
				o_string += i_expr.section[0].content;
				o_string += "]";

			}
			else {

				o_string += string_path(i_expr.section);

			}
			
			o_string += "->";

			for (TOKEN* f1_block = i_expr.target.begin(); f1_block < i_expr.target.end(); f1_block ++) {

				o_string += f1_block->content;

				if (f1_block + 1 < i_expr.target.end()) {

					o_string += ".";

				}

			}

			o_string += string_args(i_expr.args);

		}
		else if	(i_expr.isMtd()) {

			o_string += string_path(i_expr.target);
			o_string += string_args(i_expr.args);

		}
		else {

			if (i_expr.isRoot()) {

				o_string += ".";

			}

			o_string += i_expr.target[0].content;

			if (i_expr.args.size() > 0 || !i_expr.isPrivate()) {

				o_string += string_args(i_expr.args);

			}
			
			if (i_expr.isHidden()) {

				o_string += "?";

			}

			if (i_expr.isPrivate() && i_expr.args.size() > 0) {

				o_string += "!";

			}

			o_string += " ";
			o_string += string_path(i_expr.section);
			o_string += " ";
			o_string += string_block(i_expr.block, i_tab + 1);

		}

		o_string += ";";

		return o_string;

	}

	STRING string_arg(ARG i_arg) {

		STRING o_string;

		if (!i_arg.is()) return o_string;

		if (i_arg.name.is()) {

			if (i_arg.prefix.size() > 0) {

				o_string += "[";
				o_string += string_path(i_arg.prefix);
				o_string += "] ";

			}

			if (i_arg.name.isGlb()) {

				if		(i_arg.isBox()) {

					o_string += "@";

				}
				else if	(i_arg.isMem()) {

					o_string += "$";

				}
				else if	(i_arg.isReg()) {

					o_string += "%";

				}
				else if	(i_arg.isVal()) {

					o_string += "#";

				}
				else if	(i_arg.isSct()) {

					o_string += "&";

				}

				o_string += "[";
				o_string += i_arg.name.content;
				o_string += "]";

			}
			else {

				o_string += i_arg.name.content;

			}

			if (i_arg.isOpt()) {

				o_string += "?";

			}

			if (i_arg.isFrc()) {

				o_string += "!";

			}

			if	(!i_arg.isNul()) {

				o_string += " : ";

			}

		}

		if		(i_arg.isTrg()) {

			o_string += string_path(i_arg.target); 

		}
		else if	(i_arg.isGlb()) {

			if		(i_arg.isBox()) {

				o_string += "@";

			}
			else if	(i_arg.isMem()) {

				o_string += "$";

			}
			else if	(i_arg.isReg()) {

				o_string += "%";

			}
			else if	(i_arg.isVal()) {

				o_string += "#";

			}
			else if	(i_arg.isSct()) {

				o_string += "&";

			}

			o_string += "[";
			o_string += i_arg.target[0].content;
			o_string += "]";

		}
		else if	(i_arg.isLtr()) {

			if		(i_arg.isBox()) {

				o_string += string_args(i_arg.args);

			}
			else if	(i_arg.isMem()) {

				if (i_arg.args.size() > 0) {

					o_string += string_sym(i_arg.args);

				}
				else {

					o_string += i_arg.target[0].content;

				}

			}
			else if	(i_arg.isReg()) {

				o_string += "%";
				o_string += i_arg.target[0].content;

			}
			else if	(i_arg.isVal()) {

				o_string += "0x";
				o_string += i_arg.target[0].value.hex();

			}
			else if	(i_arg.isSct()) {

				o_string += "&";
				o_string += i_arg.target[0].content;

			}

		}
		else if	(i_arg.isCal()) {

			if		(i_arg.isMem()) {

				o_string += string_sym(i_arg.args);

			}
			else if	(i_arg.isVal()) {

				o_string += string_val(i_arg.args);

			}

		}

		return o_string;

	}

	STRING string_block(BLOCK i_block, DWORD i_tab) {

		STRING o_string;

		if (i_tab != 0) {

			o_string += "{\n\n";

		}

		for (ARG* f1_arg = i_block.argList.begin(); f1_arg < i_block.argList.end(); f1_arg ++) {

			o_string += string_tab(i_tab);
			o_string += string_arg(*f1_arg);
			o_string += ";";
			o_string += "\n";

		}

		for (EXPR* f1_expr = i_block.exprList.begin(); f1_expr < i_block.exprList.end(); f1_expr ++) {

			o_string += string_tab(i_tab);
			o_string += string_expr(*f1_expr, i_tab);
			o_string += "\n";

		}

		if (i_tab != 0) {

			o_string += "\n";
			o_string += string_tab(i_tab - 1);
			o_string += "}";

		}
		else {

			o_string = o_string.to(-1);

		}

		return o_string;

	}
	
	STRING string_args(LIST<LIST<ARG>> i_args) {

		STRING o_string;

		o_string += "(";

		for (LIST<ARG>* f1_argList = i_args.begin(); f1_argList < i_args.end(); f1_argList ++) {

			for (ARG* f2_arg = f1_argList->begin(); f2_arg < f1_argList->end(); f2_arg ++) {

				o_string += string_arg(*f2_arg);

				if (f2_arg + 1 < f1_argList->end()) {

					o_string += "|";

				}

			}

			if (f1_argList + 1 < i_args.end()) {

				o_string += ", ";

			}

		}

		o_string += ")";

		return o_string;

	}

	STRING string_path(LIST<TOKEN> i_path) {

		STRING o_string;

		for (TOKEN* f1_token = i_path.begin(); f1_token < i_path.end(); f1_token ++) {

			if (!f1_token->is()) {

				if (f1_token == i_path.begin()) {
					
					while (f1_token < i_path.end() && !f1_token->is()) {
						
						o_string += ".";

						f1_token ++;

					}

					f1_token --;

				}
				else o_string += "->";

			}
			else {
				
				if	(f1_token->is() && f1_token != i_path.begin() && (f1_token - 1)->is())
					o_string += ".";

				if (f1_token->isSym() &&
				   (f1_token == i_path.begin() || !(f1_token - 1)->is())) {

					for (TOKEN* f2_token = f1_token; f2_token < i_path.end(); f2_token ++) {

						if (f2_token + 1 == i_path.end() || !(f2_token + 1)->is()) {

							o_string += f2_token->content[0];

							break;

						}

					}

				}

				if (f1_token->isSym()) {

					o_string += f1_token->content.from(1);

				}
				else {
			
					o_string += f1_token->content;

				}

			}

		}

		return o_string;

	}

	STRING string_tab(DWORD i_tab) {

		STRING o_string;

		for (DWORD f1_tab = 0; f1_tab < i_tab; f1_tab ++) {
			
			o_string += "\t";

		}

		return o_string;

	}

	STRING string_sym(LIST<LIST<ARG>> i_args) {

		STRING o_string;

		o_string += "$[";
		
		o_string += i_args[0][0].target[0].content;

		o_string += ": ";

		if (i_args[0][1].is()) {

			o_string += string_arg(i_args[0][1]);

		}

		if (i_args[0][2].is()) {

			if (i_args[0][1].is()) {

				o_string += ", ";

			}

			o_string += string_arg(i_args[0][2]);

			if (i_args[0][3].is()) {

				o_string += " * ";
				o_string += BYTE('0' + i_args[0][3].target[0].value[0]);

			}
			else {

				o_string += " * 1";

			}

		}

		if (i_args[0][4].is()) {

			if (i_args[0][1].is() || i_args[0][2].is()) {

				o_string += ", ";

			}

			o_string += string_arg(i_args[0][4]);

		}

		o_string += "]";

		return o_string;

	}

	STRING string_val(LIST<LIST<ARG>> i_args) {

		STRING o_string;

		o_string += "#[";

		o_string += i_args[0][0].target[0].content;

		o_string += ": ";

		for (ARG* f1_arg = i_args[0].begin() + 1; f1_arg < i_args[0].end(); f1_arg ++) {

			o_string += string_arg(*f1_arg);

			if (f1_arg + 1 < i_args[0].end()) {

				o_string += ", ";

			}

		}

		o_string += "]";

		return o_string;

	}

	ERROR traceExpr(PARSED& i_parsed, EXPR& i_expr, STRING i_message) {

		return traceToken(*i_parsed.lexed, i_expr.target[0], i_message);

	}

	ERROR tracePath(PARSED& i_parsed, LIST<TOKEN> i_path, STRING i_message) {

		for (TOKEN* f1_path = i_path.end() - 1; f1_path >= i_path.begin(); f1_path --) {
			
			if (f1_path->is()) {
			
				return traceToken(*i_parsed.lexed, *f1_path, i_message);

			}

		}

		return ERROR(i_message);

	}

}