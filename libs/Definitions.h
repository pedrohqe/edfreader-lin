#ifndef Definitions_H
#define Definitions_H

typedef float SIGNALTYPE;

/// Definition bandwidth - upper and lower limits of filtering.
typedef struct bandwidth
{
public:
	/**
	 * A constructor
	 * @param bl lower limit filtering
	 * @param bh upper limit filtering
	 */
	bandwidth(const int& bl, const int& bh)
		: m_bandLow(bl), m_bandHigh(bh)
	{
		/* empty */
	}

	/// Lower limit of filtering.
	int m_bandLow;
	
	/// Upper limit filtering.
	int m_bandHigh; 
} BANDWIDTH;

#endif