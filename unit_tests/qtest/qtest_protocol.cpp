#include <QtTest/QtTest>

#include <QJsonArray>
#include <QJsonObject>
#include <QHash>
#include <QMap>
#include <QList>
#include <QPair>

#include "protocol.h"

class ProtocolTest : public QObject
{
    Q_OBJECT

private slots:

    // ============================================================
    // message_type_from_string
    // ============================================================

    void messageTypeFromString_data()
    {
        QTest::addColumn<QString>("text");
        QTest::addColumn<int>("expectedType");

        using mafia_game::MessageType;

        QTest::newRow("login")
        << QString("login")
        << static_cast<int>(MessageType::Login);

        QTest::newRow("message")
        << QString("message")
        << static_cast<int>(MessageType::Message);

        QTest::newRow("waiting_room_lists")
        << QString("waiting_room_lists")
        << static_cast<int>(MessageType::WaitingRoomLists);

        QTest::newRow("player_selection")
        << QString("player_selection")
        << static_cast<int>(MessageType::PlayerSelection);

        QTest::newRow("request_lists")
        << QString("request_lists")
        << static_cast<int>(MessageType::RequestLists);

        QTest::newRow("game_state_update")
        << QString("game_state_update")
        << static_cast<int>(MessageType::GameStateUpdate);

        QTest::newRow("join_game")
        << QString("join_game")
        << static_cast<int>(MessageType::JoinGame);

        QTest::newRow("leave_game")
        << QString("leave_game")
        << static_cast<int>(MessageType::LeaveGame);

        QTest::newRow("night_action")
        << QString("night_action")
        << static_cast<int>(MessageType::NightAction);

        QTest::newRow("vote")
        << QString("vote")
        << static_cast<int>(MessageType::Vote);

        QTest::newRow("newuser")
        << QString("newuser")
        << static_cast<int>(MessageType::NewUser);

        QTest::newRow("start_game")
        << QString("start_game")
        << static_cast<int>(MessageType::StartGame);

        QTest::newRow("role_assignment")
        << QString("role_assignment")
        << static_cast<int>(MessageType::RoleAssignment);

        QTest::newRow("userdisconnected")
        << QString("userdisconnected")
        << static_cast<int>(MessageType::UserDisconnected);

        QTest::newRow("night_started")
        << QString("night_started")
        << static_cast<int>(MessageType::NightStated);

        QTest::newRow("night_ended")
        << QString("night_ended")
        << static_cast<int>(MessageType::NightEnded);

        QTest::newRow("voting_started")
        << QString("voting_started")
        << static_cast<int>(MessageType::VotingStarted);

        QTest::newRow("voting_ended")
        << QString("voting_ended")
        << static_cast<int>(MessageType::VotingEnded);

        QTest::newRow("day_started")
        << QString("day_started")
        << static_cast<int>(MessageType::DayStarted);

        QTest::newRow("game_over")
        << QString("game_over")
        << static_cast<int>(MessageType::GameOver);

        // Funkcija koristi Qt::CaseInsensitive.
        QTest::newRow("case insensitive")
        << QString("VoTiNg_StArTeD")
        << static_cast<int>(MessageType::VotingStarted);

        // Nepoznat string treba da postane Unknown.
        QTest::newRow("unknown")
        << QString("something_that_does_not_exist")
        << static_cast<int>(MessageType::Unknown);
    }

    void messageTypeFromString()
    {
        QFETCH(QString, text);
        QFETCH(int, expectedType);

        const mafia_game::MessageType actual =
        mafia_game::message_type_from_string(text);

        QCOMPARE(
            static_cast<int>(actual),
                 expectedType
        );
    }


    // ============================================================
    // message_type_to_string
    // ============================================================

    void messageTypeToString_data()
    {
        QTest::addColumn<int>("type");
        QTest::addColumn<QString>("expectedText");

        using mafia_game::MessageType;

        QTest::newRow("login")
        << static_cast<int>(MessageType::Login)
        << QString("login");

        QTest::newRow("message")
        << static_cast<int>(MessageType::Message)
        << QString("message");

        QTest::newRow("player_selection")
        << static_cast<int>(MessageType::PlayerSelection)
        << QString("player_selection");

        QTest::newRow("waiting_room_lists")
        << static_cast<int>(MessageType::WaitingRoomLists)
        << QString("waiting_room_lists");

        QTest::newRow("request_lists")
        << static_cast<int>(MessageType::RequestLists)
        << QString("request_lists");

        QTest::newRow("join_game")
        << static_cast<int>(MessageType::JoinGame)
        << QString("join_game");

        QTest::newRow("leave_game")
        << static_cast<int>(MessageType::LeaveGame)
        << QString("leave_game");

        QTest::newRow("start_game")
        << static_cast<int>(MessageType::StartGame)
        << QString("start_game");

        QTest::newRow("night_action")
        << static_cast<int>(MessageType::NightAction)
        << QString("night_action");

        QTest::newRow("vote")
        << static_cast<int>(MessageType::Vote)
        << QString("vote");

        QTest::newRow("game_state_update")
        << static_cast<int>(MessageType::GameStateUpdate)
        << QString("game_state_update");

        QTest::newRow("role_assignment")
        << static_cast<int>(MessageType::RoleAssignment)
        << QString("role_assignment");

        QTest::newRow("newuser")
        << static_cast<int>(MessageType::NewUser)
        << QString("newuser");

        QTest::newRow("userdisconnected")
        << static_cast<int>(MessageType::UserDisconnected)
        << QString("userdisconnected");

        QTest::newRow("night_started")
        << static_cast<int>(MessageType::NightStated)
        << QString("night_started");

        QTest::newRow("night_ended")
        << static_cast<int>(MessageType::NightEnded)
        << QString("night_ended");

        QTest::newRow("voting_started")
        << static_cast<int>(MessageType::VotingStarted)
        << QString("voting_started");

        QTest::newRow("voting_ended")
        << static_cast<int>(MessageType::VotingEnded)
        << QString("voting_ended");

        QTest::newRow("day_started")
        << static_cast<int>(MessageType::DayStarted)
        << QString("day_started");

        QTest::newRow("game_over")
        << static_cast<int>(MessageType::GameOver)
        << QString("game_over");

        QTest::newRow("unknown")
        << static_cast<int>(MessageType::Unknown)
        << QString("unknown");
    }

    void messageTypeToString()
    {
        QFETCH(int, type);
        QFETCH(QString, expectedText);

        const QString actual =
        mafia_game::message_type_to_string(
            static_cast<mafia_game::MessageType>(type)
        );

        QCOMPARE(actual, expectedText);
    }


    // ============================================================
    // Role konverzije
    // ============================================================

    void roleConversions()
    {
        using mafia_game::Role;

        QCOMPARE(
            static_cast<int>(mafia_game::role_from_string("Mafia")),
                 static_cast<int>(Role::Mafia)
        );

        QCOMPARE(
            static_cast<int>(mafia_game::role_from_string("Police")),
                 static_cast<int>(Role::Police)
        );

        QCOMPARE(
            static_cast<int>(mafia_game::role_from_string("Doctor")),
                 static_cast<int>(Role::Doctor)
        );

        QCOMPARE(
            static_cast<int>(mafia_game::role_from_string("Citizen")),
                 static_cast<int>(Role::Citizen)
        );

        // Case-insensitive provera.
        QCOMPARE(
            static_cast<int>(mafia_game::role_from_string("mAfIa")),
                 static_cast<int>(Role::Mafia)
        );

        QCOMPARE(
            static_cast<int>(mafia_game::role_from_string("Wizard")),
                 static_cast<int>(Role::Unknown)
        );

        QCOMPARE(
            mafia_game::role_to_string(Role::Mafia),
                 QString("Mafia")
        );

        QCOMPARE(
            mafia_game::role_to_string(Role::Police),
                 QString("Police")
        );

        QCOMPARE(
            mafia_game::role_to_string(Role::Doctor),
                 QString("Doctor")
        );

        QCOMPARE(
            mafia_game::role_to_string(Role::Citizen),
                 QString("Citizen")
        );

        QCOMPARE(
            mafia_game::role_to_string(Role::Unknown),
                 QString("Unknown")
        );
    }


    // ============================================================
    // Login
    // ============================================================

    void createLoginMessage()
    {
        const QJsonObject message =
        mafia_game::create_login_message("Milica");

        QCOMPARE(
            message.value(mafia_game::KEY_TYPE).toString(),
                 QString("login")
        );

        QCOMPARE(
            message.value(mafia_game::KEY_USERNAME).toString(),
                 QString("Milica")
        );
    }


    // ============================================================
    // Chat message
    // ============================================================

    void createChatMessage()
    {
        const QJsonObject message =
        mafia_game::create_chat_message(
            "Ana",
            "Zdravo!",
            true
        );

        QCOMPARE(
            message.value(mafia_game::KEY_TYPE).toString(),
                 QString("message")
        );

        QCOMPARE(
            message.value(mafia_game::KEY_SENDER).toString(),
                 QString("Ana")
        );

        QCOMPARE(
            message.value(mafia_game::KEY_TEXT).toString(),
                 QString("Zdravo!")
        );

        QCOMPARE(
            message.value(mafia_game::KEY_ALIVE).toBool(),
                 true
        );
    }


    // ============================================================
    // Night action
    // ============================================================

    void createNightAction()
    {
        const QJsonObject message =
        mafia_game::create_night_action(
            mafia_game::Role::Doctor,
            "Marko"
        );

        QCOMPARE(
            message.value(mafia_game::KEY_TYPE).toString(),
                 QString("night_action")
        );

        QCOMPARE(
            message.value(mafia_game::KEY_ROLE).toString(),
                 QString("Doctor")
        );

        QCOMPARE(
            message.value(mafia_game::KEY_TARGET).toString(),
                 QString("Marko")
        );
    }


    // ============================================================
    // Ability use
    // ============================================================

    void createAbilityUse()
    {
        const QJsonObject message =
        mafia_game::create_ability_use(
            "Ana",
            "Marko"
        );

        QCOMPARE(
            message.value(mafia_game::KEY_TYPE).toString(),
                 QString("night_action")
        );

        QCOMPARE(
            message.value(mafia_game::KEY_USERNAME).toString(),
                 QString("Ana")
        );

        QCOMPARE(
            message.value(mafia_game::KEY_TARGET).toString(),
                 QString("Marko")
        );
    }


    // ============================================================
    // Vote
    // ============================================================

    void createVoteMessage()
    {
        const QJsonObject message =
        mafia_game::create_vote(
            "Ana",
            "Marko"
        );

        QCOMPARE(
            message.value(mafia_game::KEY_TYPE).toString(),
                 QString("vote")
        );

        QCOMPARE(
            message.value(mafia_game::KEY_VOTER).toString(),
                 QString("Ana")
        );

        QCOMPARE(
            message.value(mafia_game::KEY_TARGET).toString(),
                 QString("Marko")
        );
    }


    // ============================================================
    // Player state
    // ============================================================

    void createPlayerState()
    {
        const QJsonObject player =
        mafia_game::create_player_state(
            "Petar",
            mafia_game::Role::Doctor,
            true
        );

        QCOMPARE(
            player.value(mafia_game::KEY_USERNAME).toString(),
                 QString("Petar")
        );

        QCOMPARE(
            player.value(mafia_game::KEY_ROLE).toString(),
                 QString("Doctor")
        );

        QCOMPARE(
            player.value(mafia_game::KEY_ALIVE).toBool(),
                 true
        );
    }


    // ============================================================
    // Game state update
    // ============================================================

    void createGameStateUpdate()
    {
        QJsonArray players;

        players.append(
            mafia_game::create_player_state(
                "Ana",
                mafia_game::Role::Citizen,
                true
            )
        );

        players.append(
            mafia_game::create_player_state(
                "Marko",
                mafia_game::Role::Mafia,
                false
            )
        );

        const QJsonObject message =
        mafia_game::create_game_state_update(
            "day",
            3,
            players
        );

        QCOMPARE(
            message.value(mafia_game::KEY_TYPE).toString(),
                 QString("game_state_update")
        );

        QCOMPARE(
            message.value(mafia_game::KEY_PHASE).toString(),
                 QString("day")
        );

        QCOMPARE(
            message.value(mafia_game::KEY_DAY_NUMBER).toInt(),
                 3
        );

        const QJsonArray resultPlayers =
        message.value(mafia_game::KEY_PLAYERS).toArray();

        QCOMPARE(resultPlayers.size(), 2);

        QCOMPARE(
            resultPlayers.at(0)
            .toObject()
            .value(mafia_game::KEY_USERNAME)
            .toString(),
                 QString("Ana")
        );

        QCOMPARE(
            resultPlayers.at(1)
            .toObject()
            .value(mafia_game::KEY_USERNAME)
            .toString(),
                 QString("Marko")
        );
    }


    // ============================================================
    // Role assignment
    // ============================================================

    void createRoleAssignmentMessage()
    {
        QString username = "Ana";

        QJsonArray allRoles;
        allRoles.append("Mafia");
        allRoles.append("Citizen");
        allRoles.append("Doctor");

        const QJsonObject message =
        mafia_game::create_role_assignment_message(
            mafia_game::Role::Police,
            username,
            allRoles
        );

        QCOMPARE(
            message.value(mafia_game::KEY_TYPE).toString(),
                 QString("role_assignment")
        );

        QCOMPARE(
            message.value(mafia_game::KEY_USERNAME).toString(),
                 QString("Ana")
        );

        QCOMPARE(
            message.value(mafia_game::KEY_ROLE).toString(),
                 QString("Police")
        );

        QCOMPARE(
            message.value(mafia_game::KEY_ALL_ROLES).toArray(),
                 allRoles
        );
    }


    // ============================================================
    // Start game
    // ============================================================

    void createStartGame()
    {
        QList<QPair<mafia_game::Role, int>> roles;

        roles.append(
            qMakePair(
                mafia_game::Role::Mafia,
                2
            )
        );

        roles.append(
            qMakePair(
                mafia_game::Role::Citizen,
                4
            )
        );

        QJsonArray players;
        players.append("Ana");
        players.append("Marko");
        players.append("Petar");

        const QJsonObject message =
        mafia_game::create_start_game(
            roles,
            players
        );

        QCOMPARE(
            message.value(mafia_game::KEY_TYPE).toString(),
                 QString("start_game")
        );

        QCOMPARE(
            message.value(mafia_game::KEY_PLAYERS).toArray(),
                 players
        );

        const QJsonArray roleArray =
        message.value(
            mafia_game::KEY_AMOUNT_OF_ROLES
        ).toArray();

        QCOMPARE(roleArray.size(), 2);

        QCOMPARE(
            roleArray.at(0)
            .toObject()
            .value(mafia_game::KEY_ROLE)
            .toString(),
                 QString("Mafia")
        );

        QCOMPARE(
            roleArray.at(0)
            .toObject()
            .value(mafia_game::KEY_ROLE_COUNT)
            .toInt(),
                 2
        );

        QCOMPARE(
            roleArray.at(1)
            .toObject()
            .value(mafia_game::KEY_ROLE)
            .toString(),
                 QString("Citizen")
        );

        QCOMPARE(
            roleArray.at(1)
            .toObject()
            .value(mafia_game::KEY_ROLE_COUNT)
            .toInt(),
                 4
        );
    }


    // ============================================================
    // Faze igre
    // ============================================================

    void createPhaseMessages()
    {
        const QJsonObject nightStarted =
        mafia_game::create_night_started_message();

        QCOMPARE(
            nightStarted.value(mafia_game::KEY_TYPE).toString(),
                 QString("night_started")
        );


        const QJsonObject votingStarted =
        mafia_game::create_voting_started_message();

        QCOMPARE(
            votingStarted.value(mafia_game::KEY_TYPE).toString(),
                 QString("voting_started")
        );


        const QJsonObject dayStarted =
        mafia_game::create_day_started_message();

        QCOMPARE(
            dayStarted.value(mafia_game::KEY_TYPE).toString(),
                 QString("day_started")
        );
    }


    // ============================================================
    // Voting ended
    // ============================================================

    void createVotingEndedMessage()
    {
        QVector<QString> votedOut = {
            "Marko"
        };

        QVector<QPair<QString, QString>> votes = {
            qMakePair(QString("Ana"), QString("Marko")),
            qMakePair(QString("Petar"), QString("Marko")),
            qMakePair(QString("Marko"), QString("Ana"))
        };

        const QJsonObject message =
        mafia_game::create_voting_ended_message(
            votedOut,
            votes
        );

        QCOMPARE(
            message.value(mafia_game::KEY_TYPE).toString(),
                 QString("voting_ended")
        );

        const QJsonArray targets =
        message.value(mafia_game::KEY_TARGET).toArray();

        QCOMPARE(targets.size(), 1);

        QCOMPARE(
            targets.at(0).toString(),
                 QString("Marko")
        );

        const QJsonObject voteObject =
        message.value(mafia_game::KEY_VOTES).toObject();

        QCOMPARE(
            voteObject.value("Ana").toString(),
                 QString("Marko")
        );

        QCOMPARE(
            voteObject.value("Petar").toString(),
                 QString("Marko")
        );

        QCOMPARE(
            voteObject.value("Marko").toString(),
                 QString("Ana")
        );
    }


    // ============================================================
    // Night ended
    // ============================================================

    void createNightEndedMessage()
    {
        QMap<QString, QString> information;

        information.insert(
            "doctor",
            "Ana"
        );

        information.insert(
            "mafia",
            "Marko"
        );

        const QJsonObject message =
        mafia_game::create_night_ended_message(
            "Petar",
            information
        );

        QCOMPARE(
            message.value(mafia_game::KEY_TYPE).toString(),
                 QString("night_ended")
        );

        QCOMPARE(
            message.value(mafia_game::KEY_TARGET).toString(),
                 QString("Petar")
        );

        const QJsonObject info =
        message.value(mafia_game::KEY_INFO).toObject();

        QCOMPARE(
            info.value("doctor").toString(),
                 QString("Ana")
        );

        QCOMPARE(
            info.value("mafia").toString(),
                 QString("Marko")
        );
    }


    // ============================================================
    // Game over
    // ============================================================

    void createGameOverMessage()
    {
        const QJsonObject message =
        mafia_game::create_game_over_message("Mafia");

        QCOMPARE(
            message.value(mafia_game::KEY_TYPE).toString(),
                 QString("game_over")
        );

        QCOMPARE(
            message.value(mafia_game::KEY_WINNER).toString(),
                 QString("Mafia")
        );
    }


    // ============================================================
    // Player selection
    // ============================================================

    void createPlayerSelection()
    {
        const QJsonObject message =
        mafia_game::create_player_selection(
            "Ana",
            true
        );

        QCOMPARE(
            message.value(mafia_game::KEY_TYPE).toString(),
                 QString("player_selection")
        );

        QCOMPARE(
            message.value(mafia_game::KEY_USERNAME).toString(),
                 QString("Ana")
        );

        QCOMPARE(
            message.value(mafia_game::KEY_IS_SELECTED).toBool(),
                 true
        );
    }


    // ============================================================
    // Request lists
    // ============================================================

    void createListsRequest()
    {
        const QJsonObject message =
        mafia_game::create_lists_request();

        QCOMPARE(
            message.value(mafia_game::KEY_TYPE).toString(),
                 QString("request_lists")
        );
    }


    // ============================================================
    // Waiting room lists
    // ============================================================

    void createWaitingRoomLists()
    {
        QHash<QString, bool> players;

        players.insert("Ana", true);
        players.insert("Marko", false);
        players.insert("Petar", true);

        const QJsonObject message =
        mafia_game::create_waiting_room_lists(players);

        QCOMPARE(
            message.value(mafia_game::KEY_TYPE).toString(),
                 QString("waiting_room_lists")
        );

        const QJsonArray selected =
        message.value(mafia_game::KEY_SELECTED).toArray();

        const QJsonArray unselected =
        message.value(mafia_game::KEY_UNSELECTED).toArray();

        QCOMPARE(selected.size(), 2);
        QCOMPARE(unselected.size(), 1);

        bool anaFound = false;
        bool petarFound = false;
        bool markoFound = false;

        for (const QJsonValue &value : selected)
        {
            const QString username =
            value.toObject()
            .value(mafia_game::KEY_USERNAME)
            .toString();

            if (username == "Ana")
                anaFound = true;

            if (username == "Petar")
                petarFound = true;
        }

        for (const QJsonValue &value : unselected)
        {
            const QString username =
            value.toObject()
            .value(mafia_game::KEY_USERNAME)
            .toString();

            if (username == "Marko")
                markoFound = true;
        }

        QVERIFY(anaFound);
        QVERIFY(petarFound);
        QVERIFY(markoFound);
    }
};

QTEST_MAIN(ProtocolTest)

#include "qtest_protocol.moc"
