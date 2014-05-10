/////////////////////////////////////////////////////////////////////////////
// Name:        paslapp.h
// Purpose:     
// Author:      James Burns
// Modified by: 
// Created:     17/12/2008 19:03:06
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _PASLAPP_H_
#define _PASLAPP_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/image.h"
#include "pasl.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
////@end control identifiers

/*!
 * PASLApp class declaration
 */

class PASLApp: public wxApp
{    
    DECLARE_CLASS( PASLApp )
    DECLARE_EVENT_TABLE()

public:
    /// Constructor
    PASLApp();

    void Init();

    /// Initialises the application
    virtual bool OnInit();

    /// Called on exit
    virtual int OnExit();

////@begin PASLApp event handler declarations

////@end PASLApp event handler declarations

////@begin PASLApp member function declarations

////@end PASLApp member function declarations

////@begin PASLApp member variables
////@end PASLApp member variables
};

/*!
 * Application instance declaration 
 */

////@begin declare app
DECLARE_APP(PASLApp)
////@end declare app

#endif
    // _PASLAPP_H_
