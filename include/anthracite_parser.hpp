#ifndef DEF_ANTHRACITE_PARSER
#define DEF_ANTHRACITE_PARSER

#include <anthracite.hpp>
#include <anthracite_lexer.hpp>

namespace ANTHRACITE {

	struct	DEF;
	struct	ARG;
	struct	EXPR;
	struct	BLOCK;
	struct	ARGS;

	struct	PARSED;

	const BYTE a_O000	= 0b10000000;
	const BYTE a_0F00	= 0b01000000;
	const BYTE a_00N0	= 0b00000000;
	const BYTE a_00T0	= 0b00001000;
	const BYTE a_00G0	= 0b00010000;
	const BYTE a_00C0	= 0b00011000;
	const BYTE a_00L0	= 0b00100000;
	const BYTE a_000M	= 0b00000001;
	const BYTE a_000R	= 0b00000010;
	const BYTE a_000V	= 0b00000011;
	const BYTE a_000S	= 0b00000100;
	const BYTE a_000B	= 0b00000101;

	const BYTE a_ERR	= 0xFFFF;

	const BYTE a_1000	= 0b10000000;
	const BYTE a_0100	= 0b01000000;
	const BYTE a_0010	= 0b00111000;
	const BYTE a_0001	= 0b00000111;

	const BYTE e_D000	= 0b1000;
	const BYTE e_0R00	= 0b0100;
	const BYTE e_00H0	= 0b0010;
	const BYTE e_000P	= 0b0001;

	const BYTE e_ERR	= 0xFFFF;

	const BYTE e_1000	= 0b1000;
	const BYTE e_0100	= 0b0100;
	const BYTE e_0010	= 0b0010;
	const BYTE e_0001	= 0b0001;

	struct	DEF {

		LIST<TOKEN>			prefix;
		TOKEN				name;

		DEF(LIST<TOKEN> i_prefix = LIST<TOKEN>(), TOKEN i_name = TOKEN()) : prefix(i_prefix), name(i_name) {}
		DEF(const DEF& i_def) : prefix(i_def.prefix), name(i_def.name) {}

	};

	struct	ARG {

		BYTE				type;

		LIST<TOKEN>			prefix;
		TOKEN				name;

		LIST<TOKEN>			target;
		LIST<LIST<ARG>>		args;

		ARG(BYTE i_type = a_ERR, LIST<TOKEN> i_prefix = LIST<TOKEN>(), TOKEN i_name = TOKEN(), LIST<TOKEN> i_target = LIST<TOKEN>(), LIST<LIST<ARG>> i_args = LIST<LIST<ARG>>()) : type(i_type), prefix(i_prefix), name(i_name), target(i_target), args(i_args) {}
		ARG(const ARG& i_arg) : type(i_arg.type), prefix(i_arg.prefix), name(i_arg.name), target(i_arg.target), args(i_arg.args) {}

		BOOL is();
		BOOL isMem();
		BOOL isReg();
		BOOL isVal();
		BOOL isSct();
		BOOL isBox();
		BOOL isTrg();
		BOOL isGlb();
		BOOL isCal();
		BOOL isLtr();
		BOOL isNul();
		BOOL isOpt();
		BOOL isFrc();

	};

	struct	BLOCK {

		LIST<EXPR>			exprList;
		LIST<ARG>			argList;

		BLOCK(LIST<EXPR> i_exprList = LIST<EXPR>(), LIST<ARG> i_argList = LIST<ARG>()) : exprList(i_exprList), argList(i_argList) {}
		BLOCK(const BLOCK& i_block) : exprList(i_block.exprList), argList(i_block.argList) {}

	};
	
	struct	EXPR {

		BYTE				type;

		LIST<DEF>			defList;
		LIST<TOKEN>			target;
		LIST<TOKEN>			section;
		LIST<LIST<ARG>>		args;
		BLOCK				block;

		EXPR(BYTE i_type = e_ERR, LIST<DEF> i_defList = LIST<DEF>(), LIST<TOKEN> i_target = LIST<TOKEN>(), LIST<TOKEN> i_section = LIST<TOKEN>(), LIST<LIST<ARG>> i_args = LIST<LIST<ARG>>(), BLOCK i_block = BLOCK()) : type(i_type), defList(i_defList), target(i_target), section(i_section), args(i_args), block(i_block) {}
		EXPR(const EXPR& i_expr) : type(i_expr.type), defList(i_expr.defList), target(i_expr.target), section(i_expr.section), args(i_expr.args), block(i_expr.block) {}

		BOOL is();
		BOOL isDcl();
		BOOL isMtd();
		BOOL isOpr();
		BOOL isRoot();
		BOOL isHidden();
		BOOL isPrivate();

	};

	struct	PARSED {

		public :

			LEXED*				lexed;
			BLOCK				block;

			PARSED() {};
			PARSED(LEXED& i_lexed);

			STRING	string();

		private :

			EXPR			parse_expr(TOKEN*& i_token);
			ARG				parse_arg(TOKEN*& i_token, BYTE i_defMode = 2);
			BLOCK			parse_block(TOKEN*& i_token);
			LIST<LIST<ARG>>	parse_args(TOKEN*& i_token, BYTE i_defMode = 2);
			LIST<TOKEN>		parse_path(TOKEN*& i_token);
			LIST<LIST<ARG>>	parse_mem(TOKEN*& i_token);
			LIST<LIST<ARG>>	parse_val(TOKEN*& i_token);

			VOID next(TOKEN*& i_token);
			VOID expect(TOKEN* i_token);
			BOOL inRange(TOKEN* i_token);

	};

	STRING	string_expr(EXPR i_expr, DWORD i_tab = 0);
	STRING	string_arg(ARG i_arg);
	STRING	string_block(BLOCK i_block, DWORD i_tab = 0);
	STRING	string_args(LIST<LIST<ARG>> i_args);
	STRING	string_path(LIST<TOKEN> i_path);
	STRING	string_tab(DWORD i_tab);
	STRING	string_sym(LIST<LIST<ARG>> i_args);
	STRING	string_val(LIST<LIST<ARG>> i_args);
	STRING	string_tab(DWORD i_tab);

	ERROR traceExpr(PARSED& i_parsed, EXPR& i_expr, STRING i_message);
	ERROR tracePath(PARSED& i_parsed, LIST<TOKEN> i_path, STRING i_message);

}

#include <anthracite_parser.cpp>

#endif