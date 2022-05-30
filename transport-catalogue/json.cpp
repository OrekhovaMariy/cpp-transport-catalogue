#include "json.h"

#include <cassert>

using namespace std;
namespace json {

    namespace {

        Node LoadNode(istream& input);

        Node LoadArray(istream& input) {
            Array result;
            for (char c; input >> c && c != ']';) {
                if (c != ',') {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }
            if (!input) {
                throw ParsingError("Unexpected end"s);
            }
            return Node(move(result));

        }

        Node LoadString(istream& input) {
            using namespace std::literals;
            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true) {
                if (it == end) {
                    // Поток закончился до того, как встретили закрывающую кавычку?
                    throw ParsingError("String parsing error");
                }
                const char ch = *it;
                if (ch == '"') {
                    // Встретили закрывающую кавычку
                    ++it;
                    break;
                }
                else if (ch == '\\') {
                    // Встретили начало escape-последовательности
                    ++it;
                    if (it == end) {
                        // Поток завершился сразу после символа обратной косой черты
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
                    switch (escaped_char) {
                    case 'n':
                        s.push_back('\n');
                        break;
                    case 't':
                        s.push_back('\t');
                        break;
                    case 'r':
                        s.push_back('\r');
                        break;
                    case '"':
                        s.push_back('"');
                        break;
                    case '\\':
                        s.push_back('\\');
                        break;
                    default:
                        // Встретили неизвестную escape-последовательность
                        throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                }
                else if (ch == '\n' || ch == '\r') {
                    // Строковый литерал внутри- JSON не может прерываться символами \r или \n
                    throw ParsingError("Unexpected end of line"s);
                }
                else {
                    // Просто считываем очередной символ и помещаем его в результирующую строку
                    s.push_back(ch);
                }
                ++it;
            }

            return Node(s);
        }

        Node LoadDict(istream& input) {
            Dict result;
            for (char c; input >> c && c != '}';) {
                if (c == ',') {
                    input >> c;
                }
                string key = LoadString(input).AsString();
                input >> c;
                result.insert({ move(key), LoadNode(input) });
            }
            if (!input) {
                throw ParsingError("Unexpected end"s);
            }
            return Node(move(result));
        }

        Node LoadNumber(istream& input) {
            using namespace std::literals;

            std::string parsed_num;

            // Считывает в parsed_num очередной символ из input
            auto read_char = [&parsed_num, &input] {
                parsed_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read number from stream"s);
                }
            };

            // Считывает одну или более цифр в parsed_num из input
            auto read_digits = [&input, read_char] {
                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (std::isdigit(input.peek())) {
                    read_char();
                }
            };

            if (input.peek() == '-') {
                read_char();
            }
            // Парсим целую часть числа
            if (input.peek() == '0') {
                read_char();
                // После 0 в JSON не могут идти другие цифры
            }
            else {
                read_digits();
            }

            bool is_int = true;
            // Парсим дробную часть числа
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            // Парсим экспоненциальную часть числа
            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }
                read_digits();
                is_int = false;
            }
            try {
                if (is_int) {
                    // Сначала пробуем преобразовать строку в int
                    try {
                        return Node(std::stoi(parsed_num));
                    }
                    catch (...) {
                        // В случае неудачи, например, при переполнении,
                        // код ниже попробует преобразовать строку в double
                    }
                }
                return Node(std::stod(parsed_num));
            }
            catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }
        std::string LoadLiteral(std::istream& input) {
            std::string s;
            while (std::isalpha(input.peek())) {
                s.push_back(static_cast<char>(input.get()));
            }
            return s;
        }
        Node LoadBool(std::istream& input) {
            const auto s = LoadLiteral(input);
            if (s == "true"sv) {
                return Node{ true };
            }
            else if (s == "false"sv) {
                return Node{ false };
            }
            else {
                throw ParsingError("Parsing error: "s + s);
            }
        }

        Node LoadNull(std::istream& input) {
            if (auto literal = LoadLiteral(input); literal == "null"sv) {
                return Node{ nullptr };
            }
            else {
                throw ParsingError("Parsing error: "s + literal);
            }
        }

        Node LoadNode(istream& input) {
            char c;
            input >> c;

            if (c == '[') {
                return LoadArray(input);
            }
            else if (c == '{') {
                return LoadDict(input);
            }
            else if (c == '"') {
                return LoadString(input);
            }
            else if (c == 'n') {
                input.putback(c);
                return LoadNull(input);
            }
            else if (c == 't' || c == 'f') {
                input.putback(c);
                return LoadBool(input);
            }
            else {
                input.putback(c);
                return LoadNumber(input);
            }
        }

    }  // namespace

    int Node::AsInt() const {
        if (IsInt())   return std::get<int>(value_);
        else {
            throw logic_error("Failed int");
        }
    }

    double Node::AsDouble() const {
        if (IsPureDouble()) return (std::get<double>(value_));
        else if (IsInt()) return static_cast<double>(std::get<int>(value_));
        else throw logic_error("Failed double");
    }

    const string& Node::AsString() const {
        if (IsString()) return std::get<std::string>(value_);
        else throw logic_error("Failed string");
    }

    const Array& Node::AsArray() const {
        if (IsArray())return std::get<Array>(value_);
        else  throw logic_error("Failed Array");
    }


    bool Node::AsBool() const {
        if (IsBool())return std::get<bool>(value_);
        else throw logic_error("Failed bool");
    }

    const Dict& Node::AsMap() const {
        if (IsMap())return std::get<Dict>(value_);
        else throw logic_error("Failed Map");
    }

    bool Node::IsInt() const { return std::holds_alternative<int>(value_); }

    bool Node::IsDouble()const { return (std::holds_alternative<double>(value_) || IsInt()); }

    bool Node::IsNull() const { return std::holds_alternative<std::nullptr_t>(value_); }

    bool Node::IsPureDouble() const { return std::holds_alternative<double>(value_); }

    bool Node::IsString() const { return std::holds_alternative<std::string>(value_); }

    bool Node::IsBool() const { return std::holds_alternative<bool>(value_); }

    bool Node::IsArray() const { return std::holds_alternative<Array>(value_); }

    bool Node::IsMap() const { return std::holds_alternative<Dict>(value_); }

    void Node::PrintNode(std::ostream& output) const { std::visit(json::PrintStruct{ output }, value_); }

    void PrintStruct::operator()(std::string s, std::ostream& out) const {
        out << "\"";
        for (auto ch : s) {
            switch (ch)
            {
            case '\"':out << "\\\""; break;
            case '\\':out << "\\\\"; break;
            case '\n':out << "\\n"; break;
            case '\r':out << "\\r"; break;
            case '\t':out << "\t"; break;
            default:out << ch;
            }
        }
        out << "\"";
    }
    void PrintStruct::operator()(std::nullptr_t) const {
        out << "null";
    }
    void PrintStruct::operator()(double d) const { out << d; }
    void PrintStruct::operator()(int i) const { out << i; }
    void PrintStruct::operator()(std::string s) const {
        PrintStruct::operator()(s, out);
    }
    void PrintStruct::operator()(bool b) const { out << std::boolalpha << b; }
    void PrintStruct::operator()(Array arr) const {
        bool f = false;
        out << "[";
        for (auto a : arr) {
            if (!f) f = true;
            else  out << ", ";
            a.PrintNode(out);
        }
        out << "]";

    }
    void PrintStruct::operator()(Dict dic) const {
        bool f = false;
        out << "{ ";
        for (auto [key, value] : dic) {
            if (!f) f = true;
            else  out << ", ";
            PrintStruct::operator()(key, out);
            out << ": ";
            value.PrintNode(out);
        }
        out << "}";
    }

    Document::Document(Node root)
        : root_(move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    Document Load(istream& input) {
        return Document{ LoadNode(input) };
    }

    void Print(const Document& doc, std::ostream& output) {
        Node t = doc.GetRoot();
        t.PrintNode(output);

    }

}  // namespace json
