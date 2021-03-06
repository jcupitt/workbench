#ifndef __BALSA_STUDY_INFORMATION_H__
#define __BALSA_STUDY_INFORMATION_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/


#include "CaretObject.h"

class QJsonObject;

namespace caret {

    class BalsaStudyInformation : public CaretObject {
        
    public:
        BalsaStudyInformation(const QJsonObject& jsonObject);
        
        BalsaStudyInformation();
        
        BalsaStudyInformation(const AString& studyID,
                              const AString& studyTitle);
        
        virtual ~BalsaStudyInformation();
        
        BalsaStudyInformation(const BalsaStudyInformation& obj);

        BalsaStudyInformation& operator=(const BalsaStudyInformation& obj);
        
        bool operator<(const BalsaStudyInformation& obj) const;
        
        bool isEmpty() const;
        
        AString getStudyID() const;
        
        void setStudyID(const AString& studyID);
        
        AString getStudyTitle() const;
        
        void setStudyTitle(const AString& studyTitle);

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperBalsaStudyInformation(const BalsaStudyInformation& obj);

        /** study identifier generated by BALSA*/
        AString m_studyID;
        
        /** study title*/
        AString m_studyTitle;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BALSA_STUDY_INFORMATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BALSA_STUDY_INFORMATION_DECLARE__

} // namespace
#endif  //__BALSA_STUDY_INFORMATION_H__
