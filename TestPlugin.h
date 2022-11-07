#ifndef __TESTPLUGIN_H_INCLUDED
#define __TESTPLUGIN_H_INCLUDED

// You need to define this before the includes, or use /D __SUPPORT_PLUGIN__=1 
// in the C/C++ / Commandline project configuration
#define __SUPPORT_PLUGIN__ 1

#include "IPlugin.h"
#include "IRBRGame.h"
#include "PluginHelpers.h"

#include "Gendef.h" // 2D Gfx header
#include <stdio.h>

#include "AutoClutch.h"

const unsigned int NUM_SELECTIONS = 2;

//------------------------------------------------------------------------------------------------//

class CTestPlugin : public IPlugin
{
public:
	CTestPlugin	( IRBRGame* pGame )	
		:	m_pGame	( pGame )
		,	m_iSelection( 0 )
		,	m_iMap	( 10 )
		,	m_dTime ( 0 )
		,	m_dSec ( 0 )
		,	m_dsTime ( 0 )
		,	m_dsSec ( 0 )
		,	m_iSelections ( 0 )

	{
		FILE *fp = fopen( "rbrplugin.log", "wt" );
		if( fp )
		{
			fflush( fp );
			fclose( fp );
		}
	}

	virtual ~CTestPlugin( void )	
	{
	}

	//------------------------------------------------------------------------------------------------//

	virtual const char* GetName( void )	
	{
		return "Auto Clutch";
	}

	//------------------------------------------------------------------------------------------------//

	virtual void DrawResultsUI( void )
	{
		m_pGame->SetMenuColor( IRBRGame::MENU_HEADING );	
		m_pGame->SetFont  ( IRBRGame::FONT_BIG );
		m_pGame->WriteText( 130.0f, 49.0f, "Results" );

		m_pGame->SetFont  ( IRBRGame::FONT_SMALL );
		m_pGame->SetMenuColor( IRBRGame::MENU_TEXT );
		if( m_fResults[ 2 ] <= 0.0f )
		{
			m_pGame->WriteText( 200.0f, 100.0f, "DNF" );
		}
		else
		{
			char txtCP1[ 32 ];			
			char txtTimeString[ 32 ];
			char txtBuffer[ 128 ];

			sprintf( txtBuffer, "Stage Result for \"%s\" ", m_szPlayerName );
			m_pGame->WriteText( 130.0f, 100.0f, txtBuffer );
			sprintf( txtBuffer, "CheckPoint1 = %s", NPlugin::FormatTimeString( txtCP1, m_fResults[ 0 ] ) );
			m_pGame->WriteText( 130.0f, 125.0f, txtBuffer );
			sprintf( txtBuffer, "CheckPoint2 = %s", NPlugin::FormatTimeString( txtCP1, m_fResults[ 1 ] ) );
			m_pGame->WriteText( 130.0f, 150.0f, txtBuffer );
			sprintf( txtBuffer, "Finish = %s", NPlugin::FormatTimeString( txtTimeString, m_fResults[ 2 ] ) );
			m_pGame->WriteText( 130.0f, 175.0f, txtBuffer );
		}

	}

	//------------------------------------------------------------------------------------------------//

	virtual void DrawFrontEndPage( void )
	{
				m_iSelections = 4;
				m_pGame->DrawBlackOut( 0.0f, 0.0f, 0.0f, 0.0f );
				m_pGame->DrawSelection( 0.0f, 68.0f + ( static_cast< float >( m_iSelection ) * 21.0f ), 260.0f);
				m_pGame->SetMenuColor( IRBRGame::MENU_SELECTION );	
				m_pGame->DrawFlatBox( 260.0f, 49.0f, 3.0f, 271.0f);
				m_pGame->SetMenuColor( IRBRGame::MENU_ICON );
				//m_pGame->DrawBox( GEN_BOX_ICON_MULTI, 260.0f, 49.0f );

				m_pGame->SetMenuColor( IRBRGame::MENU_HEADING );	
				m_pGame->SetFont( IRBRGame::FONT_BIG );
				m_pGame->WriteText( 73.0f, 49.0f, "AutoClutch 1.0" );
				const char* aszSelections[] =
				{
					"Enabled",
					"Min RPM to fully engage",
					"Max RPM to fully release",
					"Save Changes"
				};

				m_pGame->SetMenuColor( IRBRGame::MENU_TEXT );
				for( unsigned int i = 0; i < m_iSelections; ++i )
				{
					m_pGame->WriteText( 73.0f, 70.0f + ( static_cast< float >( i ) * 21.0f ), aszSelections[ i ] );

					if (i == 0)
					{
						sprintf(m_szTemp, isEnabled ? "True" : "False");
					}
					else if (i == 1)
					{
						sprintf(m_szTemp, "%i", minRpm);
					}
					else if (i == 2)
					{
						sprintf(m_szTemp, "%i", maxRpm);
					}
					else if (i == 3)
					{
						if (m_dsSec)
						{
							if (GetTickCount() - m_dsTime > 1000)
							{
								m_dsTime = GetTickCount();
								--m_dsSec;
							}
							sprintf(m_szTemp, "File Saved...", m_dsSec);
						}
						else
						{
							m_szTemp[0] = 0;
						}
					}

					m_pGame->WriteText( 270.0f, 70.0f + ( static_cast< float >( i ) * 21.0f ), m_szTemp );
				}
		
		// m_pGame->DrawBox( GEN_BOX_LOGOS_CITROEN, 400.0f, 300.0f );
	}

	//------------------------------------------------------------------------------------------------//

	virtual void HandleFrontEndEvents( char txtKeyboard, bool bUp, bool bDown, 
		bool bLeft, bool bRight, bool bSelect )
	{

		if( bSelect )
		{
			if (m_iSelection == 3)
			{
				saveSettings();
				m_dsSec = 3;
			}
		}

		if( bUp )
		{
			--m_iSelection;
			if( m_iSelection < 0 )
				m_iSelection = m_iSelections - 1;
		}
		
		if( bDown )
		{
			++m_iSelection;
			if( m_iSelection >= m_iSelections )
				m_iSelection  = 0;
		}


		if( (bLeft || bRight) && m_iSelection == 0 )
		{
			isEnabled = !isEnabled;
		}

		if (bRight && m_iSelection == 1)
		{
			minRpm += 100;
			if (minRpm > maxRpm) {
				minRpm = maxRpm;
			}
			if (minRpm > 9000) {
				minRpm = 9000;
			}
		}

		if (bLeft && m_iSelection == 1)
		{
			minRpm -= 100;
			if (minRpm < 0) {
				minRpm = 0;
			}
		}

		if (bRight && m_iSelection == 2)
		{
			maxRpm += 100;
			if (maxRpm < minRpm) {
				maxRpm = minRpm;
			}
			if (maxRpm > 9000) {
				maxRpm = 9000;
			}
		}

		if (bLeft && m_iSelection == 2)
		{
			maxRpm -= 100;
			if (maxRpm < 0) {
				maxRpm = 0;
			}
		}
	}

	//------------------------------------------------------------------------------------------------//

	virtual void TickFrontEndPage( float fTimeDelta )
	{
	}

	//------------------------------------------------------------------------------------------------//
	/// Is called when the player timer starts (after GO! or in case of a false start)
	virtual void			StageStarted		( int iMap, 
												  const char* ptxtPlayerName,
												  bool bWasFalseStart )
	{
		FILE *fp = fopen( "rbrplugin.log", "at" );

		if( fp )
		{
			fprintf( fp, "Stage [%d] started. [%s] %s\n", iMap, ptxtPlayerName, bWasFalseStart ? "False Start" : "" );
			fclose( fp );
		}

		m_pGame->SetColor( 1.0f, 1.0f, 1.0f, 1.0f );
		m_pGame->WriteGameMessage( "Stage Started", 10.0f, 50.0f, 150.0f );
	}

	//------------------------------------------------------------------------------------------------//
	/// Is called when player finishes stage ( fFinishTime is 0.0f if player failed the stage )
	virtual void			HandleResults		( float fCheckPoint1, 
												  float fCheckPoint2,
												  float fFinishTime,
												  const char* ptxtPlayerName )
	{
		FILE *fp = fopen( "rbrplugin.log", "at" );

		if( fp )
		{
			m_fResults[ 0 ] = fCheckPoint1;
			m_fResults[ 1 ] = fCheckPoint2;
			m_fResults[ 2 ] = fFinishTime;
			strncpy( m_szPlayerName, ptxtPlayerName, 32 );

			if( fFinishTime > 0.0f )
			{
				char txtCP1[ 32 ];
				char txtCP2[ 32 ];
				char txtTimeString[ 32 ];
				fprintf( fp, "Stage Result for \"%s\": \n[CP1] = %s\n[CP2] = %s\n[Finish] = %s\n\n", ptxtPlayerName, 
					NPlugin::FormatTimeString( txtCP1, fCheckPoint1 ),
					NPlugin::FormatTimeString( txtCP2, fCheckPoint2 ),
					NPlugin::FormatTimeString( txtTimeString, fFinishTime ) );
			}
			else
			{
				fprintf( fp, "Stage Result for \"%s\": DNF.\n", ptxtPlayerName );
			}

			fclose( fp );
		}
	}

	//------------------------------------------------------------------------------------------------//
	/// Is called when a player passed a checkpoint 
	virtual void			CheckPoint			( float fCheckPointTime,
												  int   iCheckPointID,
												  const char* ptxtPlayerName )
	{
		FILE *fp = fopen( "rbrplugin.log", "at" );

		if( fp )
		{
			char txtTimeString[ 32 ];
			fprintf( fp, "[CheckPoint %d] : %s : %s\n", iCheckPointID, NPlugin::FormatTimeString( txtTimeString, fCheckPointTime ), ptxtPlayerName );
			fclose( fp );
		}

		m_pGame->SetColor( 1.0f, 1.0f, 1.0f, 1.0f );
		m_pGame->WriteGameMessage( "CHECKPOINT!!!!!!!!!!", 5.0f, 50.0f, 250.0f );
	}

private:
	IRBRGame*		m_pGame;
	int				m_iSelection;
	int				m_iSelections;
	int				m_iMap;
	DWORD			m_dTime;
	DWORD			m_dSec;
	DWORD			m_dsTime;
	DWORD			m_dsSec;
	char			m_szTemp[255];
	float			m_fResults[ 3 ];
	char			m_szPlayerName[ 32 ];
};

#endif