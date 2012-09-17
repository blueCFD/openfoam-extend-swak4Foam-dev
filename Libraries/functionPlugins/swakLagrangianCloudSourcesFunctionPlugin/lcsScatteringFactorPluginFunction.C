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
    \\  /    A nd           | Copyright (C) 1991-2008 OpenCFD Ltd.
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

 ICE Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "lcsScatteringFactorPluginFunction.H"

#include "addToRunTimeSelectionTable.H"

#include "basicThermoCloud.H"
#include "BasicReactingCloud.H"
#include "BasicReactingMultiphaseCloud.H"

namespace Foam {

defineTypeNameAndDebug(lcsScatteringFactorPluginFunction,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction,lcsScatteringFactorPluginFunction , name, lcsScatteringFactor);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

lcsScatteringFactorPluginFunction::lcsScatteringFactorPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    LagrangianCloudSourcePluginFunction(
        parentDriver,
        name,
        "volScalarField"
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void lcsScatteringFactorPluginFunction::doEvaluation()
{
    autoPtr<volScalarField> psigmap;

    // pick up the first fitting class
    castAndCall(psigmap,volScalarField,basicThermoCloud,thermoCloud,sigmap());
    castAndCall(psigmap,volScalarField,constThermoReactingCloud,reactingCloud,sigmap());
    castAndCall(psigmap,volScalarField,thermoReactingCloud,reactingCloud,sigmap());
    castAndCall(psigmap,volScalarField,icoPoly8ThermoReactingCloud,reactingCloud,sigmap());
    castAndCall(psigmap,volScalarField,constThermoReactingMultiphaseCloud,reactingMultiphaseCloud,sigmap());
    castAndCall(psigmap,volScalarField,thermoReactingMultiphaseCloud,reactingMultiphaseCloud,sigmap());
    castAndCall(psigmap,volScalarField,icoPoly8ThermoReactingMultiphaseCloud,reactingMultiphaseCloud,sigmap());

    noCloudFound(psigmap);

    result().setObjectResult(psigmap);
}

// * * * * * * * * * * * * * * * Concrete implementations * * * * * * * * * //


} // namespace

// ************************************************************************* //