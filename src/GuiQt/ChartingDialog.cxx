#include "Brain.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "CaretMappableDataFile.h"
#include "ChartingDialog.h"
#include "GuiManager.h"
#include "ui_ChartingDialog.h"
#include "TimeCourseControls.h"
#include "PaletteFile.h"
#include "Plot2d.h"
#include "Table.h"
#include "QVBoxLayout"
#include "QPoint"
using namespace caret;

ChartingDialog::ChartingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChartingDialog)
{
    ui->setupUi(this);

#ifdef CARET_OS_WINDOWS
    this->setWindowFlags(windowFlags() | Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
#endif
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(customContextMenuRequestedSlot(const QPoint &)));
    //form = new TimeCourseControls();
    table = new Table();
    //plot2d = new Plot2D();
    ui->comboBox->setCurrentIndex(1);
    ui->comboBox->hide();

}

ChartingDialog::~ChartingDialog()
{
    delete ui;    
}

void ChartingDialog::on_closeButton_clicked()
{
    this->hide();
}
#include "BrainBrowserWindow.h"

#include <QList>
#include <QObjectList>
void ChartingDialog::setChartMode(QString type)
{
    QVBoxLayout *layout = (QVBoxLayout *)this->layout();
    layout->removeWidget(this->table);
    //layout->removeWidget(this->form);
    //this->form->hide();
    this->table->hide();
    //this->plot2d->hide();
    if(type == "2D Plot")
    {
        /*layout->insertWidget(1,plot2d,50);
        layout->insertWidget(2,form, 50);
        this->form->show();
        this->plot2d->show();*/
    }
    else if(type == "Matrix View")
    { 
        layout->insertWidget(1,table,100);
        this->ui->toolBarWidget->hide();
        this->ui->toolBoxWidget->hide();
        //layout->removeWidget(this->ui->toolBarWidget);
        //layout->removeWidget(this->ui->toolBoxWidget);
        this->layout()->setSpacing(0);        
        this->layout()->setContentsMargins(0,0,0,0);
        this->table->layout()->setContentsMargins(0,0,0,0);
        QRect rect = table->adjustTableSize(this->getMatrixTableView(),layout);
        
        this->resize(rect.width(),rect.height());
        
        this->table->show();
    }
}

void ChartingDialog::showToolBar(bool show)
{
    if(show)
    //{
        this->ui->toolBarWidget->show();//this->ui->toolBarWidget->adjustSize();}
    else
        this->ui->toolBarWidget->hide();
}

void ChartingDialog::showToolBox(bool show)
{
    if(show)
        this->ui->toolBoxWidget->show();
    else
        this->ui->toolBoxWidget->hide();
}

void ChartingDialog::on_comboBox_currentIndexChanged(const QString &arg1)
{
    setChartMode(arg1);
}

#include <QScrollBar>
void ChartingDialog::showDialog()
{
    QVBoxLayout *layout = (QVBoxLayout *)this->layout();
    QRect rect = table->adjustTableSize(this->getMatrixTableView(),layout);
    //this->getMatrixTableView()->verticalScrollBar()->hide();
    //this->getMatrixTableView()->horizontalScrollBar()->hide();
    this->resize(rect.width(),rect.height()+6);
    //this->getMatrixTableView()->setFrameRect(rect);
    this->show();
}

//#include "PaletteColorMapping.h"
void ChartingDialog::openPconnMatrix(CaretMappableDataFile *pconnFile)
{
    //pconnFile->getMapPaletteColorMapping(0)->getSelectedPaletteName();
    CiftiMappableConnectivityMatrixDataFile *matrix = static_cast<CiftiMappableConnectivityMatrixDataFile *>(pconnFile);
    if(matrix == NULL) return;
    
    std::vector<int64_t> dim;
    matrix->getMapDimensions(dim);
    PaletteFile *pf = GuiManager::get()->getBrain()->getPaletteFile();
    
    int ncols = dim[0];
    std::vector<float> rgba;
    
    matrix->getMatrixRGBA(rgba,pf);
    int nrows = dim[1];
    if(rgba.size() != nrows*ncols*4) return; //add error message
    std::vector<std::vector<QColor> > cMatrix;
    cMatrix.resize(nrows);
    for(int i = 0;i<nrows;i++) cMatrix[i].resize(ncols);
    
    for(int i = 0;i<nrows;i++)
    {        
        for(int j = 0;j<ncols;j++)
        {
            
            cMatrix[i][j] = QColor(rgba[(i*ncols+j)*4]*255,rgba[(i*ncols+j)*4+1]*255,rgba[(i*ncols+j)*4+2]*255,fabs(rgba[(i*ncols+j)*4+3]*255));
        }
    }  
    table->populate(cMatrix);
    
    //this->resize(table->width()+20,table->height()+40);
}

QTableView * 
ChartingDialog::getMatrixTableView() 
{ 
    return this->table->getTableView();
}

#include <QMenu>
#include <QAction>
void ChartingDialog::customContextMenuRequestedSlot(const QPoint &pos)
{
    QMenu menu;
    QAction *toggleToolBar = new QAction("Show Toolbar",&menu);
    toggleToolBar->setCheckable(1);
    toggleToolBar->setChecked(!(ui->toolBarWidget->isHidden()));
    connect(toggleToolBar,SIGNAL(toggled(bool)),this,SLOT(showToolBar(bool)));
    menu.addAction(toggleToolBar);

    QAction *toggleToolBox = new QAction("Show Toolbox",&menu);
    toggleToolBox->setCheckable(1);
    toggleToolBox->setChecked(!(ui->toolBoxWidget->isHidden()));
    connect(toggleToolBox,SIGNAL(toggled(bool)),this,SLOT(showToolBox(bool)));
    menu.addAction(toggleToolBox);
    menu.exec(QCursor::pos());
}
