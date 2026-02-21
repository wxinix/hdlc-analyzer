#ifndef HDLC_ANALYZER_SETTINGS
#define HDLC_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>
#include <memory>

/////////////////////////////////////

// NOTE: terminology:
//    * HDLC Frame == Saleae Logic Packet
//    * HDLC Field == Saleae Logic Frame
//    * HDLC transactions not supported

// Inner frames types of HDLC frame (address, control, data, fcs, etc)
enum HdlcFieldType
{
    HDLC_FIELD_FLAG = 0,
    HDLC_FIELD_BASIC_ADDRESS,
    HDLC_FIELD_EXTENDED_ADDRESS,
    HDLC_FIELD_BASIC_CONTROL,
    HDLC_FIELD_EXTENDED_CONTROL,
    HDLC_FIELD_INFORMATION,
    HDLC_FIELD_FCS,
    HDLC_ABORT_SEQ
};
// Transmission mode (bit stuffing or byte stuffing)
enum HdlcTransmissionModeType
{
    HDLC_TRANSMISSION_BIT_SYNC = 0,
    HDLC_TRANSMISSION_BIT_SYNC_EXT_CLK,
    HDLC_TRANSMISSION_BYTE_ASYNC
};
// Types of HDLC frames (Information, Supervisory and Unnumbered)
enum HdlcFrameType
{
    HDLC_I_FRAME = 0,
    HDLC_S_FRAME = 1,
    HDLC_U_FRAME = 3
};
// Address Field type
enum HdlcAddressType
{
    HDLC_BASIC_ADDRESS_FIELD = 0,
    HDLC_EXTENDED_ADDRESS_FIELD
};
// Control Field Type
enum HdlcControlType
{
    HDLC_BASIC_CONTROL_FIELD,
    HDLC_EXTENDED_CONTROL_FIELD_MOD_128,
    HDLC_EXTENDED_CONTROL_FIELD_MOD_32768,
    HDLC_EXTENDED_CONTROL_FIELD_MOD_2147483648
};
// Frame Check Sequence algorithm
enum HdlcFcsType
{
    HDLC_CRC8 = 0,
    HDLC_CRC16 = 1,
    HDLC_CRC32 = 2
};
// Flag Field Type (Start, End or Fill)
enum HdlcFlagType
{
    HDLC_FLAG_START = 0,
    HDLC_FLAG_END = 1,
    HDLC_FLAG_FILL = 2
};


// Constexpr lookup functions for enum-to-count mappings
constexpr U32 FcsByteCount( HdlcFcsType fcsType )
{
    switch( fcsType )
    {
    case HDLC_CRC8:
        return 1;
    case HDLC_CRC16:
        return 2;
    case HDLC_CRC32:
        return 4;
    default:
        return 0;
    }
}

constexpr U32 FcsBitCount( HdlcFcsType fcsType )
{
    return FcsByteCount( fcsType ) * 8;
}

constexpr const char* FcsCrcName( HdlcFcsType fcsType )
{
    switch( fcsType )
    {
    case HDLC_CRC8:
        return "8 ";
    case HDLC_CRC16:
        return "16";
    case HDLC_CRC32:
        return "32";
    default:
        return "";
    }
}

constexpr U32 ControlFieldByteCount( HdlcControlType controlType )
{
    switch( controlType )
    {
    case HDLC_BASIC_CONTROL_FIELD:
        return 1;
    case HDLC_EXTENDED_CONTROL_FIELD_MOD_128:
        return 2;
    case HDLC_EXTENDED_CONTROL_FIELD_MOD_32768:
        return 4;
    case HDLC_EXTENDED_CONTROL_FIELD_MOD_2147483648:
        return 8;
    default:
        return 0;
    }
}

// Special values for Byte Asynchronous Transmission
constexpr U8 HDLC_FLAG_VALUE = 0x7E;
constexpr U8 HDLC_ESCAPE_SEQ_VALUE = 0x7D;
constexpr U8 HDLC_FILL_VALUE = 0xFF;
// For Frame::mFlag
constexpr U8 HDLC_ESCAPED_BYTE = 1 << 0;

/////////////////////////////////////

class HdlcAnalyzerSettings : public AnalyzerSettings
{
  public:
    HdlcAnalyzerSettings();
    ~HdlcAnalyzerSettings() override = default;

    bool SetSettingsFromInterfaces() override;
    void UpdateInterfacesFromSettings();
    void LoadSettings( const char* settings ) override;
    const char* SaveSettings() override;

    static U8 Bit5Inv( U8 value );

    Channel mInputChannel;
    Channel mClockChannel;
    U32 mBitRate;
    U32 mClockEdge;

    HdlcTransmissionModeType mTransmissionMode;
    HdlcAddressType mHdlcAddr;
    HdlcControlType mHdlcControl;
    HdlcFcsType mHdlcFcs;

  protected:
    std::unique_ptr<AnalyzerSettingInterfaceChannel> mInputChannelInterface;
    std::unique_ptr<AnalyzerSettingInterfaceChannel> mClockChannelInterface;
    std::unique_ptr<AnalyzerSettingInterfaceInteger> mBitRateInterface;
    std::unique_ptr<AnalyzerSettingInterfaceNumberList> mClockEdgeInterface;
    std::unique_ptr<AnalyzerSettingInterfaceNumberList> mHdlcAddrInterface;
    std::unique_ptr<AnalyzerSettingInterfaceNumberList> mHdlcTransmissionInterface;
    std::unique_ptr<AnalyzerSettingInterfaceNumberList> mHdlcControlInterface;
    std::unique_ptr<AnalyzerSettingInterfaceNumberList> mHdlcFcsInterface;
};

#endif // HDLC_ANALYZER_SETTINGS
