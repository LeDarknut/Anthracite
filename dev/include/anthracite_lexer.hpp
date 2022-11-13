#ifndef DEF_ANTHRACITE_LEXER
#define DEF_ANTHRACITE_LEXER

#include <anthracite.hpp>
#include <anthracite_loader.hpp>

namespace ANTHRACITE {

	struct	TOKEN;

	struct	LEXED;

	const BYTE t_MEM			= 1;
	const BYTE t_REG			= 2;
	const BYTE t_VAL			= 3;
	const BYTE t_SCT			= 4;
	const BYTE t_BOX			= 5;
	const BYTE t_LTR			= 6;
	const BYTE t_GLB			= 7;
	const BYTE t_LBL	 		= 8;
	const BYTE t_OPR	 		= 9;
	const BYTE t_ERR	 		= 0xFF;
	
	struct	TOKEN {

		BYTE			type;
		BYTE*			pointer;
		STRING			content;
		STRING			value;

		TOKEN(BYTE i_type = t_ERR, BYTE* i_pointer = 0, STRING i_content = STRING(), STRING i_value = STRING()) : type(i_type), pointer(i_pointer), content(i_content), value(i_value) {}
		TOKEN(const TOKEN& i_token) : type(i_token.type), pointer(i_token.pointer), content(i_token.content), value(i_token.value) {}

		BOOL is();
		BOOL isMem();
		BOOL isReg();
		BOOL isVal();
		BOOL isSct();
		BOOL isBox();
		BOOL isLtr();
		BOOL isGlb();
		BOOL isLbl();
		BOOL isOpr();
		BOOL isSym();

	};

	struct	LEXED {

		LOADED*			loaded;
		LIST<TOKEN>		tokenList;

		LEXED(LOADED& i_loaded);

		STRING string();
		
	};

	ERROR traceToken(LEXED& i_lexed, TOKEN& i_token, STRING i_message);

}

#include <anthracite_lexer.cpp>

#endif