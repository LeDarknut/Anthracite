#ifndef DEF_ANTHRACITE_SOLVER
#define DEF_ANTHRACITE_SOLVER

#include <anthracite.hpp>
#include <anthracite_parser.hpp>

namespace ANTHRACITE {

	struct	SOLVED;

	struct	FOUND;
	struct	LINE;
	struct	ARGV;

	struct	FOUND {

		LIST<EXPR*> 	scopeList;
		EXPR*			expr;
		ARG*			arg;
		DEF*			def;
		LIST<FOUND>		prefixed;
		LIST<ARG*>		forbidden;

		FOUND(LIST<EXPR*> i_scopeList = LIST<EXPR*>(), EXPR* i_expr = 0, ARG* i_arg = 0, DEF* i_def = 0, LIST<FOUND> i_prefixed = LIST<FOUND>(), LIST<ARG*> i_forbidden = LIST<ARG*>()) : scopeList(i_scopeList), expr(i_expr), arg(i_arg), def(i_def), prefixed(i_prefixed), forbidden(i_forbidden) {}
		FOUND(const FOUND& i_found) : scopeList(i_found.scopeList), expr(i_found.expr), arg(i_found.arg), def(i_found.def), prefixed(i_found.prefixed), forbidden(i_found.forbidden) {}

	};

	struct	LINE {

		LIST<TOKEN>			symList;
		TOKEN				section;
		TOKEN				name;
		LIST<ARGV>			argList;

		LINE(LIST<TOKEN> i_symList = LIST<TOKEN>(), TOKEN i_name = TOKEN(), TOKEN i_section = TOKEN(), LIST<ARGV> i_argList = LIST<ARGV>()) : symList(i_symList), name(i_name), section(i_section), argList(i_argList) {}
		LINE(const LINE& i_line) : symList(i_line.symList), name(i_line.name), section(i_line.section), argList(i_line.argList) {}

	};

	struct	ARGV {

		STRING				name;
		TOKEN				value;
		LIST<ARGV>			argList;

		ARGV(STRING i_name = "", TOKEN i_value = TOKEN(), LIST<ARGV> i_argList = LIST<ARGV>()) : name(i_name), value(i_value), argList(i_argList) {}
		ARGV(const ARGV& i_arg) : name(i_arg.name), value(i_arg.value), argList(i_arg.argList) {}

	};
	
	struct	SOLVED {

		public :

			PARSED*				parsed;
			LIST<LINE>			lineList;
			
			SOLVED() {};
			SOLVED(PARSED& i_parsed);

			STRING	string();

		private :

			LIST<LINE> 		solve_block(LIST<EXPR*> i_scopeList, BLOCK i_block, LIST<TOKEN> i_forbidden = LIST<TOKEN>());
			LIST<ARGV> 		argify(LIST<LIST<ARG>>& i_args);
			LIST<LIST<ARG>> solve_args(LIST<EXPR*> i_scopeList, LIST<LIST<ARG>>& i_args, LIST<ARG*> i_forbidden = LIST<ARG*>());
			ARG 			solve_arg(LIST<EXPR*> i_scopeList, ARG& i_arg, LIST<ARG*> i_forbidden = LIST<ARG*>());
			LIST<LIST<ARG>> solve_fargs(LIST<LIST<ARG>> i_fargs, LIST<LIST<ARG>> i_cargs);
			LIST<ARG> 		solve_fargList(LIST<ARG> i_fargList, LIST<ARG> i_cargList = LIST<ARG>());
			TOKEN			solve_section(LIST<EXPR*> i_scopeList, LIST<TOKEN> i_section);

			ARG toArg(FOUND& i_found);
			VOID nullArg(ARG& i_arg);
			BOOL isDifferent(FOUND& i_found, LIST<FOUND> i_foundList);
			BOOL isDifferent(ARG& i_arg1, ARG& i_arg2);
			
			LIST<FOUND> find_path(LIST<EXPR*> i_scopeList, LIST<TOKEN> i_path, LIST<FOUND> i_prefixed = LIST<FOUND>(), LIST<ARG*> i_forbidden = LIST<ARG*>());
			LIST<FOUND> find_block(LIST<EXPR*> i_scopeList, LIST<TOKEN> i_path, LIST<FOUND> i_prefixed = LIST<FOUND>(), LIST<ARG*> i_forbidden = LIST<ARG*>());
			LIST<FOUND> find_def(LIST<EXPR*> i_scopeList, LIST<TOKEN> i_path, LIST<FOUND> i_prefixed = LIST<FOUND>(), LIST<ARG*> i_forbidden = LIST<ARG*>());
			LIST<FOUND> find_arg(LIST<EXPR*> i_scopeList, LIST<TOKEN> i_path, LIST<LIST<ARG>>& i_args, LIST<FOUND> i_prefixed = LIST<FOUND>(), LIST<ARG*> i_forbidden = LIST<ARG*>());
			LIST<FOUND> find_glob(LIST<EXPR*> i_scopeList, TOKEN& i_glob, LIST<FOUND> i_prefixed = LIST<FOUND>(), LIST<ARG*> i_forbidden = LIST<ARG*>());
			
			STRING string_argList(LIST<ARGV> i_argList);
			
	};

	ARGV* arg(LIST<ARGV>& i_argList, STRING i_argName);
	ARGV* arg(LIST<ARGV>& i_argList, STRING i_argName, DWORD i_index);

	ERROR traceLine(SOLVED& i_solved, LINE& i_line, STRING i_message);

}

#include <anthracite_solver.cpp>

#endif