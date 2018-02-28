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
    provokeSignalFunctionObject

Description
    Raises a specified signal at a specified time. For parallel runs also
    the processors on which to raise the signal can be specified.

    The only purpose of this function object is the testing of the error
    handling

SourceFiles
    provokeSignalFunctionObject.C

Modifications
    This file has been modified using parts of the files located at
    "src/OSspecific/MSwindows/signals", from FSD blueCAPE's port of OpenFOAM
    for Windows (blueCFD-Core), which is derived from Symscape's
    (www.symscape.com) patches for porting OpenFOAM for Windows.

Contributors/Copyright:
    2014, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2018 FSD blueCAPE Lda <www.bluecape.com.pt>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef provokeSignalFunctionObject_H
#define provokeSignalFunctionObject_H

#include "simpleFunctionObject.H"

#include "NamedEnum.T.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class provokeSignalFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class provokeSignalFunctionObject
:
    public simpleFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    provokeSignalFunctionObject(const provokeSignalFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const provokeSignalFunctionObject&);

public:

    enum possibleSignals {
        sigFPE,
        sigSEGV,
        sigINT,
        sigTERM,
#if defined(WIN32) || defined(WIN64)
        sigBREAK
#define SIGNAL_COUNT 5
#else
        sigQUIT,
        sigUSR1,
        sigUSR2
#define SIGNAL_COUNT 7
#endif
    };

private:

    possibleSignals signalToRaise_;

    scalar timeToRaise_;

    bool raiseOnThisProc_;

    static const NamedEnum<possibleSignals,SIGNAL_COUNT> possibleSignalsNames_;

public:

    //- Runtime type information
    TypeName("provokeSignal");


    // Constructors

    //- Construct from components
    provokeSignalFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    void writeSimple();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
