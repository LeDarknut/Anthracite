#ifndef DEF_ANTHRACITE_ASSEMBLER
#define DEF_ANTHRACITE_ASSEMBLER

#include <anthracite.hpp>
#include <anthracite_encoder.hpp>

namespace ANTHRACITE {

	struct	DEFT;
	struct	REQT;

	struct	ASSEMBLED;

	struct	DEFT {

		SWORD		section;
		DWORD		offset;
		TOKEN		token;

		DEFT(SWORD i_section = s_GLB, DWORD i_offset = 0, TOKEN i_token = TOKEN()) : section(i_section), offset(i_offset), token(i_token) {}
		DEFT(const DEFT& i_deft) : section(i_deft.section), offset(i_deft.offset), token(i_deft.token) {}

	};

	struct	REQT {

		SWORD		section;
		DWORD		offset;
		BYTE		roffset;
		TOKEN		token;

		REQT(SWORD i_section = s_GLB, DWORD i_offset = 0, BYTE i_roffset = 0, TOKEN i_token = TOKEN()) : section(i_section), offset(i_offset), roffset(i_roffset), token(i_token) {}
		REQT(const REQT& i_reqt) : section(i_reqt.section), offset(i_reqt.offset), roffset(i_reqt.roffset), token(i_reqt.token) {}

	};

	struct	ASSEMBLED {

		public :

			ENCODED*			encoded;
			LIST<STRING>		sectionList;
			LIST<DEFT>			defList;
			LIST<REQT>			reqList;

			ASSEMBLED() {};
			ASSEMBLED(ENCODED& i_encoded);

			STRING	assemble();
			STRING	assemble_code();
			STRING	assemble_req();
			STRING	assemble_def();
			STRING	string();
			VOID save(STRING i_file);

		private :

			SDWORD symOffset(INST* i_inst);

			VOID addDef(INST* i_inst);
			VOID addReq(INST* i_inst);
			
	};

}

#include <anthracite_assembler.cpp>

#endif