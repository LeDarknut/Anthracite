#include <anthracite_lexer.hpp>

namespace ANTHRACITE {

	LEXED::LEXED(LOADED& i_loaded) : loaded(&i_loaded), tokenList() {

		/*
			Split the loaded string into tokens
			Convert string values to int
		*/

		BYTE* f_char = loaded->source.begin();

		if (loaded->source.size() == 0) {

			return;

		}

		while (f_char < loaded->source.end()) {

			TOKEN f1_token;

			f1_token.pointer = f_char;

			if		(
				*f_char == '@' ||
				*f_char == '$' ||
				*f_char == '%' ||
				*f_char == '#' ||
				*f_char == '&'
			) {

				//Handle symbols : @box / $memory / %register / #value / &section

				if		(*f_char == '@') f1_token.type = t_BOX;
				else if	(*f_char == '$') f1_token.type = t_MEM;
				else if	(*f_char == '%') f1_token.type = t_REG;
				else if	(*f_char == '#') f1_token.type = t_VAL;
				else 					 f1_token.type = t_SCT;

				f1_token.content += *f_char;

				f_char ++;

				//Handle boxes : $box.memory / #box.box.value / ...

				do {

					if (*f_char >= '0' && *f_char <= '9') {

						break;
						
					}

					while (
						(f_char < loaded->source.end()) &&
						(
							(*f_char >= '0' && *f_char <= '9') ||
							(*f_char >= 'a' && *f_char <= 'z') ||
							(*f_char >= 'A' && *f_char <= 'Z') ||
							(*f_char == '_')
						)
					) {

						f1_token.content += *f_char;

						f_char ++;

					}

					if (*f_char == '.') {


						TOKEN f1_pathToken;

						f1_pathToken.pointer = f1_token.pointer;

						f1_pathToken.type = t_BOX;

						f1_pathToken.content = "@" + f1_token.content.from(1);

						tokenList += f1_pathToken;

						tokenList += TOKEN(t_OPR, f_char, ".");

						f_char ++;

						f1_token.pointer = f_char;

						f1_token.content = f1_token.content[0];

					}

				} while (*(f_char - 1) == '.');

			}
			else if	(

				//Convert values to int : 13 / 0xD / 0b1101

				(*f_char >= '0' && *f_char <= '9') ||
				(
					f_char + 1 < loaded->source.end() &&
					(*f_char == '-' || *f_char == '+') &&
					*(f_char + 1) >= '0' && *(f_char + 1) <= '9'
				)
			) {

				f1_token.type = t_LTR;

				BOOL f_neg = (*f_char == '-');

				if (*f_char == '-' || *f_char == '+') {

					f1_token.content += *f_char;

					f_char ++;

				}

				if		(f_char + 1 < loaded->source.end() && *f_char == '0' && *(f_char + 1) == 'b') {

					f1_token.content += *f_char;

					f_char ++;

					f1_token.content += *f_char;

					f_char ++;

					BYTE* f_begin = f_char;

					while (*f_char >= '0' && *f_char <= '1') {
						
						f1_token.content += *f_char;

						f_char ++;

					}

					DWORD f_bitNum = 0;

					BYTE f_byte = 0;

					for (BYTE* f1_char = f_char - 1; f1_char >= f_begin; f1_char --) {

						f_byte |= (*f1_char - '0') << f_bitNum;

						f_bitNum ++;

						if (f_bitNum == 8) {

							f1_token.value += f_byte;

							f_byte = 0;

							f_bitNum = 0;

						}

					}

					if (f_byte > 0) f1_token.value += f_byte;

				}
				else if	(f_char + 1 < loaded->source.end() && *f_char == '0' && *(f_char + 1) == 'x')	{

					f1_token.content += *f_char;

					f_char ++;

					f1_token.content += *f_char;

					f_char ++;

					BYTE* f_begin = f_char;

					while (
						(*f_char >= '0' && *f_char <= '9') ||
						(*f_char >= 'a' && *f_char <= 'f') ||
						(*f_char >= 'A' && *f_char <= 'F')
					) {
						
						f1_token.content += *f_char;

						f_char ++;

					}
					
					DWORD f_bitNum = 0;

					BYTE f_byte = 0;
					
					for (BYTE* f1_char = f_char - 1; f1_char >= f_begin; f1_char --) {

						if		(*f1_char >= 'a') f_byte += (*f1_char - 'a' + 10) << f_bitNum;

						else if	(*f1_char >= 'A') f_byte += (*f1_char - 'A' + 10) << f_bitNum;

						else	f_byte += (*f1_char - '0') << f_bitNum;

						f_bitNum += 4;

						if (f_bitNum == 8) {

							f1_token.value += f_byte;

							f_byte = 0;

							f_bitNum = 0;

						}

					}

					if (f_byte > 0) f1_token.value += f_byte;

				}
				else {

					BYTE* f_begin = f_char;

					while (*f_char >= '0' && *f_char <= '9') {
						
						f1_token.content += *f_char;

						f_char ++;

					}

					DWORD f_bitNum = 0;

					QWORD f_value = 0;

					QWORD f_factor = 1;
					
					for (BYTE* f1_char = f_char - 1; f1_char >= f_begin; f1_char --) {

						f_value += ((*f1_char - '0') * f_factor) << f_bitNum;

						f_factor *= 5;

						f_bitNum ++;

						if (f_bitNum == 8) {

							f1_token.value += BYTE(f_value & 0b11111111);

							f_value = f_value >> 8;

							f_bitNum = 0;

						}

					}

					while (f_value > 0) {

						f1_token.value += BYTE(f_value & 0b11111111);

						f_value = f_value >> 8;

					}

				}

				f1_token.value += BYTE(0x00);

				while (f1_token.value.size() > 1 && f1_token.value[-1] == 0x00) {

					f1_token.value -= 1;

				}

				if (f1_token.value[-1] >= 0x80 + f_neg)
					f1_token.value += BYTE(0x00);

				if (f_neg) {

					BOOL f1_carry = true;

					for (BYTE* f1_byte = f1_token.value.begin(); f1_byte < f1_token.value.end(); f1_byte ++) {

						*f1_byte = ~*f1_byte;

						if (f1_carry) {

							(*f1_byte) ++;

							f1_carry = (*f1_byte == 0x00);

						}

					}

				}

			}
			else if	(*f_char == '"') {

				//Handle double quotes strings : "string"

				f1_token.type = t_LTR;

				f1_token.content += *f_char;

				f_char ++;

				BYTE* f_begin = f_char;

				while (*f_char != '"') {

					if (*f_char == '\\') {
						
						f1_token.content += *f_char;

						f_char ++;

						f1_token.content += *f_char;

						if		(*f_char == 'n' ) f1_token.value += '\n';
						else if	(*f_char == '0' ) f1_token.value += '\0';
						else if	(*f_char == 't' ) f1_token.value += '\t';
						else f1_token.value += *f_char;

						f_char ++;

					}
					else {

						f1_token.content += *f_char;

						f1_token.value += *f_char;

						f_char ++;

					}
					
				}

				f1_token.content += *f_char;

				f_char ++;

			}
			else if	(*f_char == '\'') {

				//Handle single quote strings : 'string'

				f1_token.type = t_GLB;

				f1_token.content += *f_char;

				f_char ++;

				BYTE* f_begin = f_char;

				while (*f_char != '\'') {

					if (*f_char == '\\') {
						
						f1_token.content += *f_char;

						f_char ++;

						f1_token.content += *f_char;

						if		(*f_char == 'n' ) f1_token.value += '\n';
						else if	(*f_char == '0' ) f1_token.value += '\0';
						else if	(*f_char == 't' ) f1_token.value += '\t';
						else f1_token.value += *f_char;

						f_char ++;

					}
					else {

						f1_token.content += *f_char;

						f1_token.value += *f_char;

						f_char ++;

					}
					
				}

				f1_token.content += *f_char;

				f_char ++;

			}
			else if	(
				(*f_char >= 'a' && *f_char <= 'z') ||
				(*f_char >= 'A' && *f_char <= 'Z') ||
				(*f_char == '_')
			) {

				//Handle block name and functions

				f1_token.type = t_LBL;

				while (
					(f_char < loaded->source.end()) && (
						(*f_char >= '0' && *f_char <= '9') ||
						(*f_char >= 'a' && *f_char <= 'z') ||
						(*f_char >= 'A' && *f_char <= 'Z') ||
						(*f_char == '_')
					)
				) {

					f1_token.content += *f_char;
					f_char ++;

				}

			}
			else if	(
				f_char + 1 < loaded->source.end() &&
				(*f_char == '-' && *(f_char + 1) == '>')
			) {

				//Handle double chars operator : ->

				f1_token.type = t_OPR;

				f1_token.content += *f_char;

				f_char ++;

				f1_token.content += *f_char;

				f_char ++;

			}
			else {

				//Handle single char operators

				f1_token.type = t_OPR;

				f1_token.content += *f_char;

				f_char ++;

			}
			
			tokenList += f1_token;

		}

	}

	STRING LEXED::string() {

		STRING o_string;

		for (TOKEN* f1_token = tokenList.begin(); f1_token < tokenList.end(); f1_token ++) {

			if		(f1_token->type == t_BOX) o_string += "BOX : '";
			else if	(f1_token->type == t_MEM) o_string += "MEM : '";
			else if	(f1_token->type == t_REG) o_string += "REG : '";
			else if	(f1_token->type == t_VAL) o_string += "VAL : '";
			else if	(f1_token->type == t_LTR) o_string += "LTR : '";
			else if	(f1_token->type == t_GLB) o_string += "GLB : '";
			else if	(f1_token->type == t_LBL) o_string += "LBL : '";
			else if	(f1_token->type == t_SCT) o_string += "SCT : '";
			else if	(f1_token->type == t_OPR) o_string += "OPR : '";

			o_string += f1_token->content + "'";

			if (f1_token->value.size()) {

				o_string += " (" + f1_token->value.hex(" ") + ")";

			}

			if (f1_token + 1 < tokenList.end()) o_string += "\n";

		}

		return o_string;

	}

	BOOL TOKEN::is() {

		return (type != t_ERR);

	}

	BOOL TOKEN::isMem() {

		return (type == t_MEM);

	}

	BOOL TOKEN::isReg() {

		return (type == t_REG);

	}

	BOOL TOKEN::isVal() {

		return (type == t_VAL);

	}

	BOOL TOKEN::isSct() {

		return (type == t_SCT);

	}

	BOOL TOKEN::isBox() {

		return (type == t_BOX);

	}

	BOOL TOKEN::isLtr() {

		return (type == t_LTR);

	}

	BOOL TOKEN::isGlb() {

		return (type == t_GLB);

	}

	BOOL TOKEN::isLbl() {

		return (type == t_LBL);

	}

	BOOL TOKEN::isOpr() {

		return (type == t_OPR);

	}

	BOOL TOKEN::isSym() {

		return (type <= t_BOX);

	}

	ERROR traceToken(LEXED& i_lexed, TOKEN& i_token, STRING i_message) {

		/*
			Trace error from token
		*/

		return tracePointer(*i_lexed.loaded, i_token.pointer, "'" + i_token.content + "' " + i_message);

	}

}