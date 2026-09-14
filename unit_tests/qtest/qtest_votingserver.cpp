#include <QtTest/QtTest>
#include <QSignalSpy>

#include "votingserver.h"
#include "GameState.h"

class VotingServerTest : public QObject
{
    Q_OBJECT

private slots:

    void init()
    {
        // Svaki test počinje van voting faze.
        GameState::set_phase(GamePhase::Intermission);
    }


    // ============================================================
    // Pokretanje glasanja
    // ============================================================

    void startVotingChangesPhaseAndEmitsSignal()
    {
        auto *server = voting::VotingServer::instance();

        QSignalSpy startSpy(
            server,
            &voting::VotingServer::start_voting_round
        );

        QVector<QString> candidates = {
            "Ana",
            "Marko",
            "Petar"
        };

        server->on_start_voting_phase(candidates);

        QCOMPARE(
            GameState::get_phase(),
            GamePhase::Voting
        );

        QCOMPARE(startSpy.count(), 1);

        const QList<QVariant> arguments =
            startSpy.takeFirst();

        const QVector<QString> receivedCandidates =
            qvariant_cast<QVector<QString>>(
                arguments.at(0)
            );

        QCOMPARE(
            receivedCandidates,
            candidates
        );
    }


    // ============================================================
    // Ponovni start voting faze
    // ============================================================

    void startingVotingTwiceIsIgnored()
    {
        auto *server = voting::VotingServer::instance();

        QSignalSpy startSpy(
            server,
            &voting::VotingServer::start_voting_round
        );

        QVector<QString> firstCandidates = {
            "Ana",
            "Marko"
        };

        QVector<QString> secondCandidates = {
            "Petar"
        };

        server->on_start_voting_phase(
            firstCandidates
        );

        server->on_start_voting_phase(
            secondCandidates
        );

        QCOMPARE(
            GameState::get_phase(),
            GamePhase::Voting
        );

        QCOMPARE(
            startSpy.count(),
            1
        );
    }


    // ============================================================
    // Jedan pobednik
    // ============================================================

    void singleVotingResultEndsVoting()
    {
        auto *server = voting::VotingServer::instance();

        QVector<QString> candidates = {
            "Ana",
            "Marko"
        };

        server->on_start_voting_phase(
            candidates
        );

        QSignalSpy resultSpy(
            server,
            &voting::VotingServer::
                inform_clients_of_voting_result
        );

        QSignalSpy concludedSpy(
            server,
            &voting::VotingServer::
                voting_has_concluded
        );

        QVector<QString> result = {
            "Marko"
        };

        QVector<QPair<QString, QString>> votes = {
            qMakePair(
                QString("Ana"),
                QString("Marko")
            )
        };

        server->on_voting_round_result(
            result,
            votes
        );

        QCOMPARE(
            GameState::get_phase(),
            GamePhase::Intermission
        );

        QCOMPARE(
            resultSpy.count(),
            1
        );

        QCOMPARE(
            concludedSpy.count(),
            1
        );
    }


    // ============================================================
    // Nema izglasanog igrača
    // ============================================================

    void emptyVotingResultEndsVoting()
    {
        auto *server = voting::VotingServer::instance();

        server->on_start_voting_phase({
            "Ana",
            "Marko"
        });

        QSignalSpy concludedSpy(
            server,
            &voting::VotingServer::
                voting_has_concluded
        );

        QVector<QString> result;

        QVector<QPair<QString, QString>> votes;

        server->on_voting_round_result(
            result,
            votes
        );

        QCOMPARE(
            GameState::get_phase(),
            GamePhase::Intermission
        );

        QCOMPARE(
            concludedSpy.count(),
            1
        );
    }


    // ============================================================
    // Tie u prvom krugu
    // ============================================================

    void tieInFirstRoundStartsSecondRound()
    {
        auto *server = voting::VotingServer::instance();

        server->on_start_voting_phase({
            "Ana",
            "Marko",
            "Petar"
        });

        QSignalSpy startSpy(
            server,
            &voting::VotingServer::
                start_voting_round
        );

        QSignalSpy resultSpy(
            server,
            &voting::VotingServer::
                inform_clients_of_voting_result
        );

        QSignalSpy concludedSpy(
            server,
            &voting::VotingServer::
                voting_has_concluded
        );

        QVector<QString> result = {
            "Ana",
            "Marko"
        };

        QVector<QPair<QString, QString>> votes = {
            qMakePair(
                QString("Ana"),
                QString("Marko")
            ),
            qMakePair(
                QString("Marko"),
                QString("Ana")
            )
        };

        server->on_voting_round_result(
            result,
            votes
        );

        QCOMPARE(
            GameState::get_phase(),
            GamePhase::Voting
        );

        QCOMPARE(
            startSpy.count(),
            1
        );

        QCOMPARE(
            resultSpy.count(),
            1
        );

        QCOMPARE(
            concludedSpy.count(),
            0
        );

        const QList<QVariant> arguments =
            resultSpy.takeFirst();

        const QVector<QString> sentResult =
            qvariant_cast<QVector<QString>>(
                arguments.at(0)
            );

        // U prvom krugu sa tie-om klijentima
        // se šalje prazan rezultat.
        QVERIFY(sentResult.isEmpty());
    }


    // ============================================================
    // Tie u drugom krugu
    // ============================================================

    void tieInSecondRoundEndsVoting()
    {
        auto *server = voting::VotingServer::instance();

        server->on_start_voting_phase({
            "Ana",
            "Marko"
        });

        QVector<QString> firstResult = {
            "Ana",
            "Marko"
        };

        QVector<QPair<QString, QString>> firstVotes;

        // Ovo prebacuje server u drugi voting round.
        server->on_voting_round_result(
            firstResult,
            firstVotes
        );

        QSignalSpy concludedSpy(
            server,
            &voting::VotingServer::
                voting_has_concluded
        );

        QVector<QString> secondResult = {
            "Ana",
            "Marko"
        };

        QVector<QPair<QString, QString>> secondVotes;

        server->on_voting_round_result(
            secondResult,
            secondVotes
        );

        QCOMPARE(
            GameState::get_phase(),
            GamePhase::Intermission
        );

        QCOMPARE(
            concludedSpy.count(),
            1
        );

        const QList<QVariant> arguments =
            concludedSpy.takeFirst();

        const QVector<QString> receivedResult =
            qvariant_cast<QVector<QString>>(
                arguments.at(0)
            );

        QCOMPARE(
            receivedResult,
            secondResult
        );
    }


    // ============================================================
    // Rezultat van voting faze
    // ============================================================

    void resultOutsideVotingPhaseIsIgnored()
    {
        auto *server = voting::VotingServer::instance();

        GameState::set_phase(
            GamePhase::Intermission
        );

        QSignalSpy resultSpy(
            server,
            &voting::VotingServer::
                inform_clients_of_voting_result
        );

        QSignalSpy concludedSpy(
            server,
            &voting::VotingServer::
                voting_has_concluded
        );

        server->on_voting_round_result(
            {"Marko"},
            {}
        );

        QCOMPARE(
            GameState::get_phase(),
            GamePhase::Intermission
        );

        QCOMPARE(
            resultSpy.count(),
            0
        );

        QCOMPARE(
            concludedSpy.count(),
            0
        );
    }


    void cleanup()
    {
        GameState::set_phase(
            GamePhase::Intermission
        );
    }
};

QTEST_MAIN(VotingServerTest)

#include "qtest_votingserver.moc"
