/*---------------------------------------------------------------------------*\
 ##   ####  ######     |
 ##  ##     ##         | Copyright: ICE Stroemungsfoschungs GmbH
 ##  ##     ####       |
 ##  ##     ##         | http://www.ice-sf.at
 ##   ####  ######     |
-------------------------------------------------------------------------------
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright  held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is based on OpenFOAM.

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Class
    writeOldTimesOnSignalFunctionObject

Description
    This function object tries to record old fields and writes them in
    case of a Signal

SourceFiles
    writeOldTimesOnSignalFunctionObject.C

Modifications
    This file has been modified using parts of the files located at
    "src/OSspecific/MSwindows/signals", from FSD blueCAPE's port of OpenFOAM
    for Windows (blueCFD-Core), which is derived from Symscape's
    (www.symscape.com) patches for porting OpenFOAM for Windows.

Contributors/Copyright:
    2014-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2014-2018 FSD blueCAPE Lda <www.bluecape.com.pt>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef writeOldTimesOnSignalFunctionObject_H
#define writeOldTimesOnSignalFunctionObject_H

#include "functionObject.H"
#include "dictionary.H"
#include "TimeCloneList.H"
#include "DynamicList.T.H"

#include <signal.h>

#include "swak.H"

#if defined(WIN32) || defined(WIN64)
#define SIG_ACTION_TYPE __p_sig_fn_t
#else
#define SIG_ACTION_TYPE struct sigaction
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class writeOldTimesOnSignalFunctionObject Declaration
\*---------------------------------------------------------------------------*/

    class objectRegistry;

class writeOldTimesOnSignalFunctionObject
:
    public functionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    writeOldTimesOnSignalFunctionObject(const writeOldTimesOnSignalFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const writeOldTimesOnSignalFunctionObject&);

    TimeCloneList times_;

    const Time &theTime_;

    class SignalHandlerInfo {
    protected:
        SIG_ACTION_TYPE oldAction_;
        word name_;
        int sig_;
        bool set_;
    public:
        SignalHandlerInfo(
            word name,
            int sig
        );

        SignalHandlerInfo()
            :
            name_("nix"),
            sig_(-1),
            set_(false)
            {}

        void resetHandler();

        ~SignalHandlerInfo() {};

        int sig() const
           { return sig_; }

        bool set() const
           { return set_; }

    };

    static void sigHandler(int);

    static writeOldTimesOnSignalFunctionObject *singleton_;

    DynamicList<SignalHandlerInfo> handlers_;

    bool writeCurrent_;

    scalar sleepSecondsBeforeReraising_;

    bool sigFPE_;
    bool sigSEGV_;
    bool sigINT_;
    bool sigTERM_;
    bool sigQUIT_;

#if !(defined(WIN32) || defined(WIN64))
    bool sigUSR1_;
    bool sigUSR2_;
#endif

    bool alreadyDumped_;

    bool itWasMeWhoReraised_;

public:

    //- Runtime type information
    TypeName("writeOldTimesOnSignal");


    // Constructors

    //- Construct from components
    writeOldTimesOnSignalFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );


    // Member Functions

    //- start is called at the start of the time-loop
    virtual bool start();

    //- execute is called at each ++ or += of the time-loop
#ifdef FOAM_FUNCTIONOBJECT_EXECUTE_HAS_NO_FORCE
    virtual bool execute();
#else
    virtual bool execute(const bool forceWrite);

#ifdef FOAM_FUNCTIONOBJECT_HAS_SEPARATE_WRITE_METHOD_AND_NO_START
    virtual bool execute() {
        return execute(false);
    }

    virtual bool write() {
        return execute(true);
    }
#endif
#endif


    //- Read and set the function object if it's data has changed
    virtual bool read(const dictionary& dict);

#ifdef FOAM_FUNCTIONOBJECT_NEEDS_UPDATEMESH

    virtual void updateMesh(const mapPolyMesh&) {
    }

    virtual void movePoints(const polyMesh&) {
    }
#endif

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
