/////////////////////////////////////////////////////////////////////////////
// Name:        paslapp.cpp
// Purpose:     
// Author:      James Burns
// Modified by: 
// Created:     17/12/2008 19:03:06
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "paslapp.h"

////@begin XPM images
////@end XPM images


/*!
 * Application instance implementation
 */

////@begin implement app
IMPLEMENT_APP( PASLApp )
////@end implement app


/*!
 * PASLApp type definition
 */

IMPLEMENT_CLASS( PASLApp, wxApp )


/*!
 * PASLApp event table definition
 */

BEGIN_EVENT_TABLE( PASLApp, wxApp )

////@begin PASLApp event table entries
////@end PASLApp event table entries

END_EVENT_TABLE()


/*!
 * Constructor for PASLApp
 */

PASLApp::PASLApp()
{
    Init();
}


/*!
 * Member initialisation
 */

void PASLApp::Init()
{
////@begin PASLApp member initialisation
////@end PASLApp member initialisation
}

/*!
 * Initialisation for PASLApp
 */

bool PASLApp::OnInit()
{    
////@begin PASLApp initialisation
	// Remove the comment markers above and below this block
	// to make permanent changes to the code.

#if wxUSE_XPM
	wxImage::AddHandler(new wxXPMHandler);
#endif
#if wxUSE_LIBPNG
	wxImage::AddHandler(new wxPNGHandler);
#endif
#if wxUSE_LIBJPEG
	wxImage::AddHandler(new wxJPEGHandler);
#endif
#if wxUSE_GIF
	wxImage::AddHandler(new wxGIFHandler);
#endif
	PASL* mainWindow = new PASL( NULL );
	mainWindow->Show(true);
////@end PASLApp initialisation

    return true;
}


/*!
 * Cleanup for PASLApp
 */

int PASLApp::OnExit()
{    
////@begin PASLApp cleanup
	return wxApp::OnExit();
////@end PASLApp cleanup
}

