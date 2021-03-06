/*
 * Copyright © 2001-2011 Reginald Marr <reginald.t.marr@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */
#include "custommethodconstructorwindow.h"
#include "ui_custommethodconstructorwindow.h"
#include "QStackedWidget"
#include "QComboBox"
#include "QTextEdit"
#include "QLineEdit"
#include "QPushButton"
#include "iostream"

//likely will want to reconsider this
//#define protected public
//#include <QTreeWidget>

Q_DECLARE_METATYPE(QTreeWidgetItem*)
Q_DECLARE_METATYPE(QStackedWidget*)
Q_DECLARE_METATYPE(QFrame*)

customMethodConstructorWindow::customMethodConstructorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::customMethodConstructorWindow)
{
    ui->setupUi(this);
    //treeItemDelegate = new controlObjectItemDelegate(this);
    //ui->methodSetupTreeWidget->setItemDelegate(treeItemDelegate);
    ui->methodSetupTreeWidget->setColumnCount(5);


    //this is all done to then later populate the comboboxes
    //tags could be dynamically passed into the setupblock
    std::vector<QStringList> setup,reaction;

    setup.push_back(QString("If;While").split(";"));
    setup.push_back(QString("Time Passed;Static Var;Tag Var").split(";"));
    setup.push_back(QString("<;=;!=;>").split(";"));
    setup.push_back(QString("Time Passed;Static Var;Tag Var").split(";"));
    setup.push_back(QString("Then;And;Or").split(";"));

    reaction.push_back(QString("Log Current Data;Use PID Control;Engage Adaptive NN;Disengae Adaptive NN;Engage Predictive NN").split(";"));
    reaction.push_back(QString("End;Else").split(";"));
    reaction.push_back(QString("End;Break").split(";"));

    methodBlocks.push_back(setup);
    methodBlocks.push_back(reaction);

    methodIndex = 0;
    loggedIfIndex = 0;
    AddRoot("Method 1",loggedIfIndex);
    for(int i = 0; i < ui->methodSetupTreeWidget->columnCount();i++)
    {
        ui->methodSetupTreeWidget->resizeColumnToContents(i);
    }

    childColumnOffset = 0;
    methodHasIfChild = false;
    ui->newMethodBtn->setEnabled(false);
    ui->addMethodBtn->setEnabled(false);
}

customMethodConstructorWindow::~customMethodConstructorWindow()
{
    delete ui;
}

void customMethodConstructorWindow::addChildRow(QTreeWidget *widgetParent, QTreeWidgetItem *itemParent,
                                                bool fillSelections,int layoutType,int rootIndex,int ifIndex)
{
    /*
     * This function will add a child to the Selected QTreeWidget Item.
     * The Child will be a row of comboboxes/line edit widgets that the user can interact with.
     * The Layout (refering to the items set in the comboboxe, the number of columns (widget items), and the position of widget items)
     * is created using a switch case with the layoutType var as its argument. LayoutType is indicated as one of seven possible layouts.
     *
     * ________________________________________________________________________________________
     * | Layout Type |      Visual Representation(showing combobox item 0)       | Cycle Size |
     * |_____________|___________________________________________________________|____________|
     * |      0      |[If][Time Passed][Seconds][<][Static Var][Line Edit][Then] |      6     |
     * |      1      |[If][Static Var][Line Edit][<][Time Passed][Seconds][Then] |      6     |
     * |      2      |[If][Time Passed][Seconds][<] [Tag Var][Then]              |      5     |
     * |      3      |[If][Tag Var][<][Time Passed][Seconds][Then]               |      5     |
     * |      4      |[If][Static Var][Line Edit][<][Tag Var][Then]              |      5     |
     * |      5      |[If][Tag Var][<][Static Var][Line Edit][Then]              |      5     |
     * |      6      |[If][Tag Var][<][Tag Var][Then]                            |      4     |
     * |      7      |[If][Tag Var][<][Time Passed][Seconds][Then]               |      5     |
     * ****************************************************************************************
     *
     * TODO:Investigate into the difference between layout 3 & 7
     *
     * The function is also passed a boolean value (fillSelections) which indicates whether the row being created has been interacted with by the user;
     * if this is the case (fillSelections == true) then as each widget is added to the row its currently selected index is chosen based of off the
     * customObjectMethods struct which contains the information regarding each currently existing rows.
    */

    //default is to fill child row with time comparison
    //TODO: fix this in the future
    //widgetParent->setColumnCount(methodBlocks.at(rowType).size());

    if(!itemParent)
    {
        std::cout << "Error! no parent here \n";
        exit(-1);
    }

    QTreeWidgetItem *childItem = new QTreeWidgetItem(itemParent);

    QVariant childItemVariant,widgetParentVarient;
    widgetParentVarient.setValue(widgetParent);
    childItemVariant.setValue(childItem);
    int widgetType,methodBlockCount = 0,rowType = 0;
    int cycleSetup = 0,cycleSize,currentIndex = itemParent->indexOfChild(childItem);
    int timeLineEditLocation = 2,lineEditLocation = 4 + timeLineEditLocation;

    bool isTop = (currentIndex == 0) ? true : false;

    switch (layoutType) {
    case 0:
    {
        cycleSize = methodBlockCount+methodBlocks.at(rowType).size() + 2;
        widgetParent->setColumnCount(cycleSize);
        timeLineEditLocation = 2,lineEditLocation = 5;
        while (cycleSetup < cycleSize)
        {

                QComboBox *itemComboBox = new QComboBox;

                itemComboBox->setProperty("rowType", rowType);//First row in a method
                //itemComboBox->setProperty("row", itemParent->indexOfChild(childItem));
                itemComboBox->setProperty("column",cycleSetup);
                itemComboBox->setProperty("childItem",childItemVariant);itemComboBox->clear();itemComboBox->clear();
                if(cycleSetup == lineEditLocation-1)
                {
                    for(uint i = 1;i< methodBlocks.at(rowType).at(methodBlockCount).size();i++)
                    {
                        itemComboBox->addItem(methodBlocks.at(rowType).at(methodBlockCount).at(i));
                    }
                }
                else if(cycleSetup == timeLineEditLocation-1)
                {
                    itemComboBox->addItem(methodBlocks.at(rowType).at(methodBlockCount).at(0));
                    for(uint i = 2;i< methodBlocks.at(rowType).at(methodBlockCount).size();i++)
                    {
                        itemComboBox->addItem(methodBlocks.at(rowType).at(methodBlockCount).at(i));
                    }
                }
                else
                {
                    itemComboBox->addItems(methodBlocks.at(rowType).at(methodBlockCount));
                }

                QVariant itemWidgetVar;
                itemWidgetVar.setValue(itemComboBox);
                childItem->setData(cycleSetup,Qt::UserRole,itemWidgetVar);
                widgetType = 0;//this means its a method blocks type combobox
                childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
                if(fillSelections)
                {
                    itemComboBox->setCurrentIndex((int)customObjectMethods.userSelection.at(itemParent->indexOfChild(childItem)).at(cycleSetup));
                }
                QObject::connect(itemComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnLogicSetupComboIndexChanged(const QString&)));

                itemParent->addChild(childItem);
                widgetParent->setItemWidget(childItem,cycleSetup,itemComboBox);
                if(cycleSetup == 0 && !isTop)
                {
                    widgetParent->itemWidget(childItem,cycleSetup)->setVisible(false);
                    widgetParent->itemWidget(childItem,cycleSetup)->setHidden(true);
                }

                cycleSetup++;
                methodBlockCount++;

                if(cycleSetup == timeLineEditLocation)
                {
                    QVariant timeCBWidgetVar;
                    QComboBox *timeItemCombobox = new QComboBox;
                    timeItemCombobox->setProperty("rowType", rowType);//First row in a method
                    timeItemCombobox->setProperty("column",cycleSetup);
                    timeItemCombobox->setProperty("childItem",childItemVariant);
                    timeItemCombobox->addItems(QString("Seconds;MilliSeconds").split(";"));

                    timeCBWidgetVar.setValue(timeItemCombobox);
                    childItem->setData(cycleSetup,Qt::UserRole,timeCBWidgetVar);
                    widgetType = 2;//this means its a time passed type
                    childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
                    if(fillSelections)// && customObjectMethods.selectionType.at(itemParent->indexOfChild(childItem)).at(cycleSetup) == 2)
                    {
                        timeItemCombobox->setCurrentIndex((int)customObjectMethods.userSelection.at(itemParent->indexOfChild(childItem)).at(cycleSetup));
                    }
                    //QObject::connect(timeItemCombobox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnTimeBoxChanged(const QString&)));
                    widgetParent->setItemWidget(childItem,cycleSetup,timeItemCombobox);
                    itemParent->addChild(childItem);
                    cycleSetup++;
                }
                else if(cycleSetup == lineEditLocation)
                {
                    QVariant timeLEWidgetVar;
                    QLineEdit *itemLineEdit = new QLineEdit;
                    itemLineEdit->setProperty("rowType", rowType);
                    itemLineEdit->setProperty("row", 0);
                    itemLineEdit->setProperty("column",cycleSetup);
                    itemLineEdit->setProperty("widgetParent",widgetParentVarient);
                    itemLineEdit->setProperty("childItem",childItemVariant);
                    timeLEWidgetVar.setValue(itemLineEdit);
                    childItem->setData(cycleSetup,Qt::UserRole,timeLEWidgetVar);
                    widgetType = 1;//this means its a time passed type combobox
                    childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
                    if(fillSelections)
                    {
                        itemLineEdit->setText(QString::number(customObjectMethods.userSelection.at(itemParent->indexOfChild(childItem)).at(cycleSetup)));
                    }
                    widgetParent->setItemWidget(childItem,cycleSetup,itemLineEdit);
                    itemParent->addChild(childItem);
                    cycleSetup++;
                }



        }

        fillSelections = false;

    }
        break;
    case 1:
    {

        cycleSize = methodBlockCount+methodBlocks.at(rowType).size() + 2;
        widgetParent->setColumnCount(cycleSize);
        timeLineEditLocation = 5,lineEditLocation = 2;
        while (cycleSetup < cycleSize)
        {

                QComboBox *itemComboBox = new QComboBox;
                itemComboBox->setProperty("rowType", rowType);//First row in a method
                //itemComboBox->setProperty("row", itemParent->indexOfChild(childItem));
                itemComboBox->setProperty("column",cycleSetup);
                itemComboBox->setProperty("childItem",childItemVariant);
                if(cycleSetup == lineEditLocation-1)
                {
                    for(uint i = 1;i< methodBlocks.at(rowType).at(methodBlockCount).size();i++)
                    {
                        itemComboBox->addItem(methodBlocks.at(rowType).at(methodBlockCount).at(i));
                    }
                }
                else if(cycleSetup == timeLineEditLocation-1)
                {
                    itemComboBox->addItem(methodBlocks.at(rowType).at(methodBlockCount).at(0));
                    for(uint i = 2;i< methodBlocks.at(rowType).at(methodBlockCount).size();i++)
                    {
                        itemComboBox->addItem(methodBlocks.at(rowType).at(methodBlockCount).at(i));
                    }
                }
                else
                {
                    itemComboBox->addItems(methodBlocks.at(rowType).at(methodBlockCount));
                }


                QVariant itemWidgetVar;
                itemWidgetVar.setValue(itemComboBox);
                childItem->setData(cycleSetup,Qt::UserRole,itemWidgetVar);
                widgetType = 0;//this means its a method blocks type combobox
                childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
                if(fillSelections)// && customObjectMethods.selectionType.at(itemParent->indexOfChild(childItem)).at(cycleSetup) == 0)
                {
                    itemComboBox->setCurrentIndex((int)customObjectMethods.userSelection.at(itemParent->indexOfChild(childItem)).at(cycleSetup));
                }
                QObject::connect(itemComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnLogicSetupComboIndexChanged(const QString&)));
                if(cycleSetup == 0 && !isTop)
                {
                    childItem->setHidden(true);
                }
                widgetParent->setItemWidget(childItem,cycleSetup,itemComboBox);
                itemParent->addChild(childItem);
                cycleSetup++;
                methodBlockCount++;

                if(cycleSetup == timeLineEditLocation)
                {
                    QVariant timeCBWidgetVar;
                    QComboBox *timeItemCombobox = new QComboBox;
                    timeItemCombobox->setProperty("rowType", rowType);//First row in a method
                    //timeItemComboBox->setProperty("row", itemParent->indexOfChild(childItem));
                    timeItemCombobox->setProperty("column",cycleSetup);
                    timeItemCombobox->setProperty("childItem",childItemVariant);
                    timeItemCombobox->addItems(QString("Seconds;MilliSeconds;Reset").split(";"));
                    timeCBWidgetVar.setValue(timeItemCombobox);
                    childItem->setData(cycleSetup,Qt::UserRole,timeCBWidgetVar);
                    widgetType = 2;//this means its a time passed type
                    childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
                    if(fillSelections)//&& customObjectMethods.selectionType.at(itemParent->indexOfChild(childItem)).at(cycleSetup) == 2)
                    {
                        timeItemCombobox->setCurrentIndex((int)customObjectMethods.userSelection.at(itemParent->indexOfChild(childItem)).at(cycleSetup));
                    }
                    //QObject::connect(timeItemCombobox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnTimeBoxChanged(const QString&)));
                    widgetParent->setItemWidget(childItem,cycleSetup,timeItemCombobox);
                    itemParent->addChild(childItem);
                    cycleSetup++;
                }
                else if(cycleSetup == lineEditLocation)
                {
                    QVariant timeLEWidgetVar;
                    QLineEdit *itemLineEdit = new QLineEdit;
                    itemLineEdit->setProperty("rowType", rowType);
                    itemLineEdit->setProperty("row", 0);
                    itemLineEdit->setProperty("column",cycleSetup);
                    itemLineEdit->setProperty("widgetParent",widgetParentVarient);
                    itemLineEdit->setProperty("childItem",childItemVariant);
                    //QObject::connect(itemComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnTimeBoxChanged(const QString&)));
                    timeLEWidgetVar.setValue(itemLineEdit);
                    childItem->setData(cycleSetup,Qt::UserRole,timeLEWidgetVar);
                    widgetType = 1;//this means its a time passed type combobox
                    childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
                    if(fillSelections)// && customObjectMethods.selectionType.at(itemParent->indexOfChild(childItem)).at(cycleSetup) == 0)
                    {
                        itemLineEdit->setText(QString::number(customObjectMethods.userSelection.at(itemParent->indexOfChild(childItem)).at(cycleSetup)));
                    }
                    widgetParent->setItemWidget(childItem,cycleSetup,itemLineEdit);
                    itemParent->addChild(childItem);
                    cycleSetup++;
                }

        }


    }
        break;
    case 2:
    {
        cycleSize =  methodBlockCount+methodBlocks.at(rowType).size() + 1;
        widgetParent->setColumnCount(cycleSize);
        timeLineEditLocation = 2;
        while (cycleSetup < cycleSize)
        {

//            if(cycleSetup == 0 && !isTop)
//            {
//                cycleSetup++;
//                methodBlockCount++;
//            }
//            else
//            {
                QComboBox *itemComboBox = new QComboBox;
                itemComboBox->setProperty("rowType", rowType);//First row in a method
                //itemComboBox->setProperty("row", itemParent->indexOfChild(childItem));
                itemComboBox->setProperty("column",cycleSetup);
                itemComboBox->setProperty("childItem",childItemVariant);itemComboBox->clear();
                if(cycleSetup == 4)
                {
                    for(uint i = 1;i< methodBlocks.at(rowType).at(methodBlockCount).size();i++)
                    {
                        itemComboBox->addItem(methodBlocks.at(rowType).at(methodBlockCount).at(i));
                    }
                    itemComboBox->setCurrentIndex(1);
                }
                else
                {
                    itemComboBox->addItems(methodBlocks.at(rowType).at(methodBlockCount));
                }


                QVariant itemWidgetVar;
                itemWidgetVar.setValue(itemComboBox);
                childItem->setData(cycleSetup,Qt::UserRole,itemWidgetVar);
                widgetType = 0;//this means its a method blocks type combobox
                childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
                if(fillSelections)// && customObjectMethods.selectionType.at(itemParent->indexOfChild(childItem)).at(cycleSetup) == 0)
                {
                    int childIndex = (int)customObjectMethods.userSelection.at(itemParent->indexOfChild(childItem)).at(cycleSetup);
                    itemComboBox->setCurrentIndex((int)customObjectMethods.userSelection.at(itemParent->indexOfChild(childItem)).at(cycleSetup));
                }
                QObject::connect(itemComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnLogicSetupComboIndexChanged(const QString&)));
                widgetParent->setItemWidget(childItem,cycleSetup,itemComboBox);
                itemParent->addChild(childItem);
                cycleSetup++;
                methodBlockCount++;

                if(cycleSetup == timeLineEditLocation)
                {
                    QVariant timeCBWidgetVar;
                    QComboBox *timeItemCombobox = new QComboBox;
                    timeItemCombobox->setProperty("rowType", rowType);//First row in a method
                    //timeItemComboBox->setProperty("row", itemParent->indexOfChild(childItem));
                    timeItemCombobox->setProperty("column",cycleSetup);
                    timeItemCombobox->setProperty("childItem",childItemVariant);
                    timeItemCombobox->addItems(QString("Seconds;MilliSeconds").split(";"));

                    timeCBWidgetVar.setValue(timeItemCombobox);
                    childItem->setData(cycleSetup,Qt::UserRole,timeCBWidgetVar);
                    widgetType = 2;//this means its a time passed type
                    childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
                    if(fillSelections)//&& customObjectMethods.selectionType.at(itemParent->indexOfChild(childItem)).at(cycleSetup) == 2)
                    {
                        timeItemCombobox->setCurrentIndex((int)customObjectMethods.userSelection.at(itemParent->indexOfChild(childItem)).at(cycleSetup));
                    }
                    //QObject::connect(timeItemCombobox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnTimeBoxChanged(const QString&)));
                    widgetParent->setItemWidget(childItem,cycleSetup,timeItemCombobox);
                    itemParent->addChild(childItem);
                    cycleSetup++;
                }
            //}

        }
    }
        break;
    case 3:
    {
        cycleSize = methodBlockCount+methodBlocks.at(rowType).size() + 1;
        widgetParent->setColumnCount(cycleSize);
        timeLineEditLocation = 4;
        while (cycleSetup < cycleSize)
        {
//            if(cycleSetup == 0 && !isTop)
//            {
//                cycleSetup++;
//                methodBlockCount++;
//            }
//            else
//            {
                QComboBox *itemComboBox = new QComboBox;
                itemComboBox->setProperty("rowType", rowType);//First row in a method
                //itemComboBox->setProperty("row", itemParent->indexOfChild(childItem));
                itemComboBox->setProperty("column",cycleSetup);
                itemComboBox->setProperty("childItem",childItemVariant);itemComboBox->clear();itemComboBox->clear();

                if(cycleSetup == 1)
                {
                    for(uint i = 1;i< methodBlocks.at(rowType).at(methodBlockCount).size();i++)
                    {
                        itemComboBox->addItem(methodBlocks.at(rowType).at(methodBlockCount).at(i));
                    }
                    itemComboBox->setCurrentIndex(1);
                }
                else
                {
                    itemComboBox->addItems(methodBlocks.at(rowType).at(methodBlockCount));
                }


                QVariant itemWidgetVar;
                itemWidgetVar.setValue(itemComboBox);
                childItem->setData(cycleSetup,Qt::UserRole,itemWidgetVar);
                widgetType = 0;//this means its a method blocks type combobox
                childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
                if(fillSelections)// && customObjectMethods.selectionType.at(itemParent->indexOfChild(childItem)).at(cycleSetup) == 0)
                {
                    itemComboBox->setCurrentIndex((int)customObjectMethods.userSelection.at(itemParent->indexOfChild(childItem)).at(cycleSetup));
                }
                QObject::connect(itemComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnLogicSetupComboIndexChanged(const QString&)));
                widgetParent->setItemWidget(childItem,cycleSetup,itemComboBox);
                itemParent->addChild(childItem);
                cycleSetup++;
                methodBlockCount++;

                if(cycleSetup == timeLineEditLocation)
                {
                    QVariant timeCBWidgetVar;
                    QComboBox *timeItemCombobox = new QComboBox;
                    timeItemCombobox->setProperty("rowType", rowType);//First row in a method
                    //timeItemComboBox->setProperty("row", itemParent->indexOfChild(childItem));
                    timeItemCombobox->setProperty("column",cycleSetup);
                    timeItemCombobox->setProperty("childItem",childItemVariant);
                    timeItemCombobox->addItems(QString("Seconds;MilliSeconds;Reset").split(";"));

                    timeCBWidgetVar.setValue(timeItemCombobox);
                    childItem->setData(cycleSetup,Qt::UserRole,timeCBWidgetVar);
                    widgetType = 2;//this means its a time passed type
                    childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
                    if(fillSelections)//&& customObjectMethods.selectionType.at(itemParent->indexOfChild(childItem)).at(cycleSetup) == 2)
                    {
                        timeItemCombobox->setCurrentIndex((int)customObjectMethods.userSelection.at(itemParent->indexOfChild(childItem)).at(cycleSetup));
                    }
                    //QObject::connect(timeItemCombobox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnTimeBoxChanged(const QString&)));
                    widgetParent->setItemWidget(childItem,cycleSetup,timeItemCombobox);
                    itemParent->addChild(childItem);
                    cycleSetup++;
                }
           // }


        }
    }
        break;
    case 4:
    {
        cycleSize = methodBlockCount+methodBlocks.at(rowType).size() + 1;
        widgetParent->setColumnCount(cycleSize);
        lineEditLocation = 2;
        while (cycleSetup < cycleSize)
        {
//            if(cycleSetup == 0 && !isTop)
//            {
//                cycleSetup++;
//                methodBlockCount++;
//            }
//            else
//            {
                QComboBox *itemComboBox = new QComboBox;
                itemComboBox->setProperty("rowType", rowType);//First row in a method
                //itemComboBox->setProperty("row", itemParent->indexOfChild(childItem));
                itemComboBox->setProperty("column",cycleSetup);
                itemComboBox->setProperty("childItem",childItemVariant);itemComboBox->clear();itemComboBox->clear();
                if(cycleSetup == lineEditLocation-1)
                {
                    itemComboBox->addItems(methodBlocks.at(rowType).at(methodBlockCount));
                    itemComboBox->setCurrentIndex(1);
                }
                else if(cycleSetup == 4)
                {
                    itemComboBox->addItem(methodBlocks.at(rowType).at(methodBlockCount).at(0));
                    for(uint i = 2;i< methodBlocks.at(rowType).at(methodBlockCount).size();i++)
                    {
                        itemComboBox->addItem(methodBlocks.at(rowType).at(methodBlockCount).at(i));
                    }
                    itemComboBox->setCurrentIndex(1);

                }
                else
                {
                    itemComboBox->addItems(methodBlocks.at(rowType).at(methodBlockCount));
                }

                QVariant itemWidgetVar;
                itemWidgetVar.setValue(itemComboBox);
                childItem->setData(cycleSetup,Qt::UserRole,itemWidgetVar);
                widgetType = 0;//this means its a method blocks type combobox
                childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
                if(fillSelections)// && customObjectMethods.selectionType.at(itemParent->indexOfChild(childItem)).at(cycleSetup) == 0)
                {
                    itemComboBox->setCurrentIndex((int)customObjectMethods.userSelection.at(itemParent->indexOfChild(childItem)).at(cycleSetup));
                }
                QObject::connect(itemComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnLogicSetupComboIndexChanged(const QString&)));
                widgetParent->setItemWidget(childItem,cycleSetup,itemComboBox);
                itemParent->addChild(childItem);
                cycleSetup++;
                methodBlockCount++;

                if(cycleSetup == lineEditLocation)
                {
                    QVariant timeLEWidgetVar;
                    QLineEdit *itemLineEdit = new QLineEdit;
                    itemLineEdit->setProperty("rowType", rowType);
                    itemLineEdit->setProperty("row", 0);
                    itemLineEdit->setProperty("column",cycleSetup);
                    itemLineEdit->setProperty("widgetParent",widgetParentVarient);
                    itemLineEdit->setProperty("childItem",childItemVariant);
                    //QObject::connect(itemComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnTimeBoxChanged(const QString&)));
                    timeLEWidgetVar.setValue(itemLineEdit);
                    childItem->setData(cycleSetup,Qt::UserRole,timeLEWidgetVar);
                    widgetType = 1;//this means its a time passed type combobox
                    childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
                    if(fillSelections)// && customObjectMethods.selectionType.at(itemParent->indexOfChild(childItem)).at(cycleSetup) == 0)
                    {
                        itemLineEdit->setText(QString::number(customObjectMethods.userSelection.at(itemParent->indexOfChild(childItem)).at(cycleSetup)));
                    }
                    widgetParent->setItemWidget(childItem,cycleSetup,itemLineEdit);
                    itemParent->addChild(childItem);
                    cycleSetup++;
                }
            //}

        }


    }
        break;
    case 5:
    {
        cycleSize = methodBlockCount+methodBlocks.at(rowType).size() + 1;
        widgetParent->setColumnCount(cycleSize);
        lineEditLocation = 4;
        while (cycleSetup < cycleSize)
        {
//            if(cycleSetup == 0 && !isTop)
//            {
//                cycleSetup++;
//                methodBlockCount++;
//            }
//            else
//            {
                QComboBox *itemComboBox = new QComboBox;
                itemComboBox->setProperty("rowType", rowType);//First row in a method
                //itemComboBox->setProperty("row", itemParent->indexOfChild(childItem));
                itemComboBox->setProperty("column",cycleSetup);
                itemComboBox->setProperty("childItem",childItemVariant);itemComboBox->clear();

                if(cycleSetup == lineEditLocation-1)
                {
                    itemComboBox->addItems(methodBlocks.at(rowType).at(methodBlockCount));
                    itemComboBox->setCurrentIndex(1);
                }
                else if(cycleSetup == 1)
                {
                    itemComboBox->addItem(methodBlocks.at(rowType).at(methodBlockCount).at(0));
                    for(uint i = 2;i< methodBlocks.at(rowType).at(methodBlockCount).size();i++)
                    {
                        itemComboBox->addItem(methodBlocks.at(rowType).at(methodBlockCount).at(i));
                    }
                    itemComboBox->setCurrentIndex(1);
                }
                else
                {
                    itemComboBox->addItems(methodBlocks.at(rowType).at(methodBlockCount));
                }

                QVariant itemWidgetVar;
                itemWidgetVar.setValue(itemComboBox);
                childItem->setData(cycleSetup,Qt::UserRole,itemWidgetVar);
                widgetType = 0;//this means its a method blocks type combobox
                childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
                if(fillSelections)// && customObjectMethods.selectionType.at(itemParent->indexOfChild(childItem)).at(cycleSetup) == 0)
                {
                    int childIndex = (int)customObjectMethods.userSelection.at(itemParent->indexOfChild(childItem)).at(cycleSetup);
                    itemComboBox->setCurrentIndex((int)customObjectMethods.userSelection.at(itemParent->indexOfChild(childItem)).at(cycleSetup));
                }
                QObject::connect(itemComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnLogicSetupComboIndexChanged(const QString&)));
                widgetParent->setItemWidget(childItem,cycleSetup,itemComboBox);
                itemParent->addChild(childItem);
                cycleSetup++;
                methodBlockCount++;

                if(cycleSetup == lineEditLocation)
                {
                    QVariant timeLEWidgetVar;
                    QLineEdit *itemLineEdit = new QLineEdit;
                    itemLineEdit->setProperty("rowType", rowType);
                    itemLineEdit->setProperty("row", 0);
                    itemLineEdit->setProperty("column",cycleSetup);
                    itemLineEdit->setProperty("widgetParent",widgetParentVarient);
                    itemLineEdit->setProperty("childItem",childItemVariant);
                    //QObject::connect(itemComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnTimeBoxChanged(const QString&)));
                    timeLEWidgetVar.setValue(itemLineEdit);
                    childItem->setData(cycleSetup,Qt::UserRole,timeLEWidgetVar);
                    widgetType = 1;//this means its a time passed type combobox
                    childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
                    if(fillSelections)// && customObjectMethods.selectionType.at(itemParent->indexOfChild(childItem)).at(cycleSetup) == 0)
                    {
                        itemLineEdit->setText(QString::number(customObjectMethods.userSelection.at(itemParent->indexOfChild(childItem)).at(cycleSetup)));
                    }
                    widgetParent->setItemWidget(childItem,cycleSetup,itemLineEdit);
                    itemParent->addChild(childItem);
                    cycleSetup++;
                }
            //}


        }

    }
        break;
    case 6:
    {
        cycleSize = methodBlockCount+methodBlocks.at(rowType).size();
        widgetParent->setColumnCount(cycleSize);
        while (cycleSetup < cycleSize)
        {
//            if(cycleSetup == 0 && !isTop)
//            {
//                cycleSetup++;
//                methodBlockCount++;
//            }
//            else
//            {
                QComboBox *itemComboBox = new QComboBox;
                itemComboBox->setProperty("rowType", rowType);//First row in a method
                //itemComboBox->setProperty("row", itemParent->indexOfChild(childItem));
                itemComboBox->setProperty("column",cycleSetup);
                itemComboBox->setProperty("childItem",childItemVariant);itemComboBox->clear();
                itemComboBox->addItems(methodBlocks.at(rowType).at(methodBlockCount));
                if(cycleSetup == 1)
                {
                    itemComboBox->setCurrentIndex(2);
                }
                else if(cycleSetup == 3)
                {
                    itemComboBox->setCurrentIndex(2);
                }

                QVariant itemWidgetVar;
                itemWidgetVar.setValue(itemComboBox);
                childItem->setData(cycleSetup,Qt::UserRole,itemWidgetVar);
                widgetType = 0;//this means its a method blocks type combobox
                childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
                if(fillSelections)// && customObjectMethods.selectionType.at(itemParent->indexOfChild(childItem)).at(cycleSetup) == 0)
                {
                    int childIndex = (int)customObjectMethods.userSelection.at(itemParent->indexOfChild(childItem)).at(cycleSetup);
                    itemComboBox->setCurrentIndex((int)customObjectMethods.userSelection.at(itemParent->indexOfChild(childItem)).at(cycleSetup));
                }
                QObject::connect(itemComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnLogicSetupComboIndexChanged(const QString&)));
                widgetParent->setItemWidget(childItem,cycleSetup,itemComboBox);
                itemParent->addChild(childItem);
                cycleSetup++;
                methodBlockCount++;
            }

        //}
    }
        break;
    case 7:
    {
        cycleSize = methodBlockCount+methodBlocks.at(rowType).size() + 1;
        widgetParent->setColumnCount(cycleSize);
        timeLineEditLocation = 4;
        while (cycleSetup < cycleSize)
        {
//            if(cycleSetup == 0 && !isTop)
//            {
//                cycleSetup++;
//                methodBlockCount++;
//            }
//            else
//            {
                QComboBox *itemComboBox = new QComboBox;
                itemComboBox->setProperty("rowType", rowType);//First row in a method
                //itemComboBox->setProperty("row", itemParent->indexOfChild(childItem));
                itemComboBox->setProperty("column",cycleSetup);
                itemComboBox->setProperty("childItem",childItemVariant);itemComboBox->clear();itemComboBox->clear();

                if(cycleSetup == 1)
                {
                    for(uint i = 1;i< methodBlocks.at(rowType).at(methodBlockCount).size();i++)
                    {
                        itemComboBox->addItem(methodBlocks.at(rowType).at(methodBlockCount).at(i));
                    }
                    itemComboBox->setCurrentIndex(1);
                }
                else
                {
                    itemComboBox->addItems(methodBlocks.at(rowType).at(methodBlockCount));
                }


                QVariant itemWidgetVar;
                itemWidgetVar.setValue(itemComboBox);
                childItem->setData(cycleSetup,Qt::UserRole,itemWidgetVar);
                widgetType = 0;//this means its a method blocks type combobox
                childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
                if(fillSelections)// && customObjectMethods.selectionType.at(itemParent->indexOfChild(childItem)).at(cycleSetup) == 0)
                {
                    itemComboBox->setCurrentIndex((int)customObjectMethods.userSelection.at(itemParent->indexOfChild(childItem)).at(cycleSetup));
                }
                QObject::connect(itemComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnLogicSetupComboIndexChanged(const QString&)));
                widgetParent->setItemWidget(childItem,cycleSetup,itemComboBox);
                itemParent->addChild(childItem);
                cycleSetup++;
                methodBlockCount++;

                if(cycleSetup == timeLineEditLocation)
                {
                    QVariant timeCBWidgetVar,timeLEWidgetVar;
                    QComboBox *timeItemCombobox = new QComboBox;
                    timeItemCombobox->setProperty("rowType", rowType);//First row in a method
                    timeItemCombobox->setProperty("column",cycleSetup);
                    timeItemCombobox->setProperty("childItem",childItemVariant);
                    timeItemCombobox->addItems(QString("Seconds;MilliSeconds").split(";"));
                    timeCBWidgetVar.setValue(timeItemCombobox);
                    childItem->setData(cycleSetup,Qt::UserRole,timeLEWidgetVar);
                    widgetType = 2;//this means its a time passed type
                    childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
                    if(fillSelections)//&& customObjectMethods.selectionType.at(itemParent->indexOfChild(childItem)).at(cycleSetup) == 2)
                    {
                        timeItemCombobox->setCurrentIndex((int)customObjectMethods.userSelection.at(itemParent->indexOfChild(childItem)).at(cycleSetup));
                    }
                    widgetParent->setItemWidget(childItem,cycleSetup,timeItemCombobox);
                    itemParent->addChild(childItem);
                    cycleSetup++;
                }
           // }


        }

    }
        break;
    }

    //this is all the information that is relavent to the entire row
    childItem->setData(0,Qt::UserRole+2,QVariant::fromValue(cycleSize)); //gives the columnCount
    childItem->setData(0,Qt::UserRole+3,layoutType);//indicates layout type
    childItem->setData(0,Qt::UserRole+4,QVariant::fromValue(rootIndex));
    childItem->setData(0,Qt::UserRole+5,QVariant::fromValue(ifIndex));
    widgetParent->setColumnCount(cycleSize);
    for(int i = 0; i < widgetParent->columnCount();i++)
    {
        widgetParent->resizeColumnToContents(i);
    }

}

void customMethodConstructorWindow::addChildRow(QTreeWidget *widgetParent, QTreeWidgetItem *itemParent)
{
    //default is to fill child row with time comparison
    //TODO: fix this in the future
    //widgetParent->setColumnCount(methodBlocks.at(rowType).size());

    QTreeWidgetItem *childItem = new QTreeWidgetItem(itemParent);
    QVariant childItemVariant = QVariant::fromValue(childItem);

    itemParent->addChild(childItem);
    QComboBox *item0ComboBox = new QComboBox;
    item0ComboBox->setProperty("childItem",childItemVariant);
    item0ComboBox->setProperty("row", 1);
    item0ComboBox->setProperty("column", 0);
    item0ComboBox->addItems(methodBlocks.at(1).at(0));
    QObject::connect(item0ComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnExecuteLogicComboIndexChanged(const QString&)));
    widgetParent->setItemWidget(childItem,0,item0ComboBox);
    itemParent->addChild(childItem);

//not currently supporting else statements
//    QComboBox *item1ComboBox = new QComboBox;
//    item1ComboBox->setProperty("childItem",childItemVariant);
//    item1ComboBox->setProperty("row", 1);
//    item1ComboBox->setProperty("column", 1);
//    item1ComboBox->addItems(methodBlocks.at(1).at(1));
//    QObject::connect(item1ComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnExecuteLogicComboIndexChanged(const QString&)));
//    widgetParent->setItemWidget(childItem,1,item1ComboBox);
//    itemParent->addChild(childItem);

}

void customMethodConstructorWindow::AddRoot(QString rootName,int ifIndex)
{
    QTreeWidgetItem *rootItem = new QTreeWidgetItem(ui->methodSetupTreeWidget);

    rootItem->setData(0,Qt::UserRole+2,QVariant::fromValue(methodIndex));

    rootItem->setText(0,rootName);

    treeRootNames.push_back(rootName);

    ui->methodSetupTreeWidget->addTopLevelItem(rootItem);

    addChildRow(ui->methodSetupTreeWidget,rootItem,0,0,methodIndex,ifIndex);

    addChildRow(ui->methodSetupTreeWidget,rootItem->child(0));

}

void customMethodConstructorWindow::AddChild(QTreeWidgetItem *parent,QStringList rootNames)
{
    QTreeWidgetItem *childItem = new QTreeWidgetItem(parent);
    for(int i = 0;i< rootNames.size();i++)
    {
        childItem->setText(i,rootNames.at(i));
    }

    parent->addChild(childItem);
}

void customMethodConstructorWindow::OnLogicSetupComboIndexChanged(const QString& text)
{
    ui->addMethodBtn->setEnabled(true);
    QComboBox* combo = qobject_cast<QComboBox*>(sender());
    if (combo)
    {
        unsigned nCol = combo->property("column").toInt();
        QTreeWidgetItem *childItem = combo->property("childItem").value<QTreeWidgetItem*>();
        unsigned int rootIndex = childItem->data(0,Qt::UserRole+4).toInt();
        QTreeWidgetItem *parentItem = childItem->parent()->treeWidget()->topLevelItem(rootIndex);//childItem->parent();
        int ifIndex = childItem->data(0,Qt::UserRole+5).toInt();
        unsigned int currentColumnCount = childItem->data(0,Qt::UserRole+2).toInt();

        QTreeWidget* widgetParent = parentItem->treeWidget();

        unsigned int currentLayoutType = childItem->data(0,Qt::UserRole+3).toInt();

        if(nCol == 2)
        {
            if(currentLayoutType == 3 || currentLayoutType == 5 || currentLayoutType == 6)
            {
                return;
            }
        }
        else if(nCol == 3 || nCol == 0)
        {
            if(currentLayoutType != 3 && currentLayoutType != 5 && currentLayoutType != 6)
            {
                return;
            }
        }

        //note that the number of columns is offset by one with reference to the nCol      
        //this is for the the last column, where we were to create the if and, or ect
        int childIndex = parentItem->indexOfChild(childItem);
        if(nCol == currentColumnCount-1)
        {
            int columnCount = 7,childCount = parentItem->childCount()-1,currentIndex = combo->currentIndex();

            columnCount = (combo->currentIndex() != 0 && (childIndex == parentItem->childCount()-1)) ?
                          updateMethodModel(parentItem->childCount(),parentItem) :
                          updateMethodModel(childIndex+1,parentItem);
            widgetParent->setColumnCount(columnCount+1);

            while(parentItem->childCount() != 0)
            {
                QTreeWidgetItem *uselessChild = parentItem->child(parentItem->childCount()-1);
                parentItem->removeChild(uselessChild);
            }

            //this recreates the recorded rows
            for(unsigned int i = 0;i<=childIndex;i++)
            {
                addChildRow(widgetParent,parentItem,true,customObjectMethods.layoutType.at(i),rootIndex,ifIndex);
            }

            if(currentIndex != 0 && (childIndex == childCount))
            {
                //this occurs when we want to shorten the number of rows and end the currently selected row
                //with "Then" rather than "And" or "Or"

                addChildRow(widgetParent,parentItem,false,0,rootIndex,ifIndex);

                addChildRow(widgetParent,parentItem->child(parentItem->childCount()-1));

                childColumnOffset = childIndex+2;
                return;
            }
            else if(currentIndex == 0 && (childIndex != childCount))
            {
                //this occurs when we want to add another row and end the currently selected row with
                //"And" or "Or"

                addChildRow(widgetParent,parentItem->child(parentItem->childCount()-1));

                childColumnOffset = childIndex+1;
                return;
            }
            else
            {
                return;
            }

        }

        if(childColumnOffset > 0)
        {
            //this occurs when there are multiple children!
            int columnCount = updateMethodModel(parentItem->childCount(),parentItem);
            widgetParent->setColumnCount(columnCount+1);

            while(parentItem->childCount() != 0)
            {
                QTreeWidgetItem *uselessChild = parentItem->child(parentItem->childCount()-1);
                parentItem->removeChild(uselessChild);
            }

            //then we recreate the tree with all the items
            for(unsigned int restoreCycle = 0;restoreCycle<childColumnOffset;restoreCycle++)
            {
                if(restoreCycle != childIndex)
                {
                    addChildRow(widgetParent,parentItem,true,customObjectMethods.layoutType.at(restoreCycle),rootIndex,ifIndex);
                }
                else if(childIndex != parentItem->childCount()-1 && restoreCycle == childIndex)
                {
                    updateLayout(currentLayoutType,nCol,text,widgetParent,parentItem,true,rootIndex,restoreCycle,ifIndex);
                }
                else
                {
                    updateLayout(currentLayoutType,nCol,text,widgetParent,parentItem,false,rootIndex,0,ifIndex);
                }

            }

        }
        else
        {
            while(parentItem->childCount() != 0)
            {
                parentItem->removeChild(parentItem->child(parentItem->childCount()-1));
            }

            updateLayout(currentLayoutType,nCol,text,widgetParent,parentItem,false,rootIndex,0,ifIndex);

            widgetParent->setColumnCount(childItem->data(0,Qt::UserRole+2).toInt()+1);
        }

        addChildRow(widgetParent,parentItem->child(parentItem->childCount()-1));
        widgetParent->setColumnCount(9);

        for(int i = 0; i < widgetParent->columnCount();i++)
        {
            widgetParent->resizeColumnToContents(i);
        }

    }
}

void customMethodConstructorWindow::OnExecuteLogicComboIndexChanged(const QString& text)
{
    QComboBox* currentCombo = qobject_cast<QComboBox*>(sender());
    if (currentCombo)
    {
         QTreeWidgetItem *childItem = currentCombo->property("childItem").value<QTreeWidgetItem*>();
        if(text == "Else")
        {
            loggedIfIndex++;
            addChildRow(childItem->treeWidget(),childItem->parent(),false,0,0,loggedIfIndex);
            //AddRoot("Method Two");
        }
    }
}

void transitionUserSelection(methodSetup *updatingModel,int startingLayout,int updatedLayout,int methodCycle,int selectedCol)
{
    int startingSize,updatedSize,sizeDiff;

    switch(startingLayout)
    {
        case 0:
        {
            startingSize = 6;
        }
            break;
        case 1:
        {
            startingSize = 6;
        }
            break;
        case 2:
        {
            startingSize = 5;
        }
            break;
        case 3:
        {
            startingSize = 5;
        }
            break;
        case 4:
        {
            startingSize = 5;
        }
            break;
        case 5:
        {
            startingSize = 5;
        }
            break;
        case 6:
        {
            startingSize = 4;
        }
            break;
        case 7:
        {
            startingSize = 5;
        }
            break;
    }

    switch(updatedLayout)
    {
        case 0:
        {
            updatedSize = 6;
        }
            break;
        case 1:
        {
            updatedSize = 6;
        }
            break;
        case 2:
        {
            updatedSize = 5;
        }
            break;
        case 3:
        {
            updatedSize = 5;
        }
            break;
        case 4:
        {
            updatedSize = 5;
        }
            break;
        case 5:
        {
            updatedSize = 5;
        }
            break;
        case 6:
        {
            updatedSize = 4;
        }
            break;
        case 7:
        {
            updatedSize = 5;
        }
            break;
    }

    sizeDiff = updatedSize-startingSize;

    std::cout << " ________________________________________ \n" << "|Size diff is " << sizeDiff << "                        | \n"
              << "|Transitioning from layout " << startingLayout << " to layout " << updatedLayout << "| \n **************************************** \n";

    switch(sizeDiff)
    {
        case -1:
        {
            int cycleResized = 0;
            std::vector<double> saveValues;
            while(cycleResized<updatingModel->userSelection.at(methodCycle).size())
            {
                if(cycleResized != selectedCol+1)
                {
                    saveValues.push_back(updatingModel->userSelection.at(methodCycle).at(cycleResized));
                }

                cycleResized++;
            }

            for(int i = 0;i<cycleResized-1;i++)
            {
                updatingModel->userSelection.at(methodCycle).at(i) = saveValues.at(i);
            }
            updatingModel->userSelection.at(methodCycle).resize(cycleResized);

            switch(updatedLayout)
            {
            case 4:
            {
                updatingModel->userSelection.at(methodCycle).at(selectedCol - 3) -= sizeDiff;
            }
                break;
            case 5:
            {
                updatingModel->userSelection.at(methodCycle).at(selectedCol + 2) -= sizeDiff;
            }
                break;
            case 6:
            {
                selectedCol == 3 ? updatingModel->userSelection.at(methodCycle).at(selectedCol - 2) -= sizeDiff : updatingModel->userSelection.at(methodCycle).at(selectedCol + 2) -= sizeDiff;

            }
                break;
            }

        }
            break;
        case 1:
        {

            int vecSize = updatingModel->userSelection.at(methodCycle).size();
            std::vector<double> saveValues;
            for(uint i = 0;i<vecSize;i++)
            {
                saveValues.push_back(updatingModel->userSelection.at(methodCycle).at(i));
            }

            updatingModel->userSelection.at(methodCycle).clear();
            updatingModel->userSelection.at(methodCycle).resize(vecSize+sizeDiff);

            uint cycleOld = 0,cycleNew = 0;

            while(cycleNew<vecSize+1)
            {
                if(cycleNew != selectedCol+1)
                {
                    updatingModel->userSelection.at(methodCycle).at(cycleNew) = saveValues.at(cycleOld);
                    cycleOld++;
                }

                cycleNew++;
            }

            if(selectedCol == 1 && updatedLayout != 1)// && updatedLayout != 0)
            {
                updatingModel->userSelection.at(methodCycle).at(selectedCol + 3) -= sizeDiff;
            }
            else if(updatedLayout != 1 && updatedLayout != 0)
            {
                updatingModel->userSelection.at(methodCycle).at(selectedCol - 2) -= sizeDiff;
            }
            else if(updatedLayout == 1 && selectedCol != 1)
            {
                updatingModel->userSelection.at(methodCycle).at(selectedCol - 3) -= sizeDiff;
            }

            updatingModel->userSelection.at(methodCycle).back() = saveValues.back();
        }
            break;
        default:
        {
            if(sizeDiff != 0)
            {
                std::cout << "caught in default \n";
            }

        }
            break;
    }

}

void customMethodConstructorWindow::updateLayout(unsigned currentLayout, unsigned selectedColumn, QString selectedText,
                                                 QTreeWidget *widgetParent, QTreeWidgetItem *parentItem,
                                                 bool fillInputs,unsigned rootIndex,int methodCycle,int ifIndex)
{

    int updatedLayout = -1;

    switch (currentLayout) {
    case 0:
    {
        if(selectedColumn == 1)
        {
            updatedLayout = 5;
        }
        else
        {
            updatedLayout = 2;
        }

    }
        break;
    case 1:
    {
        if(selectedColumn == 1)
        {
            updatedLayout = 6;
        }
        else
        {
            updatedLayout = 4;
        }
    }
        break;
    case 2:
    {
        if(selectedText == "Static Var" && selectedColumn == 4)
        {
            updatedLayout = 0;
        }
        else if(selectedColumn == 1 && selectedText == "Static Var")
        {
            updatedLayout = 4;
        }
        else
        {
            updatedLayout = 6;
        }
    }
        break;
    case 3:
    {
        if(selectedColumn == 3 && selectedText == "Static Var")
        {
            updatedLayout = 5;
        }
        else if(selectedColumn == 1 && selectedText == "Static Var")
        {
            updatedLayout = 1;
        }
        else
        {
            updatedLayout = 6;
        }
    }
        break;
    case 4:
    {
        if(selectedColumn == 4 && selectedText == "Time Passed")
        {
            updatedLayout = 1;
        }
        else if(selectedColumn == 1 && selectedText == "Time Passed")
        {
            updatedLayout = 2;
        }
        else
        {
            updatedLayout = 6;
        }
    }
        break;
    case 5:
    {
        if(selectedText == "Static Var")
        {
            updatedLayout = 0;
        }
        else if(selectedText == "Time Passed" && selectedColumn == 1)
        {
            updatedLayout = 0;
        }
        else if(selectedText == "Time Passed" && selectedColumn == 3)
        {
            updatedLayout = 7;
        }
        else
        {
            updatedLayout = 6;
        }
    }
        break;
    case 6:
    {
        if(selectedColumn == 1 && selectedText == "Time Passed")
        {
            updatedLayout = 2;
        }
        else if(selectedColumn == 1 && selectedText == "Static Var")
        {
            updatedLayout = 4;
        }
        else if(selectedColumn == 3 && selectedText == "Static Var")
        {
            updatedLayout = 5;
        }
        else
        {
            updatedLayout = 3;
        }
    }
        break;
    case 7:
    {
        if(selectedColumn == 3)
        {
            updatedLayout = 6;
        }
        else if(selectedText == "Static Var")
        {
            updatedLayout = 1;
        }
    }
        break;
    }

    if(updatedLayout != -1)
    {
        if(fillInputs)
        {
            transitionUserSelection(&customObjectMethods,currentLayout,updatedLayout,methodCycle,selectedColumn);
        }

        addChildRow(widgetParent,parentItem,fillInputs,updatedLayout,rootIndex,ifIndex);
    }

}

int customMethodConstructorWindow::updateMethodModel(int cycleCount,QTreeWidgetItem *parentItem)
{
    int maxCol = 0;

    customObjectMethods.selectionType.clear();
    customObjectMethods.userSelection.clear();
    customObjectMethods.layoutType.clear();

    for(int cycleChildren = 0; cycleChildren < cycleCount;cycleChildren++)
    {
        customObjectMethods.selectionType.resize(cycleChildren+1);
        customObjectMethods.userSelection.resize(cycleChildren+1);

        QTreeWidgetItem *saveChild = parentItem->child(cycleChildren);
        int columnCount = saveChild->data(0,Qt::UserRole+2).toInt();
        customObjectMethods.layoutType.push_back(saveChild->data(0,Qt::UserRole+3).toInt());

        for(int i = 0;i<columnCount;i++)
        {
            int widgetType = saveChild->data(i,Qt::UserRole+1).toInt();
            switch (widgetType) {
            case 0:
            {
                QComboBox *methodBlockCombo = saveChild->data(i,Qt::UserRole).value<QComboBox*>();
                if(methodBlockCombo)
                {
                    int index = methodBlockCombo->currentIndex();
                    customObjectMethods.userSelection.at(cycleChildren).push_back((double)index);
                    customObjectMethods.selectionType.at(cycleChildren).push_back(0);

                    if(maxCol < saveChild->data(0,Qt::UserRole+2).toInt())
                    {
                        maxCol = saveChild->data(0,Qt::UserRole+2).toInt();
                    }
                    //std::cout << "Caught method block at col: " << i << std::endl;
                }
                else
                {
                    //std::cout << "Missed method block at col: " << i << std::endl;
                }

            }
                break;
            case 1:
            {
                QLineEdit *staticVarLE = saveChild->data(i,Qt::UserRole).value<QLineEdit*>();

                if(staticVarLE)
                {
                    customObjectMethods.userSelection.at(cycleChildren).push_back((staticVarLE->text().toDouble()));
                    customObjectMethods.selectionType.at(cycleChildren).push_back(1);
                    //std::cout << "Caught static var at col: " << i << std::endl;
                }
                else
                {
                    //std::cout << "Missed static var at col: " << i << std::endl;
                }

            }
                break;
            case 2:
            {
                QComboBox *timeCombo = saveChild->data(i,Qt::UserRole).value<QComboBox*>();

                if(timeCombo)
                {
                    customObjectMethods.userSelection.at(cycleChildren).push_back((double)timeCombo->currentIndex());
                    customObjectMethods.selectionType.at(cycleChildren).push_back(2);
                    //std::cout << "Caught time combo at col: " << i << std::endl;
                }
                else
                {
                    //std::cout << "Missed time combo at col: " << i << std::endl;
                }
            }
                break;
            }

        }
    }

    return maxCol;
}


void customMethodConstructorWindow::on_addMethodBtn_clicked()
{
    QTreeWidgetItem *parentItem = ui->methodSetupTreeWidget->topLevelItem(0);
    updateMethodModel(parentItem->childCount(),parentItem);

    ui->newMethodBtn->setEnabled(true);

    ui->addMethodBtn->setEnabled(true);
}

void customMethodConstructorWindow::on_confirmBtn_clicked()
{
    QTreeWidgetItem *parentItem = ui->methodSetupTreeWidget->topLevelItem(0);
    updateMethodModel(parentItem->childCount(),parentItem);
    emit updatedMethodObject(customObjectMethods);
    close();
}
