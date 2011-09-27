#ifndef __MODEL_DISPLAY_CONTROLLER_VOLUME_H__
#define __MODEL_DISPLAY_CONTROLLER_VOLUME_H__

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


#include "ModelDisplayController.h"



namespace caret {

    class Brain;
    
    /// Controls the display of a volumes.
    class ModelDisplayControllerVolume : public ModelDisplayController {
        
    public:
        ModelDisplayControllerVolume(Brain* brain);
        
        virtual ~ModelDisplayControllerVolume();
        
        Brain* getBrain();
        
    private:
        ModelDisplayControllerVolume(const ModelDisplayControllerVolume&);
        
        ModelDisplayControllerVolume& operator=(const ModelDisplayControllerVolume&);
        
    private:
        void initializeMembersModelDisplayControllerVolume();
        
    public:
        AString getNameForGUI(const bool includeStructureFlag) const;
        
        virtual AString getNameForBrowserTab() const;
        
    private:
        /** Brain which contains the volumes */
        Brain* brain;
    };

} // namespace

#endif // __MODEL_DISPLAY_CONTROLLER_VOLUME_H__
