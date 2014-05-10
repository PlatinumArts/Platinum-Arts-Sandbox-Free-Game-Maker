/////////////////////////////////////////////////////////////////////////////
// Name:        pasl.h
// Purpose:     
// Author:      James Burns
// Modified by: 
// Created:     17/12/2008 19:03:58
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _PASL_H_
#define _PASL_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/frame.h"
#include "wx/notebook.h"
////@end includes
#include "wx/tokenzr.h"
#include "wx/process.h"
#include "wx/dir.h"
#include "wx/file.h"
#include "wx/utils.h"
#include "wx/dc.h"
#include "wx/dcclient.h"

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_PASL 10000
#define ID_PANEL 10001
#define ID_COMBOBOX 10002
#define ID_COMBOBOX1 10003
#define ID_COMBOBOX2 10004
#define ID_COMBOBOX6 10021
#define ID_NOTEBOOK 10005
#define ID_PANEL1 10006
#define ID_BUTTON 10011
#define ID_PANEL2 10007
#define ID_BUTTON1 10012
#define ID_PANEL3 10008
#define ID_BUTTON2 10013
#define ID_PANEL4 10009
#define ID_TEXTCTRL 10016
#define ID_COMBOBOX3 10014
#define ID_COMBOBOX4 10015
#define ID_TEXTCTRL1 10017
#define ID_COMBOBOX5 10018
#define ID_COMBOBOX7 10024
#define ID_BUTTON3 10019
#define ID_PANEL5 10010
#define ID_TEXTCTRL2 10020
#define ID_TEXTCTRL3 10022
#define ID_COMBOBOX8 10025
#define ID_BUTTON4 10023
#define ID_PANEL6 10026
#define ID_BUTTON5 10027
#define SYMBOL_PASL_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_PASL_TITLE _("SandBox Launcher")
#define SYMBOL_PASL_IDNAME ID_PASL
#define SYMBOL_PASL_SIZE wxSize(330, 450)
#define SYMBOL_PASL_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * PASL class declaration
 */

class PASL: public wxFrame
{    
    DECLARE_CLASS( PASL )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    PASL();
    PASL( wxWindow* parent, wxWindowID id = SYMBOL_PASL_IDNAME, const wxString& caption = SYMBOL_PASL_TITLE, const wxPoint& pos = SYMBOL_PASL_POSITION, const wxSize& size = SYMBOL_PASL_SIZE, long style = SYMBOL_PASL_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_PASL_IDNAME, const wxString& caption = SYMBOL_PASL_TITLE, const wxPoint& pos = SYMBOL_PASL_POSITION, const wxSize& size = SYMBOL_PASL_SIZE, long style = SYMBOL_PASL_STYLE );

    /// Destructor
    ~PASL();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin PASL event handler declarations

    /// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX
    void OnResBoxSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX1
    void OnShaderBoxSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX2
    void OnVSyncSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX6
    void OnMapBoxSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON
    void OnFPSLaunch( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON1
    void OnSSPLaunch( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON2
    void OnRPGLaunch( wxCommandEvent& event );

    /// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TEXTCTRL
    void OnServerNameTextUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX3
    void OnPlayerBoxSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX4
    void OnMasterBoxSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TEXTCTRL1
    void OnServerPasswordTextUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX5
    void OnDedBoxSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX7
    void OnModeBoxSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON3
    void OnServerLaunch( wxCommandEvent& event );

    /// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TEXTCTRL2
    void OnClientIPTextUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TEXTCTRL3
    void OnClientPasswordTextUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX8
    void OnCModeBoxSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON4
    void OnClientLaunch( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON5
    void OnMovieCubeButtonClick( wxCommandEvent& event );

////@end PASL event handler declarations

    void GetError(wxProcess*);

////@begin PASL member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end PASL member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin PASL member variables
    wxComboBox* ResBox;
    wxComboBox* ShaderBox;
    wxComboBox* VSyncBox;
    wxComboBox* MapBox;
    wxStaticBitmap* MapPrev;
    wxButton* FPSLaunch;
    wxButton* SSPLaunch;
    wxButton* RPGLaunch;
    wxTextCtrl* ServerName;
    wxComboBox* PlayerBox;
    wxComboBox* MasterBox;
    wxTextCtrl* ServerPassword;
    wxComboBox* DedBox;
    wxComboBox* ModeBox;
    wxButton* ServerLaunch;
    wxTextCtrl* ClientIP;
    wxTextCtrl* ClientPassword;
    wxComboBox* CModeBox;
    wxButton* ClientLaunch;
    wxButton* MovieCubeButton;
////@end PASL member variables
    wxString RunMsg;
    wxString ResWidth, ResHeight, CurRes;
    //wxString WinExe, LinExe, CurExe;
    wxString WinMap, LinMap, CurMap;
    //wxString WinFPS, WinRPG, WinSSP;
    //wxString LinFPSc, LinRPGc, LinSSPc;
    //wxString LinFPSs, LinRPGs, LinSSPs;
    wxString CurFPSc, CurRPGc, CurSSPc;
    wxString CurFPSs, CurRPGs, CurSSPs;
    wxString CurMovie;
    wxString Shader, VSync, Map;
    wxString ServerN, ServerP, Players, Master, Ded, Mode, CMode;
    wxString CIP, CPassword, Client;
    wxProcess* SBProc;
    wxProcess* SBSProc;
    wxImage PrevImg;
    wxString MapDir, StuffDir;
    int pid, spid;
    char msg[50];
    wxInputStream* err;
};

#endif
    // _PASL_H_
