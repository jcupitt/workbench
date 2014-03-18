
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QToolButton>

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_ATTRIBUTES_DECLARE__
#include "BrainBrowserWindowToolBarChartAttributes.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_ATTRIBUTES_DECLARE__

#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EnumComboBoxTemplate.h"
#include "ChartMatrixDisplayProperties.h"
#include "ChartMatrixScaleModeEnum.h"
#include "ChartModelDataSeries.h"
#include "ChartModelTimeSeries.h"
#include "ChartableMatrixFileSelectionModel.h"
#include "ChartableMatrixInterface.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "ModelChart.h"
#include "WuQFactory.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarChartAttributes 
 * \brief Controls for chart attributes.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parentToolBar
 *   The parent toolbar.
 */
BrainBrowserWindowToolBarChartAttributes::BrainBrowserWindowToolBarChartAttributes(BrainBrowserWindowToolBar* parentToolBar)
: BrainBrowserWindowToolBarComponent(parentToolBar)
{
    m_cartesianChartAttributesWidget = new CartesianChartAttributesWidget(this);
    
    m_matrixChartAttributesWidget = new MatrixChartAttributesWidget(this);
    
    m_stackedWidget = new QStackedWidget();
    m_stackedWidget->addWidget(m_cartesianChartAttributesWidget);
    m_stackedWidget->addWidget(m_matrixChartAttributesWidget);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
    layout->addWidget(m_stackedWidget);
    layout->addStretch();
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarChartAttributes::~BrainBrowserWindowToolBarChartAttributes()
{
}

/**
 * Update content of this tool bar component.
 *
 * @param browserTabContent
 *     Content of the browser tab.
 */
void
BrainBrowserWindowToolBarChartAttributes::updateContent(BrowserTabContent* /*browserTabContent*/)
{
    ChartModelCartesian* cartesianChart = getCartesianChart();
    ChartMatrixDisplayProperties* matrixProperties = getChartableMatrixDisplayProperties();
    
    if (cartesianChart != NULL) {
        m_stackedWidget->setCurrentWidget(m_cartesianChartAttributesWidget);
        m_cartesianChartAttributesWidget->updateContent();
        m_stackedWidget->setEnabled(true);
    }
    else if (matrixProperties) {
        m_stackedWidget->setCurrentWidget(m_matrixChartAttributesWidget);
        m_matrixChartAttributesWidget->updateContent();
        m_stackedWidget->setEnabled(true);
    }
    else {
        m_stackedWidget->setEnabled(false);
    }
}

/**
 * @return Cartesian chart in this widget.  Returned value will
 * be NULL if the chart (or no chart) is not a Cartesian Chart.
 */
ChartModelCartesian*
BrainBrowserWindowToolBarChartAttributes::getCartesianChart()
{
    ChartModelCartesian* cartesianChart = NULL;
    
    BrowserTabContent* browserTabContent = getTabContentFromSelectedTab();
    if (browserTabContent != NULL) {
        ModelChart* modelChart = browserTabContent->getDisplayedChartModel();
        
        if (modelChart != NULL) {
            const int32_t tabIndex = browserTabContent->getTabNumber();
            const ChartDataTypeEnum::Enum chartType = modelChart->getSelectedChartDataType(tabIndex);
            
            switch (chartType) {
                case ChartDataTypeEnum::CHART_DATA_TYPE_INVALID:
                    break;
                case ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                    break;
                case ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES:
                    cartesianChart = modelChart->getSelectedDataSeriesChartModel(tabIndex);  //dynamic_cast<ChartModelDataSeries*>(chart);
                    break;
                case ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES:
                    cartesianChart = modelChart->getSelectedTimeSeriesChartModel(tabIndex);  //dynamic_cast<ChartModelDataSeries*>(chart);
                    break;
            }
        }
    }
    
    return cartesianChart;
}

/**
 * @return Matrix chart interface in this widget.  Returned value will
 * be NULL if the chart (or no chart) is not a Matrix Chart.
 */
ChartMatrixDisplayProperties*
BrainBrowserWindowToolBarChartAttributes::getChartableMatrixDisplayProperties()
{
    ChartMatrixDisplayProperties* matrixDisplayProperties = NULL;

    BrowserTabContent* browserTabContent = getTabContentFromSelectedTab();
    if (browserTabContent != NULL) {
        ModelChart* modelChart = browserTabContent->getDisplayedChartModel();
        
        if (modelChart != NULL) {
            const int32_t tabIndex = browserTabContent->getTabNumber();
            const ChartDataTypeEnum::Enum chartType = modelChart->getSelectedChartDataType(tabIndex);
            
            switch (chartType) {
                case ChartDataTypeEnum::CHART_DATA_TYPE_INVALID:
                    break;
                case ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                {
                    ChartableMatrixInterface* matrixInterface = modelChart->getChartableMatrixFileSelectionModel(tabIndex)->getSelectedFile();
                    if (matrixInterface != NULL) {
                        matrixDisplayProperties = matrixInterface->getChartMatrixDisplayProperties(tabIndex);
                    }
                }
                    break;
                case ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES:
                    break;
                case ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES:
                    break;
            }
        }
    }
    
    return matrixDisplayProperties;
}

/**
 * Update the graphics.
 */
void
BrainBrowserWindowToolBarChartAttributes::updateGraphics()
{
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}




/* ===========================================================================*/

/**
 * \class caret::CartesianChartAttributesWidget
 * \brief Controls for cartesian chart attributes.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param brainBrowserWindowToolBarChartAttributes
 *   The parent attributes widget.
 */
CartesianChartAttributesWidget::CartesianChartAttributesWidget(BrainBrowserWindowToolBarChartAttributes* brainBrowserWindowToolBarChartAttributes)
{
    m_brainBrowserWindowToolBarChartAttributes = brainBrowserWindowToolBarChartAttributes;

    QLabel* cartesianLineWidthLabel = new QLabel("Line width ");
    m_cartesianLineWidthDoubleSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0,
                                                                                                       10000.0,
                                                                                                       0.1,
                                                                                                       1,
                                                                                                       this,
                                                                                                       SLOT(cartesianLineWidthValueChanged(double)));
    m_cartesianLineWidthDoubleSpinBox->setFixedWidth(65);
    
    QWidget* gridWidget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(gridWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 0, 0);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(0, 100);
    gridLayout->addWidget(cartesianLineWidthLabel, 0, 0);
    gridLayout->addWidget(m_cartesianLineWidthDoubleSpinBox, 0, 1);
    
    gridWidget->setFixedSize(gridWidget->sizeHint());
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(gridWidget);
    layout->addStretch();
}

/**
 * Destructor.
 */
CartesianChartAttributesWidget::~CartesianChartAttributesWidget()
{
    
}

/**
 * Update the content of this widget.
 */
void
CartesianChartAttributesWidget::updateContent()
{
    ChartModelCartesian* chart = m_brainBrowserWindowToolBarChartAttributes->getCartesianChart();
    if (chart != NULL) {
        m_cartesianLineWidthDoubleSpinBox->blockSignals(true);
        m_cartesianLineWidthDoubleSpinBox->setValue(chart->getLineWidth());
        m_cartesianLineWidthDoubleSpinBox->blockSignals(false);
    }
}

/**
 * Called when the cartesian line width is changed.
 *
 * @param value
 *    New value for line width.
 */
void
CartesianChartAttributesWidget::cartesianLineWidthValueChanged(double value)
{
    ChartModelCartesian* chart = m_brainBrowserWindowToolBarChartAttributes->getCartesianChart();
    if (chart != NULL) {
        chart->setLineWidth(value);
        
        m_brainBrowserWindowToolBarChartAttributes->updateGraphics();
    }
}




/* ===========================================================================*/



/**
 * \class caret::MatrixChartAttributesWidget
 * \brief Controls for matrix chart attributes.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param brainBrowserWindowToolBarChartAttributes
 *   The parent attributes widget.
 */
MatrixChartAttributesWidget::MatrixChartAttributesWidget(BrainBrowserWindowToolBarChartAttributes* brainBrowserWindowToolBarChartAttributes)
{
    m_brainBrowserWindowToolBarChartAttributes = brainBrowserWindowToolBarChartAttributes;
    
    QLabel* scaleModeLabel = new QLabel("Mode");
    m_chartMatrixScaleModeEnumComboBox = new EnumComboBoxTemplate(this);
    m_chartMatrixScaleModeEnumComboBox->setup<ChartMatrixScaleModeEnum,ChartMatrixScaleModeEnum::Enum>();
    QObject::connect(m_chartMatrixScaleModeEnumComboBox, SIGNAL(itemActivated()),
                     this, SLOT(chartMatrixScaleModeEnumComboBoxItemActivated()));

    QLabel* cellWidthLabel = new QLabel("Cell Width");
    m_cellWidthSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(1.0,
                                                                                        1000.0,
                                                                                        1.0,
                                                                                        0.1,
                                                                                        this,
                                                                                        SLOT(cellWidthSpinBoxValueChanged(double)));
    m_cellWidthSpinBox->setFixedWidth(70);

    QLabel* cellHeightLabel = new QLabel("Cell Height");
    m_cellHeightSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(1.0,
                                                                                        1000.0,
                                                                                        1.0,
                                                                                        0.1,
                                                                                        this,
                                                                                        SLOT(cellHeightSpinBoxValueChanged(double)));
    m_cellHeightSpinBox->setFixedWidth(70);
    
    QAction* resetButtonAction = WuQtUtilities::createAction("Reset",
                                                             "Reset panning (shift-mouse movement) and zooming (CTRL-mouse movement)",
                                                             this,
                                                             this,
                                                             SLOT(resetButtonClicked()));
    QToolButton* resetToolButton = new QToolButton();
    resetToolButton->setDefaultAction(resetButtonAction);
    
    
    const int32_t COLUMN_LABEL = 0;
    const int32_t COLUMN_WIDGET = 1;
    
    QWidget* gridWidget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(gridWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 2, 0);
    int32_t rowIndex = gridLayout->rowCount();
    gridLayout->addWidget(scaleModeLabel, rowIndex, COLUMN_LABEL);
    gridLayout->addWidget(m_chartMatrixScaleModeEnumComboBox->getWidget(), rowIndex, COLUMN_WIDGET);
    rowIndex++;
    gridLayout->addWidget(cellWidthLabel, rowIndex, COLUMN_LABEL);
    gridLayout->addWidget(m_cellWidthSpinBox, rowIndex, COLUMN_WIDGET);
    rowIndex++;
    gridLayout->addWidget(cellHeightLabel, rowIndex, COLUMN_LABEL);
    gridLayout->addWidget(m_cellHeightSpinBox, rowIndex, COLUMN_WIDGET);
    rowIndex++;
    gridLayout->addWidget(resetToolButton, rowIndex, COLUMN_LABEL, 1, 2, Qt::AlignHCenter);
    rowIndex++;
    
    gridWidget->setFixedSize(gridWidget->sizeHint());
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(gridWidget);
    layout->addStretch();
}

/**
 * Destructor.
 */
MatrixChartAttributesWidget::~MatrixChartAttributesWidget()
{
    
}

/**
 * Update the content of this widget.
 */
void
MatrixChartAttributesWidget::updateContent()
{
    ChartMatrixDisplayProperties* matrixDisplayProperties = m_brainBrowserWindowToolBarChartAttributes->getChartableMatrixDisplayProperties();
    if (matrixDisplayProperties != NULL) {
        const ChartMatrixScaleModeEnum::Enum scaleMode = matrixDisplayProperties->getScaleMode();
        m_chartMatrixScaleModeEnumComboBox->setSelectedItem<ChartMatrixScaleModeEnum,ChartMatrixScaleModeEnum::Enum>(scaleMode);
        
        m_cellWidthSpinBox->blockSignals(true);
        m_cellWidthSpinBox->setValue(matrixDisplayProperties->getCellWidth());
        m_cellWidthSpinBox->blockSignals(false);
        
        m_cellHeightSpinBox->blockSignals(true);
        m_cellHeightSpinBox->setValue(matrixDisplayProperties->getCellHeight());
        m_cellHeightSpinBox->blockSignals(false);
    }
}

/**
 * Called when the scale mode combo box is changed.
 */
void
MatrixChartAttributesWidget::chartMatrixScaleModeEnumComboBoxItemActivated()
{
    ChartMatrixDisplayProperties* matrixDisplayProperties = m_brainBrowserWindowToolBarChartAttributes->getChartableMatrixDisplayProperties();
    if (matrixDisplayProperties != NULL) {
        const ChartMatrixScaleModeEnum::Enum scaleMode = m_chartMatrixScaleModeEnumComboBox->getSelectedItem<ChartMatrixScaleModeEnum,ChartMatrixScaleModeEnum::Enum>();
        matrixDisplayProperties->setScaleMode(scaleMode);
        m_brainBrowserWindowToolBarChartAttributes->updateGraphics();
    }
}

/**
 * Called when the cell width spin box value is changed.
 *
 * @param value
 *    New value for cell width.
 */
void
MatrixChartAttributesWidget::cellWidthSpinBoxValueChanged(double value)
{
    ChartMatrixDisplayProperties* matrixDisplayProperties = m_brainBrowserWindowToolBarChartAttributes->getChartableMatrixDisplayProperties();
    if (matrixDisplayProperties != NULL) {
        matrixDisplayProperties->setCellWidth(value);
        m_brainBrowserWindowToolBarChartAttributes->updateGraphics();
    }
}

/**
 * Called when the cell height spin box value is changed.
 *
 * @param value
 *    New value for cell height.
 */
void
MatrixChartAttributesWidget::cellHeightSpinBoxValueChanged(double value)
{
    ChartMatrixDisplayProperties* matrixDisplayProperties = m_brainBrowserWindowToolBarChartAttributes->getChartableMatrixDisplayProperties();
    if (matrixDisplayProperties != NULL) {
        matrixDisplayProperties->setCellHeight(value);
        m_brainBrowserWindowToolBarChartAttributes->updateGraphics();
    }
}

/**
 * Called when the reset button is clicked.
 */
void
MatrixChartAttributesWidget::resetButtonClicked()
{
    ChartMatrixDisplayProperties* matrixDisplayProperties = m_brainBrowserWindowToolBarChartAttributes->getChartableMatrixDisplayProperties();
    if (matrixDisplayProperties != NULL) {
        matrixDisplayProperties->resetManualModeProperties();
        updateContent();
        m_brainBrowserWindowToolBarChartAttributes->updateGraphics();
    }
}



