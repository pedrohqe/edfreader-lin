#ifndef CSpikeDetector_H
#define	CSpikeDetector_H

#include <vector>
#include <iostream>
#include <cmath>

#include "Definitions.h"
#include "CInputEDF.h"
#include "CDSP.h"

#include "lib/Alglib/interpolation.h"

class CDetectorOutput;
class CMarker;
class CDischarges;

// structure containing settings of the spike detector
typedef struct detectorSettings
{
public: 
	int    m_band_low;                // (-fl)
	int    m_band_high;               // (-fh)
	double m_k1;               		  // (-k1)
	double m_k2;               		  // (-k2)
	double m_k3;					  // (-k3)
	int    m_winsize;     			  // (-w)
	double m_noverlap;     			  // (-n)
	int    m_buffering;               // (-buf)
	int    m_main_hum_freq;           // (-h)
	double m_discharge_tol;           // (-dt)
	double m_polyspike_union_time;	  // (-pt)
	int    m_decimation;
	
	/// A constructor
	detectorSettings(int band_low, int band_high, double k1, double k2, double k3, int winsize, double noverlap, int buffering, int main_hum_freq,
		double discharge_tol, double polyspike_union_time, int decimation)
		: m_band_low(band_low), m_band_high(band_high), m_k1(k1), m_k2(k2), m_k3(k3), m_winsize(winsize), m_noverlap(noverlap), m_buffering(buffering),
		m_main_hum_freq(main_hum_freq), m_discharge_tol(discharge_tol), m_polyspike_union_time(polyspike_union_time), m_decimation(decimation)
	{
		/* empty */
	}
	
} DETECTOR_SETTINGS;

// the main classes implementing spike detector functions
class CSpikeDetector
{
public:
	// constructor
	CSpikeDetector(CInputEDF * model, DETECTOR_SETTINGS * settings);

	// analyse one channel, is possible change file
	void AnalyseChannel(const int channelNumber, CDetectorOutput ** output, CDischarges ** discharges, const wchar_t * fileName = NULL);

private:
	/** 
	 * Calculate the starts and ends of indexes for CSpikeDetector::spikeDetector
	 * @param indexStart output vector with starts
	 * @param indexStart output vector with ends
	 * @param cntElemInCh count elements in channel
	 * @param T_seg it's equal round(countRecords / N_seg / fs)
	 * @param fs sample rate
	 * @param winsize size of the window
	 */
	void getIndexStartStop(std::vector<int>& indexStart, std::vector<int>& indexStop, const int& cntElemInCh, const double& T_seg, const int& fs, const int& winsize);

	/**
	 * Run analysis for a segment of data - data from one channel!
	 * @param data inpud data - iEEG
	 * @param counChanles count channles of input data
	 * @param inpuFS sample rate of input data
	 * @param bandwidth bandwifth
	 * @param out a pointer to output object of \ref CDetectorOutput
	 * @param discharges a pointer to output object of \ref CDischarges
	 */
	void spikeDetector(std::vector<SIGNALTYPE>*& data, const int& inputFS, const BANDWIDTH& bandwidth,
					   CDetectorOutput*& out, CDischarges*& discharges);

private:
	CInputEDF 		  * m_model;
	DETECTOR_SETTINGS * m_settings;
	/// output object - structure out
	CDetectorOutput*   m_out;
	/// output object - strucutre discharges
	CDischarges*       m_discharges;   
};

/**
 * Structure containing output data from \ref COneChannelDetect
 */
typedef struct oneChannelDetectRet
{
public:
	std::vector<bool>*   	 m_markersHigh;	
	std::vector<bool>*   	 m_markersLow;
	std::vector<double>  	 m_prahInt[2];
	std::vector<double>  	 m_envelopeCdf;
	std::vector<double>  	 m_envelopePdf;
	std::vector<SIGNALTYPE>  m_envelope;

	/// A constructor
	oneChannelDetectRet(std::vector<bool>*& markersHigh, std::vector<bool>*& markersLow, const std::vector<double> prahInt[2],
						const std::vector<double> envelopeCdf, const std::vector<double> envelopePdf, const std::vector<SIGNALTYPE>& envelope)
		: m_markersHigh(markersHigh), m_markersLow(markersLow)
	{
		m_prahInt[0].assign(prahInt[0].begin(), prahInt[0].end());
		m_prahInt[1].assign(prahInt[1].begin(), prahInt[1].end());
		m_envelopeCdf.assign(envelopeCdf.begin(), envelopeCdf.end());
		m_envelopePdf.assign(envelopePdf.begin(), envelopePdf.end());
		m_envelope.assign(envelope.begin(), envelope.end());
	}

	/// A destructor
	~oneChannelDetectRet()
	{
		/* empty */
	}

} ONECHANNELDETECTRET;

/**
 * Implementation "one_channel_detect" function from reference implementation of spike detector.
 */
class COneChannelDetect
{
// methods
public:
	/**
	 * A constructor.
	 * @param data input data
	 * @param settings settings od the detector
	 * @param fs sample rate
	 * @param index indexs
	 * @param channel number of channel
	 */
	COneChannelDetect(const std::vector<SIGNALTYPE>* data, const DETECTOR_SETTINGS* settings, const int& fs, const std::vector<int>* index, const int& channel);

	/**
	 * A virtual desctructor.
	 */
	virtual ~COneChannelDetect();

	/**
	 * Run detection
	 */
	ONECHANNELDETECTRET * Run();

private:
	/**
	 * Calculating a mean from data in vector.
	 * @param data a vector of input data
	 * @return a mean
	 */
	double mean(std::vector<double>& data);
	
	/**
	 * Calculating a variance from data in vector.
	 * @param data input vector with data
	 * @param mean mean of data in vector
	 * @return a variance
	 */
	double variance(std::vector<double>& data, const double & mean);

	/**
	 * Detection of local maxima in envelope.
	 * @param envelope envelope of input channel
	 * @param prah_int threeshold curve
	 * @param polyspike_union_time polyspike union time
	 * @return vector cintaining markers of local maxima
	 */
	std::vector<bool>* localMaximaDetection(std::vector<SIGNALTYPE>& envelope, const std::vector<double>& prah_int, const double& polyspike_union_time);
	
	/**
	 * Detecting of union and their merging.
	 * @param marker1 markers of spikes
	 * @param envelope envelope of input channel 
	 * @param union_samples union samples time
	 */
	void detectionUnion(std::vector<bool>* marker1, std::vector<SIGNALTYPE>& envelope, const double& union_samples);

	/** 
	 * Finding of the highes maxima of the section with local maxima.
	 * implement:
	 * point(:,1)=find(diff([0;marker1])>0); % start 
	 * point(:,2)=find(diff([marker1;0])<0); % end
	 * @param point
	 * @param marker1
	 */
	void findStartEndCrossing(std::vector<int> point[2], const std::vector<bool>* marker1);

// variables
public:
	/* none */

private:
	/// input data
	const std::vector<SIGNALTYPE> * m_data;
	/// settinggs of the detector			
	const DETECTOR_SETTINGS *  		m_settings;
	/// sample rate     
	const int 			  	  		m_fs;
	/// indexs of suspects areas
	const std::vector<int> * 		m_index;
	/// channel number		
	const int 				  		m_channel;		
};

// ------------------------------------------------------------------------------------------------
// OUTPUT CLASSES
// ------------------------------------------------------------------------------------------------

/**
 * Output class containing output data from the detector.
 */
class CDetectorOutput
{
// methods
public:
	/**
	 * A constructor.
	 */
	CDetectorOutput();
	
	/**
	 * A virtual desctructor.
	 */
	virtual ~CDetectorOutput();

	/**
	 * Add data to the vectors.
	 * @param pos spike position (second).
	 * @param dur spike duration (second) - fix value 5 ms.
	 * @param chan channel.
	 * @param con spike condition (1-obvious 0.5-ambiguous).
	 * @param weight statistical significance "CDF".
	 * @param pdf sstatistical significance "PDF".
	 */
	void Add(const double& pos, const double& dur, const int& chan, const double& con, const double& weight, const double& pdf);

	/**
	 * Erase records at positions.
	 * @param pos position of records to erase.
	 */
	 void Remove(const std::vector<int>& pos);
private:

// variables
public:
	/// spike position (second)
	std::vector<double>  m_pos;
	/// channel 	
	std::vector<int>     m_chan;
	/// spike duration (second) - fix value 5 ms	
	std::vector<double>  m_dur;
	/// spike condition (1-obvious 0.5-ambiguous) 	
	std::vector<double>  m_con;
	/// statistical significance "CDF" 	
	std::vector<double>  m_weight;
	/// statistical significance "PDF"
	std::vector<double>  m_pdf;    
private: 
	/* none */
};

/**
 * Discharges
 */
class CDischarges
{
// methods
public:
	/**
	 * A constructor.
	 * @param countChannels count channels.
	 */
	CDischarges(const int& countChannels);

	/**
	 * A virtual desctructor.
	 */
	virtual ~CDischarges();

	/**
	 * Erase records at positions.
	 * @param pos positions of record to erase.
	 */
	 void Remove(const std::vector<int>& pos);

	 /**
	  *	Return count channels.
	  * @return count channels.s
	  */
	inline unsigned GetCountChannels() const
	{
		return m_countChannels;
	}
private:

// variables
public:
	/// spike type 1-obvious, 0.5- ambiguous
	std::vector<double>* m_MV;
	/// max. amplitude of envelope above backround
	std::vector<double>* m_MA;
	/// event start position
	std::vector<double>* m_MP;
	/// duration of event	   
	std::vector<double>* m_MD;
	/// statistical significance "CDF"	   
	std::vector<double>* m_MW;
	/// probability of occurence	   
	std::vector<double>* m_MPDF;   
private:
	/// count channels
	unsigned 			 m_countChannels; 
};

#endif