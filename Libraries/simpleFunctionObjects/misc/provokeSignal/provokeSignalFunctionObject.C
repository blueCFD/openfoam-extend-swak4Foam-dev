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

Modifications
    This file has been modified using parts of the files located at
    "src/OSspecific/MSwindows/signals", from FSD blueCAPE's port of OpenFOAM
    for Windows (blueCFD-Core), which is derived from Symscape's
    (www.symscape.com) patches for porting OpenFOAM for Windows.

Contributors/Copyright:
    2014-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2018 FSD blueCAPE Lda <www.bluecape.com.pt>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "provokeSignalFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "swakTime.H"

#include <signal.h>

#include "HashSet.T.H"

#include "objectRegistry.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(provokeSignalFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        provokeSignalFunctionObject,
        dictionary
    );

    template<>
    const char* NamedEnum<Foam::provokeSignalFunctionObject::possibleSignals,SIGNAL_COUNT>::names[]=
    {
        "FPE",
        "SEGV",
        "INT",
        "TERM",
#if defined(WIN32) || defined(WIN64)
        "BREAK"
#else
        "QUIT",
        "USR1",
        "USR2"
#endif
    };
    const NamedEnum<provokeSignalFunctionObject::possibleSignals,SIGNAL_COUNT> provokeSignalFunctionObject::possibleSignalsNames_;

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

provokeSignalFunctionObject::provokeSignalFunctionObject
(
    const word &name,
    const Time& t,
    const dictionary& dict
)
:
    simpleFunctionObject(name,t,dict),
    signalToRaise_(
        possibleSignalsNames_[
            word(dict.lookup("signalToRaise"))
        ]
    ),
    timeToRaise_(
        readScalar(dict.lookup("timeToRaise"))
    ),
    raiseOnThisProc_(false)
{
    if(Pstream::parRun()) {
        HashSet<label> allProcs(
            labelList(
                dict.lookup("processorsToRaiseSignal")
            )
        );
        raiseOnThisProc_=allProcs.found(
            Pstream::myProcNo()
        );
    } else {
        if(!dict.found("processorsToRaiseSignal")) {
            WarningIn("provokeSignalFunctionObject::provokeSignalFunctionObject")
                << "No entry 'processorsToRaiseSignal' in " << dict.name()
                << nl << "Not needed now but needed in parallel runs" << endl;
        }
        raiseOnThisProc_=true;
    }

    if(raiseOnThisProc_) {
        Pout << endl;
        Pout << "Will raise signal " << possibleSignalsNames_[signalToRaise_]
            << " at time " << timeToRaise_
            << " and there is nothing you can do about it. "
            << "In fact the only possible use of this is testing" << endl;
        Pout << endl;
    }
}

void provokeSignalFunctionObject::writeSimple()
{
    if(
        raiseOnThisProc_
        &&
        obr_.time().value()>=timeToRaise_
    ) {
        Pout << endl
            << "The time has come. Raising "
            << possibleSignalsNames_[signalToRaise_] << endl
            << "It was nice knowing you"
            << endl;


       switch(signalToRaise_) {
            case sigFPE:
                raise(SIGFPE);
                break;
            case sigSEGV:
                raise(SIGSEGV);
                break;
            case sigINT:
                raise(SIGINT);
                break;
            case sigTERM:
                raise(SIGTERM);
                break;
#if defined(WIN32) || defined(WIN64)
            case sigBREAK:
                raise(SIGBREAK);
                break;
#else
            case sigQUIT:
                raise(SIGQUIT);
                break;
            case sigUSR1:
                raise(SIGUSR1);
                break;
            case sigUSR2:
                raise(SIGUSR2);
                break;
#endif
            default:
                FatalErrorIn("provokeSignalFunctionObject::writeSimple()")
                    << "Unimplemented signal "
                        << possibleSignalsNames_[signalToRaise_]
                        << endl;
        }
        Pout << "We signaled (this should not be seen)" << endl;
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

// ************************************************************************* //
