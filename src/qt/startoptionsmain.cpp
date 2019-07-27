//
// Created by Kolby on 6/19/2019.
//
#include "startoptionsmain.h"
#include "ui_startoptionsmain.h"
#include <startoptionsdialog.h>

#include <qt/guiutil.h>
#include <QDebug>
#include <QFileDialog>
#include <iostream>
#include "random.h"

#include <ui_interface.h>
#include "wallet/bip39.h"

#include <boost/algorithm/string.hpp>

#include <QKeyEvent>
#include <QMessageBox>
#include <QPushButton>

StartOptionsMain::StartOptionsMain(QWidget *parent)
        : QDialog(parent), ui(new Ui::StartOptionsMain)
        {
    ui->setupUi(this);
    this->setStyleSheet(GUIUtil::loadStyleSheet());

    this->setWindowTitle("Phore Wallet Setup");


    wordsList = CMnemonic::getListOfAllWordInLanguage();

    for(unsigned long i=0; i< wordsList.size() ; i++) {
        qWordList.append(QString::fromStdString(wordsList[i]));
    }

    this->setContentsMargins(0,0,0,0);
    ui->QStackTutorialContainer->setContentsMargins(0,0,0,10);
    ui->Back->setVisible(false);
    ui->Next->setVisible(false);
    startOptions = new StartOptions(this);
    ui->QStackTutorialContainer->addWidget(startOptions);
    ui->QStackTutorialContainer->setCurrentWidget(startOptions);


}

StartOptionsMain::~StartOptionsMain() {
    delete ui;
}

void StartOptionsMain::on_NewWallet_clicked()
{
    pageNum = 2;
    ui->NewWallet->setVisible(false);
    ui->RestoreWallet->setVisible(false);
    ui->Back->setVisible(true);
    ui->Next->setVisible(true);
    rows = startOptions->getRows();

    if (rows == 4) {
        words = CMnemonic::Generate(256);
    } else {
        words = CMnemonic::Generate(128);
    }



    startOptionsRevealed = new StartOptionsRevealed(words, rows, this);
    ui->QStackTutorialContainer->addWidget(startOptionsRevealed);
    ui->QStackTutorialContainer->setCurrentWidget(startOptionsRevealed);
}

void StartOptionsMain::on_RestoreWallet_clicked()
{
    pageNum = 4;
    ui->NewWallet->setVisible(false);
    ui->RestoreWallet->setVisible(false);
    ui->Back->setVisible(true);
    ui->Next->setVisible(true);
    rows = startOptions->getRows();

    startOptionsRestore = new StartOptionsRestore(qWordList, rows, this);
    ui->QStackTutorialContainer->addWidget(startOptionsRestore);
    ui->QStackTutorialContainer->setCurrentWidget(startOptionsRestore);
}

void StartOptionsMain::on_Back_clicked()
{
    /*  Pages
     * Page 1 : Main page were you select the amount of words and the option
     *      Path 1 : Create wallet
     *          Page 2 : Shows you your words
     *          Page 3 : Order the words
     *      Path 2 : Restore wallet
     *          Page 4 Enter words to restore
     */
    switch (pageNum){
        case 1:{
            // does nothing as you can not click back on page one
        }
        case 2:{
            pageNum = 1;
            ui->NewWallet->setVisible(true);
            ui->RestoreWallet->setVisible(true);
            ui->Back->setVisible(false);
            ui->Next->setVisible(false);
            ui->QStackTutorialContainer->setCurrentWidget(startOptions);
            break;
        }
        case 3:{
            pageNum = 2;
            ui->QStackTutorialContainer->addWidget(startOptionsRevealed);
            ui->QStackTutorialContainer->setCurrentWidget(startOptionsRevealed);
            break;
        }
        case 4:{
            pageNum = 1;
            ui->NewWallet->setVisible(true);
            ui->RestoreWallet->setVisible(true);
            ui->Back->setVisible(false);
            ui->Next->setVisible(false);
            ui->QStackTutorialContainer->setCurrentWidget(startOptions);
            break;
        }
    }
}

void StartOptionsMain::on_Next_clicked()
{
    /*  Pages
     * Page 1 : Main page were you select the amount of words and the option
     *      Path 1 : Create wallet
     *          Page 2 : Shows you your words
     *          Page 3 : Order the words
     *      Path 2 : Restore wallet
     *          Page 4 Enter words to restore
     */
    switch (pageNum){
        case 1:{
            // does nothing as you can not click back on page one
        }

        case 2:{
            pageNum = 3;
            startOptionsSort = new StartOptionsSort(words, rows, this);
            ui->QStackTutorialContainer->addWidget(startOptionsSort);
            ui->QStackTutorialContainer->setCurrentWidget(startOptionsSort);
            break;
        }
        case 3:{
            words_empty_str = "";
            std::list<QString> word_str = startOptionsSort->getOrderedStrings();
            // reverses the lists order
            word_str.reverse();
            for (QString &q_word : word_str) {
                if (words_empty_str.empty())
                    words_empty_str = q_word.toStdString();
                else
                    words_empty_str += "" + q_word.toStdString();
            }

            words_mnemonic = "";
            for (std::string &q_word : words) {
                if (words_mnemonic.empty())
                    words_mnemonic = q_word;
                else
                    words_mnemonic += "" + q_word;
            }
            boost::trim_right(words_empty_str);
            boost::trim_right(words_mnemonic);
            if(words_empty_str != words_mnemonic) {
                QString error = "Your words are in the wrong order Please try again";
                StartOptionsDialog dlg(error, this);
                dlg.exec();
            } else {
                wordsDone = words;
                QApplication::quit();
            }
            break;
        }
        case 4:{
            std::vector<std::string> word_str = startOptionsRestore->getOrderedStrings();

            std::string seedphrase = "";
            for (std::string &q_word : word_str) {
                if (seedphrase.empty())
                    seedphrase = q_word;
                else
                    seedphrase += " " + q_word;
            }
            // reverses the lists order
            if(CMnemonic::Check(seedphrase)){
                wordsDone = word_str;
                QApplication::quit();
            } else {
                QString error = "Your words are invalid. This could be because, you are using non real words or that they may be misspelt";
                StartOptionsDialog dlg(error, this);
                dlg.exec();
            }

            break;
        }
    }
}
