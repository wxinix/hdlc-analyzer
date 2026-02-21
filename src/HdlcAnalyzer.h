#ifndef HDLC_ANALYZER_H
#define HDLC_ANALYZER_H

#include <Analyzer.h>
#include "HdlcAnalyzerResults.h"
#include "HdlcSimulationDataGenerator.h"
#include <memory>
#include <vector>

struct HdlcByte
{
    U64 startSample;
    U64 endSample;
    U8 value;
    bool escaped;
};

class HdlcAnalyzerSettings;
class HdlcAnalyzer : public Analyzer2
{
  public:
    HdlcAnalyzer();
    ~HdlcAnalyzer() override;
    void WorkerThread() override;

    U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels ) override;
    U32 GetMinimumSampleRateHz() override;

    const char* GetAnalyzerName() const override;
    bool NeedsRerun() override;

    void SetupResults() override;

    static HdlcFrameType GetFrameType( U8 value );

  protected:
    void SetupAnalyzer();

    // Functions to read and process a HDLC frame
    void ProcessHDLCFrame();
    HdlcByte ProcessFlags();
    void ProcessAddressField( HdlcByte byteAfterFlag );
    void ProcessControlField();
    void ProcessInfoAndFcsField();
    std::vector<HdlcByte> ReadProcessAndFcsField();
    void InfoAndFcsField( const std::vector<HdlcByte>& informationAndFcs );
    void ProcessInformationField( const std::vector<HdlcByte>& information );
    void ProcessFcsField( const std::vector<HdlcByte>& fcs );
    HdlcByte ReadByte();

    // Bit Sync Transmission functions
    void BitSyncProcessFlags();
    BitState BitSyncReadBit();
    HdlcByte BitSyncReadByte();
    HdlcByte BitSyncProcessFirstByteAfterFlag( HdlcByte firstAddressByte );
    bool FlagComing();
    bool AbortComing();

    // Bit Sync External Clock Transmission functions
    void AdvanceClockToActiveEdge();
    BitState BitSyncExtClkReadBit();
    HdlcByte BitSyncExtClkReadByte();
    void BitSyncExtClkProcessFlags();
    bool ExtClkFlagComing();
    bool ExtClkAbortComing();

    // Byte Async Transmission functions
    HdlcByte ByteAsyncProcessFlags();
    void GenerateFlagsFrames( std::vector<HdlcByte> readBytes );
    HdlcByte ByteAsyncReadByte();
    HdlcByte ByteAsyncReadByte_();

    // Flag frame emission helper
    void EmitFlagFrames( const std::vector<HdlcByte>& flags, bool markLastAsStart );

    // Helper functions
    Frame CreateFrame( U8 mType, U64 mStartingSampleInclusive, U64 mEndingSampleInclusive, U64 mData1 = 0, U64 mData2 = 0,
                       U8 mFlags = 0 ) const;
    std::vector<U8> HdlcBytesToVectorBytes( const std::vector<HdlcByte>& asyncBytes ) const;
    U64 VectorToValue( const std::vector<U8>& v ) const;

    void AddFrameToResults( const Frame& frame );
    void CommitFrames();

  protected:
    std::unique_ptr<HdlcAnalyzerSettings> mSettings;
    std::unique_ptr<HdlcAnalyzerResults> mResults;
    AnalyzerChannelData* mHdlc;
    AnalyzerChannelData* mClock;

    U32 mSampleRateHz;
    U64 mSamplesInHalfPeriod;
    U64 mSamplesInAFlag;
    U32 mSamplesIn8Bits;

    std::vector<U8> mCurrentFrameBytes;

    BitState mPreviousBitState;
    U32 mConsecutiveOnes;
    bool mReadingFrame;
    bool mAbortFrame;
    bool mCurrentFrameIsSFrame;
    bool mFoundEndFlag;

    Frame mEndFlagFrame;
    Frame mAbtFrame;

    std::vector<Frame> mResultFrames;

    HdlcSimulationDataGenerator mSimulationDataGenerator;
    bool mSimulationInitialized;
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer();
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif // HDLC_ANALYZER_H
