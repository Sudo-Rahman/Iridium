//
// Created by Rahman on 11/02/2023.
//

#ifndef IRIDIUM_ITEMMENU_HPP
#define IRIDIUM_ITEMMENU_HPP

#include <QMenu>
#include "TreeFileItem.hpp"

// Forward declaration of Action type
struct Action;

template<class T>
struct is_action : std::is_same<Action, std::decay_t<T>> {};

template<class T>
struct is_bool : std::is_same<bool, std::decay_t<T>> {};

template<class T1, class T2>
struct is_action_bool_pair : std::conjunction<is_action<T1>, is_bool<T2>> {};

template<class ...Args>
struct are_valid_pairs;

template<>
struct are_valid_pairs<> : std::true_type {};

template<class T1, class T2, class ...Rest>
struct are_valid_pairs<T1, T2, Rest...> : std::conjunction<is_action_bool_pair<T1, T2>, are_valid_pairs<Rest...>> {};

template<class ...Args>
static constexpr bool are_valid_pairs_v = are_valid_pairs<Args...>::value;

class ItemMenu : public QMenu
{
	Q_OBJECT

	QAction *_info{}, *_copy{}, *_paste{}, *_delete{},
	*_new_folder{}, *_tree{}, *_sync{}, *_preview{}, *_clean_trash{};

public:
	enum Action
	{
		None,
		Copy,
		Paste,
		Info,
		Delete,
		NewFolder,
		Tree,
		Sync,
		CleanTrash,
	};

	[[nodiscard]] Action action() const { return _action; }

	explicit ItemMenu(const QList<TreeFileItem *> &files, QWidget *parent = nullptr);

	void setActionEnabled(const Action &action, bool enabled);

	// override exec and return action clicked
	Action exec(const QPoint &pos)
	{
		QMenu::exec(pos);
		return _action;
	}

private:
	Action _action{None};

signals:
	void copied();

	void pasted();

	void info();

	void deleted();

	void newFolder();

	void tree();

	void sync();

	void trashCleaned();
};

#endif //IRIDIUM_ITEMMENU_HPP
