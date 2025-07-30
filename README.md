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
- libbcrypt
- nlohmann/json

## Сборка

```bash
mkdir build && cd build
cmake ..
make
```

## База данных

Проект использует postgres базу данных sBeHappy(таблицу users для пользователей). Тутор для Linux:
```bash
psql --dbname=postgres -c"CREATE DATABASE sBeHappy" # создание базы данных
psql --dbname=sBeHappy -c"CREATE TABLE users (id SERIAL PRIMARY KEY, created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP, username VARCHAR(100) UNIQUE NOT NULL, email VARCHAR(100) UNIQUE NOT NULL, password_hash INT NOT NULL);" # создание таблицы для пользователей
```