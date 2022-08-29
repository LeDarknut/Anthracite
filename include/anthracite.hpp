#ifndef DEF_ANTHRACITE
#define DEF_ANTHRACITE

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <ios>
#include <filesystem>

namespace ANTHRACITE {

	typedef void				VOID;
	typedef bool				BOOL;
	typedef unsigned char		BYTE;
	typedef unsigned short		WORD;
	typedef unsigned long		DWORD;
	typedef unsigned long long	QWORD;
	typedef char				SBYTE;
	typedef short				SWORD;
	typedef long				SDWORD;
	typedef long long			SQWORD;

	template<class TYPE> class	LIST {

		private:

			std::vector<TYPE>		m_vector;

		public:

			LIST();
			LIST(TYPE i_element);
			LIST(const LIST<TYPE>& i_list);

			~LIST();
			
			DWORD size();
			TYPE* begin();
			TYPE* end();
			VOID empty();
			LIST<TYPE> sub(DWORD i_idx, DWORD i_length);
			LIST<TYPE> from(SDWORD i_idx);
			LIST<TYPE> from(TYPE* i_element);
			LIST<TYPE> to(SDWORD i_idx);
			LIST<TYPE> to(TYPE* i_element);

			LIST<TYPE> operator+(LIST<TYPE>&i_list);
			LIST<TYPE> operator+(TYPE i_element);

			VOID operator+=(LIST<TYPE> i_list);
			VOID operator+=(TYPE i_element);

			LIST<TYPE> operator-(DWORD i_length);

			VOID operator-=(DWORD i_length);

			TYPE& operator[](SDWORD i_idx);

			VOID operator=(LIST<TYPE>& i_list);
			VOID operator=(TYPE i_element);

			BOOL operator==(LIST<TYPE> i_list);
			BOOL operator==(TYPE i_element);

			BOOL operator!=(LIST<TYPE> i_list);
			BOOL operator!=(TYPE i_element);

	};

	class	STRING {

		private:

			std::vector<BYTE>			m_string;
			std::string					m_c;

		public:

			STRING();
			STRING(const STRING& i_string);
			STRING(std::string i_string);
			STRING(char* i_charList);
			STRING(BYTE i_char);

			~STRING();

			DWORD size();
			BYTE* begin();
			BYTE* end();
			VOID empty();
			inline const char* c();
			BOOL isZero();
			STRING sub(DWORD i_idx, DWORD i_length);
			STRING from(SDWORD i_idx);
			STRING from(BYTE* i_char);
			STRING to(SDWORD i_idx);
			STRING to(BYTE* i_char);
			STRING hex(STRING i_separator = "");

			STRING operator+(STRING& i_string);
			STRING operator+(char* i_charList);
			STRING operator+(BYTE i_char);
			STRING operator+(char i_char);
			STRING operator+(WORD i_char);
			STRING operator+(DWORD i_char);

			VOID operator+=(STRING i_string);
			VOID operator+=(char* i_charList);
			VOID operator+=(BYTE i_char);
			VOID operator+=(char i_char);
			VOID operator+=(WORD i_char);
			VOID operator+=(DWORD i_char);

			STRING operator-(DWORD i_length);

			VOID operator-=(DWORD i_length);

			BYTE& operator[](SDWORD i_idx);

			VOID operator=(STRING& i_string);
			VOID operator=(std::string i_string);
			VOID operator=(char* i_charList);
			VOID operator=(BYTE i_char);

			BOOL operator==(STRING i_string);
			BOOL operator==(char* i_charList);

			BOOL operator!=(STRING i_string);
			BOOL operator!=(char* i_charList);

	};

	template<class TYPE> std::vector<TYPE> operator+(TYPE i_element,  std::vector<TYPE> &i_list);
	STRING operator+(BYTE i_char,  STRING &i_string);
	STRING operator+(char i_char,  STRING &i_string);
	STRING operator+(char* i_charList,  STRING &i_string);
	
	struct ERROR {

		STRING	message;

		ERROR(STRING i_message);

	};

	STRING toString(DWORD i_int);
	
}

#include <anthracite.cpp>

#endif