#include "json.h"

namespace json {
    class ItemContext;
    class KeyItemContext;
    class KeyValueContext;
    class DictItemContext;
    class ArrayItemContext;
    class ArrayValueContext;

    class Builder {
    public:
        Builder();
        KeyItemContext Key(std::string key);
        Builder& Value(Node value);
        DictItemContext StartDict();
        ArrayItemContext StartArray();
        Builder& EndDict();
        Builder& EndArray();
        Node Build();

    private:
        Node root_;
        std::vector<Node*> nodes_stack_;
    };

    class ItemContext {
    public:
        ItemContext(Builder& builder) :builder_(builder) {};
        KeyItemContext Key(std::string key);
        Builder& Value(Node value);
        DictItemContext StartDict();
        ArrayItemContext StartArray();
        Builder& EndDict();
        Builder& EndArray();
    private:
        Builder& builder_;
    };

    class KeyItemContext :public ItemContext {
    public:
        KeyItemContext(Builder& builder) :ItemContext(builder) {};
        KeyItemContext Key(std::string key) = delete;
        KeyValueContext Value(Node value);
        Builder& EndDict() = delete;
        Builder& EndArray() = delete;
    };

    class KeyValueContext :public ItemContext {
    public:
        KeyValueContext(Builder& builder) :ItemContext(builder) {};
        Builder& Value(Node value) = delete;
        DictItemContext StartDict() = delete;
        ArrayItemContext StartArray() = delete;
        Builder& EndArray() = delete;
    };

    class DictItemContext :public ItemContext {
    public:
        DictItemContext(Builder& builder) :ItemContext(builder) {};
        Builder& Value(Node value) = delete;
        DictItemContext StartDict() = delete;
        ArrayItemContext StartArray() = delete;
        Builder& EndArray() = delete;
    };

    class ArrayItemContext :public ItemContext {
    public:
        ArrayItemContext(Builder& builder) :ItemContext(builder) {};
        KeyItemContext Key(std::string key) = delete;
        ArrayValueContext Value(Node value);
        Builder& EndDict() = delete;
    };

    class ArrayValueContext :public ItemContext {
    public:
        ArrayValueContext(Builder& builder) :ItemContext(builder) {};
        KeyItemContext Key(std::string key) = delete;
        ArrayValueContext Value(Node value);
        Builder& EndDict() = delete;
    };

}//namespace json
