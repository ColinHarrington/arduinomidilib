/*!
 *  @file		MIDI.cpp
 *  Project		MIDI Library
 *	@brief		MIDI Library for the Arduino
 *	@version	3.0.1
 *  @author		François Best 
 *	@date		24/02/11
 *  @license	GPL Forty Seven Effects - 2011
 */

#include "MIDI.h"
#include "Serial.h"



/*! Main instance (the class comes pre-instantiated). */
MIDI_Class MIDI;


/*! Default constructor for MIDI_Class. */
MIDI_Class::MIDI_Class() { }
/*! Default destructor for MIDI_Class.\n
 This is not really useful for the Arduino, as it is never called...
 */
MIDI_Class::~MIDI_Class() { }



void MIDI_Class::begin(const byte inChannel) {
	
#if COMPFLAG_MIDI_OUT
	
#if USE_RUNNING_STATUS
	mRunningStatus_TX = InvalidType;
#endif // USE_RUNNING_STATUS
	
#endif // COMPFLAG_MIDI_OUT
	
	
#if COMPFLAG_MIDI_IN
	
	mInputChannel = inChannel;
	mRunningStatus_RX = InvalidType;
	
#endif // COMPFLAG_MIDI_IN
	
	
#if (COMPFLAG_MIDI_IN && COMPFLAG_MIDI_OUT) // Thru
	
	mThruFilterMode = Full;
	
#endif // Thru
	
}


#if COMPFLAG_MIDI_OUT

/*! Internal method, don't care about this one.. \n It generates a status byte with given channel and type. */
const byte MIDI_Class::genstatus(const kMIDIType inType,const byte inChannel) {
	return ((byte)inType | (inChannel & 0x0F));
}

/*! Generate and send a custom MIDI mMessage.
 \param type		The message type (see type defines for reference)
 \param data1	The first data byte
 \param data2	The second data byte (if the message contains only 1 data byte, set this one to 0)
 \param channel	The output channel on which the message will be sent
 */
void MIDI_Class::send(kMIDIType type, byte data1, byte data2, byte channel) {
	
	// Protection: remove MSBs on data
	data1 &= 0x7F;
	data2 &= 0x7F;
	
	// Then test if channel is valid
	if (channel >= MIDI_CHANNEL_OFF) {
		
#if USE_RUNNING_STATUS	
		mRunningStatus_TX = InvalidType;
#endif 
		
		return; // Don't send anything
	}
	
	if ((byte)type < 0xF0) {
		
		byte statusbyte = genstatus(type,channel);
		
#if USE_RUNNING_STATUS
		// Check Running Status
		if (mRunningStatus_TX != statusbyte) {
			// New message, memorise and send header
			mRunningStatus_TX = statusbyte;
			USE_SERIAL_PORT.write(mRunningStatus_TX);
		}
#else
		// Don't care about running status, send the Control byte.
		USE_SERIAL_PORT.write(statusbyte);
#endif
		
		// Then send data
		USE_SERIAL_PORT.write(data1);
		if (type != ProgramChange && type != AfterTouchChannel) {
			USE_SERIAL_PORT.write(data2);
		}
	}
	else {
		// This is a system message and therefore, should not be sent using this command.
	}
	
}

/*! Send a Note On message */
void MIDI_Class::sendNoteOn(byte NoteNumber,byte Velocity,byte Channel) { send(NoteOn,NoteNumber,Velocity,Channel); }
/*! Send a Note Off message (a real Note Off, not a Note On with null velocity) */
void MIDI_Class::sendNoteOff(byte NoteNumber,byte Velocity,byte Channel) { send(NoteOff,NoteNumber,Velocity,Channel); }
/*! Send a Program Change message */
void MIDI_Class::sendProgramChange(byte ProgramNumber,byte Channel) { send(ProgramChange,ProgramNumber,0,Channel); }
/*! Send a Control Change message */
void MIDI_Class::sendControlChange(byte ControlNumber, byte ControlValue,byte Channel) { send(ControlChange,ControlNumber,ControlValue,Channel); }
/*! Polyphonic AfterTouch (carries the information of pressure of the given key/note) */
void MIDI_Class::sendPolyPressure(byte NoteNumber,byte Pressure,byte Channel) { send(AfterTouchPoly,NoteNumber,Pressure,Channel); }
/*! Monophonic AfterTouch */
void MIDI_Class::sendAfterTouch(byte Pressure,byte Channel) { send(AfterTouchChannel,Pressure,0,Channel); }
/*! Generate and send a System Exclusive frame.
 \param length	The size of the array to send
 \param array	The byte array containing the data to send\n
 Don't put F0 or F7 (start & stop SysEx codes), but put destination device's Manufacturer's ID and/or Device ID in the buffer array.
 */
void MIDI_Class::sendSysEx(byte length, byte * array, bool ArrayContainsBoundaries) {
	if (!ArrayContainsBoundaries) USE_SERIAL_PORT.write(0xF0);
	for (byte i=0;i<length;i++) USE_SERIAL_PORT.write(array[i]);
	if (!ArrayContainsBoundaries) USE_SERIAL_PORT.write(0xF7);
}

void MIDI_Class::sendTuneRequest() { sendRealTime(TuneRequest); }

/*! Send a Real Time (one byte) message. \n You can also send Tune Request with this method. */
void MIDI_Class::sendRealTime(kMIDIType Type) {
	switch (Type) {
		case TuneRequest: // Not really real-time, but one byte anyway.
		case Clock:
		case Start:
		case Stop:	
		case Continue:
		case ActiveSensing:
		case SystemReset:
			USE_SERIAL_PORT.write((byte)Type);
			break;
		default:
			// Invalid Real Time marker
			break;
	}
}

#endif // COMPFLAG_MIDI_OUT



#if COMPFLAG_MIDI_IN

/*! Read a MIDI message from the serial port using the main input channel (see setInputChannel() for reference). \n
 Returned value: true if any valid message has been stored in the structure, false if not.
 A valid message is a message that matches the input channel. \n\n
 If the Thru is enabled and the messages matches the filter, it is sent back on the MIDI output.
 */
bool MIDI_Class::read() {
	return read(mInputChannel);
}

/*! Reading/thru-ing method, the same as read() with a given input channel to read on. */
bool MIDI_Class::read(const byte inChannel) {
	
	/*
	 Algorithme:
	 - Lancer le parseur pour tenter de récupérer un message.
	 - Si on a quelque chose, le passer au filter pour le thru
	 
	 Algo du parseur:
	 - Récursif (lancé plusieurs fois jusqu'à obtention d'un message ou fin du buffer série)
	 - Peut utiliser des callbacks (pour la v3.5) quand un message est terminé.
	 - Compatible avec le running status entrant 
	 */
	
	
	if (inChannel >= MIDI_CHANNEL_OFF) return false; // MIDI Input disabled.
	
	
	
	
	if (parse(inChannel)) {
		/* Filtering Algorithm:
		 - Get the extracted message's channel and compare it to the input channel
		 - 
		 */
		
	}
	
	return false;
}


void MIDI_Class::filter(byte inChannel) {
	switch (mThruFilterMode) {
		case Off: // Do nothing (Thru disabled)
			break;
		case Full:
			// TODO: voir comment looper les messages complexes.
			send(mMessage.type,mMessage.data1,mMessage.data2,mMessage.channel);
			break;
		case DifferentChannel:
			
			break;
		case SameChannel:
			
			break;
		default:
			break;
	}
}
bool MIDI_Class::parse(byte inChannel) { 
	
	// If the buffer is full -> Don't Panic! Call the Vogons to destroy it.
	if (USE_SERIAL_PORT.available() == UART_BUFFER_SIZE) { 
		USE_SERIAL_PORT.flush();
	}	
	
	if (USE_SERIAL_PORT.available() <= 0) {
		// No data available.
		return false;
	}
	else {
		
		/* Parsing algorithm:
		 Get a byte from the serial buffer.
		 • If there is no pending message to be recomposed, start a new one.
			- Find type and channel (if pertinent)
			- Look for other bytes in buffer, call parser recursively, until the message is assembled or the buffer is empty.
		 • Else, add the extracted byte to the pending message, and check validity. When the message is done, store it.
		 */
		
		
		byte extracted = USE_SERIAL_PORT.read();
		
		if (mPendingMessageIndex == 0) { // Start a new pending message
			mPendingMessage[0] = extracted;
			
			// Check for running status first
			switch (getTypeFromStatusByte(mRunningStatus_RX)) {
					// Only these types allow Running Status:
				case NoteOff:
				case NoteOn:
				case AfterTouchPoly:
				case ControlChange:
				case ProgramChange:
				case AfterTouchChannel:
				case PitchBend:	
					
					// If the status byte is not received, prepend it to the pending message
					if (extracted < 0x80) {
						mPendingMessage[0] = mRunningStatus_RX;
						mPendingMessage[1] = extracted;
						mPendingMessageIndex++;
					}
					
					// TODO: gérer l'entrelacement des System Real Time avec les running status (et les autres messages..).
					
					// Else: well, we received another status byte, so the running status does not apply here.
					// It will be updated upon completion of this message.
					
					break;
					
				default:
					// No running status
					break;
			}
			
			
			switch (getTypeFromStatusByte(mPendingMessage[0])) {
					
					// 1 byte messages
				case Start:
				case Continue:
				case Stop:
				case Clock:
				case ActiveSensing:
				case SystemReset:
				case TuneRequest:
					// Handle the message type directly here.
					mMessage.type = getTypeFromStatusByte(mPendingMessage[0]);
					mMessage.data1 = 0;
					mMessage.data2 = 0;
					mMessage.valid = true;
					mPendingMessageExpectedLenght = 0;
					mPendingMessageIndex = 0;
					mRunningStatus_RX = InvalidType;
					return true;
					break;
					
					// 2 bytes messages
				case ProgramChange:
				case AfterTouchChannel:
				case TimeCodeQuarterFrame:
				case SongSelect:
					mPendingMessageExpectedLenght = 2;
					break;
					
					// 3 bytes messages
				case NoteOn:
				case NoteOff:
				case ControlChange:
				case PitchBend:
				case AfterTouchPoly:
				case SongPosition:
					mPendingMessageExpectedLenght = 3;
					break;
					
				case SystemExclusive:
					mPendingMessageExpectedLenght = MIDI_SYSEX_ARRAY_SIZE; // As the message can be any lenght between 3 and MIDI_SYSEX_ARRAY_SIZE bytes
					break;
					
				case InvalidType:
				default:
					// This is obviously wrong. Let's get the hell out'a here.
					mPendingMessageIndex = 0;
					mPendingMessageExpectedLenght = 0;
					mRunningStatus_RX = InvalidType;
					return false;
					break;
			}
			
			// Then update the index of the pending message.
			mPendingMessageIndex++;
			
			// And call the parser, again.
			parse(inChannel);
			
		}
		else { // Add extracted byte to pending message
			
			mPendingMessage[mPendingMessageIndex] = extracted;
			
			// If this is not a data byte
			if (extracted >= 0x80) {
				// Status byte, hmm.. What the hell are you doing here?
				
				// Unless you are an EOX?
				if ( (extracted == 0xF7) && (getTypeFromStatusByte(mPendingMessage[0]) == SystemExclusive) ) {
					
					// Store System Exclusive array in midimsg structure
					for (byte i=0;i<MIDI_SYSEX_ARRAY_SIZE;i++) {
						mMessage.sysex_array[i] = mPendingMessage[i];
					}
					
					// Get length
					mMessage.data1 = mPendingMessageIndex+1;
					mMessage.data2 = 0;
					mMessage.valid = true;
					
					mPendingMessageIndex = 0;
					mPendingMessageExpectedLenght = 0;
					mRunningStatus_RX = InvalidType;
					
					return true;
				}
				else {
					// Well well well.. error.
					mPendingMessageIndex = 0;
					mPendingMessageExpectedLenght = 0;
					mRunningStatus_RX = InvalidType;
					return false;
				}
			}
			
			
			// Now we are going to check if we have reached the end of the message
			if (mPendingMessageIndex >= (mPendingMessageExpectedLenght-1)) {
				
				// "FML" case: fall down here with an overflown SysEx..
				// This means we received the last possible data byte that can fit the buffer.
				// If this happens, try increasing MIDI_SYSEX_ARRAY_SIZE.
				if (getTypeFromStatusByte(mPendingMessage[0]) == SystemExclusive) {
					mPendingMessageIndex = 0;
					mPendingMessageExpectedLenght = 0;
					mRunningStatus_RX = InvalidType;
					return false;
				}
				
				
				// Reset local variables
				mPendingMessageIndex = 0;
				mPendingMessageExpectedLenght = 0;
				
				mMessage.type = getTypeFromStatusByte(mPendingMessage[0]);
				mMessage.channel = (mPendingMessage[0] & 0x0F);				// Don't care if it makes any sense.
				
				if (mPendingMessageExpectedLenght >= 2) mMessage.data1 = mPendingMessage[1]; // Checking this is futile, as 1 byte message were processed in the switch.
				if (mPendingMessageExpectedLenght >= 3) mMessage.data1 = mPendingMessage[2];
				
				mMessage.valid = true;
				
				// Activate running status (if enabled for the received type)
				switch (mMessage.type) {
					case NoteOff:
					case NoteOn:
					case AfterTouchPoly:
					case ControlChange:
					case ProgramChange:
					case AfterTouchChannel:
					case PitchBend:	
						// Running status enabled: store it from received message
						mRunningStatus_RX = mPendingMessage[0];
						break;
						
					default:
						// No running status
						mRunningStatus_RX = InvalidType;
						break;
				}
				
				return true;
			}
			else {
				// Then update the index of the pending message.
				mPendingMessageIndex++;
				
				// And call the parser, again.
				parse(inChannel);
			}
			
		}
		
		
	}
	
	// What are our chances to fall here?
	return false;
}


// Getters
/*! Getter method: access to the message type stored in the structure. \n Returns an enumerated type. */
kMIDIType MIDI_Class::getType() { return mMessage.type; }
/*! Getter method: access to the channel of the message stored in the structure. */
byte MIDI_Class::getChannel() { return mMessage.channel; }
/*! Getter method: access to the first data byte of the message stored in the structure. \n If the message is SysEx, the length of the array is stocked there. */
byte MIDI_Class::getData1() { return mMessage.data1; }
/*! Getter method: access to the second data byte of the message stored in the structure. */
byte MIDI_Class::getData2() { return mMessage.data2; }
/*! Getter method: access to the System Exclusive byte array. Array length is stocked in Data1. */
byte * MIDI_Class::getSysExArray() { return mMessage.sysex_array; }
/*! Check if a valid message is stored in the structure. */
bool MIDI_Class::check() { return mMessage.valid; }

// Setters
/*! Set the value for the input MIDI channel 
 \param channel the channel value. Valid values are 1 to 16, 
 MIDI_CHANNEL_OMNI if you want to listen to all channels, and MIDI_CHANNEL_OFF to disable MIDI input.
 */
void MIDI_Class::setInputChannel(const byte inChannel) { mInputChannel = inChannel; }


#endif // COMPFLAG_MIDI_IN




#if (COMPFLAG_MIDI_IN && COMPFLAG_MIDI_OUT) // Thru

/*! Set the filter for thru mirroring
 \param inThruFilterMode a filter mode
 See kThruFilterMode for detailed description.
 */
void MIDI_Class::setThruFilterMode(kThruFilterMode inThruFilterMode) { 
	mThruFilterMode = inThruFilterMode;
	if (mThruFilterMode != Off) mThruActivated = true;
	else mThruActivated = false;
}
/*! Set the filter for thru mirroring
 \param inThruFilterMode a filter mode
 See kThruFilterMode for detailed description. \n
 This method uses a byte parameter and is for compatibility only, please use kThruFilterMode for future programs.
 */
void MIDI_Class::setThruFilterMode(byte inThruFilterMode) { 
	mThruFilterMode = (kThruFilterMode)inThruFilterMode;
	if (mThruFilterMode != Off) mThruActivated = true;
	else mThruActivated = false;
}


/*! Setter method: turn message mirroring on. */
void MIDI_Class::turnThruOn(kThruFilterMode inThruFilterMode) { 
	mThruActivated = true;
	mThruFilterMode = inThruFilterMode;
}
/*! Setter method: turn message mirroring off. */
void MIDI_Class::turnThruOff() {
	mThruActivated = false; 
	mThruFilterMode = Off;
}

#endif // Thru


