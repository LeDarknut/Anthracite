#include <anthracite.hpp>

namespace ANTHRACITE {

	template<class TYPE> LIST<TYPE>::LIST() : m_vector() {

	}

	template<class TYPE> LIST<TYPE>::LIST(const LIST<TYPE>& i_list) : m_vector(i_list.m_vector) {

	}

	template<class TYPE> LIST<TYPE>::LIST(TYPE i_element) : m_vector() {

		m_vector.push_back(i_element);

	}

	template<class TYPE> LIST<TYPE>::~LIST() {

	}

	template<class TYPE> DWORD LIST<TYPE>::size() {

		return m_vector.size();

	}

	template<class TYPE> TYPE* LIST<TYPE>::begin() {
	
		return &m_vector[0];
		
	}

	template<class TYPE> TYPE* LIST<TYPE>::end() {
	
		return &m_vector[m_vector.size()];
		
	}

	template<class TYPE> VOID LIST<TYPE>::empty() {

		m_vector.clear();

	}

	template<class TYPE> LIST<TYPE> LIST<TYPE>::sub(DWORD i_idx, DWORD i_length) {

		LIST<TYPE> o_list;

		o_list.m_vector.insert(o_list.m_vector.end(), m_vector.begin() + i_idx, m_vector.begin() + i_idx + i_length);

		return o_list;

	}

	template<class TYPE> LIST<TYPE> LIST<TYPE>::from(SDWORD i_idx) {

		LIST<TYPE> o_list;

		if (i_idx >= 0) {

			if (i_idx > m_vector.size()) {

				throw(ERROR("Invalid index."));

			}

			o_list.m_vector.insert(o_list.m_vector.end(), m_vector.begin() + i_idx, m_vector.end());

		}
		else {

			if (m_vector.size() + i_idx < 0) {

				throw(ERROR("Invalid index."));

			}

			o_list.m_vector.insert(o_list.m_vector.end(), m_vector.end() + i_idx, m_vector.end());

		}

		return o_list;

	}

	template<class TYPE> LIST<TYPE> LIST<TYPE>::from(TYPE* i_element) {

		LIST<TYPE> o_list;

		o_list.m_vector.insert(o_list.m_vector.end(), m_vector.begin() + (i_element - begin()), m_vector.end());

		return o_list;

	}

	template<class TYPE> LIST<TYPE> LIST<TYPE>::to(SDWORD i_idx) {

		LIST<TYPE> o_list;

		if (i_idx >= 0) {

			if (i_idx > m_vector.size()) {

				throw(ERROR("Invalid index."));

			}

			o_list.m_vector.insert(o_list.m_vector.end(), m_vector.begin(), m_vector.begin() + i_idx);

		}
		else {

			if (m_vector.size() + i_idx < 0) {

				throw(ERROR("Invalid index."));

			}

			o_list.m_vector.insert(o_list.m_vector.end(), m_vector.begin(), m_vector.end() + i_idx);

		}

		return o_list;

	}

	template<class TYPE> LIST<TYPE> LIST<TYPE>::to(TYPE* i_element) {

		LIST<TYPE> o_list;

		o_list.m_vector.insert(o_list.m_vector.end(), m_vector.begin(), m_vector.begin() + (i_element - begin()));

		return o_list;

	}

	template<class TYPE> LIST<TYPE> LIST<TYPE>::operator+(LIST<TYPE>&i_list) {

		LIST<TYPE> o_list;

		o_list.m_vector.insert(o_list.m_vector.end(), m_vector.begin(), m_vector.end());

		o_list.m_vector.insert(o_list.m_vector.end(), i_list.m_vector.begin(), i_list.m_vector.end());

		return o_list;

	}

	template<class TYPE> LIST<TYPE> LIST<TYPE>::operator+(TYPE i_element) {

		LIST<TYPE> o_list;

		o_list.m_vector.insert(o_list.m_vector.end(), m_vector.begin(), m_vector.end());

		o_list.m_vector.push_back(i_element);

		return o_list;

	}

	template<class TYPE> LIST<TYPE> operator+(TYPE i_element,  LIST<TYPE> &i_list) {

		LIST<TYPE> o_list;

		o_list.m_vector.push_back(i_element);

		o_list.m_vector.insert(o_list.m_vector.end(), m_vector.begin(), m_vector.end());

		return o_list;

	}
	
	template<class TYPE> VOID LIST<TYPE>::operator+=(LIST<TYPE> i_list) {

		m_vector.insert(m_vector.end(), i_list.m_vector.begin(), i_list.m_vector.end());

	}

	template<class TYPE> VOID LIST<TYPE>::operator+=(TYPE i_element) {

		m_vector.push_back(i_element);

	}

	template<class TYPE> LIST<TYPE> LIST<TYPE>::operator-(DWORD i_length) {

		LIST<TYPE> o_list;

		o_list.m_vector.insert(o_list.m_vector.end(), m_vector.begin(), m_vector.end() - i_length);

		return o_list;

	}

	template<class TYPE> VOID LIST<TYPE>::operator-=(DWORD i_length) {

		for (DWORD f1_charId = 0; f1_charId < i_length; f1_charId ++) m_vector.pop_back();

	}

	template<class TYPE> TYPE& LIST<TYPE>::operator[](SDWORD i_idx) {

		if (i_idx >= 0) {

			if (i_idx >= m_vector.size()) {

				throw(ERROR("Invalid index."));

			}

			return m_vector[i_idx];

		}
		else {

			if (m_vector.size() + i_idx < 0) {

				throw(ERROR("Invalid index."));

			}

			return m_vector[m_vector.size() + i_idx];

		}

	}
	
	template<class TYPE> VOID LIST<TYPE>::operator=(LIST<TYPE>& i_list) {

		m_vector = i_list.m_vector;

	}

	template<class TYPE> VOID LIST<TYPE>::operator=(TYPE i_element) {

		m_vector.clear();

		m_vector.push_back(i_element);

	}

	template<class TYPE> BOOL LIST<TYPE>::operator==(LIST<TYPE> i_list) {

		return m_vector == i_list.m_vector;

	}


	template<class TYPE> BOOL LIST<TYPE>::operator!=(LIST<TYPE> i_list) {

		return m_vector != i_list.m_vector;

	}

	STRING::STRING() : m_string() {

	}

	STRING::STRING(const STRING& i_string) : m_string(i_string.m_string) {

	}

	STRING::STRING(std::string i_string) : m_string() {

		for (DWORD f1_charId = 0; f1_charId < i_string.size(); f1_charId ++) {
			
			m_string.push_back(BYTE(i_string[f1_charId]));

		}

	}

	STRING::STRING(char* i_charList) : m_string() {

		for (DWORD f1_charId = 0; i_charList[f1_charId] != '\0'; f1_charId ++) m_string.push_back(BYTE(i_charList[f1_charId]));

	}

	STRING::STRING(BYTE i_char) : m_string() {

		m_string.push_back(i_char);

	}

	STRING::~STRING() {

	}

	DWORD STRING::size() {

		return m_string.size();

	}

	BYTE* STRING::begin() {
	
		return &m_string[0];
		
	}

	BYTE* STRING::end() {
	
		return &m_string[m_string.size()];
		
	}

	VOID STRING::empty() {

		m_string.clear();

	}

	inline const char* STRING::c() {

		m_c.clear();

		for (DWORD f1_charId = 0; f1_charId < m_string.size(); f1_charId++) m_c.push_back(char(m_string[f1_charId]));

		return m_c.c_str();

	}

	BOOL STRING::isZero() {

		for (DWORD f1_charId = 0; f1_charId < m_string.size(); f1_charId++) if (m_string[f1_charId] != 0) return false;

		return true;

	}

	STRING STRING::sub(DWORD i_idx, DWORD i_length) {

		STRING o_string;

		o_string.m_string.insert(o_string.m_string.end(), m_string.begin() + i_idx, m_string.begin() + i_idx + i_length);

		return o_string;

	}

	STRING STRING::from(SDWORD i_idx) {

		STRING o_string;

		if (i_idx >= 0) {

			if (i_idx > m_string.size()) {

				throw(ERROR("Invalid index."));

			}

			o_string.m_string.insert(o_string.m_string.end(), m_string.begin() + i_idx, m_string.end());

		}
		else {

			if (m_string.size() + i_idx < 0) {

				throw(ERROR("Invalid index."));

			}

			o_string.m_string.insert(o_string.m_string.end(), m_string.end() + i_idx, m_string.end());

		}

		return o_string;

	}

	STRING STRING::from(BYTE* i_char) {

		STRING o_string;

		o_string.m_string.insert(o_string.m_string.end(), m_string.begin() + (i_char - begin()), m_string.end());

		return o_string;

	}

	STRING STRING::to(SDWORD i_idx) {

		STRING o_string;

		if (i_idx >= 0) {

			if (i_idx > m_string.size()) {

				throw(ERROR("Invalid index."));

			}

			o_string.m_string.insert(o_string.m_string.end(), m_string.begin(), m_string.begin() + i_idx);

		}
		else {

			if (m_string.size() + i_idx < 0) {

				throw(ERROR("Invalid index."));

			}

			o_string.m_string.insert(o_string.m_string.end(), m_string.begin(), m_string.end() + i_idx);

		}

		return o_string;

	}

	STRING STRING::to(BYTE* i_char) {

		STRING o_string;

		o_string.m_string.insert(o_string.m_string.end(), m_string.begin(), m_string.begin() + (i_char - begin()));

		return o_string;

	}

	STRING STRING::hex(STRING i_separator) {

		STRING o_string;

		for (DWORD f2_byteId = 0; f2_byteId < m_string.size(); f2_byteId ++) {

			BYTE& f2_byte = m_string[f2_byteId];

			BYTE f2_byteHigh = (f2_byte & 0xF0) >> 4;

			if (f2_byteHigh < 10) o_string += BYTE('0' + f2_byteHigh);
			else o_string += BYTE('A' + f2_byteHigh - 10);

			BYTE f2_byteLow = (f2_byte & 0x0F);

			if (f2_byteLow < 10) o_string += BYTE('0' + f2_byteLow);
			else o_string += BYTE('A' + f2_byteLow - 10);

			if (f2_byteId != m_string.size() - 1) o_string += i_separator;

		}

		return o_string;

	}

	STRING STRING::operator+(STRING& i_string) {

		STRING o_string;

		o_string.m_string = m_string;
		
		o_string.m_string.insert(o_string.m_string.end(), i_string.m_string.begin(), i_string.m_string.end());

		return o_string;

	}
	
	STRING STRING::operator+(char* i_charList) {

		STRING o_string;

		o_string.m_string = m_string;

		for (DWORD f1_charId = 0; i_charList[f1_charId] != '\0'; f1_charId ++) o_string.m_string.push_back(BYTE(i_charList[f1_charId]));

		return o_string;

	}

	STRING STRING::operator+(BYTE i_char) {

		STRING o_string;

		o_string.m_string = m_string;

		o_string.m_string.push_back(i_char);

		return o_string;

	}

	STRING STRING::operator+(char i_char) {

		STRING o_string;

		o_string.m_string = m_string;

		o_string.m_string.push_back(BYTE(i_char));

		return o_string;

	}

	STRING STRING::operator+(WORD i_char) {

		STRING o_string;

		o_string.m_string = m_string;
		
		o_string.m_string.push_back(BYTE(i_char & 0xFF));
		o_string.m_string.push_back(BYTE((i_char >> 8) & 0xFF));

		return o_string;

	}

	STRING STRING::operator+(DWORD i_char) {

		STRING o_string;

		o_string.m_string = m_string;
		
		o_string.m_string.push_back(BYTE(i_char & 0xFF));
		o_string.m_string.push_back(BYTE((i_char >> 8) & 0xFF));
		o_string.m_string.push_back(BYTE((i_char >> 16) & 0xFF));
		o_string.m_string.push_back(BYTE((i_char >> 24) & 0xFF));

		return o_string;

	}

	STRING operator+(BYTE i_char, STRING &i_string) {

		STRING o_string;

		o_string += i_char;

		o_string += i_string;

		return o_string;

	}

	STRING operator+(char i_char, STRING &i_string) {

		STRING o_string;

		o_string += i_char;

		o_string += i_string;

		return o_string;

	}

	STRING operator+(char* i_charList,  STRING &i_string) {

		STRING o_string;

		o_string += i_charList;

		o_string += i_string;

		return o_string;

	}
	
	VOID STRING::operator+=(STRING i_string) {

		m_string.insert(m_string.end(), i_string.m_string.begin(), i_string.m_string.end());

	}
	
	VOID STRING::operator+=(char* i_charList) {
		
		for (DWORD f1_charId = 0; i_charList[f1_charId] != '\0'; f1_charId ++) m_string.push_back(BYTE(i_charList[f1_charId]));

	}

	VOID STRING::operator+=(BYTE i_char) {

		m_string.push_back(i_char);

	}

	VOID STRING::operator+=(char i_char) {

		m_string.push_back(BYTE(i_char));

	}

	VOID STRING::operator+=(WORD i_char) {

		m_string.push_back(BYTE(i_char & 0xFF));
		m_string.push_back(BYTE((i_char >> 8) & 0xFF));
		
	}

	VOID STRING::operator+=(DWORD i_char) {

		m_string.push_back(BYTE(i_char & 0xFF));
		m_string.push_back(BYTE((i_char >> 8) & 0xFF));
		m_string.push_back(BYTE((i_char >> 16) & 0xFF));
		m_string.push_back(BYTE((i_char >> 24) & 0xFF));
		
	}

	STRING STRING::operator-(DWORD i_length) {

		STRING o_string;

		o_string.m_string.insert(o_string.m_string.end(), m_string.begin(), m_string.end() - i_length);

		return o_string;

	}

	VOID STRING::operator-=(DWORD i_length) {

		for (DWORD f1_charId = 0; f1_charId < i_length; f1_charId ++) m_string.pop_back();

	}

	BYTE& STRING::operator[](SDWORD i_idx) {

		if (i_idx >= 0) {

			if (i_idx >= m_string.size()) {

				throw(ERROR("Invalid index."));

			}

			return m_string[i_idx];

		}
		else {

			if (m_string.size() + i_idx < 0) {

				throw(ERROR("Invalid index."));

			}

			return m_string[m_string.size() + i_idx];

		}

	}

	VOID STRING::operator=(STRING& i_string) {

		m_string = i_string.m_string;

	}

	VOID STRING::operator=(std::string i_string) {

		m_string.clear();

		for (DWORD f1_charId = 0; f1_charId < i_string.size(); f1_charId ++) {
			
			m_string.push_back(BYTE(i_string[f1_charId]));

		}

	}
	
	VOID STRING::operator=(char* i_charList) {
		
		m_string.clear();

		for (DWORD f1_charId = 0; i_charList[f1_charId] != '\0'; f1_charId ++) m_string.push_back(BYTE(i_charList[f1_charId]));

	}

	VOID STRING::operator=(BYTE i_char) {

		m_string.clear();

		m_string.push_back(i_char);

	}

	BOOL STRING::operator==(STRING i_string) {

		return m_string == i_string.m_string;

	}
	
	BOOL STRING::operator==(char* i_charList) {

		DWORD f1_charId = 0;
		
		for (f1_charId = 0; i_charList[f1_charId] != '\0'; f1_charId ++) {
			
			if (f1_charId >= m_string.size()) return false;
			if (m_string[f1_charId] != i_charList[f1_charId]) return false;

		}
		
		return (f1_charId == m_string.size());

	}

	BOOL STRING::operator!=(STRING i_string) {

		return m_string != i_string.m_string;

	}
	
	BOOL STRING::operator!=(char* i_charList) {

		DWORD f1_charId = 0;
		
		for (f1_charId = 0; i_charList[f1_charId] != '\0'; f1_charId ++) {
			
			if (f1_charId >= m_string.size()) return true;
			if (m_string[f1_charId] != i_charList[f1_charId]) return true;

		}
		
		return (f1_charId != m_string.size());

	}

	STRING file(char* i_string) {

		STRING o_string;

		std::filesystem::path i_path(i_string);

		i_path = std::filesystem::absolute(i_path);

		if (std::filesystem::exists(i_path) && !std::filesystem::is_directory(i_path)) {

			o_string = i_path.string();

		}

		return o_string;

	}

	STRING folder(char* i_string) {

		STRING o_string;

		std::filesystem::path i_path(i_string);

		i_path = std::filesystem::absolute(i_path);

		if (std::filesystem::exists(i_path) && std::filesystem::is_directory(i_path)) {


			o_string = i_path.string();

		}

		return o_string;

	}

	STRING parent(char* i_string) {

		STRING o_string;

		std::filesystem::path i_path(i_string);

		i_path = std::filesystem::absolute(i_path);

		if (std::filesystem::exists(i_path)) {

			i_path = i_path.parent_path();

			o_string = i_path.string();

		}

		return o_string;

	}

	STRING toString(DWORD i_int) {

		DWORD f_size = 1;

		STRING o_string;
		
		o_string += BYTE('0');

		for (DWORD f_digit = 10; (f_digit - 1) < i_int; f_digit *= 10) {
			
			f_size ++;

			o_string += BYTE('0');

		}

		for (DWORD f1_idx = 1; f1_idx <= f_size; f1_idx ++) {

			o_string[f_size - f1_idx] = 48 + (i_int % 10);

			i_int /= 10;

		}

		return o_string;

	}

	ERROR::ERROR(STRING i_message) {

		message = i_message;

	}

}