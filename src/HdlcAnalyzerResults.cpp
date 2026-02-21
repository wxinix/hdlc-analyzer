#include "HdlcAnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "HdlcAnalyzer.h"
#include "HdlcAnalyzerSettings.h"
#include <fstream>
#include <sstream>

HdlcAnalyzerResults::HdlcAnalyzerResults( HdlcAnalyzer* analyzer, HdlcAnalyzerSettings* settings )
    : AnalyzerResults(), mSettings( settings ), mAnalyzer( analyzer )
{
}

HdlcAnalyzerResults::~HdlcAnalyzerResults()
{
}

void HdlcAnalyzerResults::GenerateBubbleText( U64 frame_index, Channel& /*channel*/, DisplayBase display_base )
{
    GenBubbleText( frame_index, display_base, false );
}

void HdlcAnalyzerResults::GenBubbleText( U64 frame_index, DisplayBase display_base, bool tabular )
{
    if( !tabular )
        ClearResultStrings();

    auto frame = GetFrame( frame_index );

    switch( frame.mType )
    {
    case HDLC_FIELD_FLAG:
        GenFlagFieldString( frame, tabular );
        break;
    case HDLC_FIELD_BASIC_ADDRESS:
    case HDLC_FIELD_EXTENDED_ADDRESS:
        GenAddressFieldString( frame, display_base, tabular );
        break;
    case HDLC_FIELD_BASIC_CONTROL:
    case HDLC_FIELD_EXTENDED_CONTROL:
        GenControlFieldString( frame, display_base, tabular );
        break;
    case HDLC_FIELD_INFORMATION:
        GenInformationFieldString( frame, display_base, tabular );
        break;
    case HDLC_FIELD_FCS:
        GenFcsFieldString( frame, display_base, tabular );
        break;
    case HDLC_ABORT_SEQ:
        GenAbortFieldString( tabular );
        break;
    }
}

void HdlcAnalyzerResults::GenFlagFieldString( const Frame& frame, bool tabular )
{
    const char* flagTypeStr = nullptr;
    switch( frame.mData1 )
    {
    case HDLC_FLAG_START:
        flagTypeStr = "Start";
        break;
    case HDLC_FLAG_END:
        flagTypeStr = "End";
        break;
    case HDLC_FLAG_FILL:
        flagTypeStr = "Fill";
        break;
    }

    if( !tabular )
    {
        AddResultString( "F" );
        AddResultString( "FL" );
        AddResultString( "FLAG" );
        AddResultString( flagTypeStr, " FLAG" );
        AddResultString( flagTypeStr, " Flag Delimiter" );
    }
    else
        AddTabularText( flagTypeStr, " Flag Delimiter" );
}

std::string HdlcAnalyzerResults::GenEscapedString( const Frame& frame )
{
    std::stringstream ss;
    if( frame.mFlags & HDLC_ESCAPED_BYTE )
    {
        char dataStr[ 32 ];
        AnalyzerHelpers::GetNumberString( frame.mData1, Hexadecimal, 8, dataStr, 32 );
        char dataInvStr[ 32 ];
        AnalyzerHelpers::GetNumberString( HdlcAnalyzerSettings::Bit5Inv( frame.mData1 ), Hexadecimal, 8, dataInvStr, 32 );

        ss << " - ESCAPED: 0x7D-" << dataStr << "=" << dataInvStr;
    }

    return ss.str();
}

void HdlcAnalyzerResults::GenAddressFieldString( const Frame& frame, DisplayBase display_base, bool tabular )
{
    char addressStr[ 64 ];
    AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, addressStr, 64 );
    char byteNumber[ 64 ];
    AnalyzerHelpers::GetNumberString( frame.mData2, Decimal, 8, byteNumber, 64 );

    auto escStr = GenEscapedString( frame );

    if( !tabular )
    {
        AddResultString( "A" );
        AddResultString( "AD" );
        AddResultString( "ADDR" );
        AddResultString( "ADDR ", byteNumber, "[", addressStr, "]", escStr.c_str() );
        AddResultString( "Address ", byteNumber, "[", addressStr, "]", escStr.c_str() );
    }
    else
        AddTabularText( "Address ", byteNumber, "[", addressStr, "]", escStr.c_str() );
}

void HdlcAnalyzerResults::GenInformationFieldString( const Frame& frame, const DisplayBase display_base, bool tabular )
{
    char informationStr[ 64 ];
    AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, informationStr, 64 );
    char numberStr[ 64 ];
    AnalyzerHelpers::GetNumberString( frame.mData2, Decimal, 32, numberStr, 64 );

    auto escStr = GenEscapedString( frame );

    if( !tabular )
    {
        AddResultString( "I" );
        AddResultString( "I ", numberStr );
        AddResultString( "I ", numberStr, " [", informationStr, "]", escStr.c_str() );
        AddResultString( "Info ", numberStr, " [", informationStr, "]", escStr.c_str() );
    }
    else
        AddTabularText( "Info ", numberStr, " [", informationStr, "]", escStr.c_str() );
}

void HdlcAnalyzerResults::GenControlFieldString( const Frame& frame, DisplayBase display_base, bool tabular )
{
    char byteStr[ 64 ];
    AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, byteStr, 64 );

    char ctlNumStr[ 64 ];
    AnalyzerHelpers::GetNumberString( frame.mData2, Decimal, 8, ctlNumStr, 64 );

    auto escStr = GenEscapedString( frame );

    const char* frameTypeStr = nullptr;
    if( frame.mData2 != 0 )
    {
        frameTypeStr = "";
    }
    else
    {
        switch( HdlcAnalyzer::GetFrameType( frame.mData1 ) )
        {
        case HDLC_I_FRAME:
            frameTypeStr = " - I-Frame";
            break;
        case HDLC_S_FRAME:
            frameTypeStr = " - S-Frame";
            break;
        case HDLC_U_FRAME:
            frameTypeStr = " - U-Frame";
            break;
        }
    }

    std::stringstream ss;
    ss << "CTL" << ctlNumStr << " [";

    if( !tabular )
    {
        AddResultString( "C", ctlNumStr );
        AddResultString( "CTL", ctlNumStr );
        AddResultString( ss.str().c_str(), byteStr, "]", escStr.c_str() );
        AddResultString( ss.str().c_str(), byteStr, "]", frameTypeStr, escStr.c_str() );
        ss.str( "" );
        ss << "Control" << ctlNumStr << " [";

        AddResultString( ss.str().c_str(), byteStr, "]", frameTypeStr, escStr.c_str() );
    }
    else
    {
        ss.str( "" );
        ss << "Control" << ctlNumStr << " [";
        AddTabularText( ss.str().c_str(), byteStr, "]", frameTypeStr, escStr.c_str() );
    }
}

void HdlcAnalyzerResults::GenFcsFieldString( const Frame& frame, DisplayBase display_base, bool tabular )
{
    U32 fcsBits = 0;
    const char* crcTypeStr = nullptr;
    switch( mSettings->mHdlcFcs )
    {
    case HDLC_CRC8:
        fcsBits = 8;
        crcTypeStr = "8 ";
        break;
    case HDLC_CRC16:
        fcsBits = 16;
        crcTypeStr = "16";
        break;
    case HDLC_CRC32:
        fcsBits = 32;
        crcTypeStr = "32";
        break;
    }

    char readFcsStr[ 128 ];
    AnalyzerHelpers::GetNumberString( frame.mData1, display_base, fcsBits, readFcsStr, 128 );
    char calcFcsStr[ 128 ];
    AnalyzerHelpers::GetNumberString( frame.mData2, display_base, fcsBits, calcFcsStr, 128 );

    std::stringstream fieldNameStr;
    if( frame.mFlags & DISPLAY_AS_ERROR_FLAG )
    {
        fieldNameStr << "!";
    }

    fieldNameStr << "FCS CRC" << crcTypeStr;

    if( !tabular )
    {
        AddResultString( "CRC" );
        AddResultString( fieldNameStr.str().c_str() );
    }

    if( frame.mFlags & DISPLAY_AS_ERROR_FLAG )
    {
        fieldNameStr << " ERROR";
    }
    else
    {
        fieldNameStr << " OK";
    }

    if( !tabular )
    {
        AddResultString( fieldNameStr.str().c_str() );
    }

    if( frame.mFlags & DISPLAY_AS_ERROR_FLAG )
    {
        fieldNameStr << " - CALC CRC[" << calcFcsStr << "] != READ CRC[" << readFcsStr << "]";
    }

    if( !tabular )
        AddResultString( fieldNameStr.str().c_str() );
    else
        AddTabularText( fieldNameStr.str().c_str() );
}

void HdlcAnalyzerResults::GenAbortFieldString( bool tabular )
{
    const char* seq = nullptr;
    if( mSettings->mTransmissionMode == HDLC_TRANSMISSION_BIT_SYNC ||
        mSettings->mTransmissionMode == HDLC_TRANSMISSION_BIT_SYNC_EXT_CLK )
    {
        seq = "(>=7 1-bits)";
    }
    else
    {
        seq = "(0x7D-0x7F)";
    }

    if( !tabular )
    {
        AddResultString( "AB!" );
        AddResultString( "ABORT!" );
        AddResultString( "ABORT SEQUENCE!", seq );
    }
    else
    {
        AddTabularText( "ABORT SEQUENCE!", seq );
    }
}

std::string HdlcAnalyzerResults::EscapeByteStr( const Frame& frame )
{
    if( ( mSettings->mTransmissionMode == HDLC_TRANSMISSION_BYTE_ASYNC ) && ( frame.mFlags & HDLC_ESCAPED_BYTE ) )
    {
        return std::string( "0x7D-" );
    }
    else
    {
        return std::string();
    }
}

void HdlcAnalyzerResults::GenerateExportFile( const char* file, DisplayBase display_base, U32 /*export_type_user_id*/ )
{
    std::ofstream fileStream( file, std::ios::out );

    auto triggerSample = mAnalyzer->GetTriggerSample();
    auto sampleRate = mAnalyzer->GetSampleRate();

    const char* sepChar = " ";

    U8 fcsBits = 0;
    switch( mSettings->mHdlcFcs )
    {
    case HDLC_CRC8:
        fcsBits = 8;
        break;
    case HDLC_CRC16:
        fcsBits = 16;
        break;
    case HDLC_CRC32:
        fcsBits = 32;
        break;
    }

    fileStream << "Time[s],Address,Control,Information,FCS" << std::endl;

    char escapeStr[ 5 ];
    AnalyzerHelpers::GetNumberString( HDLC_ESCAPE_SEQ_VALUE, display_base, 8, escapeStr, 5 );

    auto numFrames = GetNumFrames();
    U64 frameNumber = 0;

    U32 numberOfControlBytes = 0;
    switch( mSettings->mHdlcControl )
    {
    case HDLC_BASIC_CONTROL_FIELD:
        numberOfControlBytes = 1;
        break;
    case HDLC_EXTENDED_CONTROL_FIELD_MOD_128:
        numberOfControlBytes = 2;
        break;
    case HDLC_EXTENDED_CONTROL_FIELD_MOD_32768:
        numberOfControlBytes = 4;
        break;
    case HDLC_EXTENDED_CONTROL_FIELD_MOD_2147483648:
        numberOfControlBytes = 8;
        break;
    }

    if( numFrames == 0 )
    {
        UpdateExportProgressAndCheckForCancel( frameNumber, numFrames );
        return;
    }

    for( ;; )
    {
        bool doAbortFrame = false;
        // Re-sync to start reading HDLC frames from the Address Byte
        Frame firstAddressFrame;
        for( ;; )
        {
            firstAddressFrame = GetFrame( frameNumber );

            // Check for abort
            if( firstAddressFrame.mType == HDLC_FIELD_BASIC_ADDRESS ||
                firstAddressFrame.mType == HDLC_FIELD_EXTENDED_ADDRESS ) // It's and address frame
            {
                break;
            }
            else
            {
                frameNumber++;
                if( frameNumber >= numFrames )
                {
                    UpdateExportProgressAndCheckForCancel( frameNumber, numFrames );
                    return;
                }
            }
        }

        // 1)  Time [s]
        char timeStr[ 64 ];
        AnalyzerHelpers::GetTimeString( firstAddressFrame.mStartingSampleInclusive, triggerSample, sampleRate, timeStr, 64 );
        fileStream << timeStr << ",";

        // 2) Address Field
        if( mSettings->mHdlcAddr == HDLC_BASIC_ADDRESS_FIELD )
        {
            firstAddressFrame = GetFrame( frameNumber );
            if( firstAddressFrame.mType != HDLC_FIELD_BASIC_ADDRESS )
            {
                fileStream << "," << std::endl;
                continue;
            }

            char addressStr[ 64 ];
            AnalyzerHelpers::GetNumberString( firstAddressFrame.mData1, display_base, 8, addressStr, 64 );
            fileStream << EscapeByteStr( firstAddressFrame ) << addressStr << ",";
        }
        else // Check for extended address
        {
            auto nextAddress = firstAddressFrame;
            for( ;; )
            {
                // Check for abort
                if( nextAddress.mType == HDLC_ABORT_SEQ )
                {
                    fileStream << "," << std::endl;
                    doAbortFrame = true;
                    break;
                }

                if( nextAddress.mType != HDLC_FIELD_EXTENDED_ADDRESS ) // ERROR
                {
                    fileStream << "," << std::endl;
                    break;
                }

                bool endOfAddress = ( ( nextAddress.mData1 & 0x01 ) == 0 );

                char addressStr[ 64 ];
                AnalyzerHelpers::GetNumberString( nextAddress.mData1, display_base, 8, addressStr, 64 );
                auto sep = ( endOfAddress && nextAddress.mData2 == 0 ) ? std::string() : std::string( sepChar );
                fileStream << sep << EscapeByteStr( nextAddress ) << addressStr;

                if( endOfAddress ) // no more bytes of address?
                {
                    fileStream << ",";
                    break;
                }
                else
                {
                    frameNumber++;
                    if( frameNumber >= numFrames )
                    {
                        UpdateExportProgressAndCheckForCancel( frameNumber, numFrames );
                        return;
                    }
                    nextAddress = GetFrame( frameNumber );
                }
            }
        }

        if( doAbortFrame )
        {
            continue;
        }

        // 3) Control Field
        bool isUFrame = false;
        for( U32 i = 0; i < numberOfControlBytes; ++i )
        {
            frameNumber++;
            if( frameNumber >= numFrames )
            {
                UpdateExportProgressAndCheckForCancel( frameNumber, numFrames );
                return;
            }

            auto controlFrame = GetFrame( frameNumber );

            // Check for abort
            if( controlFrame.mType == HDLC_ABORT_SEQ )
            {
                doAbortFrame = true;
                fileStream << "," << std::endl;
                break;
            }

            if( !( controlFrame.mType == HDLC_FIELD_BASIC_CONTROL || controlFrame.mType == HDLC_FIELD_EXTENDED_CONTROL ) ) // ERROR
            {
                fileStream << "," << std::endl;
                continue;
            }

            if( i == 0 )
            {
                isUFrame = HdlcAnalyzer::GetFrameType( controlFrame.mData1 ) == HDLC_U_FRAME;
            }

            char controlStr[ 64 ];
            AnalyzerHelpers::GetNumberString( controlFrame.mData1, display_base, 8, controlStr, 64 );
            auto sep = ( isUFrame || mSettings->mHdlcControl == HDLC_BASIC_CONTROL_FIELD ) ? std::string() : std::string( sepChar );
            fileStream << sep.c_str() << EscapeByteStr( controlFrame ) << controlStr;

            if( i == 0 && isUFrame )
            {
                break;
            }
        }

        if( doAbortFrame )
        {
            continue;
        }

        fileStream << ",";

        frameNumber++;
        if( frameNumber >= numFrames )
        {
            UpdateExportProgressAndCheckForCancel( frameNumber, numFrames );
            return;
        }

        // 5) Information Fields
        for( ;; )
        {
            auto infoFrame = GetFrame( frameNumber );

            // Check for abort
            if( infoFrame.mType == HDLC_ABORT_SEQ )
            {
                doAbortFrame = true;
                fileStream << "," << std::endl;
                break;
            }

            // Check for flag
            if( infoFrame.mType == HDLC_FIELD_FLAG )
            {
                fileStream << ",";
                break;
            }

            // Check for info byte
            if( infoFrame.mType == HDLC_FIELD_INFORMATION ) // ERROR
            {
                char infoByteStr[ 64 ];
                AnalyzerHelpers::GetNumberString( infoFrame.mData1, display_base, 8, infoByteStr, 64 );
                fileStream << sepChar << EscapeByteStr( infoFrame ) << infoByteStr;
                frameNumber++;
                if( frameNumber >= numFrames )
                {
                    UpdateExportProgressAndCheckForCancel( frameNumber, numFrames );
                    return;
                }
            }
            else
            {
                fileStream << ",";
                break;
            }
        }

        if( doAbortFrame )
        {
            continue;
        }

        // 6) FCS Field
        auto fcsFrame = GetFrame( frameNumber );
        if( fcsFrame.mType != HDLC_FIELD_FCS )
        {
            fileStream << "," << std::endl;
        }
        else // HDLC_FIELD_FCS Frame
        {
            char fcsStr[ 128 ];
            AnalyzerHelpers::GetNumberString( fcsFrame.mData1, display_base, fcsBits, fcsStr, 128 );
            fileStream << fcsStr << std::endl;
        }

        frameNumber++;
        if( frameNumber >= numFrames )
        {
            UpdateExportProgressAndCheckForCancel( frameNumber, numFrames );
            return;
        }

        if( UpdateExportProgressAndCheckForCancel( frameNumber, numFrames ) )
        {
            return;
        }
    }

    UpdateExportProgressAndCheckForCancel( frameNumber, numFrames );
}

void HdlcAnalyzerResults::GenerateFrameTabularText( U64 frame_index, DisplayBase display_base )
{
    ClearTabularText();
    GenBubbleText( frame_index, display_base, true );
}

void HdlcAnalyzerResults::GeneratePacketTabularText( U64 packet_id, DisplayBase display_base )
{
    ClearResultStrings();
    AddResultString( "not supported" );
}

void HdlcAnalyzerResults::GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base )
{
    ClearResultStrings();
    AddResultString( "not supported" );
}
