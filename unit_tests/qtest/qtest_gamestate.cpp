#include <QtTest>

#include "GameState.h"
#include "Player.h"
#include "mafiarole.h"
#include "citizenrole.h"
#include "DoctorRole.h"
#include "DetectiveRole.h"

class GameStateTest : public QObject
{
    Q_OBJECT

private slots:

    void mafiaPlayerIsConvertedToPlayerState()
    {
        GameState gameState;

        auto player =
        std::make_shared<Player>(
            QStringLiteral("Mafija"),
                                 new MafiaRole()
        );

        player->set_alive(true);

        PlayerState state =
        gameState.player_to_player_state(player);

        QCOMPARE(
            state.username,
            QStringLiteral("Mafija")
        );

        QCOMPARE(
            state.alive,
            true
        );

        QCOMPARE(
            state.role,
            mafia_game::Role::Mafia
        );
    }

    void citizenPlayerIsConvertedToPlayerState()
    {
        GameState gameState;

        auto player =
        std::make_shared<Player>(
            QStringLiteral("Gradjanin"),
                                 new CitizenRole()
        );

        player->set_alive(true);

        PlayerState state =
        gameState.player_to_player_state(player);

        QCOMPARE(
            state.username,
            QStringLiteral("Gradjanin")
        );

        QCOMPARE(
            state.alive,
            true
        );

        QCOMPARE(
            state.role,
            mafia_game::Role::Citizen
        );
    }

    void detectivePlayerIsConvertedToPlayerState()
    {
        GameState gameState;

        auto player =
        std::make_shared<Player>(
            QStringLiteral("Detektiv"),
                                 new DetectiveRole()
        );

        player->set_alive(true);

        PlayerState state =
        gameState.player_to_player_state(player);

        QCOMPARE(
            state.username,
            QStringLiteral("Detektiv")
        );

        QCOMPARE(
            state.alive,
            true
        );

        QCOMPARE(
            state.role,
            mafia_game::Role::Police
        );
    }

    void doctorPlayerIsConvertedToPlayerState()
    {
        GameState gameState;

        auto player =
        std::make_shared<Player>(
            QStringLiteral("Doktor"),
                                 new DoctorRole()
        );

        player->set_alive(false);

        PlayerState state =
        gameState.player_to_player_state(player);

        QCOMPARE(
            state.username,
            QStringLiteral("Doktor")
        );

        QCOMPARE(
            state.alive,
            false
        );

        QCOMPARE(
            state.role,
            mafia_game::Role::Doctor
        );
    }

    void nonExistingPlayerCannotAct()
    {
        QVERIFY(
            !GameState::can_player_act(
                QStringLiteral("Unknown")
            )
        );
    }

    void deadPlayerCannotAct()
    {
        auto player =
        std::make_shared<Player>(
            QStringLiteral("Marko"),
                                 new MafiaRole()
        );

        player->set_alive(false);

        GameState::add_player(player);

        QVERIFY(
            !GameState::can_player_act(
                QStringLiteral("Marko")
            )
        );
    }

    void playerWithoutRoleCannotAct()
    {
        auto player =
        std::make_shared<Player>(
            QStringLiteral("Marko")
        );

        player->set_alive(true);

        GameState::add_player(player);

        QVERIFY(
            !GameState::can_player_act(
                QStringLiteral("Marko")
            )
        );
    }

    void mafiaCanAct()
    {
        auto player =
        std::make_shared<Player>(
            QStringLiteral("Mafija"),
                                 new MafiaRole()
        );

        player->set_alive(true);

        GameState::add_player(player);

        QVERIFY(
            GameState::can_player_act(
                QStringLiteral("Mafija")
            )
        );
    }

    void citizenCannotAct()
    {
        auto player =
        std::make_shared<Player>(
            QStringLiteral("Gradjanin"),
                                 new CitizenRole()
        );

        player->set_alive(true);

        GameState::add_player(player);

        QVERIFY(
            !GameState::can_player_act(
                QStringLiteral("Gradjanin")
            )
        );
    }

    void doctorCanAct()
    {
        auto player =
        std::make_shared<Player>(
            QStringLiteral("Doktor"),
                                 new DoctorRole()
        );

        player->set_alive(true);

        GameState::add_player(player);

        QVERIFY(
            GameState::can_player_act(
                QStringLiteral("Doktor")
            )
        );
    }

    void detectiveCanAct()
    {
        auto player =
        std::make_shared<Player>(
            QStringLiteral("Detektiv"),
                                 new DetectiveRole()
        );

        player->set_alive(true);

        GameState::add_player(player);

        QVERIFY(
            GameState::can_player_act(
                QStringLiteral("Detektiv")
            )
        );
    }

    void deadPlayerCannotChat()
    {
        auto player =
        std::make_shared<Player>(
            QStringLiteral("Marko"),
                                 new CitizenRole()
        );

        player->set_alive(false);

        GameState::add_player(player);

        GameState::set_phase(
            GamePhase::Day
        );

        QVERIFY(
            !GameState::can_player_chat(
                QStringLiteral("Marko")
            )
        );
    }

    void alivePlayerCanChatDuringDay()
    {
        auto player =
        std::make_shared<Player>(
            QStringLiteral("Marko"),
                                 new CitizenRole()
        );

        player->set_alive(true);

        GameState::add_player(player);

        GameState::set_phase(
            GamePhase::Day
        );

        QVERIFY(
            GameState::can_player_chat(
                QStringLiteral("Marko")
            )
        );
    }

    void alivePlayerCanChatDuringDiscussion()
    {
        auto player =
        std::make_shared<Player>(
            QStringLiteral("Marko"),
                                 new CitizenRole()
        );

        player->set_alive(true);

        GameState::add_player(player);

        GameState::set_phase(
            GamePhase::Discussion
        );

        QVERIFY(
            GameState::can_player_chat(
                QStringLiteral("Marko")
            )
        );
    }

    void mafiaCanChatDuringNight()
    {
        auto player =
        std::make_shared<Player>(
            QStringLiteral("Mafija"),
                                 new MafiaRole()
        );

        player->set_alive(true);

        GameState::add_player(player);

        GameState::set_phase(
            GamePhase::Night
        );

        QVERIFY(
            GameState::can_player_chat(
                QStringLiteral("Mafija")
            )
        );
    }

    void citizenCannotChatDuringNight()
    {
        auto player =
        std::make_shared<Player>(
            QStringLiteral("Gradjanin"),
                                 new CitizenRole()
        );

        player->set_alive(true);

        GameState::add_player(player);

        GameState::set_phase(
            GamePhase::Night
        );

        QVERIFY(
            !GameState::can_player_chat(
                QStringLiteral("Gradjanin")
            )
        );
    }

    void alivePlayerCannotChatDuringVoting()
    {
        auto player =
        std::make_shared<Player>(
            QStringLiteral("Marko"),
                                 new CitizenRole()
        );

        player->set_alive(true);

        GameState::add_player(player);

        GameState::set_phase(
            GamePhase::Voting
        );

        QVERIFY(
            !GameState::can_player_chat(
                QStringLiteral("Marko")
            )
        );
    }

    void nonExistingPlayerCannotChat()
    {
        GameState::set_phase(
            GamePhase::Day
        );

        QVERIFY(
            !GameState::can_player_chat(
                QStringLiteral("Unknown")
            )
        );
    }

    void init()
    {
        GameState::reset();
    }

    void addNullPlayerIsIgnored()
    {
        GameState::add_player(nullptr);

        QCOMPARE(
            GameState::get_alive_player_count(),
                 0
        );
    }

    void nonExistingPlayerIsNotAlive()
    {
        const bool alive =
        GameState::is_player_alive(
            QStringLiteral("Unknown")
        );

        QVERIFY(!alive);
    }

    void killingNonExistingPlayerIsIgnored()
    {
        GameState::kill_player(
            QStringLiteral("Unknown")
        );

        QCOMPARE(
            GameState::get_alive_player_count(),
                 0
        );
    }

    void killingAlreadyDeadPlayerIsIgnored()
    {
        auto player =
        std::make_shared<Player>(
            QStringLiteral("Marko")
        );

        player->set_alive(false);

        GameState::add_player(player);

        GameState::kill_player(
            QStringLiteral("Marko")
        );

        QVERIFY(!player->is_alive());
    }

    void nonExistingPlayerCannotVote()
    {
        GameState::set_phase(
            GamePhase::Voting
        );

        const bool canVote =
        GameState::can_player_vote(
            QStringLiteral("Unknown")
        );

        QVERIFY(!canVote);
    }

    void alivePlayerCanVoteDuringVoting()
    {
        auto player =
        std::make_shared<Player>(
            QStringLiteral("Marko")
        );

        player->set_alive(true);

        GameState::add_player(player);

        GameState::set_phase(
            GamePhase::Voting
        );

        const bool canVote =
        GameState::can_player_vote(
            QStringLiteral("Marko")
        );

        QVERIFY(canVote);
    }

    void playerCannotVoteOutsideVoting()
    {
        auto player =
        std::make_shared<Player>(
            QStringLiteral("Marko")
        );

        player->set_alive(true);

        GameState::add_player(player);

        GameState::set_phase(
            GamePhase::Day
        );

        QVERIFY(
            !GameState::can_player_vote(
                QStringLiteral("Marko")
            )
        );
    }

    void deadPlayerCannotVote()
    {
        auto player =
        std::make_shared<Player>(
            QStringLiteral("Marko")
        );

        player->set_alive(false);

        GameState::add_player(player);

        GameState::set_phase(
            GamePhase::Voting
        );

        QVERIFY(
            !GameState::can_player_vote(
                QStringLiteral("Marko")
            )
        );
    }

    void alivePlayerIsValidTarget()
    {
        auto player =
        std::make_shared<Player>(
            QStringLiteral("Marko")
        );

        player->set_alive(true);

        GameState::add_player(player);

        QVERIFY(
            GameState::is_valid_target(
                QStringLiteral("Marko")
            )
        );
    }

    void votingTimeIsTrueDuringVoting()
    {
        GameState gameState;

        GameState::set_phase(
            GamePhase::Voting
        );

        QVERIFY(
            gameState.is_voting_time()
        );
    }

    void votingTimeIsTrueDuringMafiaVoting()
    {
        GameState gameState;

        GameState::set_phase(
            GamePhase::Mafia_voting
        );

        QVERIFY(
            gameState.is_voting_time()
        );
    }

    void votingTimeIsFalseDuringDay()
    {
        GameState gameState;

        GameState::set_phase(
            GamePhase::Day
        );

        QVERIFY(
            !gameState.is_voting_time()
        );
    }

    void nightIsTrueDuringNight()
    {
        GameState gameState;

        GameState::set_phase(
            GamePhase::Night
        );

        QVERIFY(
            gameState.is_night()
        );
    }

    void nightIsFalseDuringDay()
    {
        GameState gameState;

        GameState::set_phase(
            GamePhase::Day
        );

        QVERIFY(
            !gameState.is_night()
        );
    }


    void winnerIsNoneBeforeGameOver()
    {
        QCOMPARE(
            GameState::get_winner(),
                 QStringLiteral("None")
        );
    }

    void phaseDescriptionIsDay()
    {
        GameState gameState;

        GameState::set_phase(
            GamePhase::Day
        );

        QCOMPARE(
            gameState.get_phase_description(),
                 QStringLiteral("Day")
        );
    }

    void phaseDescriptionIsNight()
    {
        GameState gameState;

        GameState::set_phase(
            GamePhase::Night
        );

        QCOMPARE(
            gameState.get_phase_description(),
                 QStringLiteral("Night")
        );
    }

    void phaseDescriptionIsDiscussion()
    {
        GameState gameState;

        GameState::set_phase(
            GamePhase::Discussion
        );

        QCOMPARE(
            gameState.get_phase_description(),
                 QStringLiteral("Discussion")
        );
    }

    void phaseDescriptionIsVoting()
    {
        GameState gameState;

        GameState::set_phase(
            GamePhase::Voting
        );

        QCOMPARE(
            gameState.get_phase_description(),
                 QStringLiteral("Voting")
        );
    }

    void phaseDescriptionIsMafiaVoting()
    {
        GameState gameState;

        GameState::set_phase(
            GamePhase::Mafia_voting
        );

        QCOMPARE(
            gameState.get_phase_description(),
                 QStringLiteral("Mafia_voting")
        );
    }

    void phaseDescriptionIsEnd()
    {
        GameState gameState;

        GameState::set_phase(
            GamePhase::End
        );

        QCOMPARE(
            gameState.get_phase_description(),
                 QStringLiteral("End")
        );
    }


    void chatPermissionIsAllAliveDuringDay()
    {
        GameState::set_phase(
            GamePhase::Day
        );

        QCOMPARE(
            GameState::get_chat_permission(),
                 ChatPermission::AllAlive
        );
    }

    void chatPermissionIsAllAliveDuringDiscussion()
    {
        GameState::set_phase(
            GamePhase::Discussion
        );

        QCOMPARE(
            GameState::get_chat_permission(),
                 ChatPermission::AllAlive
        );
    }

    void chatPermissionIsMafiaOnlyDuringNight()
    {
        GameState::set_phase(
            GamePhase::Night
        );

        QCOMPARE(
            GameState::get_chat_permission(),
                 ChatPermission::MafiaOnly
        );
    }

    void chatPermissionIsNoneDuringVoting()
    {
        GameState::set_phase(
            GamePhase::Voting
        );

        QCOMPARE(
            GameState::get_chat_permission(),
                 ChatPermission::None
        );
    }

    void gameIsNotOverAfterReset()
    {
        QVERIFY(
            !GameState::is_game_over()
        );
    }

    void existingPlayerExists()
    {
        auto player =
        std::make_shared<Player>(
            QStringLiteral("Marko")
        );

        GameState::add_player(player);

        QVERIFY(
            GameState::player_exists(
                QStringLiteral("Marko")
            )
        );
    }

    void unknownPlayerDoesNotExist()
    {
        QVERIFY(
            !GameState::player_exists(
                QStringLiteral("Unknown")
            )
        );
    }

    void nonExistingPlayerIsNotValidTarget()
    {
        const bool valid =
        GameState::is_valid_target(
            QStringLiteral("Unknown")
        );

        QVERIFY(!valid);
    }

    void deadPlayerIsNotValidTarget()
    {
        auto player =
        std::make_shared<Player>(
            QStringLiteral("Marko")
        );

        player->set_alive(false);

        GameState::add_player(player);

        const bool valid =
        GameState::is_valid_target(
            QStringLiteral("Marko")
        );

        QVERIFY(!valid);
    }
};

QTEST_MAIN(GameStateTest)

#include "qtest_gamestate.moc"
