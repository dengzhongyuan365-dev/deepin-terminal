#include "ut_remotemanagementpanel_test.h"

#include "remotemanagementpanel.h"
#include "utils.h"
#include "stub.h"
#include "service.h"

//Google GTest 相关头文件
#include <gtest/gtest.h>

//Qt单元测试相关头文件
#include <QTest>
#include <QtGui>

UT_RemoteManagementPanel_Test::UT_RemoteManagementPanel_Test()
{
}
void stub_return()
{
    return;
}

void UT_RemoteManagementPanel_Test::SetUp()
{
    //远程服务器管理
    m_serverConfigManager = ServerConfigManager::instance();
    // 初始化远程管理数据
    m_serverConfigManager->initServerConfig();

    prepareData();
}

int UT_RemoteManagementPanel_Test::getServerConfigCount()
{
    QList<ServerConfig *> serverConfigList;
    QMap<QString, QList<ServerConfig *>> severConfigs = ServerConfigManager::instance()->getServerConfigs();
    for (QMap<QString, QList<ServerConfig *>>::iterator iter = severConfigs.begin(); iter != severConfigs.end(); iter++) {
        QList<ServerConfig *> value = iter.value();
        for (int i = 0; i < value.size(); i++) {
            serverConfigList.append(value.at(i));
        }
    }

    qDebug() << serverConfigList.count();
    return serverConfigList.count();
}

void UT_RemoteManagementPanel_Test::prepareData()
{
    ServerConfigManager *serverConfigManager = ServerConfigManager::instance();
    serverConfigManager->initServerConfig();

    int serverConfigCount = getServerConfigCount();
    qDebug() << serverConfigCount << endl;

    QString groupName = QString("group_01");

    qsrand(static_cast<uint>(time(nullptr)));
    ServerConfig *config = new ServerConfig();
    config->m_serverName = QString("new_server_%1").arg(Utils::getRandString());
    config->m_address = QString("192.168.10.%1").arg(qrand() % 255);
    config->m_userName = QString("zhangsan");
    config->m_password = QString("123");
    config->m_privateKey = QString("");
    config->m_port = QString("");
    config->m_group = groupName;
    config->m_path = QString("");
    config->m_command = QString("");
    config->m_encoding = QString("");
    config->m_backspaceKey = QString("");
    config->m_deleteKey = QString("");

    serverConfigManager->saveServerConfig(config);
    EXPECT_EQ(getServerConfigCount(), serverConfigCount + 1);

    int serverCount = serverConfigManager->getServerCount(config->m_group);

    ServerConfig *currConfig = serverConfigManager->getServerConfig(config->m_serverName);
    EXPECT_NE(currConfig, nullptr);

    QTest::qWait(UT_WAIT_TIME);

    qsrand(static_cast<uint>(time(nullptr)));
    ServerConfig *newConfig = new ServerConfig();
    newConfig->m_serverName = QString("new_server_%1").arg(Utils::getRandString());
    newConfig->m_address = QString("192.168.10.%1").arg(qrand() % 255);
    newConfig->m_userName = QString("uos");
    newConfig->m_password = QString("123456");
    newConfig->m_privateKey = QString("");
    newConfig->m_port = QString("");
    newConfig->m_group = groupName;
    newConfig->m_path = QString("");
    newConfig->m_command = QString("");
    newConfig->m_encoding = QString("");
    newConfig->m_backspaceKey = QString("");
    newConfig->m_deleteKey = QString("");
    serverConfigManager->modifyServerConfig(newConfig, currConfig);
    EXPECT_EQ(newConfig, serverConfigManager->getServerConfig(newConfig->m_serverName));

    EXPECT_GE(serverCount, 1);

    serverConfigManager->delServerConfig(newConfig);
    EXPECT_EQ(getServerConfigCount(), serverConfigCount);

    EXPECT_EQ(serverConfigManager->getServerCount(groupName), serverCount - 1);

    QTest::qWait(UT_WAIT_TIME);
}

void UT_RemoteManagementPanel_Test::TearDown()
{
}

#ifdef UT_REMOTEMANAGEMENTPANEL_TEST
TEST_F(UT_RemoteManagementPanel_Test, refreshPanel)
{
    const int PANEL_WIDTH = 242;
    const int PANEL_HEIGHT = 600;
    RemoteManagementPanel panel;
    panel.resize(PANEL_WIDTH, PANEL_HEIGHT);
    panel.show();

    panel.refreshPanel();

#ifdef ENABLE_UI_TEST
    QTest::qWait(UT_WAIT_TIME);
#endif
}

TEST_F(UT_RemoteManagementPanel_Test, setFocusInPanel)
{
    const int PANEL_WIDTH = 242;
    const int PANEL_HEIGHT = 600;
    RemoteManagementPanel panel;
    panel.resize(PANEL_WIDTH, PANEL_HEIGHT);
    panel.show();

    panel.setFocusInPanel();
    int listIndex = panel.getListIndex();
    qDebug() << "listIndex:" << listIndex << endl;
    EXPECT_EQ(listIndex, -1);

    // 最后一种情况
    panel.m_searchEdit->hide();
    panel.m_listWidget->hide();
    panel.setFocusInPanel();

#ifdef ENABLE_UI_TEST
    QTest::qWait(UT_WAIT_TIME);
#endif
}

TEST_F(UT_RemoteManagementPanel_Test, setFocusBack)
{
    const int PANEL_WIDTH = 242;
    const int PANEL_HEIGHT = 600;
    RemoteManagementPanel panel;
    panel.resize(PANEL_WIDTH, PANEL_HEIGHT);
    panel.show();

    // 创建分组
    ServerConfig groupConfig;
    groupConfig.m_serverName = "group_item";
    groupConfig.m_address = "127.0.0.1";
    groupConfig.m_group = "group2020";
    groupConfig.m_userName = "dzw";
    groupConfig.m_port = "22";
    // 分组数据存储结构
    QList<ServerConfig *> list;
    list.append(&groupConfig);
    // 数据存储结构
    ServerConfigManager::instance()->m_serverConfigs.insert("group2020", list);
    // 刷新列表,填充数据
    panel.refreshPanel();
    // 不要焦点 => 没有主界面会崩
    // panel.setFocusBack("group2020");


    // 要焦点
    panel.m_listWidget->setFocusState(true);
    panel.setFocusBack("group2020");
    int listIndex = panel.getListIndex();
    EXPECT_GE(listIndex, 0);

    // 找不到情况
    panel.setFocusBack("group1988");
    listIndex = panel.getListIndex();
    EXPECT_GE(listIndex, 0);
    ServerConfigManager::instance()->m_serverConfigs.clear();
#ifdef ENABLE_UI_TEST
    QTest::qWait(UT_WAIT_TIME);
#endif
}

TEST_F(UT_RemoteManagementPanel_Test, clearListFocus)
{
    const int PANEL_WIDTH = 242;
    const int PANEL_HEIGHT = 600;
    RemoteManagementPanel panel;
    panel.resize(PANEL_WIDTH, PANEL_HEIGHT);
    panel.show();

    panel.clearListFocus();
    EXPECT_EQ(panel.m_pushButton->hasFocus(), false);
    EXPECT_EQ(panel.m_listWidget->hasFocus(), false);
    EXPECT_EQ(panel.m_searchEdit->hasFocus(), false);
    EXPECT_EQ(panel.m_listWidget->currentIndex(), -1);

#ifdef ENABLE_UI_TEST
    QTest::qWait(UT_WAIT_TIME);
#endif
}

TEST_F(UT_RemoteManagementPanel_Test, refreshSearchState)
{
    const int PANEL_WIDTH = 242;
    const int PANEL_HEIGHT = 600;
    RemoteManagementPanel panel;
    panel.resize(PANEL_WIDTH, PANEL_HEIGHT);
    panel.show();

    ServerConfigManager::instance()->m_serverConfigs.clear();
    // 一个也没有,搜索框隐藏
    panel.refreshSearchState();
    //添加数据,显示搜索框
    ServerConfig config;
    config.m_serverName = "test_item";
    config.m_address = "127.0.0.1";
    config.m_userName = "dzw";
    config.m_port = "22";
    ServerConfig config2;
    config2.m_serverName = "test_item2";
    config2.m_address = "127.0.0.1";
    config2.m_userName = "dzw";
    config2.m_port = "22";
    // 分组数据存储结构
    QList<ServerConfig *> list;
    list.append(&config);
    list.append(&config2);
    // 数据存储结构
    ServerConfigManager::instance()->m_serverConfigs.insert("", list);
    // 刷新列表,填充数据
    panel.refreshPanel();
    // 两个数据,搜索框显示
    panel.refreshSearchState();
    // list中数据的数量
    int count = panel.m_listWidget->count();
    EXPECT_EQ(count, 2);
    ServerConfigManager::instance()->m_serverConfigs.clear();

#ifdef ENABLE_UI_TEST
    QTest::qWait(UT_WAIT_TIME);
#endif
}

/*******************************************************************************
 1. @函数:    onItemClicked
 2. @作者:    ut000610 戴正文
 3. @日期:    2020-12-22
 4. @说明:    当前项被点击
*******************************************************************************/
TEST_F(UT_RemoteManagementPanel_Test, onItemClicked)
{
    // 初始化一个panel
    RemoteManagementPanel remotePanel;
    remotePanel.show();
    // 初始化一个可点击的数据
    //添加数据,显示搜索框
    ServerConfig config;
    config.m_serverName = "test_item";
    config.m_address = "127.0.0.1";
    config.m_userName = "dzw";
    config.m_port = "22";
    // 添加数据到列表中
    // 分组数据存储结构
    QList<ServerConfig *> list;
    list.append(&config);
    // 数据存储结构
    ServerConfigManager::instance()->m_serverConfigs.insert("", list);
    // 刷新列表,填充数据
    remotePanel.refreshPanel();

    // 模拟数据被点击
    // 传来被点击的key值
    remotePanel.onItemClicked("test_item");

    // 传来错误的值
    remotePanel.onItemClicked("test_item2");
    ServerConfigManager::instance()->m_serverConfigs.clear();
}

/*******************************************************************************
 1. @函数:    showCurSearchResult
 2. @作者:    ut000610 戴正文
 3. @日期:    2020-12-22
 4. @说明:    测试接口是否可以正常调用
*******************************************************************************/
TEST_F(UT_RemoteManagementPanel_Test, showCurSearchResult)
{
    // 没有搜索内容
    RemoteManagementPanel remotePanel;
    remotePanel.show();
    remotePanel.showCurSearchResult();
    // 有搜索内容
    remotePanel.m_searchEdit->setText("aaa");
    remotePanel.showCurSearchResult();
}

/*******************************************************************************
 1. @函数:    showAddServerConfigDlg
 2. @作者:    ut000610 戴正文
 3. @日期:    2020-12-22
 4. @说明:    弹出添加数据弹窗
*******************************************************************************/
TEST_F(UT_RemoteManagementPanel_Test, showAddServerConfigDlg)
{
    // 没有搜索内容
    RemoteManagementPanel remotePanel;
    remotePanel.show();
    // 打桩
    Stub s;
    s.set(ADDR(Service, setIsDialogShow), stub_return);

    // 显示弹窗
    remotePanel.showAddServerConfigDlg();
    ServerConfigOptDlg *dialog = remotePanel.findChild<ServerConfigOptDlg *>();
    // 发送弹窗添加失败
    if (dialog) {
        emit dialog->reject();
    }

    // 设置添加按钮有焦点
    remotePanel.m_pushButton->setFocus();
    remotePanel.showAddServerConfigDlg();
    dialog = remotePanel.findChild<ServerConfigOptDlg *>();
    // 发送弹窗添加失败
    if (dialog) {
        emit dialog->accept();
    }
    // 打桩还原
    s.reset(ADDR(Service, setIsDialogShow));
}

/*******************************************************************************
 1. @函数:    lambda
 2. @作者:    ut000610 戴正文
 3. @日期:    2020-12-23
 4. @说明:    测试部分lambda表达式是否正常响应信号
*******************************************************************************/
TEST_F(UT_RemoteManagementPanel_Test, lambda)
{
    // 初始化界面
    RemoteManagementPanel remotePanel;
    remotePanel.show();
    remotePanel.m_isShow = true;
    // 刷新界面
    emit ServerConfigManager::instance()->refreshList();
#ifdef ENABLE_UI_TEST
    QTest::qWait(UT_WAIT_TIME);
#endif

    // 焦点切入切出
    // 切出理由
    emit remotePanel.m_listWidget->focusOut(Qt::TabFocusReason);
#ifdef ENABLE_UI_TEST
    QTest::qWait(UT_WAIT_TIME);
#endif
    // 列表内没有index
    EXPECT_EQ(remotePanel.m_listWidget->m_currentIndex, -1);
    emit remotePanel.m_listWidget->focusOut(Qt::BacktabFocusReason);
#ifdef ENABLE_UI_TEST
    QTest::qWait(UT_WAIT_TIME);
#endif
    EXPECT_EQ(remotePanel.m_listWidget->m_currentIndex, -1);

}
#endif
