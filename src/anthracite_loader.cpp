#include <anthracite_loader.hpp>

namespace ANTHRACITE {

	LOADED::LOADED(STRING i_filename, LIST<STRING> i_pathList) : LOADED(i_filename, i_pathList, true) {}

	LOADED::LOADED(STRING i_filename, LIST<STRING> i_pathList, BOOL i_root) : source(), traceList(), linkList(), includeList() {
		
		std::ifstream f_file(i_filename.c(), std::ifstream::binary);

		if (f_file.fail()) {

			throw(ERROR("Failed to open file '" + i_filename + "'."));

		}
		
		traceList += TRACE(0, i_filename, 0);

		char f_char = 0;

		DWORD f_blockLevel = 0;

		LIST<DWORD> f_rootList = 0;

		LIST<LIST<STRING>> f_includeList = LIST<STRING>();

		BOOL f_nextRoot = false;

		DWORD f_line = 1;
		DWORD f_column = 0;

		while (f_file.get(f_char)) {

			if		(f_char == '\n') {f_line ++; f_column = 0;}
			else if	(f_char == '\t') f_column += 4;
			else	f_column ++;

			if		(f_char == '<') {

				STRING f1_macroMode;

				STRING f1_macroContent;

				BOOL f1_include = true;

				while (f_file.get(f_char) && f_char != '>') {

					if (f_char == '/') f_char = '\\';

					if (f_char == ':') {

						f1_include = false;

						f1_macroMode = f1_macroContent;

						f1_macroContent = "";

						continue;

					}

					f1_macroContent += f_char;

				}

				if (f1_include) {

					if (std::filesystem::path(f1_macroContent.c()).extension() == "") {

						f1_macroContent = std::filesystem::path(f1_macroContent.c()).replace_extension(".acs").string();

					}

					for (STRING* f2_path = i_pathList.begin(); f2_path < i_pathList.end(); f2_path ++) {

						if (std::filesystem::exists((*f2_path + "\\" + f1_macroContent).c())) {

							f1_macroContent = std::filesystem::absolute((*f2_path + "\\" + f1_macroContent).c()).string();

							break;

						}

						if (f2_path + 1 == i_pathList.end()) {

							throw(ERROR("Error caught in " + i_filename + " at L" + toString(f_line) + "C" + toString(f_column) + " : File '" + f1_macroContent + "' not found."));

						}

					}

					if (f_blockLevel != f_rootList[-1]) {

						throw(ERROR("Error caught in " + i_filename + " at L" + toString(f_line) + "C" + toString(f_column) + " : Include not in root blocks."));

					}

					if (f_includeList.size() > 1 || i_root) {

						include(f1_macroContent, i_pathList, f_includeList[-1]);

						traceList += TRACE(source.size(), i_filename, f_file.tellg());

					}
					else {

						futureInclude(f1_macroContent, f_includeList[-1]);

					}

				}
				else if	(f1_macroMode == "link") {

					for (STRING* f2_path = i_pathList.begin(); f2_path < i_pathList.end(); f2_path ++) {

						if (std::filesystem::exists((*f2_path + "\\" + f1_macroContent).c())) {

							f1_macroContent = std::filesystem::absolute((*f2_path + "\\" + f1_macroContent).c()).string();

							break;

						}

						if (f2_path + 1 == i_pathList.end()) {

							throw(ERROR("Error caught in " + i_filename + " at L" + toString(f_line) + "C" + toString(f_column) + " : File '" + f1_macroContent + "' not found."));

						}

					}

					link(f1_macroContent);

				}
				else if (f1_macroMode == "entry") {

					if (entry.size() > 0) {

						throw(ERROR("Error caught in " + i_filename + " at L" + toString(f_line) + "C" + toString(f_column) + " : Entry point already set to '" + entry + "'."));

					}

					entry = "/entry:" + f1_macroContent;

				}
				else if (f1_macroMode == "subsystem") {

					if (subsystem.size() > 0) {

						throw(ERROR("Error caught in " + i_filename + " at L" + toString(f_line) + "C" + toString(f_column) + " : Subsystem already set to '" + subsystem + "'."));

					}

					subsystem = "/subsystem:" + f1_macroContent;

				}
				else {

					throw(ERROR("Error caught in " + i_filename + " at L" + toString(f_line) + "C" + toString(f_column) + " : " + "Wrong macro '" + f1_macroMode + "'."));

				}

			}
			else if	(f_char == '/' && f_file.peek() == '*') {

				f_file.get(f_char);

				while (f_file.get(f_char) && (f_char != '*' || f_file.peek() != '/')) continue;

				f_file.get(f_char);

				traceList += TRACE(source.size(), i_filename, f_file.tellg());

			}
			else if	(f_char > 32) {

				source += f_char;

				if		(f_char == '"') {

					while (f_file.get(f_char) && f_char != '"') {
						
						if (f_char >= 32) source += f_char;

						if (f_char == '\\' && f_file.get(f_char)) source += f_char;

					}

					if (f_char == '"') source += f_char;

				}
				else if (f_char == '{') {

					f_blockLevel ++;

					if (f_nextRoot) {

						f_rootList += f_blockLevel;

						f_includeList += LIST<STRING>();

					}

					f_nextRoot = false;

				}
				else if (f_char == '}') {

					if (f_blockLevel != 0) {

						if (f_blockLevel == f_rootList[-1]) {

							f_rootList -= 1;

							f_includeList -= 1;

						}

						f_blockLevel --;

					}				

				}
				else if (f_char == '.') {

					DWORD f1_pos = f_file.tellg();

					while (f_file.get(f_char) && f_char != ';') {

						if		(f_char == '(') {

							while (f_file.get(f_char) && f_char != ';') {

								if (f_char == ')') {
									
									break;

								}

							}

							if (f_char == ';') {

								break;

							}

						}
						else if	(f_char == ')') {
									
							break;

						}
						else if	(f_char == '{') {

							f_nextRoot = true;
							
							break;

						}

					}

					f_file.seekg(f1_pos);

				}

			}

		}

		includeList = f_includeList[0];

	}

	VOID LOADED::futureInclude(STRING i_filename, LIST<STRING>& i_includeList) {

		for (STRING* f2_include = i_includeList.begin(); f2_include < i_includeList.end(); f2_include ++) {

			if (*f2_include == i_filename) {

				return;

			}

		}

		i_includeList += i_filename;

	}

	VOID LOADED::include(STRING i_filename, LIST<STRING> i_pathList, LIST<STRING>& i_includeList) {

		for (STRING* f1_include = i_includeList.begin(); f1_include < i_includeList.end(); f1_include ++) {

			if (*f1_include == i_filename) {

				return;

			}

		}

		i_includeList += i_filename;

		LOADED f_loaded(i_filename, i_pathList, false);

		for (TRACE* f1_trace = f_loaded.traceList.begin(); f1_trace < f_loaded.traceList.end(); f1_trace ++) {

			f1_trace->start += source.size();

		}

		source = source + f_loaded.source;

		traceList += f_loaded.traceList;

		if (entry.size() == 0) {

			entry = f_loaded.entry;

		}

		if (subsystem.size() == 0) {

			subsystem = f_loaded.subsystem;

		}

		for (STRING* f1_link = f_loaded.linkList.begin(); f1_link < f_loaded.linkList.end(); f1_link ++) {

			link(*f1_link);

		}

		for (STRING* f1_include = f_loaded.includeList.begin(); f1_include < f_loaded.includeList.end(); f1_include ++) {

			include(*f1_include, i_pathList, i_includeList);

		}

	}

	VOID LOADED::link(STRING i_filename) {

		for (STRING* f1_link = linkList.begin(); f1_link < linkList.end(); f1_link ++) {

			if (*f1_link == i_filename) {

				return;

			}

		}

		linkList += i_filename;

	}

	STRING LOADED::string() {

		return source;

	}

	ERROR tracePointer(LOADED& i_loaded, BYTE* i_char, STRING i_message) {

		if (
			i_char < i_loaded.source.begin() ||
			i_char >= i_loaded.source.end()
		) {
			
			return ERROR("Untracable error : " + i_message);

		}

		DWORD f_charId = i_char - i_loaded.source.begin();

		TRACE* f_trace = i_loaded.traceList.begin();

		while (
			f_trace < i_loaded.traceList.end() &&
			f_trace->start <= f_charId
		) {

			f_trace ++;

		}
		f_trace --;

		DWORD f_line = 1;
		DWORD f_column = 0;

		std::ifstream f_file(f_trace->file.c(), std::ifstream::binary);

		if (f_file.fail()) return ERROR("Untracable error : " + i_message);

		DWORD f_pos = f_trace->offset + f_charId - f_trace->start;

		char f_char;

		BOOL f_space = false;
		BOOL f_macro = false;

		while (f_file.get(f_char)) {

			if (f_file.tellg() > f_pos) {

				break;

			}

			if		(f_char == '\n') {f_line ++; f_column = 0;}
			else if	(f_char == '\t') f_column += 4;
			else	f_column ++;

			if (f_space) {

				if (f_char != '\\' && f_file.peek() == '"') {

					f_file.get(f_char);

					f_column ++;
					
					f_space = false;

				}

			}
			else {

				if	(f_char == '<') {
					
					f_macro = true;

				}

				if (f_file.tellg() >= f_trace->offset &&
					(
						f_char <= 32 ||
						f_macro
					)
				) {
					
					f_pos ++;

				}
				
				if		(f_char == '"') {
					
					f_space = true;

				}
				else if	(f_char == '>') {
					
					f_macro = false;

				}

			}

		}

		return ERROR("Error caught in " + f_trace->file + " at L" + toString(f_line) + "C" + toString(f_column) + " : " + i_message);

	}

}