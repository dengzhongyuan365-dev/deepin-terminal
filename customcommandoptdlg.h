#ifndef CUSTOMCOMMANDOPTDLG_H
#define CUSTOMCOMMANDOPTDLG_H

#include <DDialog>
#include <DAbstractDialog>
#include <DLineEdit>
#include <DKeySequenceEdit>

#include <QAction>
#include <QWidget>

DWIDGET_USE_NAMESPACE

class CustomCommandOptDlg : public DAbstractDialog
{
    Q_OBJECT
public:
    enum CustomCmdOptType {
        CCT_ADD,     // the add type of custom command operation
        CCT_MODIFY,  // the modify type of custom command operation
    };

    explicit CustomCommandOptDlg(CustomCmdOptType type = CCT_ADD, QAction *curAction = nullptr,
                                 QWidget *parent = nullptr);
    ~CustomCommandOptDlg();

    QAction *getCurCustomCmd();
    bool isDelCurCommand();

private slots:
    void slotAddSaveButtonClicked();
    void slotDelCurCustomCommand();

private:
    CustomCmdOptType m_type;
    QAction *m_action = nullptr;
    QAction *m_newAction = nullptr;
    DLineEdit *m_nameLineEdit = nullptr;
    DLineEdit *m_commandLineEdit = nullptr;
    DKeySequenceEdit *m_shortCutsLineEdit = nullptr;
    bool m_bDelOpt;
};

#endif  // CUSTOMCOMMANDOPTDLG_H
