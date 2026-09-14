#include <cstddef>
#include <cstdint>

#include <QByteArray>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QList>
#include <QMap>
#include <QPair>
#include <QString>
#include <QVector>

#include "protocol.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size == 0 || size > 4096) {
        return 0;
    }

    const QByteArray bytes(
        reinterpret_cast<const char *>(data),
                           static_cast<int>(size)
    );

    // ============================================================
    // 1. QString konverzije
    // ============================================================

    const QString input = QString::fromUtf8(bytes);

    const mafia_game::MessageType input_message_type =
    mafia_game::message_type_from_string(input);

    const mafia_game::Role input_role =
    mafia_game::role_from_string(input);

    mafia_game::message_type_to_string(input_message_type);
    mafia_game::role_to_string(input_role);


    // ============================================================
    // 2. JSON parsiranje
    // ============================================================

    QJsonParseError parse_error;

    const QJsonDocument document =
    QJsonDocument::fromJson(bytes, &parse_error);

    if (parse_error.error != QJsonParseError::NoError) {
        return 0;
    }

    if (!document.isObject()) {
        return 0;
    }

    const QJsonObject object = document.object();


    // ============================================================
    // 3. Type
    // ============================================================

    const QJsonValue type_value =
    object.value(mafia_game::KEY_TYPE);

    if (type_value.isString()) {
        const mafia_game::MessageType type =
        mafia_game::message_type_from_string(
            type_value.toString()
        );

        mafia_game::message_type_to_string(type);
    }


    // ============================================================
    // 4. Role
    // ============================================================

    mafia_game::Role role = mafia_game::Role::Unknown;

    const QJsonValue role_value =
    object.value(mafia_game::KEY_ROLE);

    if (role_value.isString()) {
        role =
        mafia_game::role_from_string(
            role_value.toString()
        );

        mafia_game::role_to_string(role);
    }


    // ============================================================
    // 5. Osnovna polja
    // ============================================================

    QString username =
    object.value(mafia_game::KEY_USERNAME).toString();

    const QString sender =
    object.value(mafia_game::KEY_SENDER).toString();

    const QString text =
    object.value(mafia_game::KEY_TEXT).toString();

    const QString target =
    object.value(mafia_game::KEY_TARGET).toString();

    const QString phase =
    object.value(mafia_game::KEY_PHASE).toString();

    const QString winner =
    object.value(mafia_game::KEY_WINNER).toString();

    const QString voter =
    object.value(mafia_game::KEY_VOTER).toString();

    const QString reason =
    object.value(mafia_game::KEY_REASON).toString();

    const bool alive =
    object.value(mafia_game::KEY_ALIVE).toBool();

    const bool is_selected =
    object.value(mafia_game::KEY_IS_SELECTED).toBool();

    const int day_number =
    object.value(mafia_game::KEY_DAY_NUMBER).toInt();

    const QJsonArray players =
    object.value(mafia_game::KEY_PLAYERS).toArray();


    // ============================================================
    // 6. Obrada players niza
    // ============================================================

    for (const QJsonValue &player_value : players) {

        if (!player_value.isObject()) {
            continue;
        }

        const QJsonObject player =
        player_value.toObject();

        const QString player_username =
        player.value(
            mafia_game::KEY_USERNAME
        ).toString();

        const QJsonValue player_role_value =
        player.value(
            mafia_game::KEY_ROLE
        );

        if (player_role_value.isString()) {
            const mafia_game::Role player_role =
            mafia_game::role_from_string(
                player_role_value.toString()
            );

            mafia_game::role_to_string(player_role);
        }

        const bool player_alive =
        player.value(
            mafia_game::KEY_ALIVE
        ).toBool();

        (void)player_username;
        (void)player_alive;
    }


    // ============================================================
    // 7. Voting podaci
    // ============================================================

    QVector<QString> voted_out;
    QVector<QPair<QString, QString>> votes;

    const QJsonValue target_value =
    object.value(mafia_game::KEY_TARGET);

    if (target_value.isArray()) {

        const QJsonArray targets =
        target_value.toArray();

        for (const QJsonValue &value : targets) {

            if (value.isString()) {
                voted_out.append(
                    value.toString()
                );
            }
        }
    }

    const QJsonValue votes_value =
    object.value(mafia_game::KEY_VOTES);

    if (votes_value.isObject()) {

        const QJsonObject votes_object =
        votes_value.toObject();

        for (auto it = votes_object.constBegin();
             it != votes_object.constEnd();
        ++it)
             {
                 if (it.value().isString()) {

                     votes.append(
                         qMakePair(
                             it.key(),
                                   it.value().toString()
                         )
                     );
                 }
             }
    }


    // ============================================================
    // 8. Night information
    // ============================================================

    QMap<QString, QString> night_information;

    const QJsonValue info_value =
    object.value(mafia_game::KEY_INFO);

    if (info_value.isObject()) {

        const QJsonObject info =
        info_value.toObject();

        for (auto it = info.constBegin();
             it != info.constEnd();
        ++it)
             {
                 if (it.value().isString()) {

                     night_information.insert(
                         it.key(),
                                              it.value().toString()
                     );
                 }
             }
    }


    // ============================================================
    // 9. Role assignment
    // ============================================================

    QJsonArray all_roles =
    object.value(
        mafia_game::KEY_ALL_ROLES
    ).toArray();


    // ============================================================
    // 10. Start game podaci
    // ============================================================

    QList<QPair<mafia_game::Role, int>>
    amount_of_roles;

    const QJsonValue amount_value =
    object.value(
        mafia_game::KEY_AMOUNT_OF_ROLES
    );

    if (amount_value.isArray()) {

        const QJsonArray roles_array =
        amount_value.toArray();

        for (const QJsonValue &value : roles_array) {

            if (!value.isObject()) {
                continue;
            }

            const QJsonObject role_object =
            value.toObject();

            const mafia_game::Role parsed_role =
            mafia_game::role_from_string(
                role_object
                .value(mafia_game::KEY_ROLE)
                .toString()
            );

            const int role_count =
            role_object
            .value(mafia_game::KEY_ROLE_COUNT)
            .toInt();

            amount_of_roles.append(
                qMakePair(
                    parsed_role,
                    role_count
                )
            );
        }
    }


    // ============================================================
    // 11. Waiting room podaci
    // ============================================================

    QHash<QString, bool> selection_map;

    const QJsonValue selected_value =
    object.value(mafia_game::KEY_SELECTED);

    if (selected_value.isArray()) {

        const QJsonArray selected =
        selected_value.toArray();

        for (const QJsonValue &value : selected) {

            if (!value.isObject()) {
                continue;
            }

            const QString name =
            value.toObject()
            .value(mafia_game::KEY_USERNAME)
            .toString();

            selection_map.insert(
                name,
                true
            );
        }
    }

    const QJsonValue unselected_value =
    object.value(mafia_game::KEY_UNSELECTED);

    if (unselected_value.isArray()) {

        const QJsonArray unselected =
        unselected_value.toArray();

        for (const QJsonValue &value : unselected) {

            if (!value.isObject()) {
                continue;
            }

            const QString name =
            value.toObject()
            .value(mafia_game::KEY_USERNAME)
            .toString();

            selection_map.insert(
                name,
                false
            );
        }
    }


    // ============================================================
    // 12. Kreiranje protocol poruka
    // ============================================================

    const QJsonObject login_message =
    mafia_game::create_login_message(
        username
    );

    const QJsonObject chat_message =
    mafia_game::create_chat_message(
        sender,
        text,
        alive
    );

    const QJsonObject invalid_state_message =
    mafia_game::create_invalid_state_message(
        sender,
        reason
    );

    const QJsonObject night_action =
    mafia_game::create_night_action(
        role,
        target
    );

    const QJsonObject player_state =
    mafia_game::create_player_state(
        username,
        role,
        alive
    );

    const QJsonObject game_state =
    mafia_game::create_game_state_update(
        phase,
        day_number,
        players
    );

    const QJsonObject night_started =
    mafia_game::create_night_started_message();

    const QJsonObject ability_use =
    mafia_game::create_ability_use(
        username,
        target
    );

    const QJsonObject voting_started =
    mafia_game::create_voting_started_message();

    const QJsonObject voting_ended =
    mafia_game::create_voting_ended_message(
        voted_out,
        votes
    );

    const QJsonObject vote =
    mafia_game::create_vote(
        voter,
        target
    );

    const QJsonObject role_assignment =
    mafia_game::create_role_assignment_message(
        role,
        username,
        all_roles
    );

    const QJsonObject start_game =
    mafia_game::create_start_game(
        amount_of_roles,
        players
    );

    const QJsonObject game_over =
    mafia_game::create_game_over_message(
        winner
    );

    const QJsonObject night_ended =
    mafia_game::create_night_ended_message(
        target,
        night_information
    );

    const QJsonObject day_started =
    mafia_game::create_day_started_message();

    const QJsonObject player_selection =
    mafia_game::create_player_selection(
        username,
        is_selected
    );

    const QJsonObject lists_request =
    mafia_game::create_lists_request();

    const QJsonObject waiting_room_lists =
    mafia_game::create_waiting_room_lists(
        selection_map
    );


    // ============================================================
    // 13. Serijalizacija svih generisanih objekata
    // ============================================================

    QJsonDocument(login_message)
    .toJson(QJsonDocument::Compact);

    QJsonDocument(chat_message)
    .toJson(QJsonDocument::Compact);

    QJsonDocument(invalid_state_message)
    .toJson(QJsonDocument::Compact);

    QJsonDocument(night_action)
    .toJson(QJsonDocument::Compact);

    QJsonDocument(player_state)
    .toJson(QJsonDocument::Compact);

    QJsonDocument(game_state)
    .toJson(QJsonDocument::Compact);

    QJsonDocument(night_started)
    .toJson(QJsonDocument::Compact);

    QJsonDocument(ability_use)
    .toJson(QJsonDocument::Compact);

    QJsonDocument(voting_started)
    .toJson(QJsonDocument::Compact);

    QJsonDocument(voting_ended)
    .toJson(QJsonDocument::Compact);

    QJsonDocument(vote)
    .toJson(QJsonDocument::Compact);

    QJsonDocument(role_assignment)
    .toJson(QJsonDocument::Compact);

    QJsonDocument(start_game)
    .toJson(QJsonDocument::Compact);

    QJsonDocument(game_over)
    .toJson(QJsonDocument::Compact);

    QJsonDocument(night_ended)
    .toJson(QJsonDocument::Compact);

    QJsonDocument(day_started)
    .toJson(QJsonDocument::Compact);

    QJsonDocument(player_selection)
    .toJson(QJsonDocument::Compact);

    QJsonDocument(lists_request)
    .toJson(QJsonDocument::Compact);

    QJsonDocument(waiting_room_lists)
    .toJson(QJsonDocument::Compact);

    return 0;
}
