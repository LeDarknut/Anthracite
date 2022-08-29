#include <anthracite_assembler.hpp>

using namespace ANTHRACITE;

int main(int argc, char** argv) {

	try {

		if (argc < 2) {

			throw(ERROR("Command line target expected."));

		}

		STRING f_path = std::filesystem::absolute(argv[0]).parent_path().string();

		if (!std::filesystem::exists(f_path.c()) || !std::filesystem::is_directory(f_path.c())) {

			throw(ERROR("Command line path invalid."));

		}

		STRING f_source = std::filesystem::absolute(argv[1]).string();

		if (!std::filesystem::exists(f_source.c()) || std::filesystem::is_directory(f_source.c())) {

			throw(ERROR("Command line target invalid."));

		}

		STRING f_destination;

		STRING f_object;

		BOOL f_execute = false;

		LIST<STRING> f_pathList;

		for (DWORD f1_argId = 2; f1_argId < argc; f1_argId ++) {

			if		(STRING(argv[f1_argId]) == "-d") {

				f1_argId ++;

				if (f1_argId == argc) {
					
					throw(ERROR("Command line destination expected."));

				}

				f_destination = std::filesystem::absolute(argv[f1_argId]).string();

				if (std::filesystem::exists(f_destination.c()) && std::filesystem::is_directory(f_destination.c())) {

					throw(ERROR("Command line destination invalid."));

				}

			}
			else if	(STRING(argv[f1_argId]) == "-o") {

				f1_argId ++;

				if (f1_argId == argc) {
					
					throw(ERROR("Command line object expected."));

				}

				f_object = std::filesystem::absolute(argv[f1_argId]).string();

				if (!std::filesystem::exists(f_object.c()) || std::filesystem::is_directory(f_object.c())) {

					throw(ERROR("Command line object invalid."));

				}

			}
			else if	(STRING(argv[f1_argId]) == "-p") {

				f1_argId ++;

				if (f1_argId == argc) {
					
					throw(ERROR("Command line path expected."));

				}

				f_pathList += std::filesystem::absolute(argv[f1_argId]).string();

				if (!std::filesystem::exists(f_pathList.end()->c()) || !std::filesystem::is_directory(f_pathList.end()->c())) {

					throw(ERROR("Command line path invalid."));

				}			

			}
			else if	(STRING(argv[f1_argId]) == "-x") f_execute = true;
			else {

				throw(ERROR("Wrong argument '" + STRING(argv[f1_argId]) + "' ."));

			}

		}

		f_pathList += f_path + "\\lib";
		f_pathList += f_path + "\\include";

		if (f_destination.size() == 0) {
			
			f_destination = std::filesystem::absolute(f_source.c()).replace_extension(".exe").string();

		}

		if (f_object.size() == 0) {
			
			f_object = std::filesystem::absolute(f_source.c()).replace_extension(".obj").string();

		}

		LOADED f_loaded(f_source, f_pathList);

		LEXED f_lexed(f_loaded);

		PARSED f_parsed(f_lexed);

		SOLVED f_solved(f_parsed);

		ENCODED f_encoded(f_solved);

		ASSEMBLED f_assembled(f_encoded);

		f_assembled.save(f_object);		

		STRING f_libs;

		for (STRING* f1_link = f_loaded.linkList.begin(); f1_link < f_loaded.linkList.end(); f1_link ++) {

			f_libs += " ";
			f_libs += *f1_link;

		}

		DWORD f_link = system(("link /nologo /nodefaultlib " + f_loaded.entry + " " + f_loaded.subsystem + " " + f_object + " /libpath:" + f_path + "\\lib" + f_libs + " /out:" + f_destination).c());

		if (f_link != 0) {
			
			throw(ERROR("Linking error."));

		}

		if (f_execute) {

			DWORD f_execute = system(f_destination.c());

			if (f_execute != 0) {
				
				throw(ERROR("Execution error."));

			}

		}

	} catch(ERROR i_error) {

		std::cout << i_error.message.c() << std::endl;

	}

}