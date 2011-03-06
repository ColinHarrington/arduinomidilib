/*!
 *  @file		MIDI.h
 *  Project		MIDI Library
 *	@brief		MIDI Library for the Arduino
 *	Version		3.0
 *  @author		Francois Best 
 *	@date		24/02/11
 *  License		GPL Forty Seven Effects - 2011
 */

#ifndef LIB_MIDI_H_
#define LIB_MIDI_H_

#include <inttypes.h> 


/*  
    ###############################################################
    #                                                             #
    #    CONFIGURATION AREA                                       #
    #                                                             #
    #    Here are a few settings you can change to customize      #
    #    the library for your own project. You can for example    #
    #    choose to compile only parts of it so you gain flash     #
    #    space and optimise the speed of your sketch.             #
    #                                                             #
    ###############################################################
 */

#define COMPATIBILITY_V25		1			// Enable compatibility with MIDI Library v2.5


#define COMPFLAG_MIDI_IN        1           // Set this setting to 1 to use the MIDI input.
#define COMPFLAG_MIDI_OUT       1           // Set this setting to 1 to use the MIDI output. 


#if USE_USB_CONNECTION
  #define USE_SERIAL_PORT       Serial      // This Serial port is the one connected to the USB plug.
#else
  #define USE_SERIAL_PORT       Serial1     // Change the number (to Serial1 for example) if you want
#endif                                      // to use a different serial port for MIDI I/O.


#define USE_RUNNING_STATUS		1			// Running status enables short messages when sending multiple values
                                            // of the same type and channel.
                                            // Set to 0 if you have troubles with controlling you hardware.


// END OF CONFIGURATION AREA 
// (do not modify anything under this line unless you know what you are doing)

#if COMPATIBILITY_V25
#include "Compatibility_v2.5.h"
#endif

#define MIDI_BAUDRATE			31250


#define MIDI_CHANNEL_OMNI		0
#define MIDI_CHANNEL_OFF		17 // and over

#define MIDI_SYSEX_ARRAY_SIZE	255

/*! Type definition for practical use (because "unsigned char" is a bit long to write.. )*/
typedef uint8_t byte;

/*! Enumeration of MIDI types */
enum kMIDIType {
	NoteOff	              = 0x80,	// Note Off
	NoteOn                = 0x90,	// Note On
	AfterTouchPoly        = 0xA0,	// Polyphonic AfterTouch
	ControlChange         = 0xB0,	// Control Change / Channel Mode
	ProgramChange         = 0xC0,	// Program Change
	AfterTouchChannel     = 0xD0,	// Channel (monophonic) AfterTouch
	PitchBend             = 0xE0,	// Pitch Bend
	SystemExclusive       = 0xF0,	// System Exclusive
	TimeCodeQuarterFrame  = 0xF1,	// System Common - MIDI Time Code Quarter Frame
	SongPosition          = 0xF2,	// System Common - Song Position Pointer
	SongSelect            = 0xF3,	// System Common - Song Select
	TuneRequest           = 0xF6,	// System Common - Tune Request
	Clock                 = 0xF8,	// System Real Time - Timing Clock
	Start                 = 0xFA,	// System Real Time - Start
	Continue              = 0xFB,	// System Real Time - Continue
	Stop                  = 0xFC,	// System Real Time - Stop
	ActiveSensing         = 0xFE,	// System Real Time - Active Sensing
	SystemReset           = 0xFF,	// System Real Time - System Reset
	InvalidType           = 0x00    // For notifying errors
};

/*! Enumeration of Thru filter modes */
enum kThruFilterMode {
	Off                   = 0,  // Thru disabled (nothing passes through).
	Full                  = 1,  // Fully enabled Thru (every incoming message is sent back).
	SameChannel           = 2,  // Only the messages on the Input Channel will be sent back.
	DifferentChannel      = 3   // All the messages but the ones on the Input Channel will be sent back.
};


/*! The midimsg structure contains decoded data of a MIDI message read from the serial port with read() or thru(). \n */
struct midimsg {
	/*! The MIDI channel on which the message was recieved. \n Value goes from 1 to 16. */
	byte channel; 
	/*! The type of the message (see the define section for types reference) */
	kMIDIType type;
	/*! The first data byte.\n Value goes from 0 to 127.\n If the message is SysEx, this byte contains the array length. */
	byte data1;
	/*! The second data byte. If the message is only 2 bytes long, this one is null.\n Value goes from 0 to 127. */
	byte data2;
	/*! System Exclusive dedicated byte array. \n Array length is stocked in data1. */
	byte sysex_array[MIDI_SYSEX_ARRAY_SIZE];
	/*! This boolean indicates if the message is valid or not. There is no channel consideration here, validity means the message respects the MIDI norm. */
	bool valid;
};


/*! The main class for MIDI handling.
	See member descriptions to know how to use it,
	or check out the examples supplied with the library.
 */
class MIDI_Class {
	
	
public:
	// Constructor and Destructor
	MIDI_Class();
	~MIDI_Class();
	
	
	void begin(const byte inChannel = 1);
	
	
	
	
/* ####### OUTPUT COMPILATION BLOCK ####### */	
#if COMPFLAG_MIDI_OUT

public:	
	
	void sendNoteOn(byte NoteNumber,byte Velocity,byte Channel);
	void sendNoteOff(byte NoteNumber,byte Velocity,byte Channel);
	void sendProgramChange(byte ProgramNumber,byte Channel);
	void sendControlChange(byte ControlNumber, byte ControlValue,byte Channel);
	void sendPitchBend(unsigned int PitchValue,byte Channel);
	void sendPitchBend(double PitchValue,byte Channel);
	void sendPolyPressure(byte NoteNumber,byte Pressure,byte Channel);
	void sendAfterTouch(byte Pressure,byte Channel);
	void sendSysEx(byte length, byte * array,bool ArrayContainsBoundaries = false);	
	void sendTimeCodeQuarterFrame(byte TypeNibble, byte ValuesNibble);
	void sendTimeCodeQuarterFrame(byte data);
	void sendSongPosition(unsigned int Beats);
	void sendSongSelect(byte SongNumber);
	void sendTuneRequest();
	void sendRealTime(kMIDIType Type);
	
	
private:
	
	const byte genstatus(const kMIDIType inType,const byte inChannel);
	void send(kMIDIType type, byte param1, byte param2, byte channel);
	
	// Attributes
#if USE_RUNNING_STATUS
	byte			mRunningStatus_TX;
#endif // USE_RUNNING_STATUS

#endif	// COMPFLAG_MIDI_OUT
	

	
/* ####### INPUT COMPILATION BLOCK ####### */
#if COMPFLAG_MIDI_IN	
	
public:
	
	bool read();
	bool read(const byte Channel);
	
	// Getters
	kMIDIType getType();
	byte getChannel();
	byte getData1();
	byte getData2();
	byte * getSysExArray();
	bool check();
	
	byte getInputChannel() { return mInputChannel; }
	
	// Setters
	void setInputChannel(const byte Channel);
	
private:
	
	inline const kMIDIType getTypeFromStatusByte(const byte inStatus) {
		if ((inStatus < 0x80) 
			|| (inStatus == 0xF4) 
			|| (inStatus == 0xF5) 
			|| (inStatus == 0xF9) 
			|| (inStatus == 0xFD)) return InvalidType; // data bytes and undefined.
		if (inStatus < 0xF0) return (kMIDIType)(inStatus & 0xF0);	// Channel message, remove channel nibble.
		else return (kMIDIType)inStatus;
	}
	
	bool filter(byte inChannel);
	bool parse(byte inChannel);
	
	
	// Attributes
	byte			mRunningStatus_RX;
	byte			mInputChannel;
	
	byte			mPendingMessage[MIDI_SYSEX_ARRAY_SIZE];
	byte			mPendingMessageExpectedLenght;
	byte			mPendingMessageIndex;
	
	midimsg			mMessage;
	
#endif // COMPFLAG_MIDI_IN
	

/* ####### THRU COMPILATION BLOCK ####### */
#if (COMPFLAG_MIDI_IN && COMPFLAG_MIDI_OUT) // Thru
	
public:
	
	// Getters
	kThruFilterMode getFilterMode() { return mThruFilterMode; }
	bool getThruState() { return mThruActivated; }
	
	
	// Setters
	void turnThruOn(kThruFilterMode inThruFilterMode = Full);
	void turnThruOff();
	
	void setThruFilterMode(const byte inThruFilterMode);	// For compatibility only, avoid in future programs.
	void setThruFilterMode(const kThruFilterMode inThruFilterMode);
	
	
private:
	
	bool				mThruActivated;
	kThruFilterMode		mThruFilterMode;
	
#endif // Thru
	
};

extern MIDI_Class MIDI;

#endif // LIB_MIDI_H_
