#ifndef DEF_ANTHRACITE_LOADER
#define DEF_ANTHRACITE_LOADER

#include <anthracite.hpp>

namespace ANTHRACITE {

	struct	TRACE;
	struct	LOADED;

	struct	TRACE {

		DWORD			start;
		STRING			file;
		DWORD			offset;

		TRACE(DWORD i_start = 0, STRING i_file = STRING(), DWORD i_offset = 0) : start(i_start), file(i_file), offset(i_offset) {}
		TRACE(const TRACE& i_trace) : start(i_trace.start), file(i_trace.file), offset(i_trace.offset) {}

	};

	struct	LOADED {

		public :

			STRING				source;
			LIST<TRACE>			traceList;
			LIST<STRING>		linkList;
			LIST<STRING>		includeList;
			STRING				entry;
			STRING				subsystem;

			LOADED(STRING i_filename, LIST<STRING> i_pathList);

			STRING string();

		private :

			LOADED(STRING i_filename, LIST<STRING> i_pathList, BOOL i_root);
			VOID futureInclude(STRING i_filename, LIST<STRING>& i_includeList);
			VOID include(STRING i_filename, LIST<STRING> i_pathList, LIST<STRING>& i_includeList);
			VOID link(STRING i_filename);
		
	};

	ERROR tracePointer(LOADED& i_loaded, BYTE* i_char, STRING i_message);

}

#include <anthracite_loader.cpp>

#endif