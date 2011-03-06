/*!
 *  @file		Compatibility_v2.5.h
 *  Project		MIDI Library
 *	@brief		Compatibility file for MIDI Library v2.5
 *	Version		3.0
 *  @author		Francois Best 
 *	@date		24/02/11
 *  License		GPL Forty Seven Effects - 2011
 */

#ifndef LIB_MIDI_COMPATIBILITY_V2_5_H_
#define LIB_MIDI_COMPATIBILITY_V2_5_H_


/*! The basic baudrate for MIDI communications. */
#define MIDI_rate MIDI_BAUDRATE
/*! Message type AfterTouch Poly	*/
#define ATPoly AfterTouchPoly
/*! Message type Control Change		*/
#define CC ControlChange
/*! Message type Program Change		*/
#define PC ProgramChange
/*! Message type AfterTouch Channel	*/
#define ATCanal AfterTouchChannel
/*! Message type System Exclusive 	*/
#define SysEx SystemExclusive


#define MIDI_FILTER_OFF			0
#define MIDI_FILTER_FULL		1
#define MIDI_FILTER_CANAL		2
#define MIDI_FILTER_ANTICANAL	3


#endif // LIB_MIDI_COMPATIBILITY_V2_5_H_
