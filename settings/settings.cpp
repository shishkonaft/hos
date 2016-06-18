#include "settings.h"

using namespace std;

struct conf_str {
	string			path_to_configuration_file; // Путь к конфигурационному файлу
	vector <string>	point_to_vector; // Указатель на вектор с конфигурационным файлом
};

struct items_list_str {
	string			name_item,
					comment_item,
					type_item,
					value_item;
};

bool search_in_struct(string path, vector <conf_str> conf_vec, conf_str &out_struct) { // Поиск в векторе нужного конфигурационного файла
	unsigned int	v;
	conf_str		s_temp;

	for (v	= 0; v < conf_vec.size(); v++) {
		s_temp	= conf_vec[v];

		if (path == s_temp.path_to_configuration_file) {
			out_struct	= s_temp;
			return true;
		}
	}
	return false;
}

bool load_item(vector <string> setfile_vec, unsigned int num, string &name_item, string &comment_item, string &type_item, string &parametr_config_item, string &path_to_conf_item) {
	string temp;
	attron(COLOR_PAIR(TEXT_BLACK_RED) | A_BOLD);

	if ((temp = conf(str(num) + "_item", setfile_vec)) == "0x1") {
		mvprintw(0, 0, "Setfile is incorrect! Not found item parametr!");
		getch();
		return false;
	} else {
		name_item	= temp;
	}

	if ((temp = conf(str(num) + "_item_comment", setfile_vec)) == "0x1") {
		comment_item.clear();
	} else {
		comment_item	= temp;
	}

	if ((temp = conf(str(num) + "_type_input", setfile_vec)) == "0x1") {
		mvprintw(0, 0, "Setfile is incorrect! Not found type_input parametr!");
		getch();
		return false;
	} else {
		type_item	= temp;
	}

	if ((temp = conf(str(num) + "_parametr_config", setfile_vec)) == "0x1") {
		mvprintw(0, 0, "Setfile is incorrect! Not found parametr_config parametr!");
		getch();
		return false;
	} else {
		parametr_config_item	= temp;
	}

	if ((temp = conf(str(num) + "_conf_file", setfile_vec)) == "0x1") {
		mvprintw(0, 0, "Setfile is incorrect! Not found conf_file parametr!");
		getch();
		return false;
	} else {
		path_to_conf_item	= temp;
	}

	attroff(COLOR_PAIR(TEXT_RED_BLACK) | A_BOLD);

	return true;
}

int settings(string	path_to_settings_file) {
	if (!FileExists(path_to_settings_file)) {
		printw("Not found!");
		getch();
		return -1;
	}
	erase();

	vector <string>			setfile_vec, // Вектор для основного файла настроек
							temp_vec;

	vector <conf_str>		confs_vec; // Вектор который содержит указатели на загруженные конфигурационные файлы

	vector <items_list_str>	items_list;

	load_to_vector(path_to_settings_file, setfile_vec);
	items_list.clear();
	confs_vec.clear();

	unsigned int	maxX,
					maxY,
					selected, // Номер выделенного пункта
					all_items, //Сколько всего будет пунктов
					i,
					j,
					first_write, // С кокого элемента будет выводиться
					last_write, // До какого, включительно будет выводиться
					position_write, // С какой строки начинать писать
					cut_selected_name, // Обрезка выделенного пункта
					cut_selected_comment, // -//-
					wait_selected_name, // Задержка при достижении начала или конца строки
					wait_selected_comment, // -//-
					right_border; // С какой позиции писать значение

	bool			cycle,
					want_to_moving_selected_name, // Надо ли вообще проматывать выбранный пункт
					want_to_moving_selected_comment; // Надо ли вообще проматывать выбранный пункт

	string			temp,
					name_item, // Имя пункта
					comment_item, // Комментарий пункта
					type_item, // Тип пункта
					parametr_config_item, // Параметр для изменения
					path_to_conf_item, // Путь к конфигурационному файлу для этого пункта
					selected_name,	// Имя выделенного пункта
					selected_comment, // Комментарий выделенного пункта
					selected_type,	// Тип выбранного пункта
					selected_value;	// Значение выбранного пункта

	conf_str		temp_conf_str;

	items_list_str	item_temp;

	int				key_pressed;

	/*LANG ZONE START*/
	string	settings_lng	= "Settings",
			set_name		= conf("name_app", setfile_vec);
	/*LANG ZONE END*/

	getmaxyx(stdscr, maxY, maxX);

	timeout(-1);

	if ((temp = conf("all_items", setfile_vec)) == "0x1") {
		printw("Setfile is incorrect! Not found all_items parametr!\n");
		getch();
		return -1;
	} else {
		all_items	= atoi(temp.c_str());
	}

	cycle		= true;

	for (i	= 0; i < maxX; i++, mvprintw(0, i, "-"), mvprintw(maxY - 1, i, "-")); // Рисование верхней и нижней рамок

	for (i	= 0; i < maxY; i++, mvprintw(i - 1, 0, "|"), mvprintw(i - 1, maxX - 1, "|")); // Рисование левой и правой рамок

	if (llength(settings_lng) + llength(set_name) > maxX - 3) {
		set_name.erase(maxX - 6 - llength(settings_lng), llength(set_name));
		set_name	+= "...";
	}

	attron(COLOR_PAIR(TEXT_BLACK_WHITE) | A_BOLD);
	mvprintw (0, 1, "%s %s", settings_lng.c_str(), set_name.c_str());
	attroff(COLOR_PAIR(TEXT_BLACK_WHITE) | A_BOLD);

	for (i	= 1; i <= all_items; i++) { // Загрузка всех конфигурационных файлов
		path_to_conf_item	= conf(str(i) + "_conf_file", setfile_vec); // Путь к файлу конфигураций

		if ((confs_vec.size() == 0) || (!search_in_struct(path_to_conf_item, confs_vec, temp_conf_str))) { // Проверка есть ли уже такой загруженный файл конфигураций
			load_to_vector(path_to_conf_item, temp_vec); // Загрузка файла конфигураций
			temp_conf_str.path_to_configuration_file	= path_to_conf_item;
			temp_conf_str.point_to_vector				= temp_vec;
			confs_vec.insert(confs_vec.end(), temp_conf_str);
		}

		if (!load_item(setfile_vec, i, name_item, comment_item, type_item, parametr_config_item, path_to_conf_item)) { // Получение значений пункта
			return -1;
		}

		item_temp.name_item		= name_item;
		item_temp.comment_item	= comment_item;
		item_temp.type_item		= type_item;
		search_in_struct(path_to_conf_item, confs_vec, temp_conf_str);
		temp_vec				= temp_conf_str.point_to_vector;
		item_temp.value_item	= conf(parametr_config_item, temp_vec);

		if (item_temp.value_item == "0x1") {
			erase();
			printw("Error read value %s in %s", parametr_config_item.c_str(), path_to_conf_item.c_str());
			getch();
			return -1;
		}

		items_list.insert(items_list.end(), item_temp); // Заполнение списка-вектора для вывода на экран
	}

	first_write				= 0;
	last_write				= all_items;
	position_write			= 1;
	selected				= 1;
	key_pressed				= KEY_UP;
	right_border			= (maxX - 2) - maxX / 4;
	cut_selected_name		= 0;
	cut_selected_comment	= 0;
	wait_selected_name		= 0;
	wait_selected_comment	= 0;

want_to_moving_selected_name = true;
want_to_moving_selected_comment = true;

	while (cycle) {
		timeout(200);
		position_write	= 2;
		if ((key_pressed == KEY_UP) || (key_pressed == KEY_DOWN)) { // Вывод невыделенных пунктов
			for (i	= first_write; i < last_write; i++) {
				item_temp = items_list[i];

				if (selected == i) { // Пропуск выделенного пункта
					selected_name			= item_temp.name_item;
					selected_comment		= item_temp.comment_item;
					selected_type			= item_temp.type_item;
					selected_value			= item_temp.value_item;
					cut_selected_name		= 0;
					wait_selected_name		= 0;
					cut_selected_comment	= 0;
					wait_selected_comment	= 0;
					position_write			+= 3;
					continue;
				}

				for (j	= 0; j < maxX - 2; j++, mvprintw(position_write, j, " "), mvprintw(position_write + 1, j, " ")); // Зачистка поля

				if (llength(item_temp.name_item) + 3 > right_border) {
					item_temp.name_item.erase(right_border - 6, llength(item_temp.name_item));
					item_temp.name_item	+= "...";
				}

				mvprintw(position_write, 2, "%s", item_temp.name_item.c_str());

				if (!item_temp.comment_item.empty()) {
					if (llength(item_temp.comment_item) + 4 > right_border) {
						item_temp.comment_item.erase(right_border - 7, llength(item_temp.comment_item));
						item_temp.comment_item	+= "...";
					}
					mvprintw(position_write + 1, 3, "%s", item_temp.comment_item.c_str());
				}

				if (item_temp.type_item == "bool") {
					if (item_temp.value_item == "1") {
						attron(COLOR_PAIR(TEXT_GREEN_BLACK) | A_BOLD);
						mvprintw(position_write, maxX - ((maxX - right_border) / 2), "ON");
						attroff(COLOR_PAIR(TEXT_GREEN_BLACK) | A_BOLD);
					} else {
						attron(COLOR_PAIR(TEXT_RED_BLACK) | A_BOLD);
						mvprintw(position_write, maxX - ((maxX - right_border) / 2), "OFF");
						attroff(COLOR_PAIR(TEXT_RED_BLACK) | A_BOLD);
					}
				}

				position_write	+= 3;
			}
		}

		/*DRAW SELECTED ELEMENT START*/
		if ((key_pressed == KEY_UP) || (key_pressed == KEY_DOWN)) { // Вывод невыделенных пунктов
			attron(COLOR_PAIR(TEXT_BLACK_WHITE) | A_BOLD);
			for (j	= 0; j < maxX - 2; j++, mvprintw(2 + selected * 3, j, " "), mvprintw(3 + selected * 3, j, " ")); // Заполнение цветом выделения
			attroff(COLOR_PAIR(TEXT_BLACK_WHITE) | A_BOLD);
		
			if (selected_type == "bool") {
				if (selected_value == "1") {
					attron(COLOR_PAIR(TEXT_GREEN_WHITE) | A_BOLD);
					mvprintw(2 + selected * 3, maxX - ((maxX - right_border) / 2), "ON");
					attroff(COLOR_PAIR(TEXT_GREEN_WHITE) | A_BOLD);
				} else {
					attron(COLOR_PAIR(TEXT_RED_WHITE) | A_BOLD);
					mvprintw(2 + selected * 3, maxX - ((maxX - right_border) / 2), "OFF");
					attroff(COLOR_PAIR(TEXT_RED_WHITE) | A_BOLD);
				}
			}
		}

		attron(COLOR_PAIR(TEXT_BLACK_WHITE) | A_BOLD);

		if (want_to_moving_selected_name) {
			if (wait_selected_name == 0) { // Двигатель имени пункта
				temp	= selected_name;
				temp.erase(0, cut_selected_name); // Обрезка начала

				if (cut_selected_name == 0) // Если начато всё сначала остановиться и подождать
					wait_selected_name	= WAIT_START;

				cut_selected_name++; // Сколько символов нужно обрезать спереди

				if (llength(temp) + 3 > right_border) { // Обрезка конца
					temp.erase(right_border - 3, llength(temp));
				} else {
					wait_selected_name	= WAIT_END; // Если достигло конца, то подождать
					cut_selected_name	= 0;
				}

				mvprintw(2 + selected * 3, 2, "%s", temp.c_str()); // Вывести на экран
			} else {
				wait_selected_name--; // Счётчик ожидания
			}

		} else {
			mvprintw(2 + selected * 3, 2, "%s", selected_name.c_str());
		}

		if (want_to_moving_selected_comment) {
			if (wait_selected_comment == 0) { // Двигатель имени пункта
				temp	= selected_comment;
				temp.erase(0, cut_selected_comment); // Обрезка начала

				if (cut_selected_comment == 0) // Если начато всё сначала остановиться и подождать
					wait_selected_comment	= WAIT_START;

				cut_selected_comment++; // Сколько символов нужно обрезать спереди

				if (llength(temp) + 4 > right_border) { // Обрезка конца
					temp.erase(right_border - 4, llength(temp));
				} else {
					wait_selected_comment	= WAIT_END; // Если достигло конца, то подождать
					cut_selected_comment	= 0;
				}

				mvprintw(3 + selected * 3, 3, "%s", temp.c_str()); // Вывести на экран
			} else {
				wait_selected_comment--; // Счётчик ожидания
			}

		} else {
			mvprintw(3 + selected * 3, 3, "%s", selected_comment.c_str());
		}

		attroff(COLOR_PAIR(TEXT_BLACK_WHITE) | A_BOLD);
		/*DRAW SELECTED ELEMENT END*/

		key_pressed	= getch();

		switch (key_pressed) {
			case 27:	cycle = false;
						break;
		}
	}

	return 0;
}