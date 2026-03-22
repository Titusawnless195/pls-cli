#include <cstddef>
#include <string>
#include <curl/curl.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <cstdlib>


// Callback: вызывается cURL-ом, когда приходит порция данных от сервера
size_t write_callback(void* contents, std::size_t size, std::size_t nmemb, std::string* userp)
{
    // Дописываем полученные байты в нашу строку-буфер
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Функция отправляет текст в буфер обмена системы
void copy_to_clipboard(const std::string& text)
{
#ifdef __APPLE__ 
    FILE* pipe = popen("pbcopy", "w"); 
    if (pipe) {
        fwrite(text.c_str(), 1, text.length(), pipe);
        pclose(pipe);
    }
#else
    // Попробуем wl-copy (Wayland)
    if (FILE* pipe = popen("wl-copy", "w")) {
        fwrite(text.c_str(), 1, text.length(), pipe);
        if (pclose(pipe) == 0) return; // Успех
    }
    // Если не вышло, пробуем xclip (X11)
    if (FILE* pipe = popen("xclip -selection clipboard", "w")) {
        fwrite(text.c_str(), 1, text.length(), pipe);
        pclose(pipe);
    }
#endif
}

std::vector<std::string> get_models() {
    std::vector<std::string> models;
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:11434/api/tags");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        if (curl_easy_perform(curl) == CURLE_OK) {
            size_t pos = 0;
            while ((pos = response.find("\"name\":\"", pos)) != std::string::npos) {
                pos += 8;
                size_t end = response.find("\"", pos);
                if (end != std::string::npos) {
                    models.push_back(response.substr(pos, end - pos));
                    pos = end;
                }
            }
        }
        curl_easy_cleanup(curl);
    }
    return models;
}

std::string get_saved_model()
{
    const char* home = getenv("HOME");
    if (!home) return ""; // Или путь по умолчанию
    std::filesystem::create_directories(std::string(home) + "/.config");
    std::string path = std::string(home) + "/.config/pls_model.txt";

    std::ifstream file(path);
    std::string model;
    if (file >> model) return model;
    return "";
}

void save_model_preference(const std::string& model_name) {
    // Получаем путь к домашней директории пользователя
    const char* home = getenv("HOME");
    if (!home) return; // Или путь по умолчанию
    std::filesystem::create_directories(std::string(home) + "/.config");
    std::string path = std::string(home) + "/.config/pls_model.txt";

    std::ofstream configFile(path);
    if (configFile.is_open()) {
        configFile << model_name;
        configFile.close();
    } else {
        std::cerr << "Failed to save settings.\n";
    }
}

std::string get_distro_name() {
    std::ifstream file("/etc/os-release");
    std::string line;

    if (!file.is_open()) return "";

    while (std::getline(file, line)) {
        // Ищем строку, начинающуюся с PRETTY_NAME
        if (line.starts_with("PRETTY_NAME=")) {
            size_t start = line.find('"');
            size_t end = line.rfind('"');

            if (start != std::string::npos && end != std::string::npos && start != end) {
                return line.substr(start + 1, end - start - 1);
            }
        }
    }
    return "";
}

std::string scan_dir() {

    std::string context = "";

    // получаем путь, из которого запущена программа
    std::filesystem::path current_dir = std::filesystem::current_path();

    // сканируем папку
    try {
        int count = 0;
        for (const auto& entry : std::filesystem::directory_iterator(current_dir)) {
            // 3. Проверяем, является ли объект директорией

            // МАГИЯ ДЛЯ НЕЙРОСЕТИ: Явно помечаем, где директория, а где файл
            if (std::filesystem::is_directory(entry)) {
                context += "[DIR] " + entry.path().filename().string() + ", ";
            } else {
                context += "[FILE] " + entry.path().filename().string() + ", ";
            }
            count++;
        }

        if (count == 0) return "Directory is empty.";
        return "Directory contents: " + context;

    } catch (const std::filesystem::filesystem_error& e) {
        return "Cannot read directory contents (permission denied).";
    }
}

// Читаем данные из конейера (pipe), если они есть
std::string get_piped_input() {
    // Если ввод идет с терминала (человек), а не из пайпа — выходим
    if (isatty(STDIN_FILENO)) {
        return "";
    }

    std::string piped_data = "";
    std::string line;
    size_t max_length = 4000; // Лимит контекста (~1000 токенов), чтобы ИИ не сошел с ума

    while (std::getline(std::cin, line)) {
        piped_data += line + "\n";
        if (piped_data.length() > max_length) {
            piped_data = piped_data.substr(0, max_length) + "\n...[LOG TRUNCATED FOR MEMORY SAFETY]...";
            break;
        }
    }
    return piped_data;
}

int main(int argc, char* argv[])
{
    // Сначала обрабатываем спец-флаги, которым не нужен основной цикл
    if (argc >= 2) {
        std::string flag = argv[1];

        if (flag == "--list" || flag == "-l") {
            std::vector<std::string> models = get_models();
            for (const auto& m : models) std::cout << m << std::endl;
            return 0;
        }

        if (flag == "--set" || flag == "-s") {
            if (argc < 3) {
                std::cerr << "Error: Please enter a model name.\n";
                return 1;
            }
            std::string model_pick = argv[2]; // Берем название модели
            // Чистим пробелы
            model_pick.erase(std::remove(model_pick.begin(), model_pick.end(), ' '), model_pick.end());

            std::vector<std::string> models = get_models();
            if (std::ranges::contains(models, model_pick)) {
                save_model_preference(model_pick);
            } else {
                std::cerr << "Model '" << model_pick << "' is not found in Ollama.\n";
            }
            return 0;
        }
        if (flag == "--now" || flag == "-n") {
            std::string current = get_saved_model();
            if (current.empty()) {
                std::cout << "The current model is not set. Run: pls --set <name>\n";
            } else {
                std::cout << "\033[1;36m" << current << "\033[0m\n";
            }
            return 0; // Обязательно выходим, чтобы не пойти в основной цикл!
        }

        if (flag == "--help" || flag == "-h") {
            std::cout << R"(pls - AI Command Line Assistant (Powered by Ollama)
                DESCRIPTION:
                Translates natural language requests into executable bash/zsh commands.
                Fully context-aware (OS, Package Manager, Current Directory, Pipes).

                USAGE:
                pls [FLAGS] <your request>

                FLAGS:
                -c            Copy the generated command to clipboard (Wayland).
                -e            Prompt to execute the command immediately.

                MODEL MANAGEMENT:
                -l, --list        List all installed Ollama models.
                -s, --set <name>  Set the default model (e.g., pls -s qwen2.5:1.5b).
                -n, --now         Show the currently active model.
                -h, --help        Show this help message.

                EXAMPLES:
                pls update my system
                pls -c find all log files larger than 1GB
                cat error.log | pls -e fix this error
                )" << '\n';
            return 0;
        }
    }

    // основной режим
    if (argc < 2) {
        std::cerr << "Usage: pls <request> | pls --set <model> | pls --list\n";
        return 1;
    }

    // 2. Обработка флагов и сбор запроса (ЕДИНЫЙ ЦИКЛ)
    bool execute_flag = false;
    bool copy_flag = false;
    std::string user_request = "";

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "-c") {
            copy_flag = true;
        } else if (arg == "-e") {
            execute_flag = true;
        } else {
            if (!user_request.empty()) user_request += " ";
            user_request += arg;
        }
    }

    // Проверка: если запрос пустой (например, ввели только флаги)
    if (user_request.empty() || user_request.find_first_not_of(' ') == std::string::npos) {
        std::cerr << "Usage: pls [-c] [-e] <request>\n";
        return 1;
    }

    std::string model = getSavedModel();
    if (model.empty()) {
        std::cerr << "The current model is not set. Run: pls --set <name>\n";
        return 1; // Обязательно выходим, иначе дальше будет ошибка
    }

    // получаем дистрибутив
    std::string distro = get_distro_name();
    if (distro.empty()) distro = "Linux";

    // Определяем пакетный менеджер на основе дистрибутива для более точного промпта
    std::string pkg_manager = "your system package manager";
    if (distro.find("SUSE") != std::string::npos) pkg_manager = "zypper";
    else if (distro.find("Arch") != std::string::npos || distro.find("Manjaro") != std::string::npos) pkg_manager = "pacman";
    else if (distro.find("Ubuntu") != std::string::npos || distro.find("Debian") != std::string::npos || distro.find("Mint") != std::string::npos) pkg_manager = "apt";
    else if (distro.find("Fedora") != std::string::npos || distro.find("Red Hat") != std::string::npos || distro.find("CentOS") != std::string::npos) pkg_manager = "dnf";
    else if (distro.find("Alpine") != std::string::npos) pkg_manager = "apk";

    // Сканируем текущую директорию
    std::string dir_context = scan_dir();

    // Читаем пайп
    std::string piped_data = get_piped_input();

    // Формируем системную инструкцию
    std::string prompt =
        "You are a strict, headless command-line translator for " + distro + ". "
        "Task: translate the following user request into a single valid bash command. "
        "CRITICAL RULES: "
        "1. Output ONLY the raw, executable command. "
        "2. NO markdown formatting, NO backticks, NO explanations. "
        "3. DO NOT prepend the command with '$' or '#'. "
        "4. Use '" + pkg_manager + "' for any package management tasks. "
        "CONTEXT (" + dir_context + "). ";

    // ЕСЛИ В НАС ЧТО-ТО ВЛИЛИ ЧЕРЕЗ ПАЙП — СКАРМЛИВАЕМ ЭТО НЕЙРОСЕТИ
    if (!piped_data.empty()) {
        prompt += "PIPED STDIN DATA (Log/Output to analyze): \n" + piped_data + "\n";
    }

    prompt += "User request: '" + user_request + "'";

    // Бронебойное экранирование JSON (спасает от логов с пайпов и спецсимволов)
    std::string safe_prompt = "";
    for (char c : prompt) {
        if (c == '"') safe_prompt += "\\\"";
        else if (c == '\\') safe_prompt += "\\\\";
        else if (c == '\n') safe_prompt += "\\n";
        else if (c == '\r') safe_prompt += "\\r";
        else if (c == '\t') safe_prompt += "\\t";
        else safe_prompt += c;
    }

    // Подготовка JSON-тела запроса
    std::string json_data = "{\"model\": \"" + model + "\", \"prompt\": \"" + safe_prompt + "\", \"stream\": false, \"options\": {\"temperature\": 0.0}}";

    // Инициализируем cURL
    CURL* curl = curl_easy_init();
    std::string response_string; // Сюда запишется ответ от Ollama

    if (curl) {
        struct curl_slist* headers = NULL;
        // Указываем, что отправляем именно JSON
        headers = curl_slist_append(headers, "Content-Type: application/json");

        // Настройка cURL: адрес локальной Ollama, заголовки, данные и callback
        curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:11434/api/generate");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

        // Скрываем индикатор прогресса загрузки
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);

        // Выполняем запрос
        CURLcode res = curl_easy_perform(curl);

        // Очистка ресурсов cURL
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

        if (res != CURLE_OK) {
            std::cerr << "\nError connecting to Ollama: " << curl_easy_strerror(res) << "\n";
            return 1;
        }
    }
    // Парсим ответ вручную (ищем значение ключа "response" в JSON)
    std::string cmd = "";
    std::string_view resp_view(response_string);

    // 1. Ищем начало значения в JSON ключе "response"
    size_t val_start = resp_view.find("\"response\":\"");
    if (val_start != std::string_view::npos) {
        val_start += 12; // Длина "\"response\":\""

        // 2. Ищем конец JSON-строки, пропуская экранированные кавычки (\")
        size_t val_end = val_start;
        while (val_end < resp_view.size()) {
            val_end = resp_view.find('"', val_end);
            if (val_end == std::string_view::npos) break;

            // Если перед кавычкой стоит \, значит это не конец строки
            if (resp_view[val_end - 1] != '\\') break;
            val_end++;
        }

        if (val_end != std::string_view::npos) {
            // Вырезаем содержимое
            std::string raw_content = std::string(resp_view.substr(val_start, val_end - val_start));

            // 3. Декодируем базовые JSON-сущности экранированные кавычки и переносы
            size_t p = 0;
            while ((p = raw_content.find("\\\"", p)) != std::string::npos) raw_content.replace(p, 2, "\"");
            p = 0;
            while ((p = raw_content.find("\\n", p)) != std::string::npos) raw_content.replace(p, 2, "\n");

            // 4. Извлекаем команду из Markdown блоков, если они есть
            // Ищем тройные кавычки ```bash или ```
            size_t code_start = raw_content.find("```");
            if (code_start != std::string::npos) {
                code_start = raw_content.find('\n', code_start) + 1; // Пропускаем "```bash"
                size_t code_end = raw_content.find("```", code_start);
                if (code_end != std::string::npos) {
                    cmd = raw_content.substr(code_start, code_end - code_start);
                }
            } else {
                // Если блоков нет, чистим одиночные кавычки `...`
                cmd = raw_content;
                cmd.erase(std::remove(cmd.begin(), cmd.end(), '`'), cmd.end());
            }
        }
    }

    // 5. Финальная чистка
    // Убираем лишние пробелы и переносы в начале и конце (C++20 starts_with/ends_with)
    while (!cmd.empty() && (cmd.front() == ' ' || cmd.front() == '\n' || cmd.front() == '\r')) cmd.erase(0, 1);
    while (!cmd.empty() && (cmd.back() == ' ' || cmd.back() == '\n' || cmd.back() == '\r')) cmd.pop_back();

    // Убиваем двойные или одинарные кавычки, если ИИ обернул в них команду
    if (!cmd.empty() && ((cmd.front() == '"' && cmd.back() == '"') || (cmd.front() == '\'' && cmd.back() == '\''))) {
        cmd = cmd.substr(1, cmd.length() - 2);
    }

    // Декодируем ебучий амперсанд (\u0026 -> &) прямо в готовой команде
    size_t fix_pos = 0;
    while ((fix_pos = cmd.find("\\u0026", fix_pos)) != std::string::npos) {
        cmd.replace(fix_pos, 6, "&");
    }

    // Если ИИ добавил "bash " в начало строки вне блока кода
    if (cmd.starts_with("bash ")) cmd.erase(0, 5);

    std::cout << "\r\033[1;32m" << cmd << "\033[0m\n";

    if (copy_flag) {
        // Копируем результат в буфер Wayland
        copy_to_clipboard(cmd);
    }

    if (execute_flag && !cmd.empty()) {
        std::cout << "\nexecute? [y/N]: " << std::flush;
        char confirm = 'N';

        if (!isatty(STDIN_FILENO)) {
            // Магия UNIX: Если stdin забит пайпом, читаем напрямую с клавиатуры терминала!
            FILE* tty = fopen("/dev/tty", "r");
            if (tty) {
                confirm = fgetc(tty);
                fclose(tty);
            }
        } else {
            // Обычный режим
            std::cin >> confirm;
        }

        if (confirm == 'y' || confirm == 'Y') {
            std::cout << "--- Execution ---\n";
            std::system(cmd.c_str());
            std::cout << "------------------\n";
        }
    }

    return 0;
}
