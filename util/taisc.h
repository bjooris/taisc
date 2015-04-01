﻿ #ifndef HEADER_TAISC_BASE
#define HEADER_TAISC_BASE


#define REMOVE_ATTR_PACKED
#define norace 
#define nx_struct 		struct
#define nx_union 		union
#define nx_uint8_t		uint8_t
#define nx_int8_t		int8_t
#define nx_uint16_t		uint16_t
#define nx_int16_t		int16_t
#define nx_uint32_t		uint32_t
#define nx_int32_t		int32_t
#define nx_uint64_t		uint64_t
#define nx_int64_t		int64_t
#define nxle_uint64_t		uint64_t
#define nxle_uint32_t		uint32_t
#define nxle_uint16_t		uint16_t
#define nxle_uint8_t		uint8_t

typedef enum {
	CC2520_CCA_DISABLED				= 0x00,  //CCA always set to ‘1’
	CC2520_CCA_THRESHOLD_HYST			= 0x08,  //CCA = ‘1’ when RSSI < CCA_THR-CCA_HYST, CCA = ‘0’ when RSSI >= CCA_THR
	CC2520_CCA_NO_FRAME_RECEPTION			= 0x10,  //CCA = ‘1’ when not receiving a frame, else CCA = ‘0’
	CC2520_CCA_HYBRID				= 0x18,  //CCA = ‘1’ when RSSI < CCA_THR-CCA_HYST and not receiving a frame, CCA=0 when RSSI >= CCA_THR or receiving a frame
} CC2520_CCA;

typedef enum {
	CC2520_RSSI_OFFSET				= 76,	 //see datasheet swrs068 p 84
} CC2520_RSSI_KNOWLEDGEBASE;

typedef enum {
	CC2520_CCA_HYST_0				= 0x00,
	CC2520_CCA_HYST_1				= 0x01,
	CC2520_CCA_HYST_2				= 0x02,
	CC2520_CCA_HYST_3				= 0x03,
	CC2520_CCA_HYST_4				= 0x04,
	CC2520_CCA_HYST_5				= 0x05,
	CC2520_CCA_HYST_6				= 0x06,
	CC2520_CCA_HYST_7				= 0x07,
} CC2520_CCA_HYST;


void sync(void);

#endif //HEADER_TAISC_BASE
 #ifndef HEADER_TAISC_COMMON
#define HEADER_TAISC_COMMON
 //DO NOT EDIT THIS FILE. IT WILL BE OVERWRITTEN BY THE NEXT BUILD!!! PLEASE CONTACT TAISC team @IBCN-UGENT.
//JIRA https://ibcn-jira.intec.ugent.be/browse/TAISC
//As networking uses big endian we will use big endian for all storage!
#include <stdint.h>
typedef void (*taisc_instruction)();
typedef void (*subInstructionT)(void);

#define  TAISC_MAX_CHAINS 4
#define  TAISC_ROM_SIZE 200
#define  TAISC_RAM_SIZE 400
#define  TAISC_MAX_STATS 4
#define  TAISC_MAX_INSTRUCTIONS 45
#define  TAISC_MAX_FRAME 65

typedef nx_uint16_t 	byteCodeT;
typedef uint8_t 	TAISC_conditionT;
typedef uint8_t 	TAISC_channelT;
typedef nx_uint16_t 	TAISC_sizeT;
typedef nx_uint16_t 	TAISC_absROMaddrT;
typedef nx_uint16_t 	TAISC_absRAMaddrT;
typedef nx_uint16_t 	TAISC_relRAMaddrT;
typedef uint8_t 	TAISC_ChainIDT;
typedef uint8_t 	TAISC_instructionIDT;
typedef uint8_t 	TAISC_instructionFlagsT;
typedef nx_uint32_t 	TAISC_absTimestampT;
typedef nx_int16_t 	TAISC_relShortTimestampT;
typedef nx_int32_t 	TAISC_relBigTimestampT;
typedef uint8_t 	TAISC_DPFiFoIndexT;
typedef uint8_t 	TAISC_DPFrmSizeT;
typedef uint16_t 	TAISC_DPFrameKeyT;
typedef nx_uint32_t 	TAISC_bitMAPT;
typedef uint8_t 	TAISC_instructionFlags;
typedef uint8_t 	TAISC_ReportIDT;
typedef uint8_t		TAISC_gpRAMT[TAISC_RAM_SIZE];

typedef struct {
	TAISC_absROMaddrT		startAddr;				//RO
	TAISC_absTimestampT		refTimeAbs;				//RW	if zero, then inactive
	TAISC_ChainIDT			next;					//RW	if ==TAISC_MAX_CHAINS then end of chains
	TAISC_absROMaddrT		endAddr;				//RO	last byte of last instruction	(probably a stop), will be used to check if a chain can be overwritten
} 
#ifndef REMOVE_ATTR_PACKED 
__attribute__ ((packed))
#endif
 ChainEntryT;

typedef struct {
	TAISC_instructionIDT		instruction;
	TAISC_relShortTimestampT	annotatedTime;				//negative slaves should have a fixed relative time to the ref, positive slaves hold the time needed to finalize the instruction
} 
#ifndef REMOVE_ATTR_PACKED 
__attribute__ ((packed))
#endif
 instructionHeaderT;

typedef union  {
	ChainEntryT			chains[TAISC_MAX_CHAINS];
	uint8_t				raw[TAISC_MAX_CHAINS * sizeof(ChainEntryT)];
} 
#ifndef REMOVE_ATTR_PACKED 
__attribute__ ((packed))
#endif
 TAISCchainsT;

TAISC_ChainIDT				currentChain;
norace TAISC_absROMaddrT		TAISC_PC;
norace TAISC_absROMaddrT		TAISC_EVENT_VECTOR;
TAISC_absRAMaddrT			heap 
#ifdef TAISC_COMPILER_ACCESS 
= 0 
#endif
;
const uint8_t				TAISC_ROM[TAISC_ROM_SIZE];
TAISCchainsT				TAISC_CHAINS;

typedef enum {
	TAISC_EVENT_dataplane_txFrameAvailable                  = 1,             //1
	TAISC_EVENT_dataplane_rxFrameAvailable                  = 2,             //2
	TAISC_EVENT_cc2520_startOfFrame                         = 4,             //3
	TAISC_EVENT_cc2520_endOfFrame                           = 8,             //4
	TAISC_EVENT_cc2520_stateOfMediumChanged                 = 16,            //5
	TAISC_EVENT_all                                         = 31             //ALL
} TAISC_eventT;


typedef enum {
	TAISC_STATE_cc2520_OFF                                  = 1,             //1
	TAISC_STATE_cc2520_SLEEP                                = 2,             //2
	TAISC_STATE_cc2520_IDLE                                 = 4,             //3
	TAISC_STATE_cc2520_RX                                   = 8,             //4
	TAISC_STATE_cc2520_SCAN                                 = 16,            //5
	TAISC_STATE_cc2520_LPM2_stabilizingVoltageRegulator     = 32,            //6
	TAISC_STATE_cc2520_LPM2_voltageRegulatorIsStable        = 64,            //7
	TAISC_STATE_cc2520_LPM2_stabilizingOscillator           = 128,           //8
	TAISC_STATE_cc2520_LPM2_oscillatorIsStable              = 256,           //9
	TAISC_STATE_cc2520_LPM2_intializing                     = 512,           //10
	TAISC_STATE_cc2520_LPM1_stabilizingOscillator           = 1024,          //11
	TAISC_STATE_cc2520_RxTurnAround                         = 2048,          //12
	TAISC_STATE_cc2520_ScanTurnAround                       = 4096,          //13
	TAISC_STATE_cc2520_TxTurnAround                         = 8192,          //14
	TAISC_STATE_cc2520_TX                                   = 16384,         //15
	TAISC_STATE_cc2520_ALL                                  = 32767          //ALL
} TAISC_cc2520_stateT;

typedef struct {
	 uint8_t		data[65];
	 TAISC_absRAMaddrT	next;
	 TAISC_sizeT		length;
} frameT;

typedef  frameT	rxQT[3];
typedef  frameT	txQT[3];

typedef struct {
	TAISC_gpRAMT			ram;
 	TAISC_absRAMaddrT               currentRxFrame;
 	rxQT                            rxQ;
 	TAISC_absRAMaddrT               currentTxFrame;
 	txQT                            txQ;
 	TAISC_absTimestampT             startOfFrameTimestampT;
 	TAISC_absTimestampT             endOfFrameTimestampT;
 	TAISC_absTimestampT             mediumChangedTimestampT;
 	TAISC_absTimestampT             uptimeTimestampT;
}  
#ifndef REMOVE_ATTR_PACKED 
__attribute__ ((packed))
#endif
 TAISC_registersT;
norace TAISC_registersT TAISC_RAM;


 
 
 #endif //HEADER_TAISC_COMMON

 TAISC_cc2520_stateT TAISC_EVENT_dataplane_txFrameAvailable_states = TAISC_STATE_cc2520_ALL;
TAISC_cc2520_stateT TAISC_EVENT_dataplane_rxFrameAvailable_states = TAISC_STATE_cc2520_RX;
TAISC_cc2520_stateT TAISC_EVENT_cc2520_startOfFrame_states = TAISC_STATE_cc2520_RX;
TAISC_cc2520_stateT TAISC_EVENT_cc2520_endOfFrame_states = TAISC_STATE_cc2520_RX;
TAISC_cc2520_stateT TAISC_EVENT_cc2520_stateOfMediumChanged_states = TAISC_STATE_cc2520_SCAN + TAISC_STATE_cc2520_RX;

#ifdef TAISC_COMPILER_ACCESS
void LPM2_checkVoltageRegulator();
void LPM2_startOscillator();
void LPM2_checkOscillator();
void LPM2_initialize();
void LPM1_checkOscillator();
void wait4Rx();
void wait4Scan();
void wait4Tx();
void wait4TxFrame();
#endif // TAISC_COMPILER_ACCESS

//core
void nopi() {
	 byteCodeT bytecode = 0;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
}

//core
void stop(TAISC_relBigTimestampT duration) {
	 byteCodeT bytecode = 1;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
}

//core
void wait(TAISC_relBigTimestampT staticTransitionTime) {
	 byteCodeT bytecode = 2;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
}

//core
void loadChain(TAISC_ChainIDT id, TAISC_relBigTimestampT timeOffset) {
	 byteCodeT bytecode = 3;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
}

//core
void storeTAISCtime(TAISC_absRAMaddrT destination) {
	 byteCodeT bytecode = 4;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
}

//core
void storeTAISCChainRef(TAISC_absRAMaddrT destination) {
	 byteCodeT bytecode = 5;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
}

//core
int waitForTrigger(TAISC_relBigTimestampT staticTransitionTime, TAISC_bitMAPT eventBitMask) {
	 byteCodeT bytecode = 6;
	 TAISC_absROMaddrT jumpTo = 0;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
}

//core
void report(TAISC_ReportIDT id, TAISC_absRAMaddrT dataPointer, TAISC_sizeT size) {
	 byteCodeT bytecode = 7;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
}

//core
void lookup(TAISC_absRAMaddrT lookupPointer, TAISC_relRAMaddrT lookupOffset, TAISC_absRAMaddrT destinationPointer) {
	 byteCodeT bytecode = 8;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
}

//core
void jumpTo() {
	 byteCodeT bytecode = 9;
	 TAISC_absROMaddrT jumpTo = 0;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
}

//core
int check(TAISC_absRAMaddrT value1Pointer, TAISC_relRAMaddrT value1Offset, TAISC_absRAMaddrT value2Pointer, TAISC_relRAMaddrT value2Offset, TAISC_absRAMaddrT maskPointer, TAISC_relRAMaddrT maskOffset, TAISC_sizeT size) {
	 byteCodeT bytecode = 10;
	 TAISC_absROMaddrT jumpTo = 0;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
}

//arithmetic
void copy(TAISC_absRAMaddrT sourcePointer, TAISC_relRAMaddrT sourceOffset, TAISC_absRAMaddrT destinationPointer, TAISC_relRAMaddrT destinationOffset, TAISC_absRAMaddrT maskPointer, TAISC_relRAMaddrT maskOffset, TAISC_sizeT size) {
	 byteCodeT bytecode = 11;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
}

//arithmetic
void add(TAISC_absRAMaddrT a, TAISC_absRAMaddrT b, TAISC_absRAMaddrT m, TAISC_sizeT size) {
	 byteCodeT bytecode = 12;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
}

//arithmetic
void sub(TAISC_absRAMaddrT a, TAISC_absRAMaddrT b, TAISC_absRAMaddrT m, TAISC_sizeT size) {
	 byteCodeT bytecode = 13;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
}

//dataplane
void rxTrigger(TAISC_conditionT flag) {
	 byteCodeT bytecode = 14;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
}

//dataplane
void txTrigger(TAISC_conditionT flag) {
	 byteCodeT bytecode = 15;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
}

//cc2520
void onFromOff() {
	 byteCodeT bytecode = 16;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
	 TAISC_cc2520_stateT nextState = TAISC_STATE_cc2520_LPM2_stabilizingVoltageRegulator;
	 subInstructionT nextInstruction = LPM2_checkVoltageRegulator;
	 TAISC_bitMAPT allowedStates = TAISC_STATE_cc2520_OFF;
}

//cc2520
void onFromSleep() {
	 byteCodeT bytecode = 17;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
	 TAISC_cc2520_stateT nextState = TAISC_STATE_cc2520_LPM1_stabilizingOscillator;
	 subInstructionT nextInstruction = LPM1_checkOscillator;
	 TAISC_bitMAPT allowedStates = TAISC_STATE_cc2520_SLEEP;
}

#ifdef TAISC_COMPILER_ACCESS
//cc2520
void LPM2_checkVoltageRegulator() {
	 byteCodeT bytecode = 18;
	 TAISC_relBigTimestampT staticExecutionTime = 100;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
	 TAISC_cc2520_stateT nextState = TAISC_STATE_cc2520_LPM2_voltageRegulatorIsStable;
	 subInstructionT nextInstruction = LPM2_startOscillator;
	 TAISC_bitMAPT allowedStates = TAISC_STATE_cc2520_LPM2_stabilizingVoltageRegulator;
}
#endif // TAISC_COMPILER_ACCESS

#ifdef TAISC_COMPILER_ACCESS
//cc2520
void LPM2_startOscillator() {
	 byteCodeT bytecode = 19;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
	 TAISC_cc2520_stateT nextState = TAISC_STATE_cc2520_LPM2_stabilizingOscillator;
	 subInstructionT nextInstruction = LPM2_checkOscillator;
	 TAISC_bitMAPT allowedStates = TAISC_STATE_cc2520_LPM2_voltageRegulatorIsStable;
}
#endif // TAISC_COMPILER_ACCESS

#ifdef TAISC_COMPILER_ACCESS
//cc2520
void LPM2_checkOscillator() {
	 byteCodeT bytecode = 20;
	 TAISC_relBigTimestampT staticExecutionTime = 100;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
	 TAISC_cc2520_stateT nextState = TAISC_STATE_cc2520_LPM2_oscillatorIsStable;
	 subInstructionT nextInstruction = LPM2_initialize;
	 TAISC_bitMAPT allowedStates = TAISC_STATE_cc2520_LPM2_stabilizingOscillator;
}
#endif // TAISC_COMPILER_ACCESS

#ifdef TAISC_COMPILER_ACCESS
//cc2520
void LPM2_initialize() {
	 byteCodeT bytecode = 21;
	 TAISC_relBigTimestampT staticExecutionTime = 1830;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
	 TAISC_cc2520_stateT nextState = TAISC_STATE_cc2520_IDLE;
	 TAISC_bitMAPT allowedStates = TAISC_STATE_cc2520_LPM2_oscillatorIsStable;
}
#endif // TAISC_COMPILER_ACCESS

#ifdef TAISC_COMPILER_ACCESS
//cc2520
void LPM1_checkOscillator() {
	 byteCodeT bytecode = 22;
	 TAISC_relBigTimestampT staticExecutionTime = 100;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
	 TAISC_cc2520_stateT nextState = TAISC_STATE_cc2520_IDLE;
	 TAISC_bitMAPT allowedStates = TAISC_STATE_cc2520_LPM1_stabilizingOscillator;
}
#endif // TAISC_COMPILER_ACCESS

//cc2520
void off() {
	 byteCodeT bytecode = 23;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
	 TAISC_cc2520_stateT nextState = TAISC_STATE_cc2520_OFF;
	 TAISC_bitMAPT allowedStates = TAISC_STATE_cc2520_ALL;
}

//cc2520
void sleep() {
	 byteCodeT bytecode = 24;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
	 TAISC_cc2520_stateT nextState = TAISC_STATE_cc2520_SLEEP;
	 TAISC_bitMAPT allowedStates = TAISC_STATE_cc2520_IDLE + TAISC_STATE_cc2520_RX;
}

//cc2520
void idle() {
	 byteCodeT bytecode = 25;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
	 TAISC_cc2520_stateT nextState = TAISC_STATE_cc2520_IDLE;
	 TAISC_bitMAPT allowedStates = TAISC_STATE_cc2520_RX;
}

//cc2520
void rx() {
	 byteCodeT bytecode = 26;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
	 TAISC_cc2520_stateT nextState = TAISC_STATE_cc2520_RxTurnAround;
	 subInstructionT nextInstruction = wait4Rx;
	 TAISC_bitMAPT allowedStates = TAISC_STATE_cc2520_IDLE;
}

#ifdef TAISC_COMPILER_ACCESS
//cc2520
void wait4Rx() {
	 byteCodeT bytecode = 27;
	 TAISC_relBigTimestampT staticExecutionTime = 192;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
	 TAISC_cc2520_stateT nextState = TAISC_STATE_cc2520_RX;
	 TAISC_bitMAPT allowedStates = TAISC_STATE_cc2520_RxTurnAround;
}
#endif // TAISC_COMPILER_ACCESS

//cc2520
void scan() {
	 byteCodeT bytecode = 28;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
	 TAISC_cc2520_stateT nextState = TAISC_STATE_cc2520_ScanTurnAround;
	 subInstructionT nextInstruction = wait4Scan;
	 TAISC_bitMAPT allowedStates = TAISC_STATE_cc2520_IDLE;
}

#ifdef TAISC_COMPILER_ACCESS
//cc2520
void wait4Scan() {
	 byteCodeT bytecode = 29;
	 TAISC_relBigTimestampT staticExecutionTime = 192;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
	 TAISC_cc2520_stateT nextState = TAISC_STATE_cc2520_SCAN;
	 TAISC_bitMAPT allowedStates = TAISC_STATE_cc2520_ScanTurnAround;
}
#endif // TAISC_COMPILER_ACCESS

//cc2520
void tx() {
	 byteCodeT bytecode = 30;
	 TAISC_relBigTimestampT staticExecutionTime = 192;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
	 TAISC_cc2520_stateT nextState = TAISC_STATE_cc2520_TxTurnAround;
	 subInstructionT nextInstruction = wait4Tx;
	 TAISC_bitMAPT allowedStates = TAISC_STATE_cc2520_RX + TAISC_STATE_cc2520_IDLE;
}

#ifdef TAISC_COMPILER_ACCESS
//cc2520
void wait4Tx() {
	 byteCodeT bytecode = 31;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
	 TAISC_cc2520_stateT nextState = TAISC_STATE_cc2520_TX;
	 subInstructionT nextInstruction = wait4TxFrame;
	 TAISC_bitMAPT allowedStates = TAISC_STATE_cc2520_TxTurnAround;
}
#endif // TAISC_COMPILER_ACCESS

#ifdef TAISC_COMPILER_ACCESS
//cc2520
void wait4TxFrame() {
	 byteCodeT bytecode = 32;
	 TAISC_relBigTimestampT staticExecutionTime = 0;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 32;
	 TAISC_cc2520_stateT nextState = TAISC_STATE_cc2520_RxTurnAround;
	 subInstructionT nextInstruction = wait4Rx;
	 TAISC_bitMAPT allowedStates = TAISC_STATE_cc2520_TX;
}
#endif // TAISC_COMPILER_ACCESS

//cc2520
void setAutoAck(TAISC_absRAMaddrT activate) {
	 byteCodeT bytecode = 33;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
	 TAISC_bitMAPT allowedStates = TAISC_STATE_cc2520_IDLE + TAISC_STATE_cc2520_RX + TAISC_STATE_cc2520_TX;
}

//cc2520
void storeRSSI(TAISC_absRAMaddrT activate) {
	 byteCodeT bytecode = 34;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
	 TAISC_bitMAPT allowedStates = TAISC_STATE_cc2520_RX + TAISC_STATE_cc2520_TX;
}

//cc2520
void setPromiscous(TAISC_absRAMaddrT activate) {
	 byteCodeT bytecode = 35;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
	 TAISC_bitMAPT allowedStates = TAISC_STATE_cc2520_IDLE + TAISC_STATE_cc2520_RX;
}

//cc2520
void setChannel(TAISC_absRAMaddrT channel) {
	 byteCodeT bytecode = 36;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
	 TAISC_bitMAPT allowedStates = TAISC_STATE_cc2520_IDLE + TAISC_STATE_cc2520_RX + TAISC_STATE_cc2520_TX;
}

//cc2520
void loadFrame(TAISC_absRAMaddrT framePointer, TAISC_sizeT size) {
	 byteCodeT bytecode = 37;
	 TAISC_relBigTimestampT staticExecutionTime = 50;
	 TAISC_relBigTimestampT dynamicExecutionTime = 1;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
	 TAISC_bitMAPT allowedStates = TAISC_STATE_cc2520_IDLE + TAISC_STATE_cc2520_RX;
}

//cc2520
void flushRx() {
	 byteCodeT bytecode = 38;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
	 TAISC_bitMAPT allowedStates = TAISC_STATE_cc2520_IDLE + TAISC_STATE_cc2520_RX;
}

//cc2520
void flushTx() {
	 byteCodeT bytecode = 39;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
	 TAISC_bitMAPT allowedStates = TAISC_STATE_cc2520_IDLE + TAISC_STATE_cc2520_RX;
}

//cc2520
void controlCCA(TAISC_absRAMaddrT CCA_mode, TAISC_absRAMaddrT CCA_hysteresis, TAISC_absRAMaddrT CCA_threshold) {
	 byteCodeT bytecode = 40;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
	 TAISC_bitMAPT allowedStates = TAISC_STATE_cc2520_IDLE + TAISC_STATE_cc2520_RX;
}

//cc2520
void controlFifoP(TAISC_absRAMaddrT Fifop_threshold) {
	 byteCodeT bytecode = 41;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
	 TAISC_bitMAPT allowedStates = TAISC_STATE_cc2520_IDLE + TAISC_STATE_cc2520_RX;
}

//cc2520
int cc2520_checkState(TAISC_cc2520_stateT state) {
	 byteCodeT bytecode = 42;
	 TAISC_absROMaddrT jumpTo = 0;
	 TAISC_relBigTimestampT staticExecutionTime = 10;
	 TAISC_relBigTimestampT dynamicExecutionTime = 0;
	 TAISC_relBigTimestampT staticTransitionTime = 0;
	 TAISC_relBigTimestampT dynamicTransitionTime = 0;
	 TAISC_bitMAPT allowedStates = TAISC_STATE_cc2520_ALL;
}


