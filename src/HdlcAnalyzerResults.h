#ifndef HDLC_ANALYZER_RESULTS
#define HDLC_ANALYZER_RESULTS

#include <AnalyzerResults.h>
#include <string>

class HdlcAnalyzer;
class HdlcAnalyzerSettings;

class HdlcAnalyzerResults : public AnalyzerResults
{
  public:
    HdlcAnalyzerResults( HdlcAnalyzer* analyzer, HdlcAnalyzerSettings* settings );
    ~HdlcAnalyzerResults() override;

    void GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base ) override;
    void GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id ) override;

    void GenerateFrameTabularText( U64 frame_index, DisplayBase display_base ) override;
    void GeneratePacketTabularText( U64 packet_id, DisplayBase display_base ) override;
    void GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base ) override;

  protected: // functions
    void GenBubbleText( U64 frame_index, DisplayBase display_base, bool tabular );

    void GenFlagFieldString( const Frame& frame, bool tabular );
    void GenAddressFieldString( const Frame& frame, DisplayBase display_base, bool tabular );
    void GenControlFieldString( const Frame& frame, DisplayBase display_base, bool tabular );
    void GenInformationFieldString( const Frame& frame, DisplayBase display_base, bool tabular );
    void GenFcsFieldString( const Frame& frame, DisplayBase display_base, bool tabular );
    void GenAbortFieldString( bool tabular );

    std::string EscapeByteStr( const Frame& frame );
    std::string GenEscapedString( const Frame& frame );

  protected: // vars
    HdlcAnalyzerSettings* mSettings;
    HdlcAnalyzer* mAnalyzer;
};

#endif // HDLC_ANALYZER_RESULTS
