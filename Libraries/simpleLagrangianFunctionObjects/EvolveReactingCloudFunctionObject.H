//  OF-extend Revision: $Id$
/*---------------------------------------------------------------------------*\
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
    EvolveReactingCloudFunctionObject

Description
    Function object that creates and evolves a ReactingCloud at every timestep

SourceFiles
    EvolveReactingCloudFunctionObject.C

\*---------------------------------------------------------------------------*/

#ifndef EvolveReactingCloudFunctionObject_H
#define EvolveReactingCloudFunctionObject_H

#include "EvolveCloudFunctionObject.H"
#include "BasicReactingCloud.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class EvolveReactingCloudFunctionObject Declaration
\*---------------------------------------------------------------------------*/

template<class CloudType>
class EvolveReactingCloudFunctionObject
:
    public EvolveCloudFunctionObject<CloudType>
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    EvolveReactingCloudFunctionObject(const EvolveReactingCloudFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const EvolveReactingCloudFunctionObject&);

public:

    //- Runtime type information
    TypeName("EvolveReactingCloudFunctionObject");


    // Constructors

    //- Construct from components
    EvolveReactingCloudFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );


    // Member Functions

    //- start is called at the start of the time-loop
    virtual bool start();

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

#ifdef NoRepository
#   include "EvolveReactingCloudFunctionObject.C"
#endif


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //