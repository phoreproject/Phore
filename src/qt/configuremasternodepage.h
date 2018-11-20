// Copyright (c) 2018 The Phore developers
// Copyright (c) 2018 The Curium developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_CONFIGUREMASTERNODEPAGE_H
#define BITCOIN_QT_CONFIGUREMASTERNODEPAGE_H

#include "masternodelist.h"

#include <QAbstractButton>
#include <QAction>
#include <QDialog>
#include <QList>
#include <QMenu>
#include <string>
#include <QPoint>
#include <QString>
#include <QTreeWidgetItem>
#include <univalue.h>
#include "wallet.h"

class WalletModel;
class MasternodeList;

namespace Ui
{
class ConfigureMasternodePage;
}

QT_BEGIN_NAMESPACE
class QDataWidgetMapper;
QT_END_NAMESPACE

/** Dialog for editing an address and associated information.
 */
class ConfigureMasternodePage : public QDialog
{
    Q_OBJECT

public:
    enum Mode {
        NewConfigureMasternode,
        EditConfigureMasternode
    };

    explicit ConfigureMasternodePage(Mode mode, QWidget* parent);
    ~ConfigureMasternodePage();

	void counter(int counter);
    void loadAlias(QString strAlias);
    void loadIP(QString strIP);
    void loadPrivKey(QString strPrivKey);
    void loadTxHash(QString strTxHash);
    void loadOutputIndex(QString strOutputIndex);
	void updateAlias(std::string Alias, std::string IP, std::string PrivKey, std::string TxHash, std::string OutputIndex);
	int getCounters()
	{
		return counters;
	}
	
	int setCounters(int counter)
	{
		counters = counter;
	}

    QString getAddress() const;
    void setAddress(const QString& address);

public slots:
    void accept();
    void on_AutoFillPrivKey_clicked();
    void on_AutoFillOutputs_clicked();

private:
    void saveCurrentRow();
	int counters;
    Ui::ConfigureMasternodePage* ui;
    QDataWidgetMapper* mapper;
    Mode mode;

    QString address;
};

#endif // BITCOIN_QT_EDITADDRESSDIALOG_H