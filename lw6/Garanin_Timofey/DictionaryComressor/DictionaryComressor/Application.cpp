#include "stdafx.h"
#include "Repository.h"
#include "IOManager.h"
#include "Compressor.h"
#include "Application.h"
#include "CustomThread.h"

bool ThreadFunction(CApplication *app)
{
	return app->EditNextFragment();
}


CApplication::CApplication(CIOManager *manager)
	: m_iomanager(manager)
	, m_nextThreadOrderForWrite(0)
{
	m_myRepository = CMyRepository();
}

void CApplication::SetInputFileName(std::string const &name)
{
	m_iomanager->SetInputFileName(name);
}

void CApplication::SetOutputFileName(std::string const &name)
{
	m_iomanager->SetOutputFileName(name);
}

void CApplication::ProcessFile()
{
	try
	{
		m_iomanager->SettingInputFile();
		int innerCount = 0;
		std::vector<CCustomThread*> threads;

		for (size_t i = 0; i != CSingletonSystemInfo::GetInstance()->GetNumberOfProcessors() - 1; ++i)
		{
			threads.push_back(new CCustomThread(this));
			threads.back()->Run();
		}
		while (ThreadFunction(this)) {}
		int i = threads.size() - 1;
		int numberThreadsFinishedWorking = 0;
		while (numberThreadsFinishedWorking < threads.size())
		{
			if (threads.at(i)->IsFinished() && !threads.at(i)->IsChecked())
			{
				threads.at(i)->Checked();
				++numberThreadsFinishedWorking;
			}
			--i;
			if (i == -1)
			{
				i = threads.size() - 1;
				std::this_thread::sleep_for(std::chrono::seconds(2));
			}

		}
		threads.clear();

		std::cout << "File processed" << std::endl;
		OutputResultsInFile();
		std::cout << "Results output in files" << std::endl;
		return;
	}
	catch (std::runtime_error const &ex)
	{
		std::cerr << "Error! " << ex.what() << std::endl;
		return;
	}
}

void CApplication::SaveNewInformation(CCompressor &compressor)
{
	m_myRepository.SetNewDictionaryFragment(compressor.GetAllDictionary());
	m_myRepository.SetNewTextFragment(compressor.GetAllProcessingText());
}

void CApplication::OutputResultsInFile()
{
	m_iomanager->OutputDictionary(m_myRepository.GetDictionary());
	m_iomanager->OutputProcessedText(m_myRepository.GetText());
}

bool CApplication::EditNextFragment()
{
	try
	{
		m_mutex.lock();
		CCompressor compressor(m_iomanager->GetOrder());
		auto dataPtr = m_iomanager->GetViewMappingFile();
		m_mutex.unlock();
		compressor.SetTextFragment(dataPtr);
		compressor.SetLengthFragment(m_iomanager->GetSizeView());
		compressor.EditFragment();

		while (true)
		{
			if (compressor.GetOrder() == m_nextThreadOrderForWrite)
			{
				m_mutex.lock();
				SaveNewInformation(compressor);
				m_mutex.unlock();
				++m_nextThreadOrderForWrite;
				break;
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::seconds(2));
			}
		}
		return true;
	}
	catch (std::runtime_error const &ex)
	{
		m_mutex.unlock();
		return false;
	}
}

CApplication::~CApplication()
{
	delete m_iomanager;
}
