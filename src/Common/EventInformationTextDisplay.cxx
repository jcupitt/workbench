/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include "EventInformationTextDisplay.h"

using namespace caret;

/**
 * Construct an event for display text in the information windows.
 * @param text
 *   Text that will be displayed.
 */
EventInformationTextDisplay::EventInformationTextDisplay(const AString& text)
: Event(EventTypeEnum::EVENT_INFORMATION_TEXT_DISPLAY)
{
    this->text = text;
    this->important = true;
}

/**
 *  Destructor.
 */
EventInformationTextDisplay::~EventInformationTextDisplay()
{
    
}

/**
 * Set the message as not important so that the toolbox
 * does NOT switch to the information panel.
 */
EventInformationTextDisplay*
EventInformationTextDisplay::setNotImportant()
{
    this->important = false;
    return this;
}

/**
 * @return Is this message important.  If so,
 * the Toolbox will switch to the information 
 * display.
 */
bool 
EventInformationTextDisplay::isImportant() const
{
    return this->important;
}


/**
 * @return  The text for display in the information windows.
 */
AString 
EventInformationTextDisplay::getText() const
{
    return this->text; 
}


