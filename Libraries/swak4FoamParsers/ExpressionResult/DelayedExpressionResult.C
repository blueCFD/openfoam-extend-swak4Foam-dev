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

#include "DelayedExpressionResult.H"
#include "vector.H"
#include "tensor.H"
#include "symmTensor.H"
#include "sphericalTensor.H"

#include <cassert>

namespace Foam {

defineTypeNameAndDebug(DelayedExpressionResult,0);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

DelayedExpressionResult::DelayedExpressionResult()
:
    ExpressionResult(),
    name_("None"),
    startupValueExpression_(""),
    settingResult_(),
    storeInterval_(1),
    delay_(10)
{
}

DelayedExpressionResult::DelayedExpressionResult(
    const DelayedExpressionResult &rhs
)
:
    ExpressionResult(rhs),
    name_(rhs.name_),
    startupValueExpression_(rhs.startupValueExpression_),
    settingResult_(rhs.settingResult_),
    storedValues_(rhs.storedValues_),
    storeInterval_(rhs.storeInterval_),
    delay_(rhs.delay_)
{
}

DelayedExpressionResult::DelayedExpressionResult(const dictionary &dict)
:
    ExpressionResult(dict.subOrEmptyDict("value")),
    name_(dict.lookup("name")),
    startupValueExpression_(dict.lookup("startupValue")),
    storeInterval_(readScalar(dict.lookup("storeInterval"))),
    delay_(readScalar(dict.lookup("delay")))
{
    if(dict.found("storedValues")) {
        storedValues_=DLList<ValueAtTime>(
            dict.lookup("storedValues")
        );
    }
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

DelayedExpressionResult::~DelayedExpressionResult()
{
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

void DelayedExpressionResult::operator=(const DelayedExpressionResult& rhs)
{
    // Check for assignment to self
    if (this == &rhs)
    {
        FatalErrorIn("DelayedExpressionResult::operator=(const DelayedExpressionResult&)")
            << "Attempted assignment to self"
            << exit(FatalError);
    }

    static_cast<ExpressionResult&>(*this)=rhs;

    name_=rhs.name_;
    startupValueExpression_=rhs.startupValueExpression_;
    settingResult_=rhs.settingResult_;
    storedValues_=rhs.storedValues_;
    storeInterval_=rhs.storeInterval_;
    delay_=rhs.delay_;
}

void DelayedExpressionResult::operator=(const ExpressionResult& rhs)
{
    if(debug) {
        Pout << "Setting " << name() << " with " << rhs << endl;
    }
    settingResult_=rhs;
}

bool DelayedExpressionResult::updateReadValue(const scalar &time)
{
    if(debug) {
        Pout << "Updating delayed " << name() << " for t=" << time << endl;
    }

    if(storedValues_.size()<=0) {
        if(debug) {
            Pout << "Delayed variable " << name() << " has no stored values"
                << endl;
        }
        // no shirt, no data, no service
        return false;
    }
    const ValueAtTime &first=storedValues_.first();
    if(debug) {
        Pout << "First stored value for t=" << first.first()
            << " -> " << bool(first.first()>(time-delay_)) << endl;
    }

    if(first.first()>(time-delay_)) {
        if(debug) {
            Pout << "No matching data yet" << endl;
        }
        // we have no matching data yet
        return false;
    }
    if(storedValues_.size()<=1) {
        FatalErrorIn("DelayedExpressionResult::updateReadValue(const scalar &time)")
            << "Only one stored value yet at time " << time
                << " for delayedVariable " << name() << endl
                << "Check the values for the interval " << storeInterval_
                << " and delay " << delay_ << endl
                << "Probably the interval is to big"
                << endl
                << exit(FatalError);

    }
    DLList<ValueAtTime>::const_iterator current=storedValues_.cbegin();
    DLList<ValueAtTime>::const_iterator next=current;
    ++next;
    while(next!=storedValues_.end()) {
        if(debug) {
            Pout << "Basepoints  t=" << current().first()
                << " and t=" << next().first() << " - " << flush;
        }
        if(
            (time-delay_)>=current().first()
            &&
            (time-delay_)<=next().first()
        ) {
            if(debug) {
                Pout << " fit" << endl;
            }
            break;
        }
        if(debug) {
            Pout << " next" << endl;
        }
        current=next;
        ++next;
    }

    assert(current!=next);
    assert((time-delay_)>=current().first() && (time-delay_)<=next().first());

    const scalar step=next().first()-current().first();
    const scalar f=((time-delay_)-current().first())/step;

    if(debug) {
        Pout << "Using f=" << f << " (step " << step << ")" << endl;
    }

    ExpressionResult val((1-f)*current().second()+f*next().second());
    if(debug) {
        Pout << "New value " << val << endl;
    }

    setReadValue(val);

    return true;
}

void DelayedExpressionResult::setReadValue(const ExpressionResult &val)
{
    static_cast<ExpressionResult&>(*this)=val;
}

void DelayedExpressionResult::storeValue(const scalar &time)
{
    if(debug) {
        Pout << "Storing value " << name() << " at t="
            << time << endl;
    }
    bool append=false;

    if(storedValues_.size()<=0) {
        if(debug) {
            Pout << "First value - appending" << endl;
        }
        append=true;
    } else {
        const scalar lastTime=storedValues_.last().first();
        if(debug) {
            Pout << "Last stored time t=" << lastTime << flush;
        }
        if(lastTime+SMALL>=time) {
            // basically the same. Fall through to replace
            if(debug) {
                Pout << " Almost same - replacing" << endl;
            }
        } else if((time-lastTime)>=0.999*storeInterval_) {
            if(debug) {
                Pout << " Interval " << storeInterval_
                    << "passed - appending"<< endl;
            }
            append=true;
        } else {
            if(debug) {
                Pout << " Middle. Nothing done" << endl;
            }
            // we're in the middle. Forget it
            return;
        }
    }

    if(append) {
        scalar oldLastTime=-1;
        bool notEmpty=false;

        if(storedValues_.size()>0) {
            oldLastTime=storedValues_.last().first();
            notEmpty=true;
        }

        if(debug) {
            Pout << "Appending " << settingResult_ << endl;
        }
        storedValues_.append(ValueAtTime(time,settingResult_));
        if(notEmpty) {
            while(
                oldLastTime-storedValues_.first().first()
                >=
                delay_
            ) {
                if(debug) {
                    Pout << "Removing t=" << storedValues_.first().first()
                        << " because it is older than " << delay_ << endl;
                }
                storedValues_.removeHead();
            }
        }
    } else {
        if(debug) {
            Pout << "Replacing with " << settingResult_ << endl;
        }
        storedValues_.last().second()=settingResult_;
    }
}

// * * * * * * * * * * * * * * * Friend Functions  * * * * * * * * * * * * * //

Ostream & operator<<(Ostream &out,const DelayedExpressionResult &data)
{
    out << token::BEGIN_BLOCK << endl;

    out.writeKeyword("name");
    out << word(data.name_) << token::END_STATEMENT << nl;

    out.writeKeyword("startupValue");
    out << data.startupValueExpression_ << token::END_STATEMENT << nl;

    if(data.settingResult_.valueType()!="Void") {
        out.writeKeyword("settingResult");
        out << data.settingResult_ << token::END_STATEMENT << nl;
    }

    out.writeKeyword("storedValues");
    out << data.storedValues_ << token::END_STATEMENT << nl;

    out.writeKeyword("storeInterval");
    out << data.storeInterval_ << token::END_STATEMENT << nl;

    out.writeKeyword("delay");
    out << data.delay_ << token::END_STATEMENT << nl;

    out.writeKeyword("value");
    out << static_cast<const ExpressionResult &>(data);

    out << token::END_BLOCK << endl;

    return out;
}

Istream & operator>>(Istream &in,DelayedExpressionResult &data)
{
    dictionary dict(in);

    data=DelayedExpressionResult(dict);

    return in;
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

bool operator!=(const DelayedExpressionResult &,const DelayedExpressionResult &)
{
    return false;
}

} // namespace

// ************************************************************************* //