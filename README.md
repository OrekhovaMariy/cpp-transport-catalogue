# TransportCatalogue
Транспортный справочник, маршрутизация городского транспорта.

На запросы через JSON программа создает базу данных или выдает необходимую информацию по оптимальным маршрутам, по определенным автобусам или остановкам. Формат вывода: .json и .svg

Написан справочник с использованием стандарта ISO C++17 (/std:c++17).

Применена идиома RAII, использованы хеш-функции, алгоритмы стандартной библиотеки, программа написана с использованием CMake 3.10 и protobuf
