
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QToolButton>
#include <QVBoxLayout>

#define __OVERLAY_VIEW_CONTROLLER_DECLARE__
#include "OverlayViewController.h"
#undef __OVERLAY_VIEW_CONTROLLER_DECLARE__

#include "CaretMappableDataFile.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "EventMapScalarDataColorMappingEditorShow.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "Overlay.h"
#include "WuQtUtilities.h"
#include "WuQGridLayoutGroup.h"
#include "WuQWidgetObjectGroup.h"

using namespace caret;


    
/**
 * \class caret::OverlayViewController 
 * \brief View Controller for an overlay.
 */

/**
 * Constructor.
 *
 * @param browserWindowIndex
 *    Index of browser window in which this view controller resides.
 * @param showTopHorizontalLine
 *    If true, display a horizontal line above the controls.
 * @param parent
 *    The parent widget.
 */
OverlayViewController::OverlayViewController(const Qt::Orientation orientation,
                                             QGridLayout* gridLayout,
                                             const int32_t browserWindowIndex,
                                             QObject* parent)
: QObject(parent)
{
    this->browserWindowIndex = browserWindowIndex;
    this->overlay = NULL;
    
    int minComboBoxWidth = 200;
    int maxComboBoxWidth = 400;
    if (orientation == Qt::Horizontal) {
        minComboBoxWidth = 50;
        maxComboBoxWidth = 100000;
    }

    /*
     * Enabled Check Box
     */
    const QString checkboxText = ((orientation == Qt::Horizontal) ? " " : " ");
    this->enabledCheckBox = new QCheckBox(checkboxText);
    QObject::connect(this->enabledCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(enabledCheckBoxStateChanged(int)));
    this->enabledCheckBox->setToolTip("Enables display of this overlay");
    
    /*
     * File Selection Check Box
     */
    this->fileComboBox = new QComboBox();
    this->fileComboBox->setMinimumWidth(minComboBoxWidth);
    this->fileComboBox->setMaximumWidth(maxComboBoxWidth);
    QObject::connect(this->fileComboBox, SIGNAL(activated(int)),
                     this, SLOT(fileComboBoxSelected(int)));
    this->fileComboBox->setToolTip("Selects file for this overlay");
    
    /*
     * Map Selection Check Box
     */
    this->mapComboBox = new QComboBox();
    this->mapComboBox->setMinimumWidth(minComboBoxWidth);
    this->mapComboBox->setMaximumWidth(maxComboBoxWidth);
    QObject::connect(this->mapComboBox, SIGNAL(activated(int)),
                     this, SLOT(mapComboBoxSelected(int)));
    this->mapComboBox->setToolTip("Selects map within the selected file");
    
    /*
     * Opacity double spin box
     */
    this->opacityDoubleSpinBox = new QDoubleSpinBox();
    this->opacityDoubleSpinBox->setMinimum(0.0);
    this->opacityDoubleSpinBox->setMaximum(1.0);
    this->opacityDoubleSpinBox->setSingleStep(0.10);
    this->opacityDoubleSpinBox->setDecimals(1);
    this->opacityDoubleSpinBox->setFixedWidth(50);
    QObject::connect(this->opacityDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(opacityDoubleSpinBoxValueChanged(double)));
    
    /*
     * ColorBar Tool Button
     */
    QIcon colorBarIcon;
    const bool colorBarIconValid = WuQtUtilities::loadIcon(":/overlay_colorbar.png",
                                                           colorBarIcon);
    this->colorBarAction = WuQtUtilities::createAction("CB", 
                                                       "Display color bar for this overlay", 
                                                       this, 
                                                       this, 
                                                       SLOT(colorBarActionTriggered(bool)));
    this->colorBarAction->setCheckable(true);
    if (colorBarIconValid) {
        this->colorBarAction->setIcon(colorBarIcon);
    }
    QToolButton* colorBarToolButton = new QToolButton();
    colorBarToolButton->setDefaultAction(this->colorBarAction);
    
    /*
     * Settings Tool Button
     */
    QIcon settingsIcon;
    const bool settingsIconValid = WuQtUtilities::loadIcon(":/overlay_wrench.png",
                                                           settingsIcon);

    this->settingsAction = WuQtUtilities::createAction("S", 
                                                          "Edit color mapping for this overlay", 
                                                          this, 
                                                          this, 
                                                          SLOT(settingsActionTriggered()));
    if (settingsIconValid) {
        this->settingsAction->setIcon(settingsIcon);
    }
    QToolButton* settingsToolButton = new QToolButton();
    settingsToolButton->setDefaultAction(this->settingsAction);
    
    /*
     * Add Tool Button
     */
    this->addAction = WuQtUtilities::createAction("Add", 
                                                  "Add an overlay", 
                                                  this, 
                                                  this, 
                                                  SLOT(addActionTriggered()));
    
    /*
     * Remove Tool Button
     */
    this->removeAction = WuQtUtilities::createAction("Remove", 
                                                  "Remove this overlay", 
                                                  this, 
                                                  this, 
                                                  SLOT(removeActionTriggered()));
    
    
    /*
     * Move Up Tool Button
     */
    this->moveUpAction = WuQtUtilities::createAction("Up", 
                                                     "Move this overlay up", 
                                                     this, 
                                                     this, 
                                                     SLOT(moveUpActionTriggered()));
    
    /*
     * Move Down Tool Button
     */
    this->moveDownAction = WuQtUtilities::createAction("Down", 
                                                     "Move this overlay down", 
                                                     this, 
                                                     this, 
                                                     SLOT(moveUpActionTriggered()));
    
    /*
     * Use layout group so that items can be shown/hidden
     */
    this->gridLayoutGroup = new WuQGridLayoutGroup(gridLayout, this);
    
    if (orientation == Qt::Horizontal) {
        int row = this->gridLayoutGroup->rowCount();
        this->gridLayoutGroup->addWidget(this->enabledCheckBox,
                                         row, 0,
                                         Qt::AlignHCenter);
        this->gridLayoutGroup->addWidget(settingsToolButton,
                                         row, 1,
                                         Qt::AlignHCenter);
        this->gridLayoutGroup->addWidget(colorBarToolButton,
                                         row, 2);
        this->gridLayoutGroup->addWidget(this->opacityDoubleSpinBox,
                                         row, 3);
        this->gridLayoutGroup->addWidget(this->fileComboBox,
                                         row, 4);
        this->gridLayoutGroup->addWidget(this->mapComboBox,
                                         row, 5);
        
    }
    else {
        QFrame* bottomHorizontalLineWidget = new QFrame();
        bottomHorizontalLineWidget->setLineWidth(0);
        bottomHorizontalLineWidget->setMidLineWidth(1);
        bottomHorizontalLineWidget->setFrameStyle(QFrame::HLine | QFrame::Raised);
        
        QLabel* fileLabel = new QLabel("File");
        QLabel* mapLabel = new QLabel("Map");
        
        int row = this->gridLayoutGroup->rowCount();
        this->gridLayoutGroup->addWidget(this->enabledCheckBox,
                                         row, 0,
                                         2, 1);
        this->gridLayoutGroup->addWidget(settingsToolButton,
                                         row, 1);
        this->gridLayoutGroup->addWidget(colorBarToolButton,
                                         row, 2);
        this->gridLayoutGroup->addWidget(fileLabel,
                              row, 3);
        this->gridLayoutGroup->addWidget(this->fileComboBox,
                              row, 4);
        
        row++;
        this->gridLayoutGroup->addWidget(this->opacityDoubleSpinBox,
                                         row, 1,
                                         1, 2,
                                         Qt::AlignCenter);
        this->gridLayoutGroup->addWidget(mapLabel,
                              row, 3);
        this->gridLayoutGroup->addWidget(this->mapComboBox,
                              row, 4);
        
        row++;
        this->gridLayoutGroup->addWidget(bottomHorizontalLineWidget,
                                         row, 0, 1, -1);
    }
    //this->setFixedHeight(this->sizeHint().height());
}

/**
 * Destructor.
 */
OverlayViewController::~OverlayViewController()
{
    
}

/**
 * Set the visiblity of this overlay view controller.
 */
void 
OverlayViewController::setVisible(bool visible)
{
    this->gridLayoutGroup->setVisible(visible);
}

/**
 * Called when a selection is made from the file combo box.
 * @parm indx
 *    Index of selection.
 */
void 
OverlayViewController::fileComboBoxSelected(int indx)
{
    void* pointer = this->fileComboBox->itemData(indx).value<void*>();
    CaretMappableDataFile* file = (CaretMappableDataFile*)pointer;
    overlay->setSelectionData(file, 0);
    
    this->updateViewController(this->overlay);
    
    this->updateUserInterfaceAndGraphicsWindow();    
}

/**
 * Called when a selection is made from the file combo box.
 * @parm indx
 *    Index of selection.
 */
void 
OverlayViewController::mapComboBoxSelected(int indx)
{
    const int32_t fileIndex = this->fileComboBox->currentIndex();
    void* pointer = this->fileComboBox->itemData(fileIndex).value<void*>();
    CaretMappableDataFile* file = (CaretMappableDataFile*)pointer;
    overlay->setSelectionData(file, indx);
    
    this->updateUserInterfaceAndGraphicsWindow();
}

/**
 * Called when enabled checkbox state is changed
 * @parm state
 *    New state (selection status).
 */
void 
OverlayViewController::enabledCheckBoxStateChanged(int state)
{
    const bool selected = (state == Qt::Checked);
    overlay->setEnabled(selected);
    
    this->updateUserInterfaceAndGraphicsWindow();
}

/**
 * Called when colorbar toolbutton is toggled.
 * @param status
 *    New status.
 */
void 
OverlayViewController::colorBarActionTriggered(bool status)
{
    this->overlay->setPaletteDisplayEnabled(status);
    
    this->updateUserInterfaceAndGraphicsWindow();
}

/**
 * Called when opacity value is changed.
 * @param value
 *    New value.
 */
void 
OverlayViewController::opacityDoubleSpinBoxValueChanged(double value)
{
    this->overlay->setOpacity(value);
    
    this->updateUserInterfaceAndGraphicsWindow();
}

/**
 * Called when the settings action is selected.
 */
void 
OverlayViewController::settingsActionTriggered()
{
    CaretMappableDataFile* mapFile;
    int32_t mapIndex = -1;
    this->overlay->getSelectionData(mapFile, 
                                    mapIndex);
    if (mapFile != NULL) {
        if (mapFile->isMappedWithPalette()) {
            if (mapFile != NULL) {
                EventMapScalarDataColorMappingEditorShow pcme(this->browserWindowIndex,
                                                          mapFile,
                                                          mapIndex);
                EventManager::get()->sendEvent(pcme.getPointer());
            }
        }
        else if (mapFile->isMappedWithLabelTable()) {
            QMessageBox::information(this->enabledCheckBox, "", "File is not mapped with palette");
        }
    }
}

/**
 * Update this view controller using the given overlay.
 * @param overlay
 *   Overlay that is used in this view controller.
 */
void 
OverlayViewController::updateViewController(Overlay* overlay)
{
    this->overlay = overlay;

//    this->widgetsGroup->blockAllSignals(true);
    
    this->fileComboBox->clear();
    this->mapComboBox->clear();
    
    /*
     * Get the selection information for the overlay.
     */
    std::vector<CaretMappableDataFile*> dataFiles;
    CaretMappableDataFile* selectedFile = NULL;
    AString selectedMapUniqueID = "";
    int32_t selectedMapIndex = -1;
    if (this->overlay != NULL) {
        this->overlay->getSelectionData(dataFiles, 
                                  selectedFile, 
                                  selectedMapUniqueID, 
                                  selectedMapIndex);
    }
    
    /*
     * Load the file selection combo box.
     */
    int32_t selectedFileIndex = -1;
    const int32_t numFiles = static_cast<int32_t>(dataFiles.size());
    for (int32_t i = 0; i < numFiles; i++) {
        CaretMappableDataFile* dataFile = dataFiles[i];
        
        AString dataTypeName = DataFileTypeEnum::toName(dataFile->getDataFileType());
        switch (dataFile->getDataFileType()) {
            case DataFileTypeEnum::CONNECTIVITY_DENSE:
                dataTypeName = "CONNECTIVITY";
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
                dataTypeName = "TIME_SERIES";
                break;
            default:
                break;
        }
        AString name = dataTypeName
        + " "
        + dataFile->getFileNameNoPath();
        this->fileComboBox->addItem(name,
                                    qVariantFromValue((void*)dataFile));
        if (dataFile == selectedFile) {
            selectedFileIndex = i;
        }
    }
    if (selectedFileIndex >= 0) {
        this->fileComboBox->setCurrentIndex(selectedFileIndex);
    }
    
    /*
     * Load the column selection combo box.
     */
    if (selectedFile != NULL) {
        const int32_t numMaps = selectedFile->getNumberOfMaps();
        for (int32_t i = 0; i < numMaps; i++) {
            this->mapComboBox->addItem(selectedFile->getMapName(i));
        }
        this->mapComboBox->setCurrentIndex(selectedMapIndex);
    }
    
    Qt::CheckState checkState = Qt::Unchecked;
    if (this->overlay != NULL) {
        if (this->overlay->isEnabled()) {
            checkState = Qt::Checked;
        }
    }
    
    this->enabledCheckBox->setCheckState(checkState);
    
    this->colorBarAction->blockSignals(true);
    this->colorBarAction->setChecked(overlay->isPaletteDisplayEnabled());
    this->colorBarAction->blockSignals(false);
    
    this->opacityDoubleSpinBox->blockSignals(true);
    this->opacityDoubleSpinBox->setValue(overlay->getOpacity());
    this->opacityDoubleSpinBox->blockSignals(false);
//    this->widgetsGroup->blockAllSignals(false);
//    this->widgetsGroup->setEnabled(this->overlay != NULL);
    if (selectedFile != NULL) {
        this->settingsAction->setEnabled(selectedFile->isMappedWithPalette());
    }
}

/**
 * Update graphics and GUI after selections made
 */
void 
OverlayViewController::updateUserInterfaceAndGraphicsWindow()
{
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->browserWindowIndex).getPointer());
}

/**
 * Called when the add action is triggered.
 */
void 
OverlayViewController::addActionTriggered()
{
    
}

/**
 * Called when the add action is triggered.
 */
void 
OverlayViewController::moveUpActionTriggered()
{
    
}

/**
 * Called when the add action is triggered.
 */
void 
OverlayViewController::moveDownActionTriggered()
{
    
}

/**
 * Called when the add action is triggered.
 */
void 
OverlayViewController::removeActionTriggered()
{
    
}




