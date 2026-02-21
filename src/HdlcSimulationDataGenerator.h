#ifndef HDLC_SIMULATION_DATA_GENERATOR
#define HDLC_SIMULATION_DATA_GENERATOR

#include <SimulationChannelDescriptor.h>
#include "HdlcAnalyzerSettings.h"
#include <string>
#include <vector>

class HdlcSimulationDataGenerator
{
  public:
    HdlcSimulationDataGenerator();
    ~HdlcSimulationDataGenerator();

    void Initialize( U32 simulation_sample_rate, HdlcAnalyzerSettings* settings );
    U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel );

    static std::vector<U8> Crc8( const std::vector<U8>& stream );
    static std::vector<U8> Crc16( const std::vector<U8>& stream );
    static std::vector<U8> Crc32( const std::vector<U8>& stream );
    static std::vector<U8> CrcDivision( const std::vector<U8>& stream, const std::vector<U8>& genPoly, U32 crcNumber );
    static std::vector<BitState> BytesVectorToBitsVector( const std::vector<U8>& v, U32 numberOfBits );

  protected:
    void CreateHDLCFrame( const std::vector<U8>& address, const std::vector<U8>& control, const std::vector<U8>& information );
    void CreateFlag();

    // Sync Transmission
    void CreateFlagBitSeq();
    void CreateSyncBit( BitState bitState );
    void TransmitBitSync( const std::vector<U8>& stream );

    // Async transmission
    void TransmitByteAsync( const std::vector<U8>& stream );
    void CreateAsyncByte( U8 byte );
    void AsyncByteFill( U32 N );

    // Helper functions
    bool AbortFrameAndGenIndex( U32 N );
    std::vector<U8> GenFcs( HdlcFcsType fcsType, const std::vector<U8>& stream ) const;

    void GenerateAbortFramesIndexes();
    bool ContainsElement( U32 index ) const;

    std::vector<U8> GenAddressField( HdlcAddressType addressType, U64 addressBytes, U8 value ) const;
    std::vector<U8> GenControlField( HdlcFrameType frameType, HdlcControlType controlType, U8 value ) const;
    std::vector<U8> GenInformationField( U16 size, U8 value ) const;

    HdlcAnalyzerSettings* mSettings;
    U32 mSimulationSampleRateHz;

    std::vector<U32> mAbortFramesIndexes;
    U32 mFrameNumber;
    U32 mAbortByte;
    U32 mWrongFramesSeparation;

    U8 mControlValue;
    U8 mAddresByteValue;
    U8 mInformationByteValue;

    HdlcFrameType mFrameTypes[ 3 ];

    SimulationChannelDescriptor mHdlcSimulationData;

    U64 mSamplesInHalfPeriod;
    U64 mSamplesInAFlag;
};
#endif // HDLC_SIMULATION_DATA_GENERATOR
