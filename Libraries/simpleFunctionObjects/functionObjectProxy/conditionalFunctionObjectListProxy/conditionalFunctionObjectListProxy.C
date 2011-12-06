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

\*---------------------------------------------------------------------------*/

#include "conditionalFunctionObjectListProxy.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "Time.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(conditionalFunctionObjectListProxy, 0);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

conditionalFunctionObjectListProxy::conditionalFunctionObjectListProxy
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    functionObjectListProxy(
        name,
        t,
        dict
    )
{
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool conditionalFunctionObjectListProxy::execute()
{
    if(writeDebug()) {
        Info << this->name() << " condition::execute() condition: " 
            << condition() << endl;
    }

    if(condition()) {
        return functionObjectListProxy::execute();
    } else {
        return true;
    }
}

bool conditionalFunctionObjectListProxy::start()
{
    if(writeDebug()) {
        Info << this->name() << " condition::start() condition: " 
            << condition() << endl;
    }

    if(condition()) {
        return functionObjectListProxy::start();
    } else {
        return true;
    }
}

bool conditionalFunctionObjectListProxy::end()
{
    if(writeDebug()) {
        Info << this->name() << " condition::end() condition: " 
            << condition() << endl;
    }

    if(condition()) {
        return functionObjectListProxy::end();
    } else {
        return true;
    }
}

bool conditionalFunctionObjectListProxy::read(const dictionary& dict)
{
    if(writeDebug()) {
        Info << this->name() << " condition::read() condition: " 
            << condition() << endl;
    }

    if(condition()) {
        return functionObjectListProxy::read(dict);
    } else {
        return true;
    }
}

} // namespace Foam

// ************************************************************************* //