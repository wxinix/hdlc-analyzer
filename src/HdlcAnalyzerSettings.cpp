#include "HdlcAnalyzerSettings.h"
#include <AnalyzerHelpers.h>
#include <memory>

namespace {

// Type-safe enum serialization helpers for SimpleArchive
template<typename EnumT>
void LoadEnum( SimpleArchive& archive, EnumT& value )
{
    U32 tmp;
    archive >> tmp;
    value = static_cast<EnumT>( tmp );
}

template<typename EnumT>
void SaveEnum( SimpleArchive& archive, EnumT value )
{
    archive << static_cast<U32>( value );
}

} // anonymous namespace

HdlcAnalyzerSettings::HdlcAnalyzerSettings()
    : mInputChannel( UNDEFINED_CHANNEL ),
      mClockChannel( UNDEFINED_CHANNEL ),
      mBitRate( 2000000 ),
      mClockEdge( 0 ),
      mTransmissionMode( HDLC_TRANSMISSION_BIT_SYNC ),
      mHdlcAddr( HDLC_BASIC_ADDRESS_FIELD ),
      mHdlcControl( HDLC_BASIC_CONTROL_FIELD ),
      mHdlcFcs( HDLC_CRC16 )
{
    mInputChannelInterface = std::make_unique<AnalyzerSettingInterfaceChannel>();
    mInputChannelInterface->SetTitleAndTooltip( "HDLC", "Standard HDLC" );
    mInputChannelInterface->SetChannel( mInputChannel );

    mClockChannelInterface = std::make_unique<AnalyzerSettingInterfaceChannel>();
    mClockChannelInterface->SetTitleAndTooltip( "Clock", "External clock signal for Bit Synchronous (External Clock) mode" );
    mClockChannelInterface->SetChannel( mClockChannel );
    mClockChannelInterface->SetSelectionOfNoneIsAllowed( true );

    mBitRateInterface = std::make_unique<AnalyzerSettingInterfaceInteger>();
    mBitRateInterface->SetTitleAndTooltip( "Baud Rate", "Specify the baud rate in symbols per second." );
    mBitRateInterface->SetMax( 6000000 );
    mBitRateInterface->SetMin( 1 );
    mBitRateInterface->SetInteger( mBitRate );

    mHdlcTransmissionInterface = std::make_unique<AnalyzerSettingInterfaceNumberList>();
    mHdlcTransmissionInterface->SetTitleAndTooltip( "Transmission Mode", "Specify the transmission mode of the HDLC frames" );
    mHdlcTransmissionInterface->AddNumber( HDLC_TRANSMISSION_BIT_SYNC, "Bit Synchronous", "Bit-oriented transmission using bit stuffing (internal clock)" );
    mHdlcTransmissionInterface->AddNumber( HDLC_TRANSMISSION_BIT_SYNC_EXT_CLK, "Bit Synchronous (External Clock)", "Bit-oriented transmission using external clock signal" );
    mHdlcTransmissionInterface->AddNumber( HDLC_TRANSMISSION_BYTE_ASYNC, "Byte Asynchronous",
                                           "Byte asynchronous transmission using byte stuffing (Also known as start/stop mode)" );
    mHdlcTransmissionInterface->SetNumber( mTransmissionMode );

    mClockEdgeInterface = std::make_unique<AnalyzerSettingInterfaceNumberList>();
    mClockEdgeInterface->SetTitleAndTooltip( "Clock Edge", "Which clock edge to sample data on" );
    mClockEdgeInterface->AddNumber( 0, "Rising Edge", "Sample data on rising edge of clock" );
    mClockEdgeInterface->AddNumber( 1, "Falling Edge", "Sample data on falling edge of clock" );
    mClockEdgeInterface->SetNumber( mClockEdge );

    mHdlcAddrInterface = std::make_unique<AnalyzerSettingInterfaceNumberList>();
    mHdlcAddrInterface->SetTitleAndTooltip( "Address Field Type", "Specify the address field type of an HDLC frame." );
    mHdlcAddrInterface->AddNumber( HDLC_BASIC_ADDRESS_FIELD, "Basic", "Basic Address Field (8 bits)" );
    mHdlcAddrInterface->AddNumber( HDLC_EXTENDED_ADDRESS_FIELD, "Extended", "Extended Address Field (8 or more bits)" );
    mHdlcAddrInterface->SetNumber( mHdlcAddr );

    mHdlcControlInterface = std::make_unique<AnalyzerSettingInterfaceNumberList>();
    mHdlcControlInterface->SetTitleAndTooltip( "Control Field Format", "Specify the Control Field type of a HDLC frame." );
    mHdlcControlInterface->AddNumber( HDLC_BASIC_CONTROL_FIELD, "Basic - Modulo 8", "Control Field of 8 bits" );
    mHdlcControlInterface->AddNumber( HDLC_EXTENDED_CONTROL_FIELD_MOD_128, "Extended - Modulo 128", "Control Field of 16 bits" );
    mHdlcControlInterface->AddNumber( HDLC_EXTENDED_CONTROL_FIELD_MOD_32768, "Extended - Modulo 32768", "Control Field of 32 bits" );
    mHdlcControlInterface->AddNumber( HDLC_EXTENDED_CONTROL_FIELD_MOD_2147483648, "Extended - Modulo 2147483648",
                                      "Control Field of 64 bits" );
    mHdlcControlInterface->SetNumber( mHdlcControl );

    mHdlcFcsInterface = std::make_unique<AnalyzerSettingInterfaceNumberList>();
    mHdlcFcsInterface->SetTitleAndTooltip( "FCS Type", "Specify the Frame Check Sequence of an HDLC frame" );
    mHdlcFcsInterface->AddNumber( HDLC_CRC8, "CRC-8", "8-bit Cyclic Redundancy Check" );
    mHdlcFcsInterface->AddNumber( HDLC_CRC16, "CRC-16-CCITT", "16-bit Cyclic Redundancy Check" );
    mHdlcFcsInterface->AddNumber( HDLC_CRC32, "CRC-32", "32-bit Cyclic Redundancy Check" );
    mHdlcFcsInterface->SetNumber( mHdlcFcs );

    AddInterface( mInputChannelInterface.get() );
    AddInterface( mClockChannelInterface.get() );
    AddInterface( mBitRateInterface.get() );
    AddInterface( mHdlcTransmissionInterface.get() );
    AddInterface( mClockEdgeInterface.get() );
    AddInterface( mHdlcAddrInterface.get() );
    AddInterface( mHdlcControlInterface.get() );
    AddInterface( mHdlcFcsInterface.get() );

    AddExportOption( 0, "Export as text/csv file" );
    AddExportExtension( 0, "text", "txt" );
    AddExportExtension( 0, "csv", "csv" );

    ClearChannels();
    AddChannel( mInputChannel, "HDLC", false );
    AddChannel( mClockChannel, "Clock", false );
}

HdlcAnalyzerSettings::~HdlcAnalyzerSettings()
{
}

U8 HdlcAnalyzerSettings::Bit5Inv( U8 value )
{
    return value ^ 0x20;
}

bool HdlcAnalyzerSettings::SetSettingsFromInterfaces()
{
    mInputChannel = mInputChannelInterface->GetChannel();
    mClockChannel = mClockChannelInterface->GetChannel();
    mBitRate = mBitRateInterface->GetInteger();
    mClockEdge = static_cast<U32>( mClockEdgeInterface->GetNumber() );
    mTransmissionMode = static_cast<HdlcTransmissionModeType>( static_cast<U32>( mHdlcTransmissionInterface->GetNumber() ) );
    mHdlcAddr = static_cast<HdlcAddressType>( static_cast<U32>( mHdlcAddrInterface->GetNumber() ) );
    mHdlcControl = static_cast<HdlcControlType>( static_cast<U32>( mHdlcControlInterface->GetNumber() ) );
    mHdlcFcs = static_cast<HdlcFcsType>( static_cast<U32>( mHdlcFcsInterface->GetNumber() ) );

    if( mTransmissionMode == HDLC_TRANSMISSION_BIT_SYNC_EXT_CLK && mClockChannel == UNDEFINED_CHANNEL )
    {
        SetErrorText( "External Clock mode requires a clock channel to be selected." );
        return false;
    }

    ClearChannels();
    AddChannel( mInputChannel, "HDLC", true );
    AddChannel( mClockChannel, "Clock", mClockChannel != UNDEFINED_CHANNEL );

    return true;
}

void HdlcAnalyzerSettings::UpdateInterfacesFromSettings()
{
    mInputChannelInterface->SetChannel( mInputChannel );
    mClockChannelInterface->SetChannel( mClockChannel );
    mBitRateInterface->SetInteger( mBitRate );
    mClockEdgeInterface->SetNumber( mClockEdge );
    mHdlcTransmissionInterface->SetNumber( mTransmissionMode );
    mHdlcAddrInterface->SetNumber( mHdlcAddr );
    mHdlcControlInterface->SetNumber( mHdlcControl );
    mHdlcFcsInterface->SetNumber( mHdlcFcs );
}

void HdlcAnalyzerSettings::LoadSettings( const char* settings )
{
    SimpleArchive text_archive;
    text_archive.SetString( settings );

    text_archive >> mInputChannel;
    text_archive >> mBitRate;

    LoadEnum( text_archive, mTransmissionMode );
    LoadEnum( text_archive, mHdlcAddr );
    LoadEnum( text_archive, mHdlcControl );
    LoadEnum( text_archive, mHdlcFcs );

    text_archive >> mClockChannel;
    text_archive >> mClockEdge;

    ClearChannels();
    AddChannel( mInputChannel, "HDLC", true );
    AddChannel( mClockChannel, "Clock", mClockChannel != UNDEFINED_CHANNEL );

    UpdateInterfacesFromSettings();
}

const char* HdlcAnalyzerSettings::SaveSettings()
{
    SimpleArchive text_archive;

    text_archive << mInputChannel;
    text_archive << mBitRate;
    SaveEnum( text_archive, mTransmissionMode );
    SaveEnum( text_archive, mHdlcAddr );
    SaveEnum( text_archive, mHdlcControl );
    SaveEnum( text_archive, mHdlcFcs );
    text_archive << mClockChannel;
    text_archive << mClockEdge;

    return SetReturnString( text_archive.GetString() );
}
