#include "CInputEDF.h"
#include <qdebug.h>

using namespace std;

CInputEDF::~CInputEDF()
{
	CloseFile();
}

void CInputEDF::OpenFile(const wchar_t* fileName)
{
	char buffer[2048];
	mbstate_t mbs;
	int ret;
	
	setlocale(LC_ALL, "");
	memset(&mbs, 0, sizeof(mbs)); 
	mbrlen(NULL , 0, &mbs);
	ret = wcsrtombs ( buffer, &fileName, sizeof(buffer), &mbs );
	setlocale(LC_ALL, "C");
	
	if(ret)
		OpenFile(buffer);
	else throw "Error opening file. The name or file path is not correct.";
}

void CInputEDF::OpenFile(const char * fileName)
{
	m_endOfFile = false;
	m_fs = 0;
	m_start = 0;
	
	if(edfopen_file_readonly(fileName, &m_hdr, EDFLIB_READ_ALL_ANNOTATIONS))
	{
		string error;
		switch(m_hdr.filetype)
		{
		  case EDFLIB_MALLOC_ERROR:
		  	error = "Openning file: malloc error.";
            break;
		  case EDFLIB_NO_SUCH_FILE_OR_DIRECTORY: 
		  	error = "Can not open file, no such file or directory.";
            break;
		  case EDFLIB_FILE_CONTAINS_FORMAT_ERRORS :
	  		error = "The file is not EDF(+) or BDF(+) compliant ""(it contains format errors).";
		    break;
		  case EDFLIB_MAXFILES_REACHED: 
		  	error = "To many files opened.";
		    break;
		  case EDFLIB_FILE_READ_ERROR:
		  	error = "A read error occurred.";
	    	break;
		  case EDFLIB_FILE_ALREADY_OPENED:
		  	error = "File has already been opened.";
            break;
		  default:
		  	error = "Unknown error.";
		    break;
		}
		
	}

	// get highest sample rate and length of signal
	for (int i = 0; i < m_hdr.edfsignals; i++)
	{
		if (m_hdr.signalparam[i].smp_in_datarecord > m_fs)
		{
			m_fs = m_hdr.signalparam[i].smp_in_datarecord;
			m_countSamples = m_hdr.signalparam[i].smp_in_file;
		}
	}

	m_isOpen = true;
}

vector<SIGNALTYPE> * CInputEDF::GetSegmentFromChannel(const int& channelNumber, const int& start, const int& end)
{
	if (m_endOfFile)  
		return NULL;

	if (!m_isOpen)
		throw "Warning: isn't open any file! You must first open input file!";

	if (channelNumber < 0 || channelNumber > m_hdr.edfsignals)
		throw "Error: invalid channel number!";

	int 				 i;
	int 				 buffersize = end - start;
    double* 			 segment = new double[buffersize+10];
    int 				 ret = 0;
    vector<SIGNALTYPE> * data;

    edfseek(m_hdr.handle, channelNumber, start, EDFSEEK_SET);
    ret = edfread_physical_samples(m_hdr.handle, channelNumber, buffersize, segment);
	if (ret == -1)
		throw "Error reading samples from file!";

	data = new vector<SIGNALTYPE>;
	data->insert(data->begin(), segment, segment+ret);

	delete [] segment;

	return data;	
}

/// Close input file if is open.
void CInputEDF::CloseFile()
{
	if (m_isOpen)
	{
		edfclose_file(m_hdr.handle);
		m_isOpen = false;
		//m_channels.clear();
	}	
}
