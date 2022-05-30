#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <utility>

namespace json {

    class Node;

    using Number = std::variant<int, double>;
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;
    using Value = std::variant<std::nullptr_t, bool, int, double, std::string, Array, Dict>;

    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node {
    public:

        Node() = default;

        template<typename T>
        Node(T val) : value_(val) {}

        template<typename T>
        Node& operator=(T val)
        {
            value_ = val;
            return *this;
        }

        const Value& GetValue() const { return value_; }

        bool IsInt() const;
        bool IsNull() const;
        bool IsDouble() const;
        bool IsPureDouble() const;
        bool IsString() const;
        bool IsBool() const;
        bool IsArray() const;
        bool IsMap() const;

        double AsDouble() const;
        int  AsInt() const;
        const std::string& AsString() const;
        bool AsBool() const;
        const Array& AsArray() const;
        const Dict& AsMap() const;
        bool operator==(const Node& rhs) const { return value_ == rhs.value_; }
        bool operator !=(const Node& rhs) const { return value_ != rhs.value_; }

        void PrintNode(std::ostream& output) const;
    private:
        Value value_;

    };

    struct PrintStruct {
        std::ostream& out;
        void operator()(std::string s, std::ostream& out) const;
        void operator()(std::nullptr_t) const;
        void operator()(double d) const;
        void operator()(int i) const;
        void operator()(std::string s) const;
        void operator()(bool b) const;
        void operator()(Array arr) const;
        void operator()(Dict dic) const;
    };

    class Document {
    public:
        explicit Document(Node root);

        const Node& GetRoot() const;
        bool operator==(const Document& rhs) const { return root_ == rhs.root_; }
        bool operator !=(const Document& rhs) const { return !(root_ == rhs.root_); }
    private:
        Node root_;
    };

    Document Load(std::istream& input);

    void Print(const Document& doc, std::ostream& output);

}  // namespace json
