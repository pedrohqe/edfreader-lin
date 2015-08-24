#ifndef CInputEDF_H
#define	CInputEDF_H

#include <vector>
#include <iostream>
#include <cstring> // memset

#include "edflib.h"
#include "Definitions.h"

struct output {
	double position, CDF, PDF;
	int type;
};

class CInputEDF
{
public:
	// desctructor
	     ~CInputEDF();

	// open edf file
	void OpenFile(const wchar_t * fileName);
	void OpenFile(const char * fileName);

	// get data from one channel
	std::vector<SIGNALTYPE> * GetSegmentFromChannel(const int& channelNumber, const int& start, const int& end);

	// close open file
	void CloseFile();

	/**
	 * Returns count of samples in one channel.
	 */
	inline int GetCountSamples() const
	{
		return m_countSamples;
	}

	/**
	 * Returns the highest sample rate. 
	 */
	inline int GetMaxFS() const
	{
		return m_fs;
	}	

	/**
	 * Returns the highest sample rate. 
	 */
	inline int GetFS(const int channel) const
	{
		if (m_isOpen && m_hdr.edfsignals > channel && channel >= 0)
			return m_hdr.signalparam[channel].smp_in_datarecord;
		else return -1;
	}	

private:
	/// A private variable. Header structure.
	struct edf_hdr_struct 	m_hdr; 		 

	/// A private variable. Indicates whether is the end of the file.
	bool				  	m_endOfFile; 
	/// A private variable. Indicates whether is the file open.
	bool					m_isOpen; 	 
	/// A private variable. Positions start of a new segment.
	int 				  	m_start; 	 
	/// A private variable. T_seg.
	int  				  	m_T_seg; 
	/// sample rate of data in file 
	int 		 			m_fs;				
	/// Number of samples of signal in the file
	int 		  			m_countSamples;
};

#endif