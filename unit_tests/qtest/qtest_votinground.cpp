#include <QtTest/QtTest>

#include "votinground.h"

class VotingRoundTest : public QObject
{
    Q_OBJECT

private:

    // Pomoćna funkcija kojom odmah završavamo glasanje,
    // bez čekanja da istekne QTimer.
    bool endVotingRound()
    {
        return QMetaObject::invokeMethod(
            voting::VotingRound::instance(),
                                         "on_voting_ended",
                                         Qt::DirectConnection
        );
    }

private slots:

    // -------------------------------------------------------
    // CLEANUP
    // -------------------------------------------------------
    //
    // VotingRound je singleton i koristi statičko stanje.
    // Zato nakon svakog testa osiguravamo da glasanje
    // više nije aktivno.
    //
    // Ako glasanje nije aktivno, on_voting_ended()
    // samo odmah vraća kontrolu.

    void cleanup()
    {
        endVotingRound();
    }


    // =======================================================
    // TEST 1
    //
    // BUG:
    // "voting_for_nobody" je validan username,
    // ali VoteCounter ga tretira kao specijalnu vrednost.
    // =======================================================

    void playerNamedVotingForNobodyCanBeEliminated()
    {
        voting::VotingRound *round =
        voting::VotingRound::instance();

        QVector<QString> receivedResult;
        QVector<QPair<QString, QString>> receivedVotes;

        bool resultReceived = false;

        // Koristimo lokalni QObject kao context objekat.
        //
        // Kada test završi, receiver se uništava i Qt
        // automatski prekida konekciju, čak i ako QCOMPARE
        // prekine test zbog neuspeha.
        QObject receiver;

        connect(
            round,
            &voting::VotingRound::voting_result,
            &receiver,
            [&](QVector<QString> result,
                QVector<QPair<QString, QString>> votes)
            {
                receivedResult = result;
                receivedVotes = votes;
                resultReceived = true;
            }
        );


        // ---------------------------------------------------
        // Početak glasanja
        // ---------------------------------------------------

        QVector<QString> candidates = {
            "voting_for_nobody",
            "Marko",
            "Ana",
            "Petar"
        };

        round->on_start_voting(candidates);


        // ---------------------------------------------------
        // Glasovi
        // ---------------------------------------------------
        //
        // "voting_for_nobody" je ovde username stvarnog
        // igrača.
        //
        // Ana   -> voting_for_nobody
        // Marko -> voting_for_nobody
        // Petar -> Marko
        //
        // voting_for_nobody = 2 glasa
        // Marko             = 1 glas

        round->on_player_voted(
            "Ana",
            "voting_for_nobody"
        );

        round->on_player_voted(
            "Marko",
            "voting_for_nobody"
        );

        round->on_player_voted(
            "Petar",
            "Marko"
        );


        // ---------------------------------------------------
        // Završetak glasanja
        // ---------------------------------------------------

        QVERIFY(endVotingRound());

        QVERIFY(resultReceived);


        // ---------------------------------------------------
        // Provera registrovanih glasova
        // ---------------------------------------------------

        QCOMPARE(receivedVotes.size(), 3);

        QVERIFY(
            receivedVotes.contains(
                qMakePair(
                    QString("Ana"),
                          QString("voting_for_nobody")
                )
            )
        );

        QVERIFY(
            receivedVotes.contains(
                qMakePair(
                    QString("Marko"),
                          QString("voting_for_nobody")
                )
            )
        );

        QVERIFY(
            receivedVotes.contains(
                qMakePair(
                    QString("Petar"),
                          QString("Marko")
                )
            )
        );


        // ---------------------------------------------------
        // Provera koja trenutno otkriva bug
        // ---------------------------------------------------
        //
        // Očekivano:
        //
        // ["voting_for_nobody"]
        //
        // Trenutno:
        //
        // []
        //
        // Zato očekujemo da test trenutno PADNE.

        QCOMPARE(receivedResult.size(), 1);

        QCOMPARE(
            receivedResult.first(),
                 QString("voting_for_nobody")
        );
    }


    // =======================================================
    // TEST 2
    //
    // Glas poslat pre početka glasanja mora biti ignorisan.
    // =======================================================

    void voteBeforeVotingStartsIsIgnored()
    {
        voting::VotingRound *round =
        voting::VotingRound::instance();

        QVector<QPair<QString, QString>> receivedVotes;

        bool resultReceived = false;

        QObject receiver;

        connect(
            round,
            &voting::VotingRound::voting_result,
            &receiver,
            [&](QVector<QString>,
                QVector<QPair<QString, QString>> votes)
            {
                receivedVotes = votes;
                resultReceived = true;
            }
        );


        // ---------------------------------------------------
        // Glas pre početka runde
        // ---------------------------------------------------
        //
        // m_vote_is_ongoing == false
        //
        // Zato ovaj glas mora biti ignorisan.

        round->on_player_voted(
            "Ana",
            "Marko"
        );


        // ---------------------------------------------------
        // Sada stvarno pokrećemo glasanje
        // ---------------------------------------------------

        QVector<QString> candidates = {
            "Marko",
            "Petar"
        };

        round->on_start_voting(candidates);


        // Ovo je validan glas jer je runda aktivna.

        round->on_player_voted(
            "Petar",
            "Marko"
        );


        QVERIFY(endVotingRound());

        QVERIFY(resultReceived);


        // Treba da postoji samo glas Petar -> Marko.
        //
        // Glas Ana -> Marko, koji je poslat pre početka
        // glasanja, ne sme biti registrovan.

        QCOMPARE(receivedVotes.size(), 1);

        QVERIFY(
            receivedVotes.contains(
                qMakePair(
                    QString("Petar"),
                          QString("Marko")
                )
            )
        );

        QVERIFY(
            !receivedVotes.contains(
                qMakePair(
                    QString("Ana"),
                          QString("Marko")
                )
            )
        );
    }


    // =======================================================
    // TEST 3
    //
    // Ponovni pokušaj pokretanja glasanja dok ono već traje
    // ne sme resetovati postojeće glasove.
    // =======================================================

    void startingVotingTwiceDoesNotResetCurrentVotes()
    {
        voting::VotingRound *round =
        voting::VotingRound::instance();

        QVector<QPair<QString, QString>> receivedVotes;

        bool resultReceived = false;

        int votingStartedCount = 0;

        QObject receiver;

        connect(
            round,
            &voting::VotingRound::inform_clients_voting_started,
            &receiver,
            [&]()
            {
                ++votingStartedCount;
            }
        );

        connect(
            round,
            &voting::VotingRound::voting_result,
            &receiver,
            [&](QVector<QString>,
                QVector<QPair<QString, QString>> votes)
            {
                receivedVotes = votes;
                resultReceived = true;
            }
        );


        QVector<QString> candidates = {
            "Marko",
            "Petar",
            "Ana"
        };


        // ---------------------------------------------------
        // Prvo pokretanje
        // ---------------------------------------------------

        round->on_start_voting(candidates);

        QCOMPARE(votingStartedCount, 1);


        // Registrujemo glas.

        round->on_player_voted(
            "Ana",
            "Marko"
        );


        // ---------------------------------------------------
        // Pokušavamo ponovo da pokrenemo glasanje
        // ---------------------------------------------------
        //
        // Implementacija sadrži:
        //
        // if (m_vote_is_ongoing) {
        //     return;
        // }
        //
        // Zato ovaj poziv ne sme:
        //
        // - emitovati novi voting_started signal;
        // - obrisati prethodno registrovan glas.

        round->on_start_voting(candidates);


        // Signal mora i dalje biti emitovan samo jednom.

        QCOMPARE(votingStartedCount, 1);


        // Dodajemo još jedan glas u istoj rundi.

        round->on_player_voted(
            "Petar",
            "Marko"
        );


        QVERIFY(endVotingRound());

        QVERIFY(resultReceived);


        // Ako drugi on_start_voting() nije resetovao rundu,
        // oba glasa moraju postojati.

        QCOMPARE(receivedVotes.size(), 2);

        QVERIFY(
            receivedVotes.contains(
                qMakePair(
                    QString("Ana"),
                          QString("Marko")
                )
            )
        );

        QVERIFY(
            receivedVotes.contains(
                qMakePair(
                    QString("Petar"),
                          QString("Marko")
                )
            )
        );
    }


    // =======================================================
    // TEST 4
    //
    // Završavanje glasanja kada nijedna runda nije aktivna
    // ne sme emitovati rezultat.
    // =======================================================

    void endingVotingWhenNoRoundIsActiveDoesNothing()
    {
        voting::VotingRound *round =
        voting::VotingRound::instance();

        int resultSignalCount = 0;

        QObject receiver;

        connect(
            round,
            &voting::VotingRound::voting_result,
            &receiver,
            [&](QVector<QString>,
                QVector<QPair<QString, QString>>)
            {
                ++resultSignalCount;
            }
        );


        // Na početku testa ne postoji aktivna runda.
        //
        // on_voting_ended() treba da izvrši:
        //
        // if (!m_vote_is_ongoing) {
        //     return;
        // }
        //
        // i ne sme emitovati voting_result.

        QVERIFY(endVotingRound());

        QCOMPARE(resultSignalCount, 0);
    }
};


QTEST_MAIN(VotingRoundTest)

#include "qtest_votinground.moc"
