#ifndef TSFGDigit_hxx_seen
#define TSFGDigit_hxx_seen
#include "TDigitDummy.h"

/// A single digit for the SuperFGD CITIROC electronics.  See the "getter"
/// method definitions for documentation about the values in the class.
///
/// The relevant section from FEB Protocol V 1.8
///
/// GTS/GTS Tag/GTS Slot/GTS Event: I can't find a definition in the protocol,
/// but these terms seem to mean, depending on context:
///
/// - A clock running with a 10 us tick.
///
/// - A counter for the clock (28 bits)
///
/// - A grouping of "hits" associated with a GTS header and trailer (This
///   seems be what is meant by "GTS Slot" or "GTS event").
///
/// Tag ID: 2 LSB: These 2 bits are used for Tagging synchronization in order
/// to place Timing & Amplitude measurement within the corresponding GTS event
/// i.e. between its real Header/Trailer. The 2 LSB bits of the Tag ID must
/// correspond to the current 2 LSB bits of the GTS Tag. Due to the different
/// clock domains and the FIFOs cascade latency used for the readout, the hit
/// or amplitude ID message may be sent on the next GTS header/trailer cell if
/// the event occurs just before the GTS signal. Moreover, the probability of
/// having an amplitude event on the next GTS is higher since it takes ~9us to
/// process the analog readout (ASIC LG/HG multiplexed outputs) compared to a
/// hit event which is pushed in the readout flow within 10-20ns due to FIFO to
/// FIFO shorter latencies (2.5ns DDR clocking).
///
/// Hit ID: 3 bits: The HIT ID is a 3-bits counter (per channel) incremented at
/// the TIMING block level every time we have a rising edge on a dedicated
/// channel. Firstly, it allows us to check no rising or falling edge is
/// missing, since we should have a RISING followed later by a FALLING timing
/// word with the same HIT ID.
///
/// The association with the analog data is more delicate: the HIT ID of
/// the Timing block is latched by the Analog Block at the end of the FPGA HOLD
/// delay, i.e. just before the ADC is started:
///
/// - For CH0, if we have a HOLD delay larger enough to allows 2 events and if
///   the second is smaller than the 1st one, the ADC will latch the HIT ID of
///   the 2nd one so the association is wrong.
///
/// - For CH1, the association between Time & Amplitude will be correct. This
///   is because the FEB doesn't compare TOT and Amplitude at the FEB level
///   (the analog dead time + hold time is very large and the FPGA cannot
///   compute this at 400MHz), and also because we introduce an amplitude
///   threshold at the FPGA level.
///
/// At the Decoding DAQ or OCB side, this association should be done in the
/// following way:
///
/// - all timing event are associated within their correct GTS with the TAG ID
///   (2 LSB bits) and then within the TRIGGER type. The TOT is calculated
///   with difference between Rising and Falling.
///
/// - if an amplitude is found, it is first associated within its correct GTS
///   with the TAG ID (2 LSB bits) and within the TRIGGER type like for the
///   timing. Then the largest TOT in the HOLD Time window (got by 2 Words
///   HOLD Time Start and End) is found and the amplitude is associated
///   with. NB: the association in case of 2 identical TOT found within the
///   HOLD window has to be defined.
///
/// NB: HIT ID is a relevant info for the timing but is not really reliable
///   for the association of Analog and Timing.
///
/// Note: In the Digit, the analog amplitude is associated with all possible
///   TDC values, and a bit is set as determined by the above heuristic.
class TSFGDigit : public TDigitDummy {
public:
    TSFGDigit ();
//    virtual ~TSFGDigit();

    /// Construct a digit for a particular SFGD channel.
    ///
    /// The two TDC values correspond to the rising edge TDC (which increments
    /// the HitId, see the FEB protocol), and the next falling edget TDC
    /// (which must have the same HitId).  If there is not an associated
    /// falling edge with the same Hit Id then the falling edge TDC should be
    /// equal to the rising edge TDC.  The TDC values are relative to the GTS
    /// (global timing slot) counter which runs at 100 kHz (10 us resolution).
    ///
    /// The ADC values are for the ADC digitization window that the TDC values
    /// fell into.  Note that there can be multiple digits the same ADC value
    /// (one digit per rising edge tdc, but only one set of ADCs per ~40 us).
    /// The matchedAdcTdc provides a hint that a particular TDC should be used
    /// with the ADC.  This bit should be set by the eventUnpack code using
    /// the heuristic described on FEB protocol v1.8 page 13.
    TSFGDigit( unsigned int chan,
              unsigned int risingEdgeTDC,
              unsigned int fallingEdgeTDC,
              unsigned int highGainADC,
              unsigned int lowGainADC,
//              unsigned int gtsTime,
              bool matchedADCandTDC);

    /// Return the GTS time.  (20 bit value, 10 us resolution)
    int GetGTSCounter() const;

    /// Return the rising edge TDC counter.  This counts from the last GTS
    /// counter value (13 bits, 1.25 ns resolution, 10.24 us max)
    int GetRisingEdgeTDC() const;

    /// Return the falling edge TDC counter. This counts from the last GTS
    /// value (13 bits, 1.25 ns resolution, 10.24 us max)
    int GetFallingEdgeTDC() const;

    /// Return the high gain ADC. (12 bit value)
    int GetHighGainADC() const;

    /// Return the low gain ADC. (12 bit value)
    int GetLowGainADC() const;

    /// Return true if the adc values and tdc values are associated according
    /// to the heuristic defined in FEB protocol v1.8 (See Hit ID definition
    /// on page 12 which is also copied into this header).
    bool IsMatchedADCandTDC() const;

    /// Print the digit value.
//    virtual void ls(Option_t* opt = "") const;

private:

    unsigned int _chan;
    unsigned int _risingEdgeTDC;
    unsigned int _fallingEdgeTDC;
    unsigned int _highGainADC;
    unsigned int _lowGainADC;
    bool         _matchedADCandTDC;

    /// Implementation details to be defined will unpacking is finalized.
    /// Conceptually this will be
    ///
    /// bool matchedADCAndTDC;
    /// uint32_t gtsCounter
    /// uint16_t risingEdgeTDC;
    /// uint16_t fallingEdgetTDC;
    /// uint16_t highGainADC;
    /// uint16_t lowGainADC;
};
#endif
