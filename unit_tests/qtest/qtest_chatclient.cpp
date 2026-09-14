#include <QtTest>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSignalSpy>
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QHostAddress>

#include "chatclient.h"
#include "protocol.h"


class ChatClientTest : public QObject
{
    Q_OBJECT

private:

    void sendJson(QTcpSocket *socket, const QJsonObject &object)
    {
        QVERIFY(socket != nullptr);

        const QByteArray data =
        QJsonDocument(object).toJson(QJsonDocument::Compact);

        QDataStream stream(socket);
        stream.setVersion(QDataStream::Qt_5_7);

        stream << data;

        // Ako još ima podataka koji čekaju na slanje,
        // sačekaj da budu poslati.
        if (socket->bytesToWrite() > 0)
        {
            QVERIFY2(
                socket->waitForBytesWritten(3000),
                     qPrintable(socket->errorString())
            );
        }
    }


    QList<QJsonObject> readMessages(QTcpSocket *socket,
                                    int timeoutMs = 1000)
    {
        QList<QJsonObject> messages;

        if (socket == nullptr)
            return messages;

        if (socket->bytesAvailable() == 0)
            socket->waitForReadyRead(timeoutMs);

        QDataStream stream(socket);
        stream.setVersion(QDataStream::Qt_5_7);

        while (socket->bytesAvailable() > 0)
        {
            QByteArray data;

            stream.startTransaction();
            stream >> data;

            if (!stream.commitTransaction())
                break;

            QJsonParseError error;
            const QJsonDocument document =
            QJsonDocument::fromJson(data, &error);

            if (error.error == QJsonParseError::NoError &&
                document.isObject())
            {
                messages.append(document.object());
            }
        }

        return messages;
    }


    QTcpSocket *connectClient(ChatClient &client,
                              QTcpServer &server)
    {
        if (!server.listen(QHostAddress::LocalHost, 0))
            return nullptr;

        QSignalSpy connectedSpy(
            &client,
            &ChatClient::connected
        );

        client.connect_to_server(
            QHostAddress::LocalHost,
            server.serverPort()
        );

        if (!server.waitForNewConnection(3000))
            return nullptr;

        QTcpSocket *serverSocket =
        server.nextPendingConnection();

        if (serverSocket == nullptr)
            return nullptr;

        /*
         * Čekamo da i ChatClient potvrdi da je njegova
         * strana konekcije zaista uspostavljena.
         */
        if (connectedSpy.count() == 0)
        {
            if (!connectedSpy.wait(3000))
                return nullptr;
        }

        return serverSocket;
    }

private slots:


    void userLeftWithoutUsernameIsIgnored()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        QSignalSpy spy(
            &client,
            &ChatClient::user_left
        );

        QJsonObject message;

        message[mafia_game::KEY_TYPE] =
        mafia_game::message_type_to_string(
            mafia_game::MessageType::LeaveGame
        );

        // KEY_USERNAME namerno nije prosleđen

        sendJson(serverSocket, message);

        QTest::qWait(100);

        QCOMPARE(spy.count(), 0);
    }


    void userJoinedEmitsSignal()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        QSignalSpy spy(
            &client,
            &ChatClient::user_joined
        );

        QJsonObject message;

        message[mafia_game::KEY_TYPE] =
        mafia_game::message_type_to_string(
            mafia_game::MessageType::JoinGame
        );

        message[mafia_game::KEY_USERNAME] =
        QStringLiteral("Ana");

        sendJson(
            serverSocket,
            message
        );

        QTRY_COMPARE_WITH_TIMEOUT(
            spy.count(),
                                  1,
                                  3000
        );

        const QList<QVariant> arguments =
        spy.takeFirst();

        QCOMPARE(
            arguments.at(0).toString(),
                 QStringLiteral("Ana")
        );
    }

    void userLeftEmitsSignal()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        QSignalSpy spy(
            &client,
            &ChatClient::user_left
        );

        QJsonObject message;

        message[mafia_game::KEY_TYPE] =
        mafia_game::message_type_to_string(
            mafia_game::MessageType::LeaveGame
        );

        message[mafia_game::KEY_USERNAME] =
        QStringLiteral("Marko");

        sendJson(
            serverSocket,
            message
        );

        QTRY_COMPARE_WITH_TIMEOUT(
            spy.count(),
                                  1,
                                  3000
        );

        const QList<QVariant> arguments =
        spy.takeFirst();

        QCOMPARE(
            arguments.at(0).toString(),
                 QStringLiteral("Marko")
        );
    }

    void loginSendsCorrectJson()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        client.login(
            QStringLiteral("Ana")
        );

        QTRY_VERIFY_WITH_TIMEOUT(
            serverSocket->bytesAvailable() > 0,
                                 3000
        );

        const QList<QJsonObject> messages =
        readMessages(serverSocket);

        QCOMPARE(
            messages.size(),
                 1
        );

        const QJsonObject message =
        messages.first();

        QCOMPARE(
            message.value(
                mafia_game::KEY_TYPE
            ).toString(),
                 mafia_game::message_type_to_string(
                     mafia_game::MessageType::Login
                 )
        );

        QCOMPARE(
            message.value(
                mafia_game::KEY_USERNAME
            ).toString(),
                 QStringLiteral("Ana")
        );
    }


    void disconnectFromHostEmitsDisconnected()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        QSignalSpy spy(
            &client,
            &ChatClient::disconnected
        );

        client.disconnect_from_host();

        QTRY_COMPARE_WITH_TIMEOUT(
            spy.count(),
                                  1,
                                  3000
        );

        QTRY_VERIFY_WITH_TIMEOUT(
            serverSocket->state() ==
            QAbstractSocket::UnconnectedState,
            3000
        );
    }

    void gameStateUpdateWithoutPlayersIsIgnored()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        QSignalSpy spy(
            &client,
            &ChatClient::game_state_received
        );

        QJsonObject message;

        message[mafia_game::KEY_TYPE] =
        mafia_game::message_type_to_string(
            mafia_game::MessageType::GameStateUpdate
        );

        message[mafia_game::KEY_PHASE] =
        QStringLiteral("day");

        message[mafia_game::KEY_DAY_NUMBER] = 1;

        sendJson(serverSocket, message);

        QTest::qWait(100);

        QCOMPARE(spy.count(), 0);
    }


    void gameStateUpdateWithInvalidPlayersTypeIsIgnored()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        QSignalSpy spy(
            &client,
            &ChatClient::game_state_received
        );

        QJsonObject message;

        message[mafia_game::KEY_TYPE] =
        mafia_game::message_type_to_string(
            mafia_game::MessageType::GameStateUpdate
        );

        message[mafia_game::KEY_PLAYERS] =
        QStringLiteral("not-an-array");

        message[mafia_game::KEY_PHASE] =
        QStringLiteral("day");

        message[mafia_game::KEY_DAY_NUMBER] = 1;

        sendJson(serverSocket, message);

        QTest::qWait(100);

        QCOMPARE(spy.count(), 0);
    }


    void gameStateUpdateWithInvalidPhaseIsIgnored()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        QSignalSpy spy(
            &client,
            &ChatClient::game_state_received
        );

        QJsonObject message;

        message[mafia_game::KEY_TYPE] =
        mafia_game::message_type_to_string(
            mafia_game::MessageType::GameStateUpdate
        );

        message[mafia_game::KEY_PLAYERS] =
        QJsonArray();

        message[mafia_game::KEY_PHASE] = 123;

        message[mafia_game::KEY_DAY_NUMBER] = 1;

        sendJson(serverSocket, message);

        QTest::qWait(100);

        QCOMPARE(spy.count(), 0);
    }


    void gameStateUpdateWithNullDayNumberIsIgnored()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        QSignalSpy spy(
            &client,
            &ChatClient::game_state_received
        );

        QJsonObject message;

        message[mafia_game::KEY_TYPE] =
        mafia_game::message_type_to_string(
            mafia_game::MessageType::GameStateUpdate
        );

        message[mafia_game::KEY_PLAYERS] =
        QJsonArray();

        message[mafia_game::KEY_PHASE] =
        QStringLiteral("day");

        // KEY_DAY_NUMBER postoji, ali je njegova vrednost null
        message[mafia_game::KEY_DAY_NUMBER] =
        QJsonValue::Null;

        sendJson(serverSocket, message);

        QTest::qWait(100);

        // ChatClient treba da ignoriše GameStateUpdate
        // ako je day_number null.
        QCOMPARE(spy.count(), 0);
    }


    void gameStateUpdateSkipsNonObjectPlayer()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        QList<PlayerState> receivedPlayers;
        bool received = false;

        connect(
            &client,
            &ChatClient::game_state_received,
            this,
            [&](const QList<PlayerState> &players,
                const QString &,
                int)
            {
                receivedPlayers = players;
                received = true;
            }
        );

        QJsonArray players;

        players.append(
            QStringLiteral("invalid-player")
        );

        QJsonObject validPlayer;

        validPlayer[mafia_game::KEY_USERNAME] =
        QStringLiteral("Ana");

        validPlayer[mafia_game::KEY_ROLE] =
        mafia_game::role_to_string(
            mafia_game::Role::Citizen
        );

        validPlayer[mafia_game::KEY_ALIVE] = true;

        players.append(validPlayer);

        QJsonObject message;

        message[mafia_game::KEY_TYPE] =
        mafia_game::message_type_to_string(
            mafia_game::MessageType::GameStateUpdate
        );

        message[mafia_game::KEY_PLAYERS] = players;
        message[mafia_game::KEY_PHASE] =
        QStringLiteral("day");
        message[mafia_game::KEY_DAY_NUMBER] = 2;

        sendJson(serverSocket, message);

        QTRY_VERIFY_WITH_TIMEOUT(
            received,
            3000
        );

        QCOMPARE(receivedPlayers.size(), 1);

        QCOMPARE(
            receivedPlayers.first().username,
                 QStringLiteral("Ana")
        );
    }


    void roleAssignmentWithoutRoleIsIgnored()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        QSignalSpy spy(
            &client,
            &ChatClient::role_assigned
        );

        QJsonObject message;

        message[mafia_game::KEY_TYPE] =
        mafia_game::message_type_to_string(
            mafia_game::MessageType::RoleAssignment
        );

        message[mafia_game::KEY_USERNAME] =
        QStringLiteral("Ana");

        message[mafia_game::KEY_ALL_ROLES] =
        QJsonArray();

        sendJson(serverSocket, message);

        QTest::qWait(100);

        QCOMPARE(spy.count(), 0);
    }


    void roleAssignmentWithInvalidRolesListIsIgnored()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        QSignalSpy spy(
            &client,
            &ChatClient::role_assigned
        );

        QJsonObject message;

        message[mafia_game::KEY_TYPE] =
        mafia_game::message_type_to_string(
            mafia_game::MessageType::RoleAssignment
        );

        message[mafia_game::KEY_USERNAME] =
        QStringLiteral("Ana");

        message[mafia_game::KEY_ROLE] =
        mafia_game::role_to_string(
            mafia_game::Role::Doctor
        );

        message[mafia_game::KEY_ALL_ROLES] =
        QStringLiteral("invalid");

        sendJson(serverSocket, message);

        QTest::qWait(100);

        QCOMPARE(spy.count(), 0);
    }


    void roleAssignmentWithEmptyUsernameIsIgnored()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        QSignalSpy spy(
            &client,
            &ChatClient::role_assigned
        );

        QJsonObject message;

        message[mafia_game::KEY_TYPE] =
        mafia_game::message_type_to_string(
            mafia_game::MessageType::RoleAssignment
        );

        message[mafia_game::KEY_USERNAME] =
        QStringLiteral("");

        message[mafia_game::KEY_ROLE] =
        mafia_game::role_to_string(
            mafia_game::Role::Doctor
        );

        message[mafia_game::KEY_ALL_ROLES] =
        QJsonArray();

        sendJson(serverSocket, message);

        QTest::qWait(100);

        QCOMPARE(spy.count(), 0);
    }


    void roleAssignmentSkipsNonObjectRoleEntry()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        bool received = false;

        QString receivedUsername;
        mafia_game::Role receivedRole =
        mafia_game::Role::Unknown;

        QHash<QString, mafia_game::Role> receivedRoles;

        connect(
            &client,
            &ChatClient::role_assigned,
            this,
            [&](mafia_game::Role role,
                QString username,
                QHash<QString, mafia_game::Role> roles)
            {
                receivedRole = role;
                receivedUsername = username;
                receivedRoles = roles;
                received = true;
            }
        );

        QJsonArray allRoles;

        allRoles.append(
            QStringLiteral("invalid-entry")
        );

        QJsonObject validPlayer;

        validPlayer[mafia_game::KEY_USERNAME] =
        QStringLiteral("Marko");

        validPlayer[mafia_game::KEY_ROLE] =
        mafia_game::role_to_string(
            mafia_game::Role::Mafia
        );

        allRoles.append(validPlayer);

        QJsonObject message;

        message[mafia_game::KEY_TYPE] =
        mafia_game::message_type_to_string(
            mafia_game::MessageType::RoleAssignment
        );

        message[mafia_game::KEY_USERNAME] =
        QStringLiteral("Ana");

        message[mafia_game::KEY_ROLE] =
        mafia_game::role_to_string(
            mafia_game::Role::Doctor
        );

        message[mafia_game::KEY_ALL_ROLES] =
        allRoles;

        sendJson(serverSocket, message);

        QTRY_VERIFY_WITH_TIMEOUT(
            received,
            3000
        );

        QCOMPARE(
            receivedUsername,
            QStringLiteral("Ana")
        );

        QCOMPARE(
            receivedRole,
            mafia_game::Role::Doctor
        );

        QCOMPARE(receivedRoles.size(), 1);

        QVERIFY(
            receivedRoles.contains(
                QStringLiteral("Marko")
            )
        );

        QCOMPARE(
            receivedRoles.value(
                QStringLiteral("Marko")
            ),
            mafia_game::Role::Mafia
        );
    }


    void userJoinWithoutUsernameIsIgnored()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        QSignalSpy spy(
            &client,
            &ChatClient::user_joined
        );

        QJsonObject message;

        message[mafia_game::KEY_TYPE] =
        mafia_game::message_type_to_string(
            mafia_game::MessageType::JoinGame
        );

        sendJson(serverSocket, message);

        QTest::qWait(100);

        QCOMPARE(spy.count(), 0);
    }

    void votingForNobodyCanBeUsedAsUsername()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        client.login(
            QStringLiteral("voting_for_nobody")
        );

        QTRY_VERIFY_WITH_TIMEOUT(
            serverSocket->bytesAvailable() > 0,
                                 3000
        );

        const QList<QJsonObject> messages =
        readMessages(serverSocket);

        QCOMPARE(messages.size(), 1);

        const QJsonObject message =
        messages.first();

        QCOMPARE(
            message.value(
                mafia_game::KEY_TYPE
            ).toString(),
                 mafia_game::message_type_to_string(
                     mafia_game::MessageType::Login
                 )
        );

        QCOMPARE(
            message.value(
                mafia_game::KEY_USERNAME
            ).toString(),
                 QStringLiteral("voting_for_nobody")
        );
    }

    void gameStateUpdateEmitsParsedPlayers()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        bool signalReceived = false;
        QList<PlayerState> receivedPlayers;
        QString receivedPhase;
        int receivedDay = -1;

        connect(
            &client,
            &ChatClient::game_state_received,
            this,
            [&](const QList<PlayerState> &players,
                const QString &phase,
                int dayNumber)
            {
                signalReceived = true;
                receivedPlayers = players;
                receivedPhase = phase;
                receivedDay = dayNumber;
            }
        );

        QJsonArray players;

        QJsonObject ana;
        ana[mafia_game::KEY_USERNAME] = "Ana";
        ana[mafia_game::KEY_ROLE] =
        mafia_game::role_to_string(
            mafia_game::Role::Citizen
        );
        ana[mafia_game::KEY_ALIVE] = true;

        QJsonObject marko;
        marko[mafia_game::KEY_USERNAME] = "Marko";
        marko[mafia_game::KEY_ROLE] =
        mafia_game::role_to_string(
            mafia_game::Role::Mafia
        );
        marko[mafia_game::KEY_ALIVE] = false;

        players.append(ana);
        players.append(marko);

        QJsonObject message;

        message[mafia_game::KEY_TYPE] =
        mafia_game::message_type_to_string(
            mafia_game::MessageType::GameStateUpdate
        );

        message[mafia_game::KEY_PLAYERS] =
        players;

        message[mafia_game::KEY_PHASE] =
        QStringLiteral("day");

        message[mafia_game::KEY_DAY_NUMBER] =
        3;

        sendJson(
            serverSocket,
            message
        );

        QTRY_VERIFY_WITH_TIMEOUT(
            signalReceived,
            3000
        );

        QCOMPARE(
            receivedPlayers.size(),
                 2
        );

        QCOMPARE(
            receivedPlayers[0].username,
            QStringLiteral("Ana")
        );

        QCOMPARE(
            receivedPlayers[0].role,
            mafia_game::Role::Citizen
        );

        QCOMPARE(
            receivedPlayers[0].alive,
            true
        );

        QCOMPARE(
            receivedPlayers[1].username,
            QStringLiteral("Marko")
        );

        QCOMPARE(
            receivedPlayers[1].role,
            mafia_game::Role::Mafia
        );

        QCOMPARE(
            receivedPlayers[1].alive,
            false
        );

        QCOMPARE(
            receivedPhase,
            QStringLiteral("day")
        );

        QCOMPARE(
            receivedDay,
            3
        );
    }


    void roleAssignmentEmitsCorrectData()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        bool signalReceived = false;

        mafia_game::Role receivedRole =
        mafia_game::Role::Unknown;

        QString receivedUsername;

        QHash<QString, mafia_game::Role>
        receivedRoles;

        connect(
            &client,
            &ChatClient::role_assigned,
            this,
            [&](mafia_game::Role role,
                QString username,
                QHash<QString, mafia_game::Role> roles)
            {
                signalReceived = true;
                receivedRole = role;
                receivedUsername = username;
                receivedRoles = roles;
            }
        );

        QJsonArray allRoles;

        QJsonObject ana;
        ana[mafia_game::KEY_USERNAME] = "Ana";
        ana[mafia_game::KEY_ROLE] =
        mafia_game::role_to_string(
            mafia_game::Role::Doctor
        );

        QJsonObject marko;
        marko[mafia_game::KEY_USERNAME] = "Marko";
        marko[mafia_game::KEY_ROLE] =
        mafia_game::role_to_string(
            mafia_game::Role::Mafia
        );

        allRoles.append(ana);
        allRoles.append(marko);

        QJsonObject message;

        message[mafia_game::KEY_TYPE] =
        mafia_game::message_type_to_string(
            mafia_game::MessageType::RoleAssignment
        );

        message[mafia_game::KEY_USERNAME] =
        QStringLiteral("Ana");

        message[mafia_game::KEY_ROLE] =
        mafia_game::role_to_string(
            mafia_game::Role::Doctor
        );

        message[mafia_game::KEY_ALL_ROLES] =
        allRoles;

        sendJson(
            serverSocket,
            message
        );

        QTRY_VERIFY_WITH_TIMEOUT(
            signalReceived,
            3000
        );

        QCOMPARE(
            receivedRole,
            mafia_game::Role::Doctor
        );

        QCOMPARE(
            receivedUsername,
            QStringLiteral("Ana")
        );

        QCOMPARE(
            receivedRoles.value("Ana"),
                 mafia_game::Role::Doctor
        );

        QCOMPARE(
            receivedRoles.value("Marko"),
                 mafia_game::Role::Mafia
        );
    }


    void waitingRoomListsAreParsed()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        bool signalReceived = false;

        QVector<QString> receivedUnselected;
        QVector<QString> receivedSelected;

        connect(
            &client,
            &ChatClient::waiting_room_lists,
            this,
            [&](QVector<QString> unselected,
                QVector<QString> selected)
            {
                signalReceived = true;
                receivedUnselected = unselected;
                receivedSelected = selected;
            }
        );

        QJsonArray selected;
        QJsonArray unselected;

        QJsonObject ana;
        ana[mafia_game::KEY_USERNAME] =
        QStringLiteral("Ana");

        QJsonObject marko;
        marko[mafia_game::KEY_USERNAME] =
        QStringLiteral("Marko");

        QJsonObject niko;
        niko[mafia_game::KEY_USERNAME] =
        QStringLiteral("Niko");

        selected.append(ana);

        unselected.append(marko);
        unselected.append(niko);

        QJsonObject message;

        message[mafia_game::KEY_TYPE] =
        mafia_game::message_type_to_string(
            mafia_game::MessageType::WaitingRoomLists
        );

        message[mafia_game::KEY_SELECTED] =
        selected;

        message[mafia_game::KEY_UNSELECTED] =
        unselected;

        sendJson(
            serverSocket,
            message
        );

        QTRY_VERIFY_WITH_TIMEOUT(
            signalReceived,
            3000
        );

        QCOMPARE(
            receivedSelected.size(),
                 1
        );

        QCOMPARE(
            receivedSelected[0],
            QStringLiteral("Ana")
        );

        QCOMPARE(
            receivedUnselected.size(),
                 2
        );

        QCOMPARE(
            receivedUnselected[0],
            QStringLiteral("Marko")
        );

        QCOMPARE(
            receivedUnselected[1],
            QStringLiteral("Niko")
        );
    }


    void votingEndedEmitsVotesAndTargets()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        bool signalReceived = false;

        QVector<QString> receivedTargets;

        QVector<QPair<QString, QString>>
        receivedVotes;

        connect(
            &client,
            &ChatClient::voting_round_ended,
            this,
            [&](QVector<QString> votedOut,
                QVector<QPair<QString, QString>> votes)
            {
                signalReceived = true;
                receivedTargets = votedOut;
                receivedVotes = votes;
            }
        );

        QJsonObject votes;

        votes["Ana"] = "Marko";
        votes["Niko"] = "Marko";

        QJsonArray targets;

        targets.append("Marko");

        QJsonObject message;

        message[mafia_game::KEY_TYPE] =
        mafia_game::message_type_to_string(
            mafia_game::MessageType::VotingEnded
        );

        message[mafia_game::KEY_VOTES] =
        votes;

        message[mafia_game::KEY_TARGET] =
        targets;

        sendJson(
            serverSocket,
            message
        );

        QTRY_VERIFY_WITH_TIMEOUT(
            signalReceived,
            3000
        );

        QCOMPARE(
            receivedTargets.size(),
                 1
        );

        QCOMPARE(
            receivedTargets[0],
            QStringLiteral("Marko")
        );

        QCOMPARE(
            receivedVotes.size(),
                 2
        );

        QVERIFY(
            receivedVotes.contains(
                qMakePair(
                    QStringLiteral("Ana"),
                          QStringLiteral("Marko")
                )
            )
        );

        QVERIFY(
            receivedVotes.contains(
                qMakePair(
                    QStringLiteral("Niko"),
                          QStringLiteral("Marko")
                )
            )
        );
    }



    void nightEndedEmitsKilledPlayerAndInfo()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        bool signalReceived = false;

        QString receivedKilled;

        QMap<QString, QString>
        receivedInfo;

        connect(
            &client,
            &ChatClient::night_ended,
            this,
            [&](QString killed,
                QMap<QString, QString> info)
            {
                signalReceived = true;
                receivedKilled = killed;
                receivedInfo = info;
            }
        );

        QJsonObject information;

        information["doctor"] =
        QStringLiteral("Ana");

        information["detective"] =
        QStringLiteral("Marko");

        QJsonObject message;

        message[mafia_game::KEY_TYPE] =
        mafia_game::message_type_to_string(
            mafia_game::MessageType::NightEnded
        );

        message[mafia_game::KEY_TARGET] =
        QStringLiteral("Niko");

        message[mafia_game::KEY_INFO] =
        information;

        sendJson(
            serverSocket,
            message
        );

        QTRY_VERIFY_WITH_TIMEOUT(
            signalReceived,
            3000
        );

        QCOMPARE(
            receivedKilled,
            QStringLiteral("Niko")
        );

        QCOMPARE(
            receivedInfo.value("doctor"),
                 QStringLiteral("Ana")
        );

        QCOMPARE(
            receivedInfo.value("detective"),
                 QStringLiteral("Marko")
        );
    }

    void sendSelectedPlayerSendsCorrectJson()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        client.send_selected_player(
            QStringLiteral("Ana"),
                                    true
        );

        QTRY_VERIFY_WITH_TIMEOUT(
            serverSocket->bytesAvailable() > 0,
                                 3000
        );

        const QList<QJsonObject> messages =
        readMessages(serverSocket);

        QCOMPARE(
            messages.size(),
                 1
        );

        const QJsonObject message =
        messages.first();

        QCOMPARE(
            message.value(
                mafia_game::KEY_TYPE
            ).toString(),
                 mafia_game::message_type_to_string(
                     mafia_game::MessageType::PlayerSelection
                 )
        );

        QCOMPARE(
            message.value(
                mafia_game::KEY_USERNAME
            ).toString(),
                 QStringLiteral("Ana")
        );

        QCOMPARE(
            message.value(
                mafia_game::KEY_IS_SELECTED
            ).toBool(),
                 true
        );
    }


    void getPlayerListsSendsCorrectJson()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        client.get_player_lists();

        QTRY_VERIFY_WITH_TIMEOUT(
            serverSocket->bytesAvailable() > 0,
                                 3000
        );

        const QList<QJsonObject> messages =
        readMessages(serverSocket);

        QCOMPARE(
            messages.size(),
                 1
        );

        const QJsonObject message =
        messages.first();

        QCOMPARE(
            message.value(
                mafia_game::KEY_TYPE
            ).toString(),
                 mafia_game::message_type_to_string(
                     mafia_game::MessageType::RequestLists
                 )
        );
    }


    void sendStartGameSendsCorrectJson()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        QList<QPair<mafia_game::Role, int>>
        roles;

        roles.append(
            qMakePair(
                mafia_game::Role::Mafia,
                1
            )
        );

        roles.append(
            qMakePair(
                mafia_game::Role::Citizen,
                2
            )
        );

        QJsonArray players;

        players.append("Ana");
        players.append("Marko");
        players.append("Niko");

        client.send_start_game(
            roles,
            players
        );

        QTRY_VERIFY_WITH_TIMEOUT(
            serverSocket->bytesAvailable() > 0,
                                 3000
        );

        const QList<QJsonObject> messages =
        readMessages(serverSocket);

        QCOMPARE(
            messages.size(),
                 1
        );

        const QJsonObject message =
        messages.first();

        QCOMPARE(
            message.value(
                mafia_game::KEY_TYPE
            ).toString(),
                 mafia_game::message_type_to_string(
                     mafia_game::MessageType::StartGame
                 )
        );

        const QJsonArray receivedRoles =
        message.value(
            mafia_game::KEY_AMOUNT_OF_ROLES
        ).toArray();

        QCOMPARE(
            receivedRoles.size(),
                 2
        );

        QCOMPARE(
            receivedRoles[0]
            .toObject()
            .value(mafia_game::KEY_ROLE)
            .toString(),
                 mafia_game::role_to_string(
                     mafia_game::Role::Mafia
                 )
        );

        QCOMPARE(
            receivedRoles[0]
            .toObject()
            .value(mafia_game::KEY_ROLE_COUNT)
            .toInt(),
                 1
        );

        QCOMPARE(
            receivedRoles[1]
            .toObject()
            .value(mafia_game::KEY_ROLE)
            .toString(),
                 mafia_game::role_to_string(
                     mafia_game::Role::Citizen
                 )
        );

        QCOMPARE(
            receivedRoles[1]
            .toObject()
            .value(mafia_game::KEY_ROLE_COUNT)
            .toInt(),
                 2
        );

        const QJsonArray receivedPlayers =
        message.value(
            mafia_game::KEY_PLAYERS
        ).toArray();

        QCOMPARE(
            receivedPlayers.size(),
                 3
        );

        QCOMPARE(
            receivedPlayers[0].toString(),
                 QStringLiteral("Ana")
        );

        QCOMPARE(
            receivedPlayers[1].toString(),
                 QStringLiteral("Marko")
        );

        QCOMPARE(
            receivedPlayers[2].toString(),
                 QStringLiteral("Niko")
        );
    }





    void connectToServerEmitsConnected()
    {
        QTcpServer server;
        ChatClient client;

        QSignalSpy connectedSpy(
            &client,
            &ChatClient::connected
        );

        QVERIFY(
            server.listen(
                QHostAddress::LocalHost,
                0
            )
        );

        client.connect_to_server(
            QHostAddress::LocalHost,
            server.serverPort()
        );

        QVERIFY(
            server.waitForNewConnection(3000)
        );

        QTRY_COMPARE(
            connectedSpy.count(),
                     1
        );
    }


    void successfulLoginEmitsLoggedIn()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        QSignalSpy loggedInSpy(
            &client,
            &ChatClient::logged_in
        );

        QJsonObject response;

        response[mafia_game::KEY_TYPE] =
        mafia_game::message_type_to_string(
            mafia_game::MessageType::Login
        );

        response["success"] = true;

        sendJson(
            serverSocket,
            response
        );

        QTRY_COMPARE(
            loggedInSpy.count(),
                     1
        );
    }


    void failedLoginEmitsLoginError()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        QSignalSpy errorSpy(
            &client,
            &ChatClient::login_error
        );

        QJsonObject response;

        response[mafia_game::KEY_TYPE] =
        mafia_game::message_type_to_string(
            mafia_game::MessageType::Login
        );

        response["success"] = false;
        response["reason"] =
        QStringLiteral("Username already exists");

        sendJson(
            serverSocket,
            response
        );

        QTRY_COMPARE(
            errorSpy.count(),
                     1
        );

        QCOMPARE(
            errorSpy.at(0).at(0).toString(),
                 QStringLiteral("Username already exists")
        );
    }


    void invalidLoginResponseIsIgnored()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        QSignalSpy loggedInSpy(
            &client,
            &ChatClient::logged_in
        );

        QSignalSpy errorSpy(
            &client,
            &ChatClient::login_error
        );

        QJsonObject response;

        response[mafia_game::KEY_TYPE] =
        mafia_game::message_type_to_string(
            mafia_game::MessageType::Login
        );

        // Nema "success"

        sendJson(
            serverSocket,
            response
        );

        QTest::qWait(100);

        QCOMPARE(
            loggedInSpy.count(),
                 0
        );

        QCOMPARE(
            errorSpy.count(),
                 0
        );
    }


    void validChatMessageIsReceived()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        QSignalSpy messageSpy(
            &client,
            &ChatClient::received_message
        );

        QJsonObject message =
        mafia_game::create_chat_message(
            QStringLiteral("Ana"),
                                        QStringLiteral("Zdravo"),
                                        true
        );

        sendJson(
            serverSocket,
            message
        );

        QTRY_COMPARE(
            messageSpy.count(),
                     1
        );

        const QList<QVariant> arguments =
        messageSpy.at(0);

        QCOMPARE(
            arguments.at(0).toString(),
                 QStringLiteral("Ana")
        );

        QCOMPARE(
            arguments.at(1).toString(),
                 QStringLiteral("Zdravo")
        );

        QCOMPARE(
            arguments.at(2).toBool(),
                 true
        );
    }


    void messageWithoutTextIsIgnored()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        QSignalSpy messageSpy(
            &client,
            &ChatClient::received_message
        );

        QJsonObject message;

        message[mafia_game::KEY_TYPE] =
        mafia_game::message_type_to_string(
            mafia_game::MessageType::Message
        );

        message[mafia_game::KEY_SENDER] =
        QStringLiteral("Ana");

        message[mafia_game::KEY_ALIVE] =
        true;

        sendJson(
            serverSocket,
            message
        );

        QTest::qWait(100);

        QCOMPARE(
            messageSpy.count(),
                 0
        );
    }


    void messageWithoutSenderIsIgnored()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        QSignalSpy messageSpy(
            &client,
            &ChatClient::received_message
        );

        QJsonObject message;

        message[mafia_game::KEY_TYPE] =
        mafia_game::message_type_to_string(
            mafia_game::MessageType::Message
        );

        message[mafia_game::KEY_TEXT] =
        QStringLiteral("Zdravo");

        message[mafia_game::KEY_ALIVE] =
        true;

        sendJson(
            serverSocket,
            message
        );

        QTest::qWait(100);

        QCOMPARE(
            messageSpy.count(),
                 0
        );
    }


    void messageWithoutAliveIsIgnored()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        QSignalSpy messageSpy(
            &client,
            &ChatClient::received_message
        );

        QJsonObject message;

        message[mafia_game::KEY_TYPE] =
        mafia_game::message_type_to_string(
            mafia_game::MessageType::Message
        );

        message[mafia_game::KEY_SENDER] =
        QStringLiteral("Ana");

        message[mafia_game::KEY_TEXT] =
        QStringLiteral("Zdravo");

        sendJson(
            serverSocket,
            message
        );

        QTest::qWait(100);

        QCOMPARE(
            messageSpy.count(),
                 0
        );
    }


    void gameOverEmitsWinner()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        QSignalSpy gameOverSpy(
            &client,
            &ChatClient::game_over
        );

        QJsonObject message =
        mafia_game::create_game_over_message(
            QStringLiteral("Mafia")
        );

        sendJson(
            serverSocket,
            message
        );

        QTRY_COMPARE(
            gameOverSpy.count(),
                     1
        );

        QCOMPARE(
            gameOverSpy.at(0).at(0).toString(),
                 QStringLiteral("Mafia")
        );
    }


    void invalidGameOverIsIgnored()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        QSignalSpy gameOverSpy(
            &client,
            &ChatClient::game_over
        );

        QJsonObject message;

        message[mafia_game::KEY_TYPE] =
        mafia_game::message_type_to_string(
            mafia_game::MessageType::GameOver
        );

        // Nema winner

        sendJson(
            serverSocket,
            message
        );

        QTest::qWait(100);

        QCOMPARE(
            gameOverSpy.count(),
                 0
        );
    }


    void votingStartedEmitsSignal()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        QSignalSpy votingSpy(
            &client,
            &ChatClient::voting_started
        );

        QJsonObject message;

        message[mafia_game::KEY_TYPE] =
        mafia_game::message_type_to_string(
            mafia_game::MessageType::VotingStarted
        );

        sendJson(
            serverSocket,
            message
        );

        QTRY_COMPARE(
            votingSpy.count(),
                     1
        );
    }


    void dayStartedEmitsSignal()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        QSignalSpy daySpy(
            &client,
            &ChatClient::day_started
        );

        QJsonObject message;

        message[mafia_game::KEY_TYPE] =
        mafia_game::message_type_to_string(
            mafia_game::MessageType::DayStarted
        );

        sendJson(
            serverSocket,
            message
        );

        QTRY_COMPARE(
            daySpy.count(),
                     1
        );
    }


    void nightStartedEmitsSignal()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        QSignalSpy nightSpy(
            &client,
            &ChatClient::night_started
        );

        QJsonObject message;

        /*
         * U ChatClient::json_received() projekat koristi
         * MessageType::NightStated.
         * Koristimo upravo naziv iz projekta.
         */
        message[mafia_game::KEY_TYPE] =
        mafia_game::message_type_to_string(
            mafia_game::MessageType::NightStated
        );

        sendJson(
            serverSocket,
            message
        );

        QTRY_COMPARE(
            nightSpy.count(),
                     1
        );
    }


    void sendMessageSendsCorrectJson()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        client.send_message(
            QStringLiteral("Zdravo")
        );

        QTRY_VERIFY_WITH_TIMEOUT(
            serverSocket->bytesAvailable() > 0,
                                 3000
        );

        const QList<QJsonObject> messages =
        readMessages(serverSocket);

        QCOMPARE(
            messages.size(),
                 1
        );

        const QJsonObject message =
        messages.first();

        QCOMPARE(
            message.value(
                mafia_game::KEY_TYPE
            ).toString(),
                 mafia_game::message_type_to_string(
                     mafia_game::MessageType::Message
                 )
        );

        QCOMPARE(
            message.value(
                mafia_game::KEY_TEXT
            ).toString(),
                 QStringLiteral("Zdravo")
        );
    }



    void emptyMessageIsNotSent()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        client.send_message(QString());

        QTest::qWait(100);

        QCOMPARE(
            serverSocket->bytesAvailable(),
                 0
        );
    }


    void sendVoteSendsCorrectJson()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        client.send_vote(
            QStringLiteral("Ana"),
                         QStringLiteral("Marko")
        );

        QTRY_VERIFY_WITH_TIMEOUT(
            serverSocket->bytesAvailable() > 0,
                                 3000
        );

        const QList<QJsonObject> messages =
        readMessages(serverSocket);

        QCOMPARE(
            messages.size(),
                 1
        );

        const QJsonObject vote =
        messages.first();

        QCOMPARE(
            vote.value(
                mafia_game::KEY_TYPE
            ).toString(),
                 mafia_game::message_type_to_string(
                     mafia_game::MessageType::Vote
                 )
        );

        QCOMPARE(
            vote.value(
                mafia_game::KEY_VOTER
            ).toString(),
                 QStringLiteral("Ana")
        );

        QCOMPARE(
            vote.value(
                mafia_game::KEY_TARGET
            ).toString(),
                 QStringLiteral("Marko")
        );
    }



    void emptyVoteIsNotSent()
    {
        QTcpServer server;
        ChatClient client;

        QTcpSocket *serverSocket =
        connectClient(client, server);

        QVERIFY(serverSocket != nullptr);

        client.send_vote(
            QString(),
                         QStringLiteral("Marko")
        );

        QTest::qWait(100);

        QCOMPARE(
            serverSocket->bytesAvailable(),
                 0
        );

        client.send_vote(
            QStringLiteral("Ana"),
                         QString()
        );

        QTest::qWait(100);

        QCOMPARE(
            serverSocket->bytesAvailable(),
                 0
        );
    }
};


QTEST_MAIN(ChatClientTest)

#include "qtest_chatclient.moc"
