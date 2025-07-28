# Marketplace MVP Server (C++)

Простой сервер для MVP маркетплейса на C++

## Структура проекта
```
project/
├── CMakeLists.txt - Конфигурация сборки CMake
├── src/ - Исходные файлы сервера
│ ├── main.cpp - Точка входа
│ ├── ...
└── tests/ - Тесты
│ ├── CMakeLists.txt - Конфигурация тестов
│ ├── ...
```

## Требования

- C++20 или новее
- CMake 3.10+
- Boost Asio
- pqxx 7.0 и выше

## Сборка

```bash
mkdir build && cd build
cmake ..
make
```