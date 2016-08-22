////////////////////////////////////////////////////
// 用来进简单测试的类
// Wupeng 
// qq:4398151
////////////////////////////////////////////////////

#include "UnitTest.h"

TestCaseBase* UnitTest::RegisterTestCase(TestCaseBase* pTest)
{
	m_TestCases.push_back(pTest);
	return pTest;
}

int UnitTest::Run()
{
	m_bTestResult = TRUE;
	for (std::vector<TestCaseBase*>::iterator it = m_TestCases.begin();
		it != m_TestCases.end(); ++it)
	{
		TestCaseBase* pTest = *it;
		m_pCurrentTestCase = pTest;
		DOLOG("======================================");
		DOLOG("----------- (" + pTest->m_CaseName + " Test Start)--------");
		pTest->Run();
		DOLOG("----------- (" + pTest->m_CaseName + " End )--------");
		if (pTest->m_bResult)
		{
			m_nPassedCount++;
		}
		else
		{
			m_nFailedCount++;
			m_bTestResult = FALSE;
		}
	}

	DOLOG(" == == == == == == == == == == == == == == == == == == == ");
	DOLOG("Total TestCase : " + m_TestCases.size());
	DOLOG("Passed Count: " + m_nPassedCount);;
	DOLOG("Failed Count : " + m_nFailedCount);
	return m_bTestResult;
}

#include "gtest/gtest.h"
#ifdef DEBUGGTEST

void wmain(int argc, wchar_t** argv)
{
	testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();
}
#endif
