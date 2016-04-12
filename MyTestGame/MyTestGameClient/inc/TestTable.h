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
	void* m_pColounm[5];
	int m_1[9];
	int m_2[9];
	int m_3[9];
	int m_4[9];
	int m_5[9];
};