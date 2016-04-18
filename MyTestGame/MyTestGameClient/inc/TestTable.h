#pragma once				

class UserTable{
public:
	struct Row{
		wchar_t* m_Name;
		int m_Age;
		float m_Salary;
		__int64 m_Property;
	};
	const static Row m_Row[9];
	const static int m_size;
};

const int UserTable::m_size = 9;

const UserTable::Row UserTable::m_Row[9]= { 
		{ L"OK1", 1, 12.1, 345655,},
		{ L"OK2", 2, 12.2, 345345666,},
		{ L"OK3", 3, 12.3, 3423345, },
		{ L"OK4", 4, 12.4, 31111115,},
		{ L"OK5", 5, 12.5, 3458788885,},
		{ L"OK6", 6, 12.5, 34993765,},
		{ L"OK", 7, 12.6, 345,},
		{ L"OK", 8, 12.7, 345666765,},
		{ L"OK9", 9, 12.8, 99955,},
};

class UserIndexTable{
public:
	static const void* s_pColounm[5];
	static const int m_1[9];
	static const int m_2[9];
	static const int m_3[9];
	static const int m_4[9];
	static const int m_5[9];
	const static int m_Size;
	template<typename T>
	static bool GetCell(int x, int y, T& res){
		if (ISINRANGE(x, 0, 8) && ISINRANGE(y, 0, 4)){
			res = ((T*)s_pColounm[y])[x];
			return true;
		}
		else{
			return false;
		}
	}
};

const int UserIndexTable::m_Size = 9;
const void* UserIndexTable::s_pColounm[5] = { (void*)m_1, (void*)m_2, (void*)m_3, (void*)m_4, (void*)m_5, };
const int UserIndexTable::m_1[9] = { 0, 0, 0, 1, 1, 0, 0, 0, 1, };
const int UserIndexTable::m_2[9] = { 1, 1, 0, 0, 1, 0, 1, 0, 1, };
const int UserIndexTable::m_3[9] = { 1, 1, 1, 0, 1, 0, 1, 0, 1, };
const int UserIndexTable::m_4[9] = { 1, 1, 1, 0, 1, 0, 1, 0, 0, };
const int UserIndexTable::m_5[9] = { 1, 1, 1, 0, 0, 0, 1, 1, 1, };