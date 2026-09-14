#include <QtTest/QtTest>

#include <QTcpSocket>
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QElapsedTimer>

#include "chatserver.h"
#include "GameState.h"
#include "Player.h"
#include "citizenrole.h"
#include "votingserver.h"
#include "mafiarole.h"

#include <memory>

class ChatServerTest : public QObject
{
    Q_OBJECT

private:

    // ============================================================
    // Slanje jedne JSON poruke serveru
    // ============================================================

    static void sendJson(QTcpSocket &socket, const QJsonObject &object)
    {

        const QByteArray data =
        QJsonDocument(object).toJson(QJsonDocument::Compact);

        QDataStream stream(&socket);
        stream.setVersion(QDataStream::Qt_5_7);

        stream << data;

        QVERIFY2(
            socket.waitForBytesWritten(3000),
                 qPrintable(socket.errorString())
        );
    }


    // ============================================================
    // Čitanje svih JSON poruka koje su trenutno stigle
    // ============================================================

    static QVector<QJsonObject> readMessages(
        QTcpSocket &socket,
        int timeoutMs = 1000)
    {
        QVector<QJsonObject> messages;

        QElapsedTimer timer;
        timer.start();

        while (timer.elapsed() < timeoutMs)
        {
            QCoreApplication::processEvents();

            if (socket.bytesAvailable() == 0)
            {
                socket.waitForReadyRead(50);
                QCoreApplication::processEvents();
            }

            bool readSomething = false;

            while (socket.bytesAvailable() > 0)
            {
                QDataStream stream(&socket);
                stream.setVersion(QDataStream::Qt_5_7);

                QByteArray data;

                stream.startTransaction();
                stream >> data;

                if (!stream.commitTransaction())
                    break;

                const QJsonDocument document =
                QJsonDocument::fromJson(data);

                if (document.isObject())
                    messages.append(document.object());

                readSomething = true;
            }

            /*
             * Ako smo nešto pročitali, sačekamo još malo kako bismo
             * uhvatili eventualnu drugu poruku koju server pošalje
             * odmah nakon prve.
             */
            if (readSomething)
            {
                QTest::qWait(100);
                QCoreApplication::processEvents();

                if (socket.bytesAvailable() == 0)
                    break;
            }
        }

        return messages;
    }


    // ============================================================
    // Povezivanje klijenta
    // ============================================================

    static void connectClient(
        QTcpSocket &client,
        ChatServer &server)
    {
        client.connectToHost(
            QHostAddress::LocalHost,
            server.serverPort()
        );

        QVERIFY2(
            client.waitForConnected(3000),
                 qPrintable(client.errorString())
        );

        QCoreApplication::processEvents();
    }


    // ============================================================
    // Login
    // ============================================================

    static QVector<QJsonObject> login(
        QTcpSocket &client,
        const QString &username)
    {
        QJsonObject message;

        message["type"] = "login";
        message["username"] = username;

        sendJson(client, message);

        return readMessages(client);
    }


private slots:

    void disconnectedPlayerIsRemovedAndMarkedDead()
    {
        ChatServer server;

        QVERIFY2(
            server.listen(QHostAddress::LocalHost, 0),
                 qPrintable(server.errorString())
        );

        QTcpSocket ana;
        QTcpSocket marko;

        connectClient(ana, server);
        connectClient(marko, server);

        login(ana, "Ana");
        login(marko, "Marko");

        // Ocistimo poruke nastale tokom login-a.
        readMessages(ana, 300);
        readMessages(marko, 300);

        // Simuliramo da Ana postoji u GameState i da je ziva.
        auto anaPlayer =
        std::make_shared<Player>(
            "Ana",
            new CitizenRole()
        );

        anaPlayer->set_alive(true);
        GameState::add_player(anaPlayer);

        QVERIFY(
            GameState::get_player_by_username("Ana") != nullptr
        );

        QVERIFY(
            GameState::get_player_by_username("Ana")->is_alive()
        );

        // Ana prekida vezu sa serverom.
        ana.disconnectFromHost();

        if (ana.state() != QAbstractSocket::UnconnectedState) {
            QVERIFY(
                ana.waitForDisconnected(1000)
            );
        }

        // ServerWorker signal se obradjuje asinhrono.
        QTest::qWait(200);

        /*
         * 1. GameState bi trebalo da registruje
         *    da Ana vise nije ziva.
         */
        auto player =
        GameState::get_player_by_username("Ana");

        QVERIFY(player != nullptr);

        QVERIFY2(
            !player->is_alive(),
                 "Diskonektovani igrac nije oznacen kao mrtav u GameState."
        );

        /*
         * 2. Marko bi trebalo da dobije
         *    userdisconnected poruku.
         */
        const auto messages =
        readMessages(marko, 500);

        bool receivedDisconnectedMessage = false;

        for (const QJsonObject &message : messages) {

            if (message.value("type").toString()
                == "userdisconnected"
                &&
                message.value("username").toString()
                == "Ana") {

                receivedDisconnectedMessage = true;
            break;
                }
        }

        QVERIFY2(
            receivedDisconnectedMessage,
            "Drugi klijent nije dobio userdisconnected poruku."
        );

        /*
         * 3. Pokusavamo ponovo da se prijavimo
         *    sa username-om Ana.
         *
         * Ako je stari ServerWorker pravilno uklonjen
         * iz m_clients, login bi trebalo da uspe.
         */
        QTcpSocket newAna;

        connectClient(newAna, server);

        QJsonObject loginMessage;
        loginMessage["type"] = "login";
        loginMessage["username"] = "Ana";

        sendJson(newAna, loginMessage);

        const auto loginMessages =
        readMessages(newAna, 500);

        bool loginSucceeded = false;

        for (const QJsonObject &message : loginMessages) {

            if (message.value("type").toString() == "login"
                && message.value("success").toBool()) {

                loginSucceeded = true;
            break;
                }
        }

        QVERIFY2(
            loginSucceeded,
            "Username diskonektovanog igraca nije oslobodjen za ponovni login."
        );
    }

    void gameOverMessageIsBroadcastWhenGameEnds()
    {
        GameState::reset();
        GameState::set_phase(GamePhase::Inactive);

        ChatServer server;

        QVERIFY2(
            server.listen(QHostAddress::LocalHost, 0),
                 qPrintable(server.errorString())
        );

        // ----------------------------------------------------------
        // 1. Povezujemo dovoljan broj klijenata da bismo mogli
        //    regularno da pokrenemo igru.
        //
        // Time ChatServer pravi sve veze između:
        //
        // VotingServer -> GameManager -> ChatServer::end_game
        // ----------------------------------------------------------

        QTcpSocket ana;
        QTcpSocket marko;
        QTcpSocket petar;
        QTcpSocket jovana;
        QTcpSocket nikola;
        QTcpSocket milica;

        connectClient(ana, server);
        connectClient(marko, server);
        connectClient(petar, server);
        connectClient(jovana, server);
        connectClient(nikola, server);
        connectClient(milica, server);

        login(ana, "Ana");
        login(marko, "Marko");
        login(petar, "Petar");
        login(jovana, "Jovana");
        login(nikola, "Nikola");
        login(milica, "Milica");

        readMessages(ana, 300);
        readMessages(marko, 300);
        readMessages(petar, 300);
        readMessages(jovana, 300);
        readMessages(nikola, 300);
        readMessages(milica, 300);

        QList<QPair<mafia_game::Role, int>> roles = {
            {mafia_game::Role::Mafia,   2},
            {mafia_game::Role::Citizen, 3},
            {mafia_game::Role::Police,  1}
        };

        QJsonArray players;
        players.append("Ana");
        players.append("Marko");
        players.append("Petar");
        players.append("Jovana");
        players.append("Nikola");
        players.append("Milica");

        QJsonObject startMessage =
        mafia_game::create_start_game(roles, players);

        sendJson(ana, startMessage);

        QTest::qWait(200);
        QCoreApplication::processEvents();

        /*
         * Uklanjamo role_assignment i ostale poruke
         * nastale prilikom pokretanja igre.
         */
        readMessages(ana, 300);
        readMessages(marko, 300);
        readMessages(petar, 300);
        readMessages(jovana, 300);
        readMessages(nikola, 300);
        readMessages(milica, 300);

        // ----------------------------------------------------------
        // 2. Postavljamo poznato stanje neposredno pred kraj igre.
        //
        // Jedan mafijas + jedan gradjanin:
        // mafija je dostigla broj ostalih zivih igraca.
        // ----------------------------------------------------------

        GameState::reset();

        auto mafia =
        std::make_shared<Player>(
            "Ana",
            new MafiaRole()
        );

        auto citizen =
        std::make_shared<Player>(
            "Marko",
            new CitizenRole()
        );

        mafia->set_alive(true);
        citizen->set_alive(true);

        GameState::add_player(mafia);
        GameState::add_player(citizen);

        QVERIFY(GameState::check_game_over());

        const QString expectedWinner =
        GameState::get_winner();

        QVERIFY2(
            !expectedWinner.isEmpty(),
                 "GameState nije odredio pobednika."
        );

        // ----------------------------------------------------------
        // 3. Pratimo signal koji emituje ChatServer::end_game().
        // ----------------------------------------------------------

        QSignalSpy gameEndedSpy(
            &server,
            &ChatServer::game_ended
        );

        QSignalSpy logSpy(
            &server,
            &ChatServer::log_message
        );

        // ----------------------------------------------------------
        // 4. Aktiviramo regularnu putanju:
        //
        // VotingServer::voting_has_concluded
        //          ->
        // GameManager::on_voting_finished
        //          ->
        // game_over_check
        //          ->
        // GameManager::game_over
        //          ->
        // ChatServer::end_game
        //
        // Ne pozivamo privatni end_game direktno.
        // ----------------------------------------------------------

        QVector<QString> nobodyKilled;

        emit voting::VotingServer::instance()
        ->voting_has_concluded(nobodyKilled);

        QTRY_COMPARE_WITH_TIMEOUT(
            gameEndedSpy.count(),
                                  1,
                                  1000
        );

        // ----------------------------------------------------------
        // 5. Proveravamo winner prosledjen kroz game_ended.
        // ----------------------------------------------------------

        const QList<QVariant> arguments =
        gameEndedSpy.takeFirst();

        QCOMPARE(
            arguments.at(0).toString(),
                 expectedWinner
        );

        // ----------------------------------------------------------
        // 6. end_game() mora da resetuje GameState.
        // ----------------------------------------------------------

        QVERIFY2(
            GameState::get_alive_players_names().isEmpty(),
                 "GameState nije resetovan nakon zavrsetka igre."
        );

        // ----------------------------------------------------------
        // 7. Proveravamo log poruku.
        // ----------------------------------------------------------

        bool gameEndedLogFound = false;

        const QString expectedLog =
        QStringLiteral("Game ended! Winner: ")
        + expectedWinner;

        for (const QList<QVariant> &args : logSpy) {

            if (!args.isEmpty() &&
                args.at(0).toString() == expectedLog) {

                gameEndedLogFound = true;
            break;
                }
        }

        QVERIFY2(
            gameEndedLogFound,
            "ChatServer nije emitovao log poruku o kraju igre."
        );

        // ----------------------------------------------------------
        // 8. Proveravamo da je game-over poruka broadcast-ovana.
        // ----------------------------------------------------------

        const QVector<QJsonObject> anaMessages =
        readMessages(ana, 500);

        const QVector<QJsonObject> markoMessages =
        readMessages(marko, 500);

        bool anaReceivedGameOver = false;
        bool markoReceivedGameOver = false;

        for (const QJsonObject &message : anaMessages) {

            if (message.value("winner").toString()
                == expectedWinner) {

                anaReceivedGameOver = true;
            break;
                }
        }

        for (const QJsonObject &message : markoMessages) {

            if (message.value("winner").toString()
                == expectedWinner) {

                markoReceivedGameOver = true;
            break;
                }
        }

        QVERIFY2(
            anaReceivedGameOver,
            "Ana nije dobila game-over poruku."
        );

        QVERIFY2(
            markoReceivedGameOver,
            "Marko nije dobio game-over poruku."
        );
    }

    void cleanup()
    {
        GameState::reset();
        GameState::set_phase(GamePhase::Inactive);
    }

    void validGameStartCreatesAllPlayers()
    {
        ChatServer server;

        QVERIFY2(
            server.listen(QHostAddress::LocalHost, 0),
                 qPrintable(server.errorString())
        );

        QTcpSocket ana;
        QTcpSocket marko;
        QTcpSocket petar;
        QTcpSocket jovana;
        QTcpSocket nikola;
        QTcpSocket milica;

        connectClient(ana, server);
        connectClient(marko, server);
        connectClient(petar, server);
        connectClient(jovana, server);
        connectClient(nikola, server);
        connectClient(milica, server);

        login(ana, "Ana");
        login(marko, "Marko");
        login(petar, "Petar");
        login(jovana, "Jovana");
        login(nikola, "Nikola");
        login(milica, "Milica");

        readMessages(ana, 300);
        readMessages(marko, 300);
        readMessages(petar, 300);
        readMessages(jovana, 300);
        readMessages(nikola, 300);
        readMessages(milica, 300);

        QList<QPair<mafia_game::Role, int>> roles = {
            {mafia_game::Role::Mafia,   2},
            {mafia_game::Role::Citizen, 3},
            {mafia_game::Role::Police,  1}
        };

        QJsonArray players;
        players.append("Ana");
        players.append("Marko");
        players.append("Petar");
        players.append("Jovana");
        players.append("Nikola");
        players.append("Milica");

        QJsonObject startMessage =
        mafia_game::create_start_game(roles, players);

        sendJson(ana, startMessage);

        QTest::qWait(200);

        auto anaPlayer =
        GameState::get_player_by_username("Ana");
        auto markoPlayer =
        GameState::get_player_by_username("Marko");
        auto petarPlayer =
        GameState::get_player_by_username("Petar");
        auto jovanaPlayer =
        GameState::get_player_by_username("Jovana");
        auto nikolaPlayer =
        GameState::get_player_by_username("Nikola");
        auto milicaPlayer =
        GameState::get_player_by_username("Milica");

        QVERIFY(anaPlayer != nullptr);
        QVERIFY(markoPlayer != nullptr);
        QVERIFY(petarPlayer != nullptr);
        QVERIFY(jovanaPlayer != nullptr);
        QVERIFY(nikolaPlayer != nullptr);
        QVERIFY(milicaPlayer != nullptr);

        QVERIFY(anaPlayer->is_alive());
        QVERIFY(markoPlayer->is_alive());
        QVERIFY(petarPlayer->is_alive());
        QVERIFY(jovanaPlayer->is_alive());
        QVERIFY(nikolaPlayer->is_alive());
        QVERIFY(milicaPlayer->is_alive());
    }


    void duplicatePlayerInStartGameIsRejected()
    {
        ChatServer server;

        QVERIFY2(
            server.listen(QHostAddress::LocalHost, 0),
                 qPrintable(server.errorString())
        );

        QSignalSpy logSpy(&server, &ChatServer::log_message);

        QTcpSocket ana;
        QTcpSocket marko;
        QTcpSocket petar;
        QTcpSocket jovana;
        QTcpSocket nikola;

        connectClient(ana, server);
        connectClient(marko, server);
        connectClient(petar, server);
        connectClient(jovana, server);
        connectClient(nikola, server);

        login(ana, "Ana");
        login(marko, "Marko");
        login(petar, "Petar");
        login(jovana, "Jovana");
        login(nikola, "Nikola");

        readMessages(ana, 300);
        readMessages(marko, 300);
        readMessages(petar, 300);
        readMessages(jovana, 300);
        readMessages(nikola, 300);

        QList<QPair<mafia_game::Role, int>> roles = {
            {mafia_game::Role::Mafia,   2},
            {mafia_game::Role::Citizen, 3},
            {mafia_game::Role::Police,  1}
        };

        QJsonArray players;
        players.append("Ana");
        players.append("Marko");
        players.append("Petar");
        players.append("Jovana");
        players.append("Nikola");

        // Duplikat
        players.append("Ana");

        QJsonObject startMessage =
        mafia_game::create_start_game(roles, players);

        sendJson(ana, startMessage);

        QTest::qWait(200);

        // Server tvrdi da je igra pokrenuta.
        bool gameStarted = false;

        for (const QList<QVariant> &arguments : logSpy) {
            if (!arguments.isEmpty() &&
                arguments.at(0).toString() ==
                "Game started! Roles assigned.") {
                gameStarted = true;
            break;
                }
        }

        QVERIFY2(
            gameStarted,
            "Ocekivano je da trenutna implementacija prijavi da je igra pokrenuta."
        );

        // Direktno proveravamo GameState.
        const QVector<QString> alivePlayers =
        GameState::get_alive_players_names();

        qDebug() << "Broj zivih igraca u GameState:"
        << alivePlayers.size();

        qDebug() << "Zivi igraci:"
        << alivePlayers;

        /*
         * StartGame sadrzi 6 mesta za igrace.
         *
         * Ispravno ponasanje:
         * server bi trebalo da odbije duplikat i igra ne bi trebalo da pocne.
         *
         * Trenutno ponasanje:
         * server pokrene igru, ali GameState sadrzi samo
         * 5 jedinstvenih igraca.
         */
        QCOMPARE(
            alivePlayers.size(),
                 6
        );
    }

    void gameDoesNotStartWhenRoleCountDiffersFromConnectedPlayers()
    {
        ChatServer server;

        QVERIFY2(
            server.listen(QHostAddress::LocalHost, 0),
                 qPrintable(server.errorString())
        );

        QSignalSpy logSpy(&server, &ChatServer::log_message);

        QTcpSocket ana;
        QTcpSocket marko;
        QTcpSocket petar;
        QTcpSocket jovana;
        QTcpSocket nikola;

        connectClient(ana, server);
        connectClient(marko, server);
        connectClient(petar, server);
        connectClient(jovana, server);
        connectClient(nikola, server);

        login(ana, "Ana");
        login(marko, "Marko");
        login(petar, "Petar");
        login(jovana, "Jovana");
        login(nikola, "Nikola");

        readMessages(ana, 300);
        readMessages(marko, 300);
        readMessages(petar, 300);
        readMessages(jovana, 300);
        readMessages(nikola, 300);

        QList<QPair<mafia_game::Role, int>> roles = {
            {mafia_game::Role::Mafia,   2},
            {mafia_game::Role::Citizen, 3},
            {mafia_game::Role::Police,  1}
        };

        QJsonArray players;
        players.append("Ana");
        players.append("Marko");
        players.append("Petar");
        players.append("Jovana");
        players.append("Nikola");

        // Ne postoji povezan klijent sa ovim username-om.
        players.append("Milica");

        QJsonObject startMessage =
        mafia_game::create_start_game(roles, players);

        sendJson(ana, startMessage);

        QTest::qWait(200);

        bool gameStarted = false;

        for (const QList<QVariant> &arguments : logSpy) {
            if (!arguments.isEmpty() &&
                arguments.at(0).toString() ==
                "Game started! Roles assigned.") {
                gameStarted = true;
            break;
                }
        }

        // Dokazujemo kontradikciju.
        QVERIFY2(
            gameStarted,
            "Trenutna implementacija bi trebalo da prijavi Game started."
        );

        const QVector<QString> alivePlayers =
        GameState::get_alive_players_names();

        qDebug() << "Broj igraca u GameState:"
        << alivePlayers.size();

        /*
         * Poslato je 6 igraca/uloga.
         *
         * Da je igra zaista uspesno pokrenuta,
         * GameState bi morao da sadrzi 6 igraca.
         *
         * Trenutno ce ih biti 0 jer je assign_roles()
         * odustao zbog neslaganja.
         */
        QCOMPARE(
            alivePlayers.size(),
                 6
        );
    }

    void startGameWithTooManyPlayersIsRejected()
    {
        ChatServer server;

        QVERIFY2(
            server.listen(QHostAddress::LocalHost, 0),
                 qPrintable(server.errorString())
        );

        QSignalSpy logSpy(&server, &ChatServer::log_message);

        QTcpSocket ana;
        connectClient(ana, server);
        login(ana, "Ana");

        readMessages(ana, 300);

        // 11 uloga -> više od MAX_PLAYERS = 10.
        QList<QPair<mafia_game::Role, int>> roles = {
            {mafia_game::Role::Mafia,   3},
            {mafia_game::Role::Citizen, 6},
            {mafia_game::Role::Doctor,  1},
            {mafia_game::Role::Police,  1}
        };

        QJsonArray players;
        players.append("Ana");

        QJsonObject startMessage =
        mafia_game::create_start_game(roles, players);

        sendJson(ana, startMessage);

        QTest::qWait(100);

        bool rejected = false;

        for (const QList<QVariant> &arguments : logSpy) {
            if (!arguments.isEmpty() &&
                arguments.at(0).toString() ==
                "Game Stopped. Too many players") {

                rejected = true;
            break;
                }
        }

        QVERIFY2(
            rejected,
            "Server nije odbio pokretanje igre sa vise od 10 igraca."
        );

        QCOMPARE(
            GameState::get_player_by_username("Ana"),
                 nullptr
        );
    }

    void startGameWithTooFewPlayersIsRejected()
    {
        ChatServer server;

        QVERIFY2(
            server.listen(QHostAddress::LocalHost, 0),
                 qPrintable(server.errorString())
        );

        QSignalSpy logSpy(&server, &ChatServer::log_message);

        QTcpSocket ana;
        QTcpSocket marko;
        QTcpSocket petar;
        QTcpSocket jovana;
        QTcpSocket nikola;

        connectClient(ana, server);
        connectClient(marko, server);
        connectClient(petar, server);
        connectClient(jovana, server);
        connectClient(nikola, server);

        login(ana, "Ana");
        login(marko, "Marko");
        login(petar, "Petar");
        login(jovana, "Jovana");
        login(nikola, "Nikola");

        readMessages(ana, 300);
        readMessages(marko, 300);
        readMessages(petar, 300);
        readMessages(jovana, 300);
        readMessages(nikola, 300);

        QList<QPair<mafia_game::Role, int>> roles = {
            {mafia_game::Role::Mafia,   2},
            {mafia_game::Role::Citizen, 3}
        };

        QJsonArray players;
        players.append("Ana");
        players.append("Marko");
        players.append("Petar");
        players.append("Jovana");
        players.append("Nikola");

        QJsonObject startMessage =
        mafia_game::create_start_game(roles, players);

        sendJson(ana, startMessage);

        QTest::qWait(100);

        bool found = false;

        for (const QList<QVariant> &args : logSpy) {
            if (!args.isEmpty() &&
                args.at(0).toString() ==
                "Game Stopped. Not enough players") {
                found = true;
            break;
                }
        }

        QVERIFY(found);

        QCOMPARE(
            GameState::get_player_by_username("Ana"),
                 nullptr
        );
    }

    void startGameWithEmptyRolesIsIgnored()
    {
        ChatServer server;

        QVERIFY2(
            server.listen(QHostAddress::LocalHost, 0),
                 qPrintable(server.errorString())
        );

        QTcpSocket ana;

        connectClient(ana, server);
        login(ana, "Ana");

        // Očistimo poruke nastale tokom login-a.
        readMessages(ana, 300);

        QList<QPair<mafia_game::Role, int>> roles;
        QJsonArray players;
        players.append("Ana");

        QJsonObject startMessage =
        mafia_game::create_start_game(roles, players);

        sendJson(ana, startMessage);

        // Server ne bi trebalo da prekine vezu niti da padne.
        QTest::qWait(100);

        QCOMPARE(
            ana.state(),
                 QAbstractSocket::ConnectedState
        );
    }

    void clientCanUseNightAbilityAsAnotherPlayer()
    {
        GameState::reset();
        GameState::set_phase(GamePhase::Inactive);

        ChatServer server;

        QVERIFY2(
            server.listen(QHostAddress::LocalHost, 0),
                 qPrintable(server.errorString())
        );

        QTcpSocket ana;

        connectClient(ana, server);
        login(ana, "Ana");

        // Očistimo poruke nastale tokom prijavljivanja.
        readMessages(ana, 300);

        auto anaPlayer = std::make_shared<Player>("Ana");
        auto markoPlayer = std::make_shared<Player>("Marko");
        auto petarPlayer = std::make_shared<Player>("Petar");

        anaPlayer->set_alive(true);
        markoPlayer->set_alive(true);
        petarPlayer->set_alive(true);

        GameState::add_player(anaPlayer);
        GameState::add_player(markoPlayer);
        GameState::add_player(petarPlayer);

        GameState::set_phase(GamePhase::Night);

        QSignalSpy abilitySpy(
            &server,
            &ChatServer::player_used_ability
        );

        // Klijent je prijavljen kao Ana,
        // ali u JSON poruci tvrdi da je Marko.
        QJsonObject abilityMessage =
        mafia_game::create_vote("Marko", "Petar");

        sendJson(ana, abilityMessage);

        QTest::qWait(100);

        QCOMPARE(
            abilitySpy.count(),
                 0
        );
    }

    void validNightAbilityEmitsPlayerUsedAbility()
    {
        GameState::reset();
        GameState::set_phase(GamePhase::Inactive);

        ChatServer server;

        QVERIFY2(
            server.listen(QHostAddress::LocalHost, 0),
                 qPrintable(server.errorString())
        );

        QTcpSocket ana;

        connectClient(ana, server);
        login(ana, "Ana");

        // Očistimo poruke nastale tokom prijavljivanja.
        readMessages(ana, 300);

        auto anaPlayer = std::make_shared<Player>("Ana");
        auto markoPlayer = std::make_shared<Player>("Marko");

        anaPlayer->set_alive(true);
        markoPlayer->set_alive(true);

        GameState::add_player(anaPlayer);
        GameState::add_player(markoPlayer);

        GameState::set_phase(GamePhase::Night);

        QSignalSpy abilitySpy(
            &server,
            &ChatServer::player_used_ability
        );

        QJsonObject abilityMessage =
        mafia_game::create_vote("Ana", "Marko");

        sendJson(ana, abilityMessage);

        QTRY_COMPARE_WITH_TIMEOUT(
            abilitySpy.count(),
                                  1,
                                  1000
        );

        const QList<QVariant> arguments =
        abilitySpy.takeFirst();

        QCOMPARE(
            arguments.at(0).toString(),
                 QString("Ana")
        );

        QCOMPARE(
            arguments.at(1).toString(),
                 QString("Marko")
        );
    }

    void messageWithoutTextIsIgnored()
    {
        ChatServer server;

        QVERIFY2(
            server.listen(QHostAddress::LocalHost, 0),
                 qPrintable(server.errorString())
        );

        QTcpSocket ana;
        QTcpSocket marko;

        connectClient(ana, server);
        connectClient(marko, server);

        login(ana, "Ana");
        login(marko, "Marko");

        readMessages(ana, 300);
        readMessages(marko, 300);

        QJsonObject message;
        message["type"] = "message";

        sendJson(ana, message);

        const QList<QJsonObject> received =
        readMessages(marko, 300);

        int chatMessages = 0;

        for (const QJsonObject &msg : received) {
            if (msg.value("type").toString() == "message")
                ++chatMessages;
        }

        QCOMPARE(chatMessages, 0);
    }

    void emptyMessageIsIgnored()
    {
        GameState::reset();

        ChatServer server;

        QVERIFY2(
            server.listen(QHostAddress::LocalHost, 0),
                 qPrintable(server.errorString())
        );

        QTcpSocket ana;
        QTcpSocket marko;

        connectClient(ana, server);
        connectClient(marko, server);

        login(ana, "Ana");
        login(marko, "Marko");

        readMessages(ana, 300);
        readMessages(marko, 300);

        QJsonObject message;
        message["type"] = "message";
        message["text"] = "      ";

        sendJson(ana, message);

        const QList<QJsonObject> received =
        readMessages(marko, 300);

        int chatMessages = 0;

        for (const QJsonObject &msg : received) {
            if (msg.value("type").toString() == "message")
                ++chatMessages;
        }

        QCOMPARE(chatMessages, 0);

        GameState::reset();
    }


    void chatMessageIsSentToAnotherClient()
    {
        ChatServer server;

        QVERIFY2(
            server.listen(QHostAddress::LocalHost, 0),
                 qPrintable(server.errorString())
        );

        QTcpSocket ana;
        QTcpSocket marko;

        connectClient(ana, server);
        connectClient(marko, server);

        login(ana, "Ana");
        login(marko, "Marko");

        // Očistimo poruke nastale tokom povezivanja i prijavljivanja.
        readMessages(ana, 300);
        readMessages(marko, 300);

        QJsonObject message;
        message["type"] = "message";
        message["text"] = "Zdravo Marko";

        sendJson(ana, message);

        const QList<QJsonObject> markoMessages =
        readMessages(marko, 500);

        bool found = false;

        for (const QJsonObject &msg : markoMessages) {
            if (msg.value("type").toString() == "message" &&
                msg.value("sender").toString() == "Ana" &&
                msg.value("text").toString() == "Zdravo Marko") {

                found = true;
            break;
                }
        }

        QVERIFY(found);
    }

    void clientCanVoteAsAnotherPlayer()
    {
        GameState::reset();

        auto ana = std::make_shared<Player>(
            "Ana",
            new CitizenRole()
        );

        auto marko = std::make_shared<Player>(
            "Marko",
            new CitizenRole()
        );

        auto petar = std::make_shared<Player>(
            "Petar",
            new CitizenRole()
        );

        ana->set_alive(true);
        marko->set_alive(true);
        petar->set_alive(true);

        GameState::add_player(ana);
        GameState::add_player(marko);
        GameState::add_player(petar);

        GameState::set_phase(GamePhase::Voting);

        ChatServer server;

        QVERIFY(
            server.listen(
                QHostAddress::LocalHost,
                0
            )
        );

        QSignalSpy voteSpy(
            &server,
            &ChatServer::player_voted
        );

        QTcpSocket client;

        client.connectToHost(
            QHostAddress::LocalHost,
            server.serverPort()
        );

        QVERIFY(client.waitForConnected(3000));

        // Klijent je autentifikovan kao ANA.
        QJsonObject loginMessage;
        loginMessage["type"] = "login";
        loginMessage["username"] = "Ana";

        sendJson(client, loginMessage);
        readMessages(client, 500);

        /*
         * Ali u vote poruci tvrdi da je voter MARKO.
         *
         * Ako server ispravno vezuje glas za autentifikovanog
         * korisnika, ova poruka ne bi smela da proizvede:
         *
         *     player_voted("Marko", "Petar")
         */
        QJsonObject forgedVote =
        mafia_game::create_vote(
            "Marko",
            "Petar"
        );

        sendJson(client, forgedVote);

        QTest::qWait(200);
        QCoreApplication::processEvents();

        /*
         * OCEKIVANO BEZBEDNO PONASANJE:
         *
         * Ana ne sme da glasa u Markovo ime.
         */
        QCOMPARE(voteSpy.count(), 0);

        GameState::reset();
    }

    void invalidTargetIsRejected()
    {
        GameState::reset();

        auto ana = std::make_shared<Player>(
            "Ana",
            new CitizenRole()
        );

        ana->set_alive(true);

        GameState::add_player(ana);
        GameState::set_phase(GamePhase::Voting);

        ChatServer server;

        QVERIFY(
            server.listen(
                QHostAddress::LocalHost,
                0
            )
        );

        QSignalSpy voteSpy(
            &server,
            &ChatServer::player_voted
        );

        QTcpSocket client;

        client.connectToHost(
            QHostAddress::LocalHost,
            server.serverPort()
        );

        QVERIFY(client.waitForConnected(3000));

        QJsonObject loginMessage;
        loginMessage["type"] = "login";
        loginMessage["username"] = "Ana";

        sendJson(client, loginMessage);
        readMessages(client, 500);

        QJsonObject vote =
        mafia_game::create_vote(
            "Ana",
            "Nepostojeci"
        );

        sendJson(client, vote);

        QTest::qWait(200);
        QCoreApplication::processEvents();

        QCOMPARE(voteSpy.count(), 0);

        GameState::reset();
    }

    void invalidVoterIsRejected()
    {
        GameState::reset();

        auto ana = std::make_shared<Player>(
            "Ana",
            new CitizenRole()
        );

        ana->set_alive(true);

        GameState::add_player(ana);
        GameState::set_phase(GamePhase::Voting);

        ChatServer server;

        QVERIFY(
            server.listen(
                QHostAddress::LocalHost,
                0
            )
        );

        QSignalSpy voteSpy(
            &server,
            &ChatServer::player_voted
        );

        QTcpSocket client;

        client.connectToHost(
            QHostAddress::LocalHost,
            server.serverPort()
        );

        QVERIFY(client.waitForConnected(3000));

        QJsonObject loginMessage;
        loginMessage["type"] = "login";
        loginMessage["username"] = "Ana";

        sendJson(client, loginMessage);
        readMessages(client, 500);

        // "Nepostojeci" nije u GameState.
        QJsonObject vote =
        mafia_game::create_vote(
            "Nepostojeci",
            "Ana"
        );

        sendJson(client, vote);

        QTest::qWait(200);
        QCoreApplication::processEvents();

        QCOMPARE(voteSpy.count(), 0);

        GameState::reset();
    }

    void validVoteEmitsPlayerVoted()
    {
        GameState::reset();

        auto ana = std::make_shared<Player>(
            "Ana",
            new CitizenRole()
        );

        auto marko = std::make_shared<Player>(
            "Marko",
            new CitizenRole()
        );

        ana->set_alive(true);
        marko->set_alive(true);

        GameState::add_player(ana);
        GameState::add_player(marko);
        GameState::set_phase(GamePhase::Voting);

        ChatServer server;

        QVERIFY2(
            server.listen(QHostAddress::LocalHost, 0),
                 qPrintable(server.errorString())
        );

        QSignalSpy voteSpy(
            &server,
            &ChatServer::player_voted
        );

        QTcpSocket client;

        client.connectToHost(
            QHostAddress::LocalHost,
            server.serverPort()
        );

        QVERIFY(client.waitForConnected(3000));

        // Loginujemo klijenta kao Ana.
        QJsonObject loginMessage;
        loginMessage["type"] = "login";
        loginMessage["username"] = "Ana";

        sendJson(client, loginMessage);

        // Pročitamo login odgovor i waiting-room poruke.
        readMessages(client, 500);

        // Ana glasa za Marka.
        QJsonObject vote =
        mafia_game::create_vote(
            "Ana",
            "Marko"
        );

        sendJson(client, vote);

        QTRY_COMPARE_WITH_TIMEOUT(
            voteSpy.count(),
                                  1,
                                  1000
        );

        const QList<QVariant> arguments =
        voteSpy.takeFirst();

        QCOMPARE(
            arguments.at(0).toString(),
                 QString("Ana")
        );

        QCOMPARE(
            arguments.at(1).toString(),
                 QString("Marko")
        );

        GameState::reset();
    }

    // ============================================================
    // POSTOJEĆI BUG:
    // rezervisano ime voting_for_nobody server prihvata
    // ============================================================

    void votingForNobodyUsernameIsAccepted()
    {
        ChatServer server;

        QVERIFY2(
            server.listen(QHostAddress::LocalHost, 0),
                 qPrintable(server.errorString())
        );

        QTcpSocket client;

        connectClient(client, server);

        const QVector<QJsonObject> messages =
        login(client, "voting_for_nobody");

        bool loginResponseFound = false;

        for (const QJsonObject &message : messages)
        {
            if (message["type"].toString() == "login")
            {
                loginResponseFound = true;

                QVERIFY2(
                    message["success"].toBool(),
                         "Server je odbio username "
                         "'voting_for_nobody'."
                );
            }
        }

        QVERIFY2(
            loginResponseFound,
            "Server nije poslao login odgovor."
        );

        client.disconnectFromHost();
        server.close();
    }


    // ============================================================
    // Normalan login
    // ============================================================

    void validUsernameIsAccepted()
    {
        ChatServer server;

        QVERIFY(
            server.listen(QHostAddress::LocalHost, 0)
        );

        QTcpSocket client;

        connectClient(client, server);

        const QVector<QJsonObject> messages =
        login(client, "Ana");

        bool loginResponseFound = false;

        for (const QJsonObject &message : messages)
        {
            if (message["type"].toString() == "login")
            {
                loginResponseFound = true;

                QCOMPARE(
                    message["success"].toBool(),
                         true
                );
            }
        }

        QVERIFY(loginResponseFound);

        client.disconnectFromHost();
        server.close();
    }


    // ============================================================
    // Duplicate username
    // ============================================================

    void duplicateUsernameIsRejected()
    {
        ChatServer server;

        QVERIFY(
            server.listen(QHostAddress::LocalHost, 0)
        );

        QTcpSocket firstClient;
        QTcpSocket secondClient;

        connectClient(firstClient, server);
        connectClient(secondClient, server);

        login(firstClient, "Ana");

        // Očistimo eventualne poruke koje je prvi klijent dobio
        // kada se drugi povezao.
        readMessages(firstClient, 200);

        const QVector<QJsonObject> messages =
        login(secondClient, "Ana");

        bool rejectionFound = false;

        for (const QJsonObject &message : messages)
        {
            if (message["type"].toString() == "login")
            {
                rejectionFound = true;

                QCOMPARE(
                    message["success"].toBool(),
                         false
                );

                QCOMPARE(
                    message["reason"].toString(),
                         QString("duplicate username")
                );
            }
        }

        QVERIFY2(
            rejectionFound,
            "Server nije poslao odgovor za duplikat username-a."
        );

        firstClient.disconnectFromHost();
        secondClient.disconnectFromHost();

        server.close();
    }


    // ============================================================
    // RequestLists treba da vrati jednu waiting_room_lists poruku
    // ============================================================

    void requestListsReturnsOneListMessage()
    {
        ChatServer server;

        QVERIFY(
            server.listen(QHostAddress::LocalHost, 0)
        );

        QTcpSocket client;

        connectClient(client, server);

        login(client, "Ana");

        // Očistimo sve login/waiting-room poruke.
        readMessages(client, 200);

        QJsonObject request;
        request["type"] = "request_lists";

        sendJson(client, request);

        const QVector<QJsonObject> messages =
        readMessages(client);

        int waitingRoomMessages = 0;

        for (const QJsonObject &message : messages)
        {
            if (message["type"].toString()
                == "waiting_room_lists")
            {
                ++waitingRoomMessages;
            }
        }

        QCOMPARE(
            waitingRoomMessages,
            1
        );

        client.disconnectFromHost();
        server.close();
    }


    // ============================================================
    // BUG:
    //
    // PlayerSelection treba jednom da broadcast-uje novu listu.
    //
    // Trenutni ChatServer::json_from_logged_in ima:
    //
    // case PlayerSelection:
    //     handle_player_selection(...);
    // case RequestLists:
    //     handle_request_lists(...);
    //
    // Nedostaje break.
    //
    // Zbog toga sender dobija dve waiting_room_lists poruke.
    // ============================================================

    void playerSelectionSendsOnlyOneWaitingRoomList()
    {
        ChatServer server;

        QVERIFY(
            server.listen(QHostAddress::LocalHost, 0)
        );

        QTcpSocket client;

        connectClient(client, server);

        login(client, "Ana");

        // Uklanjamo login odgovor i početnu waiting_room_lists
        // poruku.
        readMessages(client, 200);

        QJsonObject selection;

        selection["type"] = "player_selection";
        selection["username"] = "Ana";
        selection["is_selected"] = true;

        sendJson(client, selection);

        const QVector<QJsonObject> messages =
        readMessages(client);

        int waitingRoomMessages = 0;

        for (const QJsonObject &message : messages)
        {
            if (message["type"].toString()
                == "waiting_room_lists")
            {
                ++waitingRoomMessages;
            }
        }

        /*
         * Ispravno ponašanje:
         *
         * handle_player_selection() treba jednom da broadcast-uje
         * novu waiting-room listu.
         *
         * Trenutni kod će verovatno dati:
         *
         * Actual:   2
         * Expected: 1
         */
        QCOMPARE(
            waitingRoomMessages,
            1
        );

        client.disconnectFromHost();
        server.close();
    }
};


QTEST_MAIN(ChatServerTest)

#include "qtest_chatserver.moc"
