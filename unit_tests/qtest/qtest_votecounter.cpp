#include <QtTest/QtTest>
#include "votecounter.h"

class VoteCounterTest : public QObject
{
    Q_OBJECT

private slots:

    // Igrač čiji je username "voting_for_nobody"
    // može normalno da glasa za drugog igrača.
    void votingForNobodyCanVote()
    {
        voting::VoteCounter counter;

        counter.add_vote("voting_for_nobody", "Marko");
        counter.add_vote("Ana", "Marko");

        QVector<QString> result = counter.get_result();

        QCOMPARE(result.size(), 1);
        QCOMPARE(result.first(), QString("Marko"));
    }

    // "voting_for_nobody" dobija najviše glasova.
    // Očekujemo da se "voting_for_nobody" nalazi u rezultatu.
    void votedNoneStrictlyWins()
    {
        voting::VoteCounter counter;

        counter.add_vote("Ana", "voting_for_nobody");
        counter.add_vote("Jovan", "voting_for_nobody");
        counter.add_vote("Milica", "voting_for_nobody");
        counter.add_vote("Petar", "Marko");

        QVector<QString> result = counter.get_result();

        QCOMPARE(result.size(), 1);
        QCOMPARE(result.first(), QString("voting_for_nobody"));
    }

    // "voting_for_nobody" ima manje glasova od drugog kandidata.
    // Očekujemo da rezultat sadrži samo kandidata sa najviše glasova.
    void votedNoneIsNotWinner()
    {
        voting::VoteCounter counter;

        counter.add_vote("Ana", "voting_for_nobody");
        counter.add_vote("Jovan", "Marko");
        counter.add_vote("Milica", "Marko");
        counter.add_vote("Petar", "Nikola");

        QVector<QString> result = counter.get_result();

        QCOMPARE(result.size(), 1);
        QCOMPARE(result.first(), QString("Marko"));
    }

    // "voting_for_nobody" i Marko imaju isti maksimalan broj glasova.
    // Očekujemo da rezultat sadrži oba kandidata.
    void votedNoneTiedWithPlayer()
    {
        voting::VoteCounter counter;

        counter.add_vote("Ana", "voting_for_nobody");
        counter.add_vote("Jovan", "voting_for_nobody");
        counter.add_vote("Milica", "Marko");
        counter.add_vote("Petar", "Marko");

        QVector<QString> result = counter.get_result();

        QCOMPARE(result.size(), 2);

        QVERIFY(result.contains(QString("voting_for_nobody")));
        QVERIFY(result.contains(QString("Marko")));
    }

    // Glas igrača "voting_for_nobody" treba normalno
    // da bude registrovan u listi glasova.
    void getVotesIncludesVotingForNobodyPlayer()
    {
        voting::VoteCounter counter;

        counter.add_vote("voting_for_nobody", "Marko");
        counter.add_vote("Ana", "Nikola");

        QVector<QPair<QString, QString>> votes = counter.get_votes();

        QCOMPARE(votes.size(), 2);

        bool foundVotingForNobody = false;
        bool foundAna = false;

        for (const auto &vote : votes) {

            if (vote.first == "voting_for_nobody" &&
                vote.second == "Marko") {
                foundVotingForNobody = true;
                }

                if (vote.first == "Ana" &&
                    vote.second == "Nikola") {
                    foundAna = true;
                    }
        }

        QVERIFY(foundVotingForNobody);
        QVERIFY(foundAna);
    }

    // "voting_for_nobody" može biti i glasač i kandidat.
    // U ovom slučaju on dobija dva glasa, a Marko jedan.
    // Očekujemo da "voting_for_nobody" bude jedini pobednik.
    void votingForNobodyAsPlayerAndVoteTarget()
    {
        voting::VoteCounter counter;

        counter.add_vote("Ana", "voting_for_nobody");
        counter.add_vote("Jovan", "voting_for_nobody");
        counter.add_vote("voting_for_nobody", "Marko");

        QVector<QString> result = counter.get_result();

        QCOMPARE(result.size(), 1);
        QCOMPARE(result.first(), QString("voting_for_nobody"));
    }
};

QTEST_MAIN(VoteCounterTest)

#include "qtest_votecounter.moc"
